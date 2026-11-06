**Measurement Function**

Times each structure's build and lookups, and records how much memory it uses. These are the numbers the report compares. It doesn't run a loop on its own. It provides four small pieces, and the command-line program or the GUI calls them in a loop. That way the GUI gets control back after every single run and can move a progress bar one step (D16).

Files: `src/measurement.h` and `src/measurement.cpp`. The two bundles this part hands out, `LookupRun` and `ResultsRecord`, are defined in `measurement.h`.

**The data involved**

| Name | What it is | What it holds | Who makes it |
| ----- | ----- | ----- | ----- |
| Corpus array | All documents' word IDs, with -1 after each document | Word IDs and -1 | Pipeline |
| Question lists | One inner vector per question, index = question ID | Word IDs and -2 | Pipeline |
| Structure | A hash table or a suffix array, held as a `Structure` (the shared parent type in `contamination.h`) | Its `build`, `contains`, and `bytesUsed` functions | `makeStructure` (Piece 0) |
| Run count (R) | How many timing runs per structure | One number: 5 in the GUI, `--runs` in the command-line program | The caller |
| One-run time | The time of one build or one lookup run | A number, in milliseconds (ms), as a `double` | Measurement |
| `LookupRun` | The result of one lookup run | The time in ms and the flagged list | Measurement (Piece 2) |
| Flagged list | Question IDs flagged in one lookup run | IDs, in increasing order | Question checker, through measurement |
| `ResultsRecord` | Everything measured for one structure | Name, median build time, median lookup time, bytes used, flagged list | The caller, from measurement's pieces |

**Rules for every piece**

* Only the structure's own work goes inside the stopwatch. The pipeline (reading, cleaning, and numbering the files) is never timed, and neither is making the empty structure in Piece 0.
* Use `std::chrono::steady_clock`. It's the C++ clock that only moves forward, so a change to the computer's clock in the middle of a run can't spoil a time.
* For real numbers, use the optimized build without sanitizers (D5, D9). Sanitizers are the extra error-checking tools in the test build, and they slow everything down.
* Keep every timed result. The compiler can delete work whose result is never used, and a deleted loop times as zero.

**Piece 0: Make a new structure (`makeStructure`)**

* Takes which kind to make: hash table or suffix array.
* Returns a brand-new, empty structure of that kind, held in a `std::unique_ptr<Structure>`. A `unique_ptr` is a C++ holder that deletes the structure automatically once nothing holds it anymore.
* Every build run needs a fresh structure (see Piece 1), and the GUI has to keep the current structure alive between redraws. Holding it in a `unique_ptr` handles both: the caller keeps the holder as long as it needs the structure, and replacing or dropping the holder deletes the old one.
* Why it lives here and not in `contamination.h`: making a hash table or a suffix array means including `hash_table.h` and `suffix_array.h`, and both of those include `contamination.h`. If `contamination.h` included them back, the files would include each other in a loop and wouldn't compile. Measurement already uses both structures, and neither structure includes measurement, so there's no loop here.

**Piece 1: Time one build (`timeOneBuild`)**

* Takes a brand-new, empty structure from Piece 0 and the corpus array.
* Start the clock, call `build(corpus)`, stop the clock.
* Returns the time in ms. The structure is left built.
* The caller must pass a new structure every time. Rebuilding one that's already filled could reuse memory it set aside earlier, which would look faster than a real build.

**Piece 2: Time one lookup run (`timeOneLookupRun`)**

* Takes a built structure and the question lists.
* Start the clock, call the question checker (`checkQuestions`), stop the clock.
* Returns a `LookupRun`: the time in ms and the flagged list.

**Piece 3: Median (`median`)**

* Takes a list of times. Sorts a copy and returns the entry at position R / 2, counting from 0.
* With 5 times, that's the 3rd smallest. With 1 time, it's that time.
* Sorting a handful of numbers isn't one of the compared structures, so `std::sort` is fine here.

**How the caller uses them, once per structure**

1. R times: call Piece 0 for a new structure, then Piece 1 on it. Keep the last structure built; each earlier one is deleted when its holder is replaced.
2. Call `bytesUsed()` once on the structure you kept.
3. Call Piece 2 R times on it. Keep the flagged list from one run, since every run returns the same list.
4. Call Piece 3 on the R build times and on the R lookup times.
5. Put together the `ResultsRecord`: structure name, median build time, median lookup time, bytes used, and flagged list.
6. Drop the structure's holder before starting the other structure, so both are never in memory at once. Once the `unique_ptr` holding it is reset or goes out of scope (the code block it was created in ends), the structure is deleted.

The command-line program does these steps in one go, in its `measureStructure` function. The GUI makes one call per redraw, so a bar moves after each run.

**Optional extra for the report**

* Record the suffix array's temporary merge sort array (number of entries × 4 bytes) as a separate number, since it only exists during the build.

**Tests to write**

* `makeStructure` gives back an empty structure: `bytesUsed` is 0 before `build` is called. (Don't call `contains` on an unbuilt hash table: its table has 0 slots, and h % 0 crashes.)
* The median of [5, 1, 4, 2, 3] is 3.
* The median of [7] is 7.
* The flagged list from Piece 2 matches calling the question checker directly.
* After Piece 1, the structure answers `contains` correctly, so it was really built.

**Not this part's job**

* Running the loop (that's the command-line program or the GUI).
* Deciding which questions are flagged (that's the question checker).
* Checking correctness (that's the correctness check).
* Displaying anything.
