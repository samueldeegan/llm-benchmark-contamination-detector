**Question Checker Function**

Answers one question about each benchmark question: does any 13-word stretch of it appear in the corpus? It works with either structure, since it only calls `contains`. Measurement calls it once per lookup run for each structure.

Function: `checkQuestions`. Files: `src/question_checker.h` and `src/question_checker.cpp`.

**The data involved**

| Name | What it is | What each entry holds | Who makes it |
| ----- | ----- | ----- | ----- |
| Structure | The hash table or the suffix array, already built, held as a `const Structure&` (read-only) | Only its `contains` function is used | Hash table / suffix array, made by `makeStructure` |
| Question lists (`questions`) | A vector of vectors, one inner vector per benchmark question, in original benchmark order | A word ID, or −2 for a word the corpus never has | Pipeline |
| Question ID | A question's index in the outer vector | Same as its row number in the benchmark | Pipeline order |
| Start position (`s`) | Where a 13-word window starts inside one question's list | One number | The checker, as it slides |
| Flagged list (`flagged`) | The output | Question IDs that matched, in increasing order | The checker |

A window is never copied into its own list. The checker passes the question's list and the start position to `contains`, and the structure reads the 13 words in place. The checker never reads the corpus array or the word table.

**Step 1: Go through the questions in order**

* For each question ID i, from 0 to (number of questions − 1), take `questions[i]`. Call its length L.

**Step 2: Skip short questions**

* If L is less than 13, the question has no 13-word window and can never be flagged. Skip it.
* The pipeline (`pickQuestionsToPlant`) already counts these for the report, so the checker doesn't count them again.

**Step 3: Slide the window**

* Window starts run from s = 0 up to s = L − 13, inclusive. A question with L words has L − 12 windows.
* The window at s is word IDs s through s + 12.

**Step 4: Check each window**

* Call `contains(questions[i], s)`.
* If it returns true, add i to the flagged list, stop checking this question's windows, and go to the next question.
* If every window returns false, the question isn't flagged.
* Don't skip the question when it contains −2. `contains` already returns false for any window holding −2, and a later window may still match.

**Step 5: Return the flagged list**

* Return the flagged list after the last question.
* Because questions are checked in order, the IDs come out in increasing order. That lets the correctness check compare the two structures' lists directly.

**Example, using 3-word windows instead of 13**

Corpus: "the cat sat on the mat" | "the cat ran," with the = 0, cat = 1, sat = 2, on = 3, mat = 4, ran = 5.

| ID | Question | Word IDs | Windows checked | Result |
| ----- | ----- | ----- | ----- | ----- |
| 0 | did the cat ran | [−2, 0, 1, 5] | s = 0: [−2, 0, 1] false; s = 1: [0, 1, 5] true | Flagged |
| 1 | the mat sat | [0, 4, 2] | s = 0: [0, 4, 2] false | Not flagged |
| 2 | cat ran | [1, 5] | None: shorter than 3 | Skipped |

Returned flagged list: [0]

**Big O (worst case, for the report)**

* O(W × C), where W is the total number of windows across all questions and C is the cost of one `contains` call: O(1) on average for the hash table, O(log n) for the suffix array.

**How it's used**

* Measurement's `timeOneLookupRun` starts a timer, calls `checkQuestions`, and stops the timer. It's repeated several times per structure, and the median is reported as the lookup time.
* Every run returns the same flagged list, so measurement keeps the list from one run.
* The correctness check compares the two structures' lists and checks them against the answer key.

**Tests to write**

* The 3-word example above, rebuilt with 13-word questions on a handmade corpus: a question whose second window matches is flagged; one with no match isn't; one shorter than 13 is skipped.
* A question containing −2 early on but matching in a later window is still flagged.
* The flagged list comes out in increasing order.
