**Suffix Array Function**

Answers one question: is this exact 13-word sequence somewhere in the corpus? It takes in the corpus array from the pipeline and provides three functions: build, contains, and bytes used.

**The data involved**

There are five separate things. Each holds one kind of number, and they're never mixed.

| Name | What it is | What each entry holds | Who makes it | Does the suffix array change it? |
| ----- | ----- | ----- | ----- | ----- |
| Word table (dictionary) | A lookup from each word to its number | Word → word ID | Pipeline | Never touches it |
| Corpus array (`corpus`) | Every word in every document, in order, as word IDs, with −1 after each document | A word ID, or −1 | Pipeline | Only reads it |
| Suffix array (`sa`) | A list of starting points into the corpus array, sorted | A position in the corpus array | This structure | Builds and owns it |
| Temporary array (`temp`) | Scratch space for merge sort | A position | This structure | Used during build only |
| Query window | 13 words from one question | A word ID, or −2 | Question checker | Only reads it |

There are two kinds of numbers, and keeping them apart is the whole trick:

* A **word ID** is a number standing for a word (the \= 0). Word IDs live in the corpus array and the query window.  
* A **position** is a slot number in the corpus array (position 4 is the fifth slot). Positions live in the suffix array.  
* `corpus[p]` turns a position into a word ID: "what word is at slot p?"

A **suffix** is never stored anywhere. It means "the word IDs in the corpus array starting at position p, up to the next −1." It's read on the fly whenever two positions need comparing.

**Example used throughout**

Word table: the \= 0, cat \= 1, sat \= 2, on \= 3, mat \= 4, ran \= 5

```
position:  0  1  2  3  4  5   6  7  8  9  10
corpus:    0  1  2  3  0  4  -1  0  1  5  -1
           the cat sat on the mat | the cat ran |
```

**Step 1: List the starting positions**

* Make the suffix array: every position p where `corpus[p]` isn't −1.  
* Example: `sa = [0, 1, 2, 3, 4, 5, 7, 8, 9]`. Positions 6 and 10 are left out because they hold −1.  
* Short suffixes stay in. Position 9 ("ran") is only one word long, but it's kept. Nothing is removed for being shorter than 13 words.

**Step 2: The comparison rule (used by the sort)**

* Takes two positions, a and b, and decides which comes first.  
* It never compares a and b themselves. It compares the word IDs the corpus holds starting at each one:  
  * Read `corpus[a]` and `corpus[b]`. If they differ, the smaller word ID comes first.  
  * If they're equal, step both forward one slot and compare again.  
  * If both read −1 at the same step, the suffixes are equal. Stop.  
* −1 is never stored in the suffix array, but the comparison still runs into it in the corpus array when a document ends. Since −1 is smaller than every word ID, a suffix that ends sooner comes first.  
* Every document ends with −1, so the walk always stops before running off the end of the corpus array.

Worked example, positions 7 and 4:

| Step | Word ID at 7's side | Word ID at 4's side | Result |
| ----- | ----- | ----- | ----- |
| 1st | `corpus[7]` \= 0 (the) | `corpus[4]` \= 0 (the) | Equal, keep going |
| 2nd | `corpus[8]` \= 1 (cat) | `corpus[5]` \= 4 (mat) | 1 \< 4, so position 7 comes first |

The position numbers themselves (7 and 4\) play no part. That's why 7 ends up before 4 in the sorted array.

**Step 3: Sort the suffix array with merge sort**

* **Split:** cut the range of the suffix array in half, again and again, until each piece is one entry. A single entry is already sorted.  
* **Merge:** combine two neighboring sorted pieces into one:  
  * Keep a pointer at the front of each piece.  
  * Compare the two front positions using the Step 2 rule.  
  * Copy whichever comes first into the temporary array, and move that piece's pointer forward. If they're equal, take the left one.  
  * When one piece runs out, copy the rest of the other piece over unchanged.  
  * Copy the temporary array back into that range of the suffix array.  
* Repeat merges up the levels until the whole suffix array is one sorted list.  
* The sort only moves positions around. The corpus array is never changed.  
* Example result: `sa = [0, 7, 4, 1, 8, 2, 3, 5, 9]`

```
sa entry   position   suffix (read from corpus, as words)
0          0          the cat sat on the mat
1          7          the cat ran
2          4          the mat
3          1          cat sat on the mat
4          8          cat ran
5          2          sat on the mat
6          3          on the mat
7          5          mat
8          9          ran
```

The order follows word IDs, not the alphabet. It only looks alphabetical here because the example happened to number the words that way.

**Step 4: Contains (takes one 13-word query window)**

* If the window contains −2 (a word the corpus never has), return false.  
* Set low \= 0 and high \= (number of entries in the suffix array) − 1\.  
* While low ≤ high:  
  * mid \= low \+ (high − low) / 2\.  
  * p \= `sa[mid]`, the position stored at that entry.  
  * Compare up to 13 words, k \= 0 to 12: read `corpus[p + k]` and window\[k\].  
    * If the corpus word ID is smaller (−1 counts as smaller), the match must be further right. Set low \= mid \+ 1 and stop comparing.  
    * If the corpus word ID is larger, the match must be further left. Set high \= mid − 1 and stop comparing.  
    * If they're equal, go to the next k.  
  * If all 13 matched, return true.  
* If low passes high, return false.

Hitting −1 here doesn't mean "not found." It means this suffix ran out of words before 13, so it sorts before the query. The search moves right and keeps going.

**Step 5: Bytes used**

* Return the number of entries in the suffix array × 4 bytes.  
* Don't count the corpus array, since both structures share it.  
* The temporary array adds the same amount again, but only during the build.

