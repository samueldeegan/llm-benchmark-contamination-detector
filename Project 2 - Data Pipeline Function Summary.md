## **Data Pipeline**

AG news training set: [https\://s3.amazonaws.com/fast-ai-nlp/ag\_news\_csv.tgz](https://s3.amazonaws.com/fast-ai-nlp/ag_news_csv.tgz)   
MMLU Benchmark: [https\://people.eecs.berkeley.edu/\~hendrycks/data.tar](https://people.eecs.berkeley.edu/~hendrycks/data.tar) 

**Step 1: Get the data files**

* **Training corpus:** AG News training set (`train.csv`, 120,000 rows). Columns: class index, title, description. Source: fast.ai mirror.  
* **Benchmark:** MMLU test set (14,042 questions). Columns: question, A, B, C, D, answer, no header. Source: the official MMLU download, combined into one CSV.

**Step 2: Read and clean the benchmark**

* Read the benchmark CSV with the C++ CSV reader. It needs to handle quoted fields, doubled quotes, and line breaks inside quotes.  
* Keep only the question field.  
* Clean each question: lowercase everything, and replace every character that isn't a–z or 0–9 with a space.  
* Keep every cleaned question in a list, using its row number as its ID.

**Step 3: Pick the questions to plant**

* Count the words in each cleaned question.  
* Questions with 13 or more words are planting candidates. Count the ones with fewer than 13 for the report, since they can never be flagged.  
* Shuffle the candidates using a random generator with a fixed seed, and take the first 100\.

**Step 4: Read and clean the corpus**

* Read the training CSV. For each row, join the title, a space, and the description into one document.  
* Clean it with the same cleanup function.  
* Store all documents in a vector. You need the total count, N, before choosing where to plant.

**Step 5: Plant the questions**

* Pick 100 *distinct* random document numbers from 0 to N − 1, using the same fixed-seed generator.  
* Append each chosen question to the end of its document, with a space before it.  
* Record the answer key: each planted question's ID and the document number it went into.

**Step 6: Build the word table and corpus list**

* For each word in each document, look it up in the word table. If it's new, give it the next ID, starting at 0\.  
* Append each word's ID to the vector `corpus`.  
* At the end of each document, append −1 to mark the boundary.

**Step 7: Convert the questions to numbers**

* For each cleaned question, look up each word in the same word table.  
* Words that aren't in the table get an "unknown" ID, such as −2, which can never match anything.