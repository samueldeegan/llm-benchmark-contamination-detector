**GUI Function**

The window version of the program, in `gui/`. It's built with SFML, a C++ library for drawing windows, shapes, and text. It follows the team's submitted wireframe, with the changes listed at the end. Like the command-line program, it does no computing of its own. It calls the same parts in the same order and shows what they return. It needs a real computer with a screen, so it won't run in the codespace.

**The data involved**

| Name | What it is | Who makes it |
| ----- | ----- | ----- |
| Inputs | The two CSV files | The person running it |
| Pipeline output | Corpus array, question lists, answer key, cleaned questions, counts | Pipeline |
| Results records | One per structure: median build time, median lookup time, bytes used, flagged list | The GUI, from measurement's pieces |
| Check report | Planted found, whether the lists agree, unplanted hits | Correctness check |

**The sequence (same as the command-line program)**

1. Run the pipeline, planting 100 questions. Every GUI run plants 100. There's no on/off switch (D15).
2. Hash table: follow measurement's "How the caller uses them" steps 1 to 6, with 5 runs, making one call per redraw.
3. Suffix array: the same.
4. Run the correctness check.
5. Show the results.

**Window 1: Main window**

* "Insert Training Text CSV here" and "Insert Benchmark CSV here" each open a way to pick a file. After a pick, show the chosen file's name.
  * **Will decides how files are picked.** Option A is tinyfiledialogs: one .c file and one .h file, under the zlib license (a permissive license that lets us include it in our repo). It opens the computer's normal "Open file" window on Windows, Mac, and Linux. Option B is a text box where the user types the file's path. SFML has no file picker and no way to drop files onto the window, so it has to be one of these.
* Start only works once both files are picked.
* The text columns are fixed (D17): AG News title and description, MMLU question, no header rows. There's no column picker. The README says which columns are used.

**Window 2: Loading screen (new compared with the wireframe)**

* Draw "Loading data…" once, then run the pipeline.
* The window can't redraw while the pipeline runs. Drawing this first keeps it from sitting on a blank or half-drawn frame.

**Window 3: Efficiency window**

* Four progress bars: hash table build, suffix array build, hash table lookup, suffix array lookup. They sit in the wireframe's order on screen.
* Each bar fills in 5 steps of 20%, one step per timing run (D16). We time everything 5 times and report the middle time, so each step is one of those 5 runs.
* No second thread (a second line of work running at the same time). Instead, each pass through the window loop does this:
  1. Handle window events, like the close button.
  2. Make the next single call to measurement.
  3. Move that bar forward one step.
  4. Redraw.
* Run order: hash table builds, hash table lookups, suffix array builds, suffix array lookups. So on screen, the hash table bars fill first, then the suffix array bars.
* Memory bars: one for each structure, showing how many bytes it uses. A longer bar means more memory. Use the same scale for both bars so they can be compared, and label each with its size, for example in MB.
  * These aren't progress bars. Each one appears at full length once its structure's 5 builds are done, because memory can only be measured on a structure that has been built.
  * So the hash table's memory bar shows up early. The suffix array's shows up later, once the suffix array's builds finish.
* The window still freezes during each single run. As far as I know, Windows marks a window "Not responding" if it doesn't redraw for about 5 seconds. If one run takes longer than that, the label goes away when the run ends. We'll know once the corpus word count is known (A5).

**Window 4: Test Complete window**

* One line at the top: "x benchmark questions were found in the training set (includes the 100 planted test questions)." x counts flagged questions, not how many times they appear. This replaces the wireframe's Safe/Unsafe boxes (D18).
* Two lines below it: "Planted found: x/100" and "Both structures agree: yes/no."
* Two boxes, one per structure: median build time, median lookup time, memory, and number of questions flagged.
* For the lists of IDs behind these lines (missed planted questions, disagreements, unplanted hits), run the command-line program, which prints them.

**Build note**

* For real timing numbers, use the GUI's optimized build without sanitizers (D9). Sanitizers are the extra error-checking tools in the test build, and they slow everything down.

**Checks to do by hand**

* Start does nothing until both files are picked.
* On the sample files, the planted line shows all planted questions found, and the structures agree.
* Each progress bar moves in exactly 5 steps and ends at 100%.
* Each memory bar appears right after its structure's builds finish.

**Changes from the submitted wireframe (list these in the report's "changes after proposal")**

* The Safe/Unsafe boxes are replaced by a count. The program plants 100 questions on every run, so "Safe" could never appear.
* Added the "Loading data…" screen.
* Progress bars move in 5 steps, one per timing run, instead of filling smoothly.
* Each memory bar appears once its structure is built, instead of both at once.
* The results screen adds the "Planted found" and "Both structures agree" lines.
* "Occurrences" is reworded to "benchmark questions found," since the program counts questions.

**Not this part's job**

* Any actual computing. Every number it shows comes from the pipeline, measurement, or the correctness check.
