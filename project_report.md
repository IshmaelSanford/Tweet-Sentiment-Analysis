# CS 101 Project 3: Sentiment Analysis & Political Alignment Report

**Date:** December 9, 2025  
**Author:** Ishmael Sanford

---

## 1. Design Document

### 1.1 Overview
This project implements a sentiment analysis tool for US Senator tweets. It analyzes the emotional tone (positive/negative) of tweets, identifies the most talkative senators, determines the sentiment towards "Joe Biden", and predicts the political alignment (Republican/Democrat) of tweets based on key terms.

## 2. Design Choices & Constraints

### 2.1 Assignment Constraints
The project follows these rules:

- **No classes / objects / structs in the implementation**
- **No STL associative containers**  
  - No `std::map`, `std::set`, `unordered_map`, etc.
- **No pointers / dynamic allocation**  
  - No `new` / `delete`.
- **Only allowed headers**:  
  `<iostream>`, `<fstream>`, `<sstream>`, `<vector>`, `<string>`, `<iomanip>`, `<algorithm>`, `<cctype>`, and the provided `stemmer.h`.

Because maps and structs are not allowed, I rely on:

- **2D vectors** to represent rows of CSV data.
- **Parallel vectors** to simulate “records” for the extra credit (e.g., `vocab[i]`, `repCounts[i]`, `demCounts[i]` all describe the same word).

All code is placed in a single file: `project3_student.cpp` (or `main.cpp`).

### 1.3 Data Structures
*   **Tweets**: Stored as a `vector<vector<string>>`. Each inner vector represents a row (tweet), and indices correspond to columns (0:ID, 3:Senator, 4:Text).
*   **Word Lists**: Positive and negative words are stored in `vector<string>`.
*   **Vocabulary**: For the political alignment task, unique words are stored in a `vector<string>`, with parallel `vector<int>`s for Republican and Democrat frequency counts.

### 1.4 Algorithms

#### Sentiment Analysis (Part 1)
1.  **Stemming**: The Porter Stemmer (`stemmer.h`) is used to reduce words to their root form (e.g., "running" -> "run"). This ensures that "vote" and "voting" are treated as the same word.
2.  **Binary Search**: To optimize performance, the positive and negative word lists are sorted. `std::binary_search` (O(log N)) is used instead of linear search (O(N)) to check if a word exists in the lists.

#### Political Alignment (Extra Credit)
1.  **Training (Term Identification)**:
    *   The program iterates through all tweets to build a frequency map (using parallel vectors) for every word, separated by party.
    *   **Stop Word Filtering**: Common English words ("the", "is", "and") are ignored to focus on meaningful content.
    *   **Key Term Selection**: The top 15 words with the highest frequency difference between parties are selected as "Key Terms".
2.  **Prediction**:
    *   Each tweet is scored based on the presence of these key terms (+1 for Republican term, -1 for Democrat term).
    *   **Negation Handling**: A window of +/- 2 words is checked for negation terms ("not", "no", "never"). If found, the term's score is flipped.
    *   **Classification**: Positive score -> Republican, Negative score -> Democrat.

---

## 2. Results Analysis

### 2.1 Sentiment Analysis
The following table shows the percentage of positive and negative words used by each senator.

| Senator | Positive % | Negative % |
| :--- | :--- | :--- |
| Christopher Murphy | 7.09877 | 3.35097 |
| Cory Gardner | 12.11039 | 2.46753 |
| Dan Sullivan | 7.24876 | 4.28336 |
| Dianne Feinstein | 9.21279 | 5.70643 |
| ... | ... | ... |
| **Richard Blumenthal** | **14.02656** | **5.76612** |

*   **Most Positive**: Richard Blumenthal (14.02%)
*   **Most Negative**: Tom Cotton (6.96%)

### 2.2 Additional Capabilities
1.  **Most Talkative Senator**:
    *   **By Tweet Count**: John Boozman (100 tweets)
    *   **By Word Count**: Michael Bennet (37.72 words/tweet)
2.  **Biden Sentiment**:
    *   Tweets mentioning "Biden" were found to be **Mostly POSITIVE** (7.55% Positive vs 5.92% Negative).

### 2.3 Political Alignment (Extra Credit)
The algorithm successfully identified distinct political vocabularies:

*   **Republican Terms**: *biden, alaska, energy, prices, gas, veterans*
*   **Democrat Terms**: *gun, health, violence, care, assault, action*

**Accuracy Results**:
*   **Overall Accuracy**: **83.3%**
*   **Best Prediction**: Dan Sullivan (94.4%)
*   **Worst Prediction**: Cory Gardner (53.3%)

The high accuracy (83.3%) demonstrates that simple keyword frequency analysis combined with basic negation handling is surprisingly effective at detecting political affiliation in this dataset.

---

## 3. Code Walkthrough

### `main()`
The entry point orchestrates the program flow:
1.  Calls `read_tweets_csv_file` to load data.
2.  Calls `readEmotionFile` to load sentiment dictionaries.
3.  Executes Part 1 (`calculateSentiment`).
4.  Executes Part 2 (`findMostTalkative`, `analyzeBidenSentiment`).
5.  Executes Extra Credit (`analyzePoliticalAlignment`).

### `calculateSentiment()`
*   **Input**: Tweets vector, senator list, positive/negative word lists.
*   **Process**:
    1.  Stems all positive/negative words and sorts them.
    2.  Iterates through tweets for each senator.
    3.  Stems every word in the tweet.
    4.  Uses `binary_search` to count matches.
    5.  Calculates percentages: `(count / total_words) * 100`.

