
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "stemmer.h"
#include <iomanip>
#include <algorithm>

using namespace std;

// Function Prototypes
vector<vector<string>> read_tweets_csv_file();
vector<string> readEmotionFile(string path);
vector<string> getUniqueSenators(const vector<vector<string>>& tweets);
string getParty(string senator);
void calculateSentiment(const vector<vector<string>>& tweets, const vector<string>& senators, const vector<string>& positiveWords, const vector<string>& negativeWords);
void findMostTalkative(const vector<vector<string>>& tweets, const vector<string>& senators);
void analyzeBidenSentiment(const vector<vector<string>>& tweets, const vector<string>& positiveWords, const vector<string>& negativeWords);
void analyzePoliticalAlignment(const vector<vector<string>>& tweets, const vector<string>& senators);

int main() {
    cout << "Reading data files..." << endl;
    vector<vector<string>> tweets = read_tweets_csv_file();
    vector<string> positiveWords = readEmotionFile("positive-words.txt");
    vector<string> negativeWords = readEmotionFile("negative-words.txt");
    vector<string> senators = getUniqueSenators(tweets);

    cout << "Data loaded." << endl;
    cout << "Tweets: " << tweets.size() << endl;
    cout << "Positive Words: " << positiveWords.size() << endl;
    cout << "Negative Words: " << negativeWords.size() << endl;
    cout << "Senators: " << senators.size() << endl;
    cout << endl;

    // Part 1: Sentiment Analysis
    cout << "--- Part 1: Sentiment Analysis ---" << endl;
    calculateSentiment(tweets, senators, positiveWords, negativeWords);
    cout << endl;

    // Part 2: Two Capabilities
    cout << "--- Part 2: Additional Capabilities ---" << endl;
    
    // Capability 1: Most Talkative Senator
    cout << "1. Most Talkative Senator:" << endl;
    findMostTalkative(tweets, senators);
    cout << endl;

    // Capability 2: Biden Sentiment
    cout << "2. Biden Sentiment Analysis:" << endl;
    analyzeBidenSentiment(tweets, positiveWords, negativeWords);
    cout << endl;

    // Extra Credit
    cout << "--- Extra Credit: Political Alignment Analysis ---" << endl;
    analyzePoliticalAlignment(tweets, senators);

    return 0;
}

// Reads the tweets CSV file into a 2D vector
vector<vector<string>> read_tweets_csv_file() {
    vector<vector<string>> tweets;
    fstream fin;
    fin.open("tweets.csv", ios::in);
    if (!fin.is_open()) {
        cerr << "Error: Could not open tweets.csv" << endl;
        return tweets;
    }

    string line, word;
    vector<string> row;
    
    // Skip header
    getline(fin, line);

    while (getline(fin, line)) {
        row.clear();
        stringstream s(line);
        while (getline(s, word, '|')) {
            row.push_back(word);
        }
        // Ensure we have all columns (ID, UserID, Date, Senator, Text)
        if (row.size() >= 5) {
            tweets.push_back(row);
        }
    }
    fin.close();
    return tweets;
}

// Reads an emotion word file into a vector
vector<string> readEmotionFile(string path) {
    vector<string> words;
    fstream fin;
    fin.open(path, ios::in);
    if (!fin.is_open()) {
        cerr << "Error: Could not open " << path << endl;
        return words;
    }

    string word;
    while (fin >> word) {
        words.push_back(word);
    }
    fin.close();
    return words;
}

// Returns a vector of unique senator names
vector<string> getUniqueSenators(const vector<vector<string>>& tweets) {
    vector<string> senators;
    for (const auto& row : tweets) {
        if (row.size() >= 4) {
            string name = row[3];
            bool found = false;
            for (const string& s : senators) {
                if (s == name) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                senators.push_back(name);
            }
        }
    }
    // Sort for consistent output
    sort(senators.begin(), senators.end());
    return senators;
}

// Helper to determine party affiliation
string getParty(string senator) {
    // Hardcoded mapping based on known affiliations of the senators in the dataset
    if (senator == "Dan Sullivan" || senator == "Cory Gardner" || senator == "Jeff Flake" || 
        senator == "John Boozman" || senator == "Jon Kyl" || senator == "Lisa Murkowski" || 
        senator == "Richard Shelby" || senator == "Tom Cotton") {
        return "Republican";
    } else {
        return "Democrat";
    }
}

