**Hash Table Function**

Answers one question: is this exact 13-word sequence somewhere in the corpus? It follows the shared parent type `Structure` from `contamination.h`, so it provides the same three functions as the suffix array: `build`, `contains`, and `bytesUsed`. Files: `src/hash_table.h` and `src/hash_table.cpp`.

**The data involved**

| Name | What it is | What each entry holds | Who makes it |
| ----- | ----- | ----- | ----- |
| Corpus array (`corpus`) | Every word in every document, in order, as word IDs, with −1 after each document | A word ID, or −1 | Pipeline. The hash table remembers where it is (a reference) and never copies or changes it. |
| Table (`table`) | The hash table's slots, a `vector<int>` | A corpus position where a 13-word window starts, or −1 for an empty slot | This structure |
| One question's list | The word IDs of one benchmark question | A word ID, or −2 for a word the corpus never has | Pipeline. The question checker passes it in. |
| Start position | Where the 13-word window starts inside that question's list | One number | Question checker |

**Helpers (used only inside the hash table)**

* `isValidWindow(pos)`: true if the 13 corpus numbers starting at `pos` contain no −1.
* `hashWindow(list, start)`: the hash of the 13 numbers in `list` starting at `start`, as a slot index (Step 3).
* `windowsMatch(storedPos, list, start)`: true if the 13 corpus numbers at `storedPos` equal the 13 numbers in `list` starting at `start`.

`hashWindow` and `windowsMatch` take a list and a start position so they work on both the corpus (during `build`) and a question (during `contains`).

**Step 1: Count the valid windows (in `build`)**

* Remember the corpus array passed to `build`.
* A window is the 13 numbers starting at one position in the corpus array.
* A window is valid only if none of its 13 numbers is −1, so a match can never span two documents.
* Check every starting position from 0 up to (corpus length − 13) with `isValidWindow`, and count the valid windows. Call the count W.

**Step 2: Create the table (in `build`)**

* Make a vector with at least 2 × W slots. If W is 0, use at least 1 slot so the next step never divides by zero.
* Set every slot to −1, meaning empty. Real positions are always 0 or more, so −1 can't be mistaken for one.
* The table is sized once and never resized, which keeps it about half empty.

**Step 3: Hash each valid window (`hashWindow`)**

* Start with h = 0, stored as an unsigned 64-bit integer.
* For each of the window's 13 numbers in order: h = h × 31 + number.
* The slot index is h % tableSize.
* Each window is hashed from scratch, with no rolling hash.

**Step 4: Insert each valid window with linear probing (in `build`)**

* Go to the slot from Step 3.
* If the slot is empty, store the window's starting position there.
* If the slot holds a position, compare the window with the one stored there using `windowsMatch(storedPos, corpus, pos)`.
  * If they're identical, the sequence is already stored. Skip it.
  * If they differ, move to the next slot (wrapping from the last slot back to 0) and repeat.

**Step 5: Contains (takes one question's list and a start position)**

* The window is the 13 numbers in the question's list from `start` to `start + 12`. Nothing is copied; the hash table reads them in place.
* If any of those 13 numbers is −2 (a word the corpus never has), return false.
* Compute the slot index with `hashWindow(question, start)`.
* If the slot is empty, return false.
* If the slot holds a position, compare with `windowsMatch(storedPos, question, start)`.
  * If they're identical, return true.
  * If they differ, move to the next slot (wrapping around) and repeat.
* `contains` doesn't change the hash table, so it's marked `const`.
* Only call `contains` after `build`. Before `build` the table has 0 slots, and h % 0 crashes.

**Step 6: Bytes used**

* Return the number of slots × 4 bytes (each slot is an int).
* Don't count the corpus array, since both structures share it.
* Before `build`, this is 0.

**Big O (worst case, for the report)**

* `build`: O(n) on average, O(n²) in the worst case, where every window lands in the same slot and each insert probes past all earlier ones. n is the corpus length.
* `contains`: O(1) on average, O(n) in the worst case, for the same reason.
* Space: O(n), since the table has about 2 × W slots and W ≤ n.

**Tests to write**

Use a small handmade corpus: two documents of about 15–20 numbers each, separated by −1.

* `contains` is true for every valid window in the corpus.
* `contains` is false for a window that would cross a −1. Make one by copying the last few numbers of document 1 followed by the first few of document 2.
* `contains` is false for any window holding −2.
* A corpus with the same 13-number window twice stores it only once.
* `bytesUsed` is 0 before `build` and (number of slots × 4) after.