### `analyzePoliticalAlignment()`

This function implements a sophisticated, multi-stage algorithm to predict the political alignment of tweets. It operates without external machine learning libraries, relying instead on statistical frequency analysis and rule-based sentiment logic.

#### **Phase 1: Vocabulary Building & Feature Extraction**
The first step is to "learn" the language of each political party from the dataset itself.

1.  **Data Collection**: The function iterates through every tweet in the dataset.
2.  **Preprocessing**:
    *   **Tokenization**: Each tweet is split into individual words.
    *   **Cleaning**: Non-alphabetic characters are removed, and all text is converted to lowercase to ensure case-insensitive matching (e.g., "Biden" and "biden" are treated identically).
    *   **Stop Word Filtering**: A custom list of 40+ common English stop words (e.g., "the", "is", "and", "rt") is used to filter out noise. This ensures the algorithm focuses on content-rich words.
    
    ```cpp
    // Code Snippet: Stop Word Filtering
    bool isStop = false;
    for (const string& sw : stopWords) {
        if (cleanWord == sw) {
            isStop = true;
            break;
        }
    }
    if (isStop) continue;
    ```
    *Explanation*: This loop checks if the current `cleanWord` exists in our `stopWords` vector. If it does, we skip it (`continue`), ensuring we don't analyze common words like "the" or "and".

3.  **Frequency Counting**:
    *   Three parallel vectors are maintained: `vocab` (the word), `repCounts` (frequency in Republican tweets), and `demCounts` (frequency in Democrat tweets).
    *   For every valid word found, its corresponding counter is incremented based on the author's party affiliation.

#### **Phase 2: Feature Selection (Key Term Identification)**
Once the vocabulary is built, the algorithm identifies the most "partisan" words.

1.  **Difference Calculation**: For every word in the vocabulary, a "partisan score" is calculated as the difference between its usage by the two parties:
    *   `Republican Score = repCount - demCount`
    *   `Democrat Score = demCount - repCount`
2.  **Selection Logic**:
    *   The algorithm selects the top 15 words with the highest Republican Score.
    *   It then selects the top 15 words with the highest Democrat Score.
    *   **Thresholding**: To avoid outliers from very rare words, a word must appear at least 5 times in total to be considered.

    ```cpp
    // Code Snippet: Finding Top Republican Words
    for (int k = 0; k < 15; ++k) {
        int maxDiff = -1;
        int bestIdx = -1;
        for (size_t i = 0; i < vocab.size(); ++i) {
            int diff = repCounts[i] - demCounts[i];
            // ... (check if already picked) ...
            if (!picked && diff > maxDiff && (repCounts[i] + demCounts[i] > 5)) {
                maxDiff = diff;
                bestIdx = i;
            }
        }
        // ... (add bestIdx to keyTerms) ...
    }
    ```
    *Explanation*: This nested loop finds the 15 words that are most uniquely Republican. The outer loop runs 15 times (once for each term we want to find). The inner loop scans the entire vocabulary to find the unselected word with the highest `repCounts - demCounts` difference.

3.  **Result**: This process automatically generates a list of 30 key political terms (e.g., "energy" for Republicans, "health" for Democrats) without any manual hardcoding.

#### **Phase 3: Classification & Negation Handling**
With the key terms identified, the function re-scans the tweets to predict their alignment.

1.  **Scoring**:
    *   Each tweet starts with a `score` of 0.
    *   If a **Republican key term** is found, +1 is added to the score.
    *   If a **Democrat key term** is found, -1 is added to the score.
2.  **Context-Aware Negation**:
    *   Simple keyword matching can be misleading (e.g., "not a conservative").
    *   To solve this, the algorithm implements a **sliding window context check**.
    *   When a key term is found at index `i`, the algorithm checks the words in the range `[i-2, i+2]`.
    *   If a negation word ("not", "no", "never") is found within this window, the term's value is flipped (e.g., +1 becomes -1).

    ```cpp
    // Code Snippet: Negation Window Check
    bool negated = false;
    int start = (int)i - 2;
    int end = (int)i + 2;
    // ... (bounds checking) ...

    for (int j = start; j <= end; ++j) {
        if (j == (int)i) continue;
        if (tweetWords[j] == "not" || tweetWords[j] == "no" || tweetWords[j] == "never") {
            negated = true;
        }
    }

    if (negated) termVal *= -1;
    ```
    *Explanation*: This is the core of the negation logic. We define a window from 2 words before (`i-2`) to 2 words after (`i+2`) the current key term. We loop through this window, and if we find any negation words, we set the `negated` flag to true. Finally, we multiply the term's value by -1, effectively flipping its political contribution.

3.  **Prediction**:
    *   `Score > 0`: Predicted **Republican**.
    *   `Score < 0`: Predicted **Democrat**.
    *   `Score == 0`: Predicted **Neutral** (no prediction made).

#### **Phase 4: Evaluation & Metrics**
Finally, the predictions are validated against the ground truth (the actual party of the senator).

1.  **Accuracy Calculation**:
    *   The algorithm tracks `correctPredictions` and `totalPredictions`.
    *   It also maintains per-senator statistics to identify which senators are easiest or hardest to classify.
2.  **Output**:
    *   The function prints the identified key terms, the accuracy per senator, and the overall system accuracy to the console.

This approach demonstrates how statistical analysis combined with basic linguistic rules (negation handling) can achieve high classification accuracy (83.3%) on a complex task like political alignment detection, all within the constraints of a CS101 project.