// Part 1: Calculates and prints sentiment percentages
void calculateSentiment(const vector<vector<string>>& tweets, const vector<string>& senators, const vector<string>& positiveWords, const vector<string>& negativeWords) {
    // Pre-stem emotion words for efficiency
    vector<string> stemmedPositive;
    vector<string> stemmedNegative;

    for (const string& w : positiveWords) {
        stemmedPositive.push_back(stemString(w));
    }
    for (const string& w : negativeWords) {
        stemmedNegative.push_back(stemString(w));
    }

    // Sort for binary search
    sort(stemmedPositive.begin(), stemmedPositive.end());
    sort(stemmedNegative.begin(), stemmedNegative.end());

    cout << left << setw(20) << "Senator" << right << setw(15) << "Positive %" << setw(15) << "Negative %" << endl;
    cout << string(50, '-') << endl;

    for (const string& senator : senators) {
        int totalWords = 0;
        int posCount = 0;
        int negCount = 0;

        for (const auto& row : tweets) {
            if (row[3] == senator) {
                stringstream ss(row[4]);
                string word;
                while (ss >> word) {
                    string stemmed = stemString(word);
                    totalWords++;

                    if (binary_search(stemmedPositive.begin(), stemmedPositive.end(), stemmed)) {
                        posCount++;
                    }
                    if (binary_search(stemmedNegative.begin(), stemmedNegative.end(), stemmed)) {
                        negCount++;
                    }
                }
            }
        }

        double posPct = (totalWords > 0) ? (double)posCount / totalWords * 100.0 : 0.0;
        double negPct = (totalWords > 0) ? (double)negCount / totalWords * 100.0 : 0.0;

        cout << left << setw(20) << senator << right << setw(15) << fixed << setprecision(5) << posPct << setw(15) << negPct << endl;
    }
}

// Part 2 - Capability 1: Most Talkative Senator
void findMostTalkative(const vector<vector<string>>& tweets, const vector<string>& senators) {
    string mostTweetsSenator;
    int maxTweets = -1;

    string mostWordsSenator;
    double maxAvgWords = -1.0;

    cout << left << setw(20) << "Senator" << right << setw(15) << "Tweet Count" << setw(20) << "Avg Words/Tweet" << endl;

    for (const string& senator : senators) {
        int tweetCount = 0;
        int totalWords = 0;

        for (const auto& row : tweets) {
            if (row[3] == senator) {
                tweetCount++;
                stringstream ss(row[4]);
                string word;
                while (ss >> word) {
                    totalWords++;
                }
            }
        }

        double avgWords = (tweetCount > 0) ? (double)totalWords / tweetCount : 0.0;

        cout << left << setw(20) << senator << right << setw(15) << tweetCount << setw(20) << fixed << setprecision(2) << avgWords << endl;

        if (tweetCount > maxTweets) {
            maxTweets = tweetCount;
            mostTweetsSenator = senator;
        }
        if (avgWords > maxAvgWords) {
            maxAvgWords = avgWords;
            mostWordsSenator = senator;
        }
    }

    cout << endl;
    cout << "Most tweets: " << mostTweetsSenator << " (" << maxTweets << ")" << endl;
    cout << "Highest average word count: " << mostWordsSenator << " (" << maxAvgWords << ")" << endl;
}

// Part 2 - Capability 2: Biden Sentiment
void analyzeBidenSentiment(const vector<vector<string>>& tweets, const vector<string>& positiveWords, const vector<string>& negativeWords) {
    // Pre-stem emotion words
    vector<string> stemmedPositive;
    vector<string> stemmedNegative;

    for (const string& w : positiveWords) {
        stemmedPositive.push_back(stemString(w));
    }
    for (const string& w : negativeWords) {
        stemmedNegative.push_back(stemString(w));
    }
    sort(stemmedPositive.begin(), stemmedPositive.end());
    sort(stemmedNegative.begin(), stemmedNegative.end());

    int totalWords = 0;
    int posCount = 0;
    int negCount = 0;
    int bidenTweetCount = 0;

    for (const auto& row : tweets) {
        string text = row[4];
        // Simple case-insensitive check for "Biden"
        string lowerText = text;
        transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        
        if (lowerText.find("biden") != string::npos) {
            bidenTweetCount++;
            stringstream ss(text);
            string word;
            while (ss >> word) {
                string stemmed = stemString(word);
                totalWords++;
                if (binary_search(stemmedPositive.begin(), stemmedPositive.end(), stemmed)) {
                    posCount++;
                }
                if (binary_search(stemmedNegative.begin(), stemmedNegative.end(), stemmed)) {
                    negCount++;
                }
            }
        }
    }

    cout << "Found " << bidenTweetCount << " tweets mentioning Biden." << endl;
    if (totalWords > 0) {
        double posPct = (double)posCount / totalWords * 100.0;
        double negPct = (double)negCount / totalWords * 100.0;
        cout << "Positive %: " << posPct << endl;
        cout << "Negative %: " << negPct << endl;
        
        if (posPct > negPct) {
            cout << "Conclusion: Tweets about Biden are mostly POSITIVE." << endl;
        } else if (negPct > posPct) {
            cout << "Conclusion: Tweets about Biden are mostly NEGATIVE." << endl;
        } else {
            cout << "Conclusion: Tweets about Biden are NEUTRAL." << endl;
        }
    } else {
        cout << "No words found in Biden tweets." << endl;
    }
}

