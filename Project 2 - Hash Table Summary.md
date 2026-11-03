**Hash Table Function**

Takes in the corpus number list from the pipeline (word IDs, with −1 after each document). Provides three functions: build, contains, and bytes used.

**Step 1: Count the valid windows**

* A window is the 13 numbers starting at one position in the corpus list.  
* A window is valid only if none of its 13 numbers is −1, so a match can never span two documents.  
* Check every starting position from 0 up to (corpus length − 13), and count the valid windows. Call the count W.

**Step 2: Create the table**

* Make a vector with at least 2 × W slots. If W is 0, use at least 1 slot so the next step never divides by zero.  
* Set every slot to −1, meaning empty. Real positions are always 0 or more, so −1 can’t be mistaken for one.  
* The table is sized once and never resized, which keeps it about half empty.

**Step 3: Hash each valid window**

* Start with h \= 0, stored as an unsigned 64-bit integer.  
* For each of the window’s 13 numbers in order: h \= h × 31 \+ number.  
* The slot index is h % tableSize.  
* Each window is hashed from scratch, with no rolling hash.

**Step 4: Insert each valid window with linear probing**

* Go to the slot from Step 3\.  
* If the slot is empty, store the window’s starting position there.  
* If the slot holds a position, compare the 13 corpus numbers at that position with this window’s 13 numbers.  
  * If they’re identical, the sequence is already stored. Skip it.  
  * If they differ, move to the next slot (wrapping from the last slot back to 0\) and repeat.

**Step 5: Contains (takes one 13-number sequence)**

* If the sequence contains −2 (a word the corpus never has), return false.  
* Compute the slot index the same way as Step 3\.  
* If the slot is empty, return false.  
* If the slot holds a position, compare the 13 corpus numbers at that position with the sequence.  
  * If they’re identical, return true.  
  * If they differ, move to the next slot (wrapping around) and repeat.

**Step 6: Bytes used**

* Return the number of slots × 4 bytes (each slot is an int).  
* Don’t count the corpus list, since both structures share it.

