**Question Checker Function**

Answers one question about each benchmark question: does any 13-word stretch of it appear in the corpus? It works with either structure, since it only calls `contains`. Measurement calls it once per run for each structure.

**The data involved**

| Name | What it is | What each entry holds | Who makes it |
| ----- | ----- | ----- | ----- |
| Structure | The hash table or the suffix array, already built | Only its `contains` function is used | Hash table / suffix array |
| Question lists (`questions`) | A vector of vectors, one inner vector per benchmark question, in original benchmark order | A word ID, or −2 for a word the corpus never has | Pipeline (Step 7\) |
| Question ID | A question's index in the outer vector | Same as its row number in the benchmark | Pipeline order |
| Window | 13 consecutive word IDs from one question | A word ID, or −2 | The checker, on the fly |
| Flagged list (`flagged`) | The output | Question IDs that matched, in increasing order | The checker |

The checker never reads the corpus array or the word table. It only calls `contains`.

**Step 1: Go through the questions in order**

* For each question ID i, from 0 to (number of questions − 1), take `questions[i]`. Call its length L.

**Step 2: Skip short questions**

* If L is less than 13, the question has no 13-word window and can never be flagged. Skip it.  
* Your pipeline's Step 3 already counts these for the report, so the checker doesn't need to count them again. Pick one place to count.

**Step 3: Slide the window**

* Window starts run from s \= 0 up to s \= L − 13, inclusive. A question with L words has L − 12 windows.  
* The window at s is word IDs s through s \+ 12\.

**Step 4: Check each window**

* Pass the window to `contains`.  
* If it returns true, add i to the flagged list, stop checking this question's windows, and go to the next question.  
* If every window returns false, the question isn't flagged.  
* Don't skip the question when it contains −2. `contains` already returns false for any window holding −2, and a later window may still match.

**Step 5: Return the flagged list**

* Return the flagged list after the last question.  
* Because questions are checked in order, the IDs come out in increasing order. That lets the correctness check compare the two structures' lists directly.

**Example, using 3-word windows instead of 13**

Corpus: "the cat sat on the mat" \| "the cat ran," with the \= 0, cat \= 1, sat \= 2, on \= 3, mat \= 4, ran \= 5\.

| ID | Question | Word IDs | Windows checked | Result |
| ----- | ----- | ----- | ----- | ----- |
| 0 | did the cat ran | \[−2, 0, 1, 5\] | \[−2, 0, 1\] false; \[0, 1, 5\] true | Flagged |
| 1 | the mat sat | \[0, 4, 2\] | \[0, 4, 2\] false | Not flagged |
| 2 | cat ran | \[1, 5\] | None: shorter than 3 | Skipped |

Returned flagged list: \[0\]

**How it's used**

* Measurement starts a timer, calls the checker, and stops the timer. It repeats this several times per structure and reports the median as the lookup time.  
* Every run returns the same flagged list, so measurement keeps the list from any one run.  
* The correctness check compares the two structures' lists and checks them against the answer key.

