**Command-Line Program Function**

Runs the whole program from a terminal with no window, and prints the results as text. It’s `app/main.cpp`. It does no work of its own. It calls the other parts in order and prints what they return. Use its optimized (timing) build for the report’s numbers, and its test build to run everything end to end in the codespace.

**How it’s run**

```
./detector <corpus.csv> <benchmark.csv> [options]
```

| Option | Meaning | Default |
| ----- | ----- | ----- |
| `--corpus-cols` | Which corpus columns hold the text, counted from 0 | `1,2` (AG News title and description) |
| `--corpus-header` | Whether the corpus CSV’s first row is a header | `no` (confirm when you open train.csv) |
| `--bench-cols` | Which benchmark columns hold the text | `0` (the MMLU question) |
| `--bench-header` | Whether the benchmark CSV’s first row is a header | `no` |
| `--plant` | How many questions to plant | `100` |
| `--seed` | The fixed random seed for planting | `42` |
| `--runs` | Timing runs per structure (R) | `5` |

Examples:

```
./detector data/train.csv data/mmlu_test.csv
./detector data/sample/corpus.csv data/sample/bench.csv --plant 2 --runs 1
```

The second runs on the small sample files. `--plant 2` is needed there because the sample doesn’t have 100 questions to plant.

**The data involved**

| Name | What it is | Who makes it |
| ----- | ----- | ----- |
| Arguments | The two file paths plus any options | The person running it |
| Pipeline output | Corpus array, question lists, answer key, cleaned questions, counts | Pipeline |
| Results records | One per structure: build time, lookup time, bytes, flagged list | Measurement |
| Check report | Planted found, agreement, unplanted hits | Correctness check |

**Step 1: Read the arguments**

* Read the two paths, then any options. Use the defaults for options not given.  
* If a path is missing or a file can’t be opened, print a usage line and the reason, then exit with a nonzero code (a number other than 0 tells the terminal the run failed).

**Step 2: Run the pipeline**

* Run every pipeline step in order: read and clean the benchmark, pick the questions to plant, read and clean the corpus, plant, build the word table and corpus array, and convert the questions to numbers.  
* Print a data summary: number of documents, corpus words, questions, questions shorter than 13 words, and questions planted.  
* If fewer questions qualify for planting than `--plant` asks for, print a warning and plant all that qualify.

**Step 3: Measure the hash table**

* Call measurement with the hash table, the corpus array, the question lists, and R. Keep the results record.  
* Let the hash table be destroyed before the next step, so both structures never sit in memory at once. In C++, this happens when the variable holding it goes out of scope (when the code block it was created in ends).

**Step 4: Measure the suffix array**

* Same as Step 3, with the suffix array.

**Step 5: Run the correctness check**

* Call it with both flagged lists, the answer key, the question count, and the cleaned questions.

**Step 6: Print the results**

```
Data
  Documents: 120,000   Corpus words: ___
  Questions: 14,042    Shorter than 13 words: ___   Planted: 100

Performance (median of 5 runs)
  Structure      Build (ms)   Lookup (ms)   Memory (bytes)
  Hash table     ___          ___           ___
  Suffix array   ___          ___           ___

Correctness
  Planted found:  hash table ___/100, suffix array ___/100
  Planted missed: none
  Both structures flagged the same questions: yes
  Unplanted hits: ___ (first 20 shown)
    [ID 4821] which of the following ...
```

* For unplanted hits, print the count and only the first 20 with their text. There could be hundreds if MMLU’s shared wording gets planted.  
* If the lists disagree or a planted question is missed, print those IDs too, under the matching heading.

**Step 7: Exit**

* Exit with code 0 after a normal run, even if the correctness check found problems. The printed report says what went wrong.

**Tests to write**

* Run it with a missing file. It prints the usage line and exits with a nonzero code.  
* Run it on the sample files with `--plant 2 --runs 1`. Every planted question is found, and the structures agree.  
* Run it on the sample files twice. The output is identical except for the times, which shows the fixed seed works.

**Not this part’s job**

* Any actual computing. Every number it prints comes from the pipeline, measurement, or the correctness check.

