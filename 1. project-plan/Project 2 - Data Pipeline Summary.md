## **Data Pipeline**

AG news training set: [https\://s3.amazonaws.com/fast-ai-nlp/ag\_news\_csv.tgz](https://s3.amazonaws.com/fast-ai-nlp/ag_news_csv.tgz)
MMLU Benchmark: [https\://people.eecs.berkeley.edu/\~hendrycks/data.tar](https://people.eecs.berkeley.edu/~hendrycks/data.tar)

Turns the two CSV files into numbers the structures can use. Files:

| File | Functions |
| ----- | ----- |
| `src/csv_reader.h/.cpp` | `readCsv`, `extractText` |
| `src/text_processing.h/.cpp` | `cleanText`, `splitWords`, `buildCorpus`, `questionsToIds` |
| `src/planting.h/.cpp` | `pickQuestionsToPlant`, `plantQuestions` |
| `src/pipeline.h/.cpp` | `runPipeline`, plus the `PipelineSettings` and `PipelineOutput` bundles |

**What we know about the files (checked Nov 5)**

| Fact | Evidence |
| ----- | ----- |
| AG News `train.csv`: 120,000 rows, 3 columns (class index, title, description), every field in double quotes, **no header row** | Row count and first line of a copy of the same release; the dataset's own readme |
| AG News has no line breaks inside fields. Line breaks in the original text appear as a lone backslash (`dwindling\band`), 26,977 of them. Cleanup turns the backslash into a space. | Counted in the same copy |
| AG News has leftover web codes: `#39;` (an apostrophe) 44,316 times, `quot;` 9,596 times, `#36;` (a dollar sign) 1,307 times. Cleanup turns them into stray words like "39" and "quot". | Counted in the same copy |
| AG News has no characters outside plain ASCII. | Checked every row |
| The cleaned AG News corpus has about 4.76 million words (about 40 per document, at most 183). | Counted with the cleanup rule below |
| MMLU test: 57 subject files, columns question, A, B, C, D, answer, **no header row**. 14,042 questions. | The official MMLU script reads them with `header=None`; counted in a copy of the files |
| The joined MMLU file has 18,738 lines but 14,042 rows. 1,127 questions have line breaks inside their quotes. | Counted with a CSV-aware reader |
| 18 of the 57 MMLU files end each line with `\r\n` (Windows style); the rest use `\n`. | Checked each file |
| 9,938 MMLU questions have 13 or more words after cleanup; 4,104 are shorter. | Counted with the cleanup rule below |

**Step 1: Get the data files**

Done once, by hand, in the codespace terminal. The big files go in `data/raw/`, which is in `.gitignore` and never committed.

```
mkdir -p data/raw && cd data/raw
curl -L -O https://s3.amazonaws.com/fast-ai-nlp/ag_news_csv.tgz
curl -L -O https://people.eecs.berkeley.edu/~hendrycks/data.tar
tar -xzf ag_news_csv.tgz
tar -xf data.tar
awk 1 data/test/*_test.csv > mmlu_test.csv
```

* **Training corpus:** `data/raw/ag_news_csv/train.csv`.
* **Benchmark:** `data/raw/mmlu_test.csv`. `awk 1` joins the 57 files in alphabetical order by subject, adding a line break at the end of any file missing one. Question IDs depend on this order, so everyone uses this same command.

**Step 2: Read and clean the benchmark (`readCsv`, `extractText`, `cleanText`)**

* `readCsv` reads the benchmark CSV. It needs to handle quoted fields, doubled quotes, line breaks inside quotes, and `\r\n` at the end of a row. Treat `\r\n` as a normal row end, or a stray `\r` sticks to the last field.
* `extractText` keeps only the question field (column 0). It never drops a row.
* `cleanText` cleans each question: lowercase everything, and replace every character that isn't a–z or 0–9 with a space.
* Keep every cleaned question in a list, using its row number as its ID.

**Step 3: Pick the questions to plant (`pickQuestionsToPlant`, `splitWords`)**

* Count the words in each cleaned question with `splitWords`.
* Questions with 13 or more words are planting candidates. Count the ones with fewer than 13 for the report, since they can never be flagged. This is the only place they're counted.
* Shuffle the candidates using a random generator with a fixed seed, and take the first 100.

**Step 4: Read and clean the corpus (`readCsv`, `extractText`, `cleanText`)**

* `readCsv` reads the training CSV. `extractText` joins the title (column 1), a space, and the description (column 2) into one document per row.
* Clean it with the same `cleanText`.
* Store all documents in a vector. You need the total count, N, before choosing where to plant.

**Step 5: Plant the questions (`plantQuestions`)**

* Pick 100 *distinct* random document numbers from 0 to N − 1, using the same fixed-seed generator.
* Append each chosen question to the end of its document, with a space before it.
* Record the answer key: each planted question's ID and the document number it went into.

**Step 6: Build the word table and corpus array (`buildCorpus`)**

* For each word in each document, look it up in the word table (a `std::unordered_map<std::string, int>`). If it's new, give it the next ID, starting at 0.
* Append each word's ID to the vector `corpus`.
* At the end of each document, append −1 to mark the boundary.

**Step 7: Convert the questions to numbers (`questionsToIds`)**

* For each cleaned question, look up each word in the same word table.
* Words that aren't in the table get the "unknown" ID, −2, which can never match anything.

**Step 8: Run it all in one call (`runPipeline`)**

* Takes the two file paths and the settings (which columns, header yes/no, how many to plant, seed).
* Makes the random generator from the seed, then runs Steps 2 to 7 in order. The same generator goes to Step 3 and then Step 5, which is what makes the same seed give the same plants every run.
* Gives back the pipeline output: the corpus array, the question lists, the answer key, the cleaned questions, and the counts (documents, corpus words, questions, questions under 13 words, questions planted).
* The word table stays inside. Nothing after Step 7 needs it.
* Every step above stays its own function, so each can be tested on its own.

**Tests to write**

* `readCsv` on a tiny CSV with a comma inside quotes, a doubled quote, a line break inside quotes, and a `\r\n` row end gives the right rows and fields.
* `readCsv` on the real files: 120,000 rows for AG News, 14,042 for MMLU.
* `cleanText` then `splitWords` on `It's $5, OK?` gives `it`, `s`, `5`, `ok`.
* `pickQuestionsToPlant` with the same seed twice gives the same IDs, and every chosen question has 13 or more words.
* `plantQuestions` never puts two questions in the same document.
* `buildCorpus` on "the cat sat on the mat" and "the cat ran" gives `0 1 2 3 0 4 -1 0 1 5 -1`.
* `questionsToIds` turns a word the corpus never has into −2.
* `runPipeline` on the real files gives 120,000 documents, 14,042 questions, 100 planted, and 4,104 under 13 words.
* End to end: a hash table built on `runPipeline`'s corpus returns true from `contains` for the first 13 words of every planted question.