// Extra Credit: Political Alignment Analysis
void analyzePoliticalAlignment(const vector<vector<string>>& tweets, const vector<string>& senators) {
    // 1. Build Vocabulary and Counts
    vector<string> vocab;
    vector<int> repCounts;
    vector<int> demCounts;
    
    // Common stop words to filter out
    vector<string> stopWords;
    stopWords.push_back("the"); stopWords.push_back("is"); stopWords.push_back("and"); stopWords.push_back("to");
    stopWords.push_back("of"); stopWords.push_back("a"); stopWords.push_back("in"); stopWords.push_back("for");
    stopWords.push_back("on"); stopWords.push_back("with"); stopWords.push_back("at"); stopWords.push_back("by");
    stopWords.push_back("from"); stopWords.push_back("up"); stopWords.push_back("about"); stopWords.push_back("into");
    stopWords.push_back("over"); stopWords.push_back("after"); stopWords.push_back("this"); stopWords.push_back("that");
    stopWords.push_back("it"); stopWords.push_back("are"); stopWords.push_back("was"); stopWords.push_back("be");
    stopWords.push_back("has"); stopWords.push_back("have"); stopWords.push_back("will"); stopWords.push_back("as");
    stopWords.push_back("an"); stopWords.push_back("or"); stopWords.push_back("but"); stopWords.push_back("not");
    stopWords.push_back("no"); stopWords.push_back("we"); stopWords.push_back("our"); stopWords.push_back("us");
    stopWords.push_back("my"); stopWords.push_back("i"); stopWords.push_back("you"); stopWords.push_back("your");
    stopWords.push_back("he"); stopWords.push_back("she"); stopWords.push_back("they"); stopWords.push_back("their");
    stopWords.push_back("his"); stopWords.push_back("her"); stopWords.push_back("rt"); stopWords.push_back("amp");

    cout << "Building political term list from tweets..." << endl;

    for (const auto& row : tweets) {
        if (row.size() < 5) continue;
        string party = getParty(row[3]);
        string text = row[4];
        
        stringstream ss(text);
        string word;
        while (ss >> word) {
            string cleanWord = "";
            for (char c : word) {
                if (isalpha(c)) cleanWord += tolower(c);
            }
            if (cleanWord.length() < 3) continue; 

            bool isStop = false;
            for (const string& sw : stopWords) {
                if (cleanWord == sw) {
                    isStop = true;
                    break;
                }
            }
            if (isStop) continue;

            int index = -1;
            for (size_t i = 0; i < vocab.size(); ++i) {
                if (vocab[i] == cleanWord) {
                    index = i;
                    break;
                }
            }

            if (index == -1) {
                vocab.push_back(cleanWord);
                repCounts.push_back(0);
                demCounts.push_back(0);
                index = vocab.size() - 1;
            }

            if (party == "Republican") {
                repCounts[index]++;
            } else {
                demCounts[index]++;
            }
        }
    }

    // 2. Identify Key Terms
    vector<string> keyTerms;
    vector<string> keyTermParty; 
    
    // Find top Republican words
    for (int k = 0; k < 15; ++k) {
        int maxDiff = -1;
        int bestIdx = -1;
        for (size_t i = 0; i < vocab.size(); ++i) {
            int diff = repCounts[i] - demCounts[i];
            bool picked = false;
            for(const string& s : keyTerms) if(s == vocab[i]) picked = true;
            
            if (!picked && diff > maxDiff && (repCounts[i] + demCounts[i] > 5)) {
                maxDiff = diff;
                bestIdx = i;
            }
        }
        if (bestIdx != -1) {
            keyTerms.push_back(vocab[bestIdx]);
            keyTermParty.push_back("Republican");
        }
    }

    // Find top Democrat words
    for (int k = 0; k < 15; ++k) {
        int maxDiff = -1;
        int bestIdx = -1;
        for (size_t i = 0; i < vocab.size(); ++i) {
            int diff = demCounts[i] - repCounts[i]; 
            bool picked = false;
            for(const string& s : keyTerms) if(s == vocab[i]) picked = true;

            if (!picked && diff > maxDiff && (repCounts[i] + demCounts[i] > 5)) {
                maxDiff = diff;
                bestIdx = i;
            }
        }
        if (bestIdx != -1) {
            keyTerms.push_back(vocab[bestIdx]);
            keyTermParty.push_back("Democrat");
        }
    }

    cout << "Identified Key Political Terms:" << endl;
    for(size_t i=0; i<keyTerms.size(); ++i) {
        cout << left << setw(20) << keyTerms[i] << keyTermParty[i] << endl;
    }
    cout << endl;

    // 3. Analyze Tweets
    cout << "Analyzing tweets for alignment..." << endl;
    
    int correctPredictions = 0;
    int totalPredictions = 0;
    
    vector<int> senCorrect(senators.size(), 0);
    vector<int> senTotal(senators.size(), 0);

    for (const auto& row : tweets) {
        if (row.size() < 5) continue;
        string actualParty = getParty(row[3]);
        string text = row[4];
        
        vector<string> tweetWords;
        stringstream ss(text);
        string word;
        while(ss >> word) {
             string cleanWord = "";
            for (char c : word) {
                if (isalpha(c)) cleanWord += tolower(c);
            }
            tweetWords.push_back(cleanWord);
        }

        double score = 0;
        int termsFound = 0;

        for (size_t i = 0; i < tweetWords.size(); ++i) {
            string w = tweetWords[i];
            
            int termIdx = -1;
            for(size_t k=0; k<keyTerms.size(); ++k) {
                if (keyTerms[k] == w) {
                    termIdx = k;
                    break;
                }
            }

            if (termIdx != -1) {
                double termVal = (keyTermParty[termIdx] == "Republican") ? 1.0 : -1.0;
                
                // Check negation (window +/- 2)
                bool negated = false;
                // int is a type cast to avoid signed/unsigned comparison warnings
                int start = (int)i - 2;
                int end = (int)i + 2;
                if (start < 0) start = 0;
                if (end >= (int)tweetWords.size()) end = (int)tweetWords.size() - 1;

                for (int j = start; j <= end; ++j) {
                    if (j == (int)i) continue;
                    if (tweetWords[j] == "not" || tweetWords[j] == "no" || tweetWords[j] == "never") {
                        negated = true;
                    }
                }

                if (negated) termVal *= -1;
                
                score += termVal;
                termsFound++;
            }
        }

        if (termsFound > 0) {
            string predicted = (score > 0) ? "Republican" : (score < 0) ? "Democrat" : "Neutral";
            if (predicted != "Neutral") {
                totalPredictions++;
                if (predicted == actualParty) correctPredictions++;
                
                // Update senator stats
                for(size_t s=0; s<senators.size(); ++s) {
                    if (senators[s] == row[3]) {
                        senTotal[s]++;
                        if (predicted == actualParty) senCorrect[s]++;
                        break;
                    }
                }
            }
        }
    }

    cout << "Alignment Analysis Results by Senator:" << endl;
    cout << left << setw(20) << "Senator" << setw(15) << "Party" << setw(15) << "Accuracy" << endl;
    cout << string(50, '-') << endl;
    
    for(size_t s=0; s<senators.size(); ++s) {
        double acc = (senTotal[s] > 0) ? (double)senCorrect[s] / senTotal[s] * 100.0 : 0.0;
        cout << left << setw(20) << senators[s] << setw(15) << getParty(senators[s]) << fixed << setprecision(1) << acc << "% (" << senCorrect[s] << "/" << senTotal[s] << ")" << endl;
    }
    cout << endl;
    
    cout << "Overall Accuracy: " << (totalPredictions > 0 ? (double)correctPredictions / totalPredictions * 100.0 : 0.0) << "%" << endl;
}

