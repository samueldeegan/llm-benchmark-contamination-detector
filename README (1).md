# Mise en Place: Benchmark Contamination Detector

COP 3530 Project 2. Samuel Deegan ([samueldeegan](https://github.com/samueldeegan)). Individual submission.

Video demo: [link]

This program checks whether questions from a benchmark (MMLU) appear word for word in a training corpus (AG News). A question is flagged if any 13-word stretch of it, after cleanup, appears anywhere in the corpus.

AG News contains no MMLU questions on its own, so the program plants 100 questions into random articles using a fixed seed and records where each one went. That gives a known answer to check the detector against.

The corpus is stored in a hash table written from scratch (open addressing with linear probing). The planned suffix array and GUI were not built. The report explains why.

## Requirements

- g++ with C++17 support. The GitHub Codespace for this repo already has it.
- No other libraries. The code uses only the C++ standard library.

## Data

Both input files are in the repo, so there is nothing to download.

| File | Contents |
| --- | --- |
| `data/full/ag_news_train.csv` | AG News training set: 120,000 news articles with columns class, title, and description. No header row. |
| `data/full/mmlu_test.csv` | MMLU test set: 14,042 questions from 57 subject files, joined into one CSV. Columns are question, four answers, and the correct answer. No header row. |

You only need these steps if you want to rebuild the two files from the original downloads:

```
mkdir -p data/raw && cd data/raw
curl -L -O https://s3.amazonaws.com/fast-ai-nlp/ag_news_csv.tgz
curl -L -O https://people.eecs.berkeley.edu/~hendrycks/data.tar
tar -xzf ag_news_csv.tgz
tar -xf data.tar
awk 1 data/test/*_test.csv > mmlu_test.csv
cp ag_news_csv/train.csv ../full/ag_news_train.csv
cp mmlu_test.csv ../full/mmlu_test.csv
```

`awk 1` joins the 57 MMLU subject files in alphabetical order. Question IDs depend on that order.

## Build

From the repository's top folder:

```
g++ -std=c++17 -O2 -Wall -Wextra -Isrc app/main.cpp src/pipeline/*.cpp src/structures/hash_table.cpp src/evaluation/*.cpp -o detector
```

## Run

```
./detector data/full/ag_news_train.csv data/full/mmlu_test.csv
```

A full run takes about 5 seconds. Running it without the two file paths prints the usage line.

Options go after the two file paths, and all of them are optional:

| Option | Default | Meaning |
| --- | --- | --- |
| `--corpus-cols` | `1,2` | Corpus columns that hold the text, counted from 0 (AG News title and description) |
| `--corpus-header` | `no` | `yes` if the corpus CSV's first row is a header |
| `--bench-cols` | `0` | Benchmark column that holds the question |
| `--bench-header` | `no` | `yes` if the benchmark CSV's first row is a header |
| `--plant` | `100` | How many questions to plant |
| `--seed` | `42` | Random seed for planting |
| `--runs` | `5` | How many timed runs; the median is reported |

Examples:

```
./detector data/full/ag_news_train.csv data/full/mmlu_test.csv --seed 7 --runs 1
./detector data/full/ag_news_train.csv data/full/mmlu_test.csv --plant 0
```

The first plants a different set of questions. All 100 are still found. The second plants nothing and finds 0 questions, which shows that AG News contains no MMLU questions on its own.

## Example output

```
Data
  Documents: 120000   Corpus words: 4765177
  Questions: 14042    Shorter than 13 words: 4104   Planted: 100

Result
  994 benchmark questions were found in the training set
  (includes the 100 planted test questions)

Performance (median of 5 runs)
  Structure      Build (ms)   Lookup (ms)   Memory (bytes)   Flagged
  Hash table     367.465          61.0203           26601420              994

Correctness
  Planted found: 100/100
  Planted missed: none
  Unplanted hits: 894 (first 20 shown)
    ...
```

Times vary from machine to machine. The 894 unplanted hits are MMLU questions that weren't planted but share at least 13 words with one that was. Most are moral scenarios questions, which all open with the same sentence. The report has the full breakdown.

## Code layout

| Path | Contents |
| --- | --- |
| `app/main.cpp` | The command-line program: reads the options, runs everything in order, prints the results |
| `src/contamination.h` | Shared constants (the 13-word window, −1 for the end of a document, −2 for an unknown word) and the `Structure` base class |
| `src/pipeline/` | CSV reader, text cleanup and word numbering, question planting, and `runPipeline`, which runs them in order |
| `src/structures/hash_table.*` | The hash table |
| `src/structures/suffix_array.*` | Empty: the suffix array was not implemented |
| `src/evaluation/` | Question checker, timing and medians, correctness check |
| `1. project-plan/` | The original proposal and the design documents |

## Data sources

- AG News: Zhang, X., Zhao, J., & LeCun, Y. (2015). Character-level convolutional networks for text classification. *NeurIPS 28.* Downloaded from the fast.ai mirror linked above.
- MMLU: Hendrycks, D., et al. (2021). Measuring massive multitask language understanding. *ICLR 2021.*
