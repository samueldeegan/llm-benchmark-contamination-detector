**Measurement Function**

Times each structure's build and lookups, and records how much memory it uses. These are the numbers the report compares. It doesn't run a loop on its own. It provides three small pieces, and the command-line program or the GUI calls them in a loop. That way the GUI gets control back after every single run and can move a progress bar one step (D16).

**The data involved**

| Name | What it is | What it holds | Who makes it |
| ----- | ----- | ----- | ----- |
| Corpus array | All documents' word IDs, with -1 after each document | Word IDs and -1 | Pipeline |
| Question lists | One inner vector per question, index = question ID | Word IDs and -2 | Pipeline |
| Structure | A hash table or a suffix array | Its `build`, `contains`, and `bytesUsed` functions | Hash table / suffix array |
| Run count (R) | How many timing runs per structure | One number: 5 in the GUI, `--runs` in the command-line program | The caller |
| One-run time | The time of one build or one lookup run | A number, in milliseconds (ms) | Measurement |
| Flagged list | Question IDs flagged in one lookup run | IDs, in increasing order | Question checker, through measurement |
| Results record | Everything measured for one structure | Name, median build time, median lookup time, bytes used, flagged list | The caller, from measurement's pieces |

**Rules for every piece**

* Only the structure's own work goes inside the stopwatch. The pipeline (reading, cleaning, and numbering the files) is never timed.
* Use `std::chrono::steady_clock`. It's the C++ clock that only moves forward, so a change to the computer's clock in the middle of a run can't spoil a time.
* For real numbers, use the optimized build without sanitizers (D5, D9). Sanitizers are the extra error-checking tools in the test build, and they slow everything down.
* Keep every timed result. The compiler can delete work whose result is never used, and a deleted loop times as zero.

**Piece 1: Time one build**

* Takes a brand-new, empty structure and the corpus array.
* Start the clock, call `build(corpus)`, stop the clock.
* Returns the time in ms. The structure is left built.
* The caller must pass a new structure every time. Rebuilding one that's already filled could reuse memory it set aside earlier, which would look faster than a real build.

**Piece 2: Time one lookup run**

* Takes a built structure and the question lists.
* Start the clock, call the question checker, stop the clock.
* Returns the time in ms and the flagged list.

**Piece 3: Median**

* Takes a list of times. Sorts a copy and returns the entry at position R / 2, counting from 0.
* With 5 times, that's the 3rd smallest. With 1 time, it's that time.
* Sorting a handful of numbers isn't one of the compared structures, so `std::sort` is fine here.

**How the caller uses them, once per structure**

1. Call Piece 1 R times, with a new structure each time. Keep the last one built.
2. Call `bytesUsed()` once on it.
3. Call Piece 2 R times on it. Keep the flagged list from one run, since every run returns the same list.
4. Call Piece 3 on the R build times and on the R lookup times.
5. Put together the results record: structure name, median build time, median lookup time, bytes used, and flagged list.
6. Let the structure be destroyed before starting the other one, so both structures are never in memory at once. In C++, this happens when the variable holding it goes out of scope (when the code block it was created in ends).

The command-line program does these steps in one go. The GUI makes one call per redraw, so a bar moves after each run.

**Optional extra for the report**

* Record the suffix array's temporary merge sort array (number of entries × 4 bytes) as a separate number, since it only exists during the build.

**Tests to write**

* The median of [5, 1, 4, 2, 3] is 3.
* The median of [7] is 7.
* The flagged list from Piece 2 matches calling the question checker directly.
* After Piece 1, the structure answers `contains` correctly, so it was really built.

**Not this part's job**

* Running the loop (that's the command-line program or the GUI).
* Deciding which questions are flagged (that's the question checker).
* Checking correctness (that's the correctness check).
* Displaying anything.