/*
*   The createPoliticalWordFile function was originally used to generate a file listing every word from the tweets along with the senator’s party. 
*   This was part of a more detailed word-by-word analysis to infer political alignment. However, with a much larger dataset in mind, this approach 
*   becomes less practical and more resource-intensive. 
*
*   Instead, we're now using a more streamlined approach. We filter out common stop words and focus on a predefined set of 
*   key political terms (like party names and political figures). This makes the alignment detection more efficient and scalable for a large volume of tweets.
*
*   As a result, we no longer need to write each word to a file for this analysis. The createPoliticalWordFile function is being commented out 
*   to reflect this updated approach. We are now directly analyzing tweets using the key terms without generating an intermediate file.
*
*
void createPoliticalWordFile(const vector<vector<string>>& tweets) {
    ofstream fout("political-alignment.txt");
    if (!fout.is_open()) {
        cerr << "Error: Could not create political-alignment.txt" << endl;
        return;
    }

    for (const auto& row : tweets) {
        if (row.size() >= 5) {
            string senator = row[3];
            string party = getParty(senator);
            string text = row[4];
            
            stringstream ss(text);
            string word;
            while (ss >> word) {
                // Write word and party to file
                // Format: word party
                fout << word << " " << party << endl;
            }
        }
    }
    fout.close();
}
*/