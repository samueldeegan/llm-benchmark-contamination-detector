**Command-Line Program Function**

Runs the whole program from a terminal with no window, and prints the results as text. It's `app/main.cpp`. It does no work of its own. It calls the other parts in order and prints what they return. The GUI calls the same parts in the same order (see the GUI summary). Use the command-line program's optimized (timing) build for the report's numbers, and its test build to run everything end to end in the codespace.

**How it's run**

```
./detector <corpus.csv> <benchmark.csv> [options]
```

| Option | Meaning | Default |
| ----- | ----- | ----- |
| `--corpus-cols` | Which corpus columns hold the text, counted from 0 | `1,2` (AG News title and description) |
| `--corpus-header` | Whether the corpus CSV's first row is a header | `no` (AG News has no header) |
| `--bench-cols` | Which benchmark columns hold the text | `0` (the MMLU question) |
| `--bench-header` | Whether the benchmark CSV's first row is a header | `no` (MMLU has no header) |
| `--plant` | How many questions to plant | `100` |
| `--seed` | The fixed random seed for planting | `42` |
| `--runs` | Timing runs per structure (R) | `5` |

Examples:

```
./detector data/raw/ag_news_csv/train.csv data/raw/mmlu_test.csv
./detector data/sample/corpus.csv data/sample/bench.csv --plant 2 --runs 1
```

The second runs on the small sample files. `--plant 2` is needed there because the sample doesn't have 100 questions to plant.

**The data involved**

| Name | What it is | Who makes it |
| ----- | ----- | ----- |
| Arguments | The two file paths plus any options | The person running it |
| `PipelineSettings` | Columns, header yes/no, plant count, seed | This program, from the arguments |
| `PipelineOutput` | Corpus array, question lists, answer key, cleaned questions, counts | `runPipeline` |
| `ResultsRecord` | One per structure: median build time, median lookup time, bytes used, flagged list | This program's `measureStructure`, from measurement's pieces |
| `CheckReport` | Planted found, whether the lists agree, unplanted hits | `checkCorrectness` |

**Functions in this file**

| Function | Job |
| ----- | ----- |
| `parseArgs` | Step 1 |
| `printUsage` | Prints the usage line and the reason a run failed |
| `measureStructure` | Steps 3 and 4: measurement's "How the caller uses them" steps for one structure, in one go |
| `printResults` | Step 6 |
| `main` | Runs Steps 1 to 7 in order. Catches errors and calls `printUsage`. |

**Step 1: Read the arguments (`parseArgs`)**

* Read the two paths, then any options. Use the defaults for options not given.
* If a path is missing or an option is wrong, throw an error with a message. `main` catches it, calls `printUsage`, and exits with a nonzero code (a number other than 0 tells the terminal the run failed).

**Step 2: Run the pipeline**

* Call `runPipeline` with the two paths and the settings. It runs every pipeline step in order and gives back the pipeline output.
* If a file can't be opened, `readCsv` throws an error with a message. `main` catches it the same way as in Step 1.
* Print a data summary: number of documents, corpus words, questions, questions shorter than 13 words, and questions planted.
* If fewer questions qualify for planting than `--plant` asks for, print a warning. The pipeline plants all that qualify.

**Step 3: Measure the hash table (`measureStructure`)**

* Follow measurement's "How the caller uses them" steps 1 to 6, with R runs. For each timed build, get a new, empty hash table from `makeStructure` first. This gives the hash table's `ResultsRecord`.
* Step 6 there drops the hash table before the next step, so both structures are never in memory at once.

**Step 4: Measure the suffix array**

* Same as Step 3, with the suffix array.

**Step 5: Run the correctness check**

* Call `checkCorrectness` with both flagged lists, the answer key, the question count, and the cleaned questions.

**Step 6: Print the results (`printResults`)**

```
Data
  Documents: 120,000   Corpus words: ___
  Questions: 14,042    Shorter than 13 words: ___   Planted: 100

Result
  ___ benchmark questions were found in the training set
  (includes the 100 planted test questions)

Performance (median of 5 runs)
  Structure      Build (ms)   Lookup (ms)   Memory (bytes)   Flagged
  Hash table     ___          ___           ___              ___
  Suffix array   ___          ___           ___              ___

Correctness
  Both structures agree: yes
  Planted found:  hash table ___/100, suffix array ___/100
  Planted missed: none
  Unplanted hits: ___ (first 20 shown)
    [ID 4821] which of the following ...
```

* The Result line counts flagged questions, not how many times they appear in the corpus. There's no Safe or Unsafe verdict (D18).
* If the two structures disagree, print the Result line once for each structure, since their counts differ.
* The "includes" note and the "Planted found" line use the actual number planted. Leave both out if `--plant 0` was used.
* For unplanted hits, print the count and only the first 20 with their text. There could be hundreds if MMLU's shared wording gets planted.
* If the lists disagree or a planted question is missed, print those IDs too, under the matching heading.

**Step 7: Exit**

* Exit with code 0 after a normal run, even if the correctness check found problems. The printed report says what went wrong.

**Tests to write**

* Run it with a missing file. It prints the usage line and exits with a nonzero code.
* Run it on the sample files with `--plant 2 --runs 1`. Every planted question is found, and the structures agree.
* Run it on the sample files twice. The output is identical except for the times, which shows the fixed seed works.

**Not this part's job**

* Any actual computing. Every number it prints comes from the pipeline, measurement, or the correctness check.
