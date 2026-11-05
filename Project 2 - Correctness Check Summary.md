**Correctness Check Function**

Answers three questions: did each structure find every planted question, did the two structures flag exactly the same questions, and what else got flagged that wasn't planted? It runs once, after measurement has finished for both structures. It reports problems but doesn't fix anything.

**The data involved**

| Name | What it is | What each entry holds | Who makes it |
| ----- | ----- | ----- | ----- |
| Answer key | One entry per planted question | A question ID and the document number it went into | Planting (pipeline Step 5\) |
| Hash table flagged list (`htFlagged`) | Questions the hash table flagged | Question IDs, in increasing order | Measurement's results record |
| Suffix array flagged list (`saFlagged`) | Questions the suffix array flagged | Question IDs, in increasing order | Measurement's results record |
| Question count (Q) | How many benchmark questions there are | One number | Pipeline |
| Cleaned questions | The text of each question after cleanup | Text, indexed by question ID | Pipeline (Step 2\) |
| Planted marker (`isPlanted`) | One true/false per question | true if that question ID was planted | The check, from the answer key |
| Check report | The output | Planted found per structure, planted missed per structure, whether the lists match, IDs in only one list, unplanted hits with their text | The check |

**Step 1: Mark which questions were planted**

* Make a true/false vector of size Q, all false.  
* For each answer key entry, set `isPlanted[question ID]` to true.  
* This answers "was question i planted?" in one step, without searching the answer key.

**Step 2: Count planted questions found, once per structure**

* Count the IDs in the flagged list where `isPlanted` is true.  
* Report it as found / planted, for example 100 / 100\.  
* List any planted IDs that don't appear in the flagged list. This list should be empty.

**Step 3: Check that the structures agree**

* Both lists are in increasing order, so compare them entry by entry.  
* They agree only if they're the same length and every entry matches.  
* If they don't agree, list the IDs that appear in only one of them, and which structure flagged each.

**Step 4: List unplanted hits**

* Every flagged ID where `isPlanted` is false is an unplanted hit.  
* Include each one's cleaned question text, so a person can read it.  
* If the lists disagree, list unplanted hits separately for each structure.

**Step 5: Return the check report**

**Example**

Six questions; questions 1 and 4 were planted.

|  | Hash table | Suffix array |
| ----- | ----- | ----- |
| Flagged list | \[1, 3, 4\] | \[1, 3, 4\] |
| Planted found | 2 / 2 | 2 / 2 |
| Planted missed | none | none |

The lists agree. Question 3 is an unplanted hit.

**Reading the results**

| What you see | Most likely cause | Where to look |
| ----- | ----- | ----- |
| Both structures miss the same planted question | A bug before the structures, since both got it wrong the same way | Planting, cleanup, word numbering, or the question checker's window loop |
| The structures' lists differ | A bug in one structure | Run the reference checker on that question. Whichever structure disagrees with it is wrong. |
| An unplanted question is flagged | Not necessarily a bug | Read its text. It's a natural overlap with AG News, shared wording with a planted question, or a bug. |

**Expected trap: shared wording inside MMLU**

* Some MMLU subjects may reuse the same long sentence across many questions. As far as I know, "moral scenarios" questions all begin with the same sentence of more than 13 words. Confirm this when you open the file.  
* If one of those questions gets planted, every other question sharing that sentence is correctly flagged as an unplanted hit.  
* That's not a bug. It shows a real limit of 13-word matching, since shared boilerplate counts as contamination, and it's worth a point in the report's analysis.