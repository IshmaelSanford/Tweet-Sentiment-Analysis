
#include <vector> // Include vector library for dynamic arrays
#include <string> // Include string library for string manipulation
#include <iostream> // Include iostream for input/output operations
#include <fstream> // Include fstream for file input/output
#include <sstream> // Include sstream for string stream processing
#include "stemmer.h" // Include custom stemmer header for word stemming
#include <iomanip> // Include iomanip for output formatting
#include <algorithm> // Include algorithm for sorting and searching

using namespace std; // Use the standard namespace to avoid std:: prefix

// Function Prototypes
vector<vector<string>> read_tweets_csv_file(); // Prototype for reading tweets from CSV
vector<string> readEmotionFile(string path); // Prototype for reading emotion words from file
vector<string> getUniqueSenators(const vector<vector<string>>& tweets); // Prototype for getting unique senator names
string getParty(string senator); // Prototype for determining senator's party
void calculateSentiment(const vector<vector<string>>& tweets, const vector<string>& senators, const vector<string>& positiveWords, const vector<string>& negativeWords); // Prototype for sentiment analysis
void findMostTalkative(const vector<vector<string>>& tweets, const vector<string>& senators); // Prototype for finding most talkative senator
void analyzeBidenSentiment(const vector<vector<string>>& tweets, const vector<string>& positiveWords, const vector<string>& negativeWords); // Prototype for analyzing Biden sentiment
void analyzePoliticalAlignment(const vector<vector<string>>& tweets, const vector<string>& senators); // Prototype for political alignment analysis

int main() { // Main function entry point
    cout << "Reading data files..." << endl; // Print status message
    vector<vector<string>> tweets = read_tweets_csv_file(); // Read tweets from CSV file
    vector<string> positiveWords = readEmotionFile("positive-words.txt"); // Read positive words from file
    vector<string> negativeWords = readEmotionFile("negative-words.txt"); // Read negative words from file
    vector<string> senators = getUniqueSenators(tweets); // Get list of unique senators

    cout << "Data loaded." << endl; // Print status message
    cout << "Tweets: " << tweets.size() << endl; // Print number of tweets loaded
    cout << "Positive Words: " << positiveWords.size() << endl; // Print number of positive words loaded
    cout << "Negative Words: " << negativeWords.size() << endl; // Print number of negative words loaded
    cout << "Senators: " << senators.size() << endl; // Print number of senators found
    cout << endl; // Print empty line

    // Part 1: Sentiment Analysis
    cout << "--- Part 1: Sentiment Analysis ---" << endl; // Print section header
    calculateSentiment(tweets, senators, positiveWords, negativeWords); // Perform sentiment analysis
    cout << endl; // Print empty line

    // Part 2: Two Capabilities
    cout << "--- Part 2: Additional Capabilities ---" << endl; // Print section header
    
    // Capability 1: Most Talkative Senator
    cout << "1. Most Talkative Senator:" << endl; // Print capability header
    findMostTalkative(tweets, senators); // Find and print most talkative senator
    cout << endl; // Print empty line

    // Capability 2: Biden Sentiment
    cout << "2. Biden Sentiment Analysis:" << endl; // Print capability header
    analyzeBidenSentiment(tweets, positiveWords, negativeWords); // Analyze and print Biden sentiment
    cout << endl; // Print empty line

    // Extra Credit
    cout << "--- Extra Credit: Political Alignment Analysis ---" << endl; // Print extra credit header
    analyzePoliticalAlignment(tweets, senators); // Perform political alignment analysis

    return 0; // Return 0 to indicate successful execution
}

// Reads the tweets CSV file into a 2D vector
vector<vector<string>> read_tweets_csv_file() { // Function definition
    vector<vector<string>> tweets; // Create a 2D vector to store tweets
    fstream fin; // Create file stream object
    fin.open("tweets.csv", ios::in); // Open tweets.csv in input mode
    if (!fin.is_open()) { // Check if file opened successfully
        cerr << "Error: Could not open tweets.csv" << endl; // Print error message if failed
        return tweets; // Return empty vector
    }

    string line, word; // Declare string variables for line and word
    vector<string> row; // Declare vector to store a single row
    
    // Skip header
    getline(fin, line); // Read and discard the first line (header)

    while (getline(fin, line)) { // Loop through each line in the file
        row.clear(); // Clear the row vector for the new line
        stringstream s(line); // Create stringstream from the line
        while (getline(s, word, '|')) { // Split line by '|' delimiter
            row.push_back(word); // Add each column value to the row vector
        }
        // Ensure we have all columns (ID, UserID, Date, Senator, Text)
        if (row.size() >= 5) { // Check if row has enough columns
            tweets.push_back(row); // Add valid row to tweets vector
        }
    }
    fin.close(); // Close the file
    return tweets; // Return the populated tweets vector
}

// Reads an emotion word file into a vector
vector<string> readEmotionFile(string path) { // Function definition
    vector<string> words; // Create vector to store words
    fstream fin; // Create file stream object
    fin.open(path, ios::in); // Open file at 'path' in input mode
    if (!fin.is_open()) { // Check if file opened successfully
        cerr << "Error: Could not open " << path << endl; // Print error message if failed
        return words; // Return empty vector
    }

    string word; // Declare string variable for word
    while (fin >> word) { // Read words one by one from file
        words.push_back(word); // Add word to vector
    }
    fin.close(); // Close the file
    return words; // Return the populated words vector
}

// Returns a vector of unique senator names
vector<string> getUniqueSenators(const vector<vector<string>>& tweets) { // Function definition
    vector<string> senators; // Create vector to store unique senator names
    for (const auto& row : tweets) { // Loop through each tweet row
        if (row.size() >= 4) { // Check if row has enough columns
            string name = row[3]; // Get senator name from 4th column (index 3)
            bool found = false; // Flag to check if name already exists
            for (const string& s : senators) { // Loop through existing senators
                if (s == name) { // Check if name matches
                    found = true; // Set found flag to true
                    break; // Exit inner loop
                }
            }
            if (!found) { // If name was not found
                senators.push_back(name); // Add name to senators vector
            }
        }
    }
    // Sort for consistent output
    sort(senators.begin(), senators.end()); // Sort senator names alphabetically
    return senators; // Return the vector of unique senators
}

// Helper to determine party affiliation
string getParty(string senator) { // Function definition
    // Hardcoded mapping based on known affiliations of the senators in the dataset
    if (senator == "Dan Sullivan" || senator == "Cory Gardner" || senator == "Jeff Flake" || // Check for Republican senators
        senator == "John Boozman" || senator == "Jon Kyl" || senator == "Lisa Murkowski" || // Check for Republican senators
        senator == "Richard Shelby" || senator == "Tom Cotton") { // Check for Republican senators
        return "Republican"; // Return "Republican" if match found
    } else { // Otherwise
        return "Democrat"; // Return "Democrat" (assuming only two parties in dataset)
    }
}

// Part 1: Calculates and prints sentiment percentages
void calculateSentiment(const vector<vector<string>>& tweets, const vector<string>& senators, const vector<string>& positiveWords, const vector<string>& negativeWords) { // Function definition
    // Pre-stem emotion words for efficiency
    vector<string> stemmedPositive; // Vector for stemmed positive words
    vector<string> stemmedNegative; // Vector for stemmed negative words

    for (const string& w : positiveWords) { // Loop through positive words
        stemmedPositive.push_back(stemString(w)); // Stem and add to vector
    }
    for (const string& w : negativeWords) { // Loop through negative words
        stemmedNegative.push_back(stemString(w)); // Stem and add to vector
    }

    // Sort for binary search
    sort(stemmedPositive.begin(), stemmedPositive.end()); // Sort stemmed positive words
    sort(stemmedNegative.begin(), stemmedNegative.end()); // Sort stemmed negative words

    cout << left << setw(20) << "Senator" << right << setw(15) << "Positive %" << setw(15) << "Negative %" << endl; // Print table header
    cout << string(50, '-') << endl; // Print separator line

    for (const string& senator : senators) { // Loop through each senator
        int totalWords = 0; // Initialize total word count
        int posCount = 0; // Initialize positive word count
        int negCount = 0; // Initialize negative word count

        for (const auto& row : tweets) { // Loop through all tweets
            if (row[3] == senator) { // Check if tweet belongs to current senator
                stringstream ss(row[4]); // Create stringstream from tweet text
                string word; // Declare word variable
                while (ss >> word) { // Loop through words in tweet
                    string stemmed = stemString(word); // Stem the word
                    totalWords++; // Increment total word count

                    if (binary_search(stemmedPositive.begin(), stemmedPositive.end(), stemmed)) { // Check if word is positive
                        posCount++; // Increment positive count
                    }
                    if (binary_search(stemmedNegative.begin(), stemmedNegative.end(), stemmed)) { // Check if word is negative
                        negCount++; // Increment negative count
                    }
                }
            }
        }

        double posPct = (totalWords > 0) ? (double)posCount / totalWords * 100.0 : 0.0; // Calculate positive percentage
        double negPct = (totalWords > 0) ? (double)negCount / totalWords * 100.0 : 0.0; // Calculate negative percentage

        cout << left << setw(20) << senator << right << setw(15) << fixed << setprecision(5) << posPct << setw(15) << negPct << endl; // Print senator stats
    }
}

// Part 2 - Capability 1: Most Talkative Senator
void findMostTalkative(const vector<vector<string>>& tweets, const vector<string>& senators) { // Function definition
    string mostTweetsSenator; // Variable to store senator with most tweets
    int maxTweets = -1; // Variable to store max tweet count

    string mostWordsSenator; // Variable to store senator with most words/tweet
    double maxAvgWords = -1.0; // Variable to store max average words

    cout << left << setw(20) << "Senator" << right << setw(15) << "Tweet Count" << setw(20) << "Avg Words/Tweet" << endl; // Print table header

    for (const string& senator : senators) { // Loop through each senator
        int tweetCount = 0; // Initialize tweet count
        int totalWords = 0; // Initialize total word count

        for (const auto& row : tweets) { // Loop through all tweets
            if (row[3] == senator) { // Check if tweet belongs to current senator
                tweetCount++; // Increment tweet count
                stringstream ss(row[4]); // Create stringstream from tweet text
                string word; // Declare word variable
                while (ss >> word) { // Loop through words in tweet
                    totalWords++; // Increment total word count
                }
            }
        }

        double avgWords = (tweetCount > 0) ? (double)totalWords / tweetCount : 0.0; // Calculate average words per tweet

        cout << left << setw(20) << senator << right << setw(15) << tweetCount << setw(20) << fixed << setprecision(2) << avgWords << endl; // Print senator stats

        if (tweetCount > maxTweets) { // Check if current senator has more tweets
            maxTweets = tweetCount; // Update max tweets
            mostTweetsSenator = senator; // Update most tweets senator
        }
        if (avgWords > maxAvgWords) { // Check if current senator has higher average words
            maxAvgWords = avgWords; // Update max average words
            mostWordsSenator = senator; // Update most words senator
        }
    }

    cout << endl; // Print empty line
    cout << "Most tweets: " << mostTweetsSenator << " (" << maxTweets << ")" << endl; // Print result for most tweets
    cout << "Highest average word count: " << mostWordsSenator << " (" << maxAvgWords << ")" << endl; // Print result for highest average words
}

// Part 2 - Capability 2: Biden Sentiment
void analyzeBidenSentiment(const vector<vector<string>>& tweets, const vector<string>& positiveWords, const vector<string>& negativeWords) { // Function definition
    // Pre-stem emotion words
    vector<string> stemmedPositive; // Vector for stemmed positive words
    vector<string> stemmedNegative; // Vector for stemmed negative words

    for (const string& w : positiveWords) { // Loop through positive words
        stemmedPositive.push_back(stemString(w)); // Stem and add to vector
    }
    for (const string& w : negativeWords) { // Loop through negative words
        stemmedNegative.push_back(stemString(w)); // Stem and add to vector
    }
    sort(stemmedPositive.begin(), stemmedPositive.end()); // Sort stemmed positive words
    sort(stemmedNegative.begin(), stemmedNegative.end()); // Sort stemmed negative words

    int totalWords = 0; // Initialize total word count
    int posCount = 0; // Initialize positive word count
    int negCount = 0; // Initialize negative word count
    int bidenTweetCount = 0; // Initialize Biden tweet count

    for (const auto& row : tweets) { // Loop through all tweets
        string text = row[4]; // Get tweet text
        // Simple case-insensitive check for "Biden"
        string lowerText = text; // Create copy of text for lowercase conversion
        transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower); // Convert text to lowercase
        
        if (lowerText.find("biden") != string::npos) { // Check if "biden" is in text
            bidenTweetCount++; // Increment Biden tweet count
            stringstream ss(text); // Create stringstream from tweet text
            string word; // Declare word variable
            while (ss >> word) { // Loop through words in tweet
                string stemmed = stemString(word); // Stem the word
                totalWords++; // Increment total word count
                if (binary_search(stemmedPositive.begin(), stemmedPositive.end(), stemmed)) { // Check if word is positive
                    posCount++; // Increment positive count
                }
                if (binary_search(stemmedNegative.begin(), stemmedNegative.end(), stemmed)) { // Check if word is negative
                    negCount++; // Increment negative count
                }
            }
        }
    }

    cout << "Found " << bidenTweetCount << " tweets mentioning Biden." << endl; // Print number of Biden tweets
    if (totalWords > 0) { // Check if any words were processed
        double posPct = (double)posCount / totalWords * 100.0; // Calculate positive percentage
        double negPct = (double)negCount / totalWords * 100.0; // Calculate negative percentage
        cout << "Positive %: " << posPct << endl; // Print positive percentage
        cout << "Negative %: " << negPct << endl; // Print negative percentage
        
        if (posPct > negPct) { // Check if positive > negative
            cout << "Conclusion: Tweets about Biden are mostly POSITIVE." << endl; // Print positive conclusion
        } else if (negPct > posPct) { // Check if negative > positive
            cout << "Conclusion: Tweets about Biden are mostly NEGATIVE." << endl; // Print negative conclusion
        } else { // Otherwise
            cout << "Conclusion: Tweets about Biden are NEUTRAL." << endl; // Print neutral conclusion
        }
    } else { // If no words processed
        cout << "No words found in Biden tweets." << endl; // Print message
    }
}

// Extra Credit: Political Alignment Analysis
void analyzePoliticalAlignment(const vector<vector<string>>& tweets, const vector<string>& senators) { // Function definition
    // 1. Build Vocabulary and Counts
    vector<string> vocab; // Vector to store unique words (vocabulary)
    vector<int> repCounts; // Vector to store Republican word counts
    vector<int> demCounts; // Vector to store Democrat word counts
    
    // Common stop words to filter out
    vector<string> stopWords; // Vector to store stop words
    stopWords.push_back("the"); stopWords.push_back("is"); stopWords.push_back("and"); stopWords.push_back("to"); // Add stop words
    stopWords.push_back("of"); stopWords.push_back("a"); stopWords.push_back("in"); stopWords.push_back("for"); // Add stop words
    stopWords.push_back("on"); stopWords.push_back("with"); stopWords.push_back("at"); stopWords.push_back("by"); // Add stop words
    stopWords.push_back("from"); stopWords.push_back("up"); stopWords.push_back("about"); stopWords.push_back("into"); // Add stop words
    stopWords.push_back("over"); stopWords.push_back("after"); stopWords.push_back("this"); stopWords.push_back("that"); // Add stop words
    stopWords.push_back("it"); stopWords.push_back("are"); stopWords.push_back("was"); stopWords.push_back("be"); // Add stop words
    stopWords.push_back("has"); stopWords.push_back("have"); stopWords.push_back("will"); stopWords.push_back("as"); // Add stop words
    stopWords.push_back("an"); stopWords.push_back("or"); stopWords.push_back("but"); stopWords.push_back("not"); // Add stop words
    stopWords.push_back("no"); stopWords.push_back("we"); stopWords.push_back("our"); stopWords.push_back("us"); // Add stop words
    stopWords.push_back("my"); stopWords.push_back("i"); stopWords.push_back("you"); stopWords.push_back("your"); // Add stop words
    stopWords.push_back("he"); stopWords.push_back("she"); stopWords.push_back("they"); stopWords.push_back("their"); // Add stop words
    stopWords.push_back("his"); stopWords.push_back("her"); stopWords.push_back("rt"); stopWords.push_back("amp"); // Add stop words

    cout << "Building political term list from tweets..." << endl; // Print status message

    for (const auto& row : tweets) { // Loop through all tweets
        if (row.size() < 5) continue; // Skip invalid rows
        string party = getParty(row[3]); // Get party of tweet author
        string text = row[4]; // Get tweet text
        
        stringstream ss(text); // Create stringstream from text
        string word; // Declare word variable
        while (ss >> word) { // Loop through words in tweet
            string cleanWord = ""; // Variable for cleaned word
            for (char c : word) { // Loop through characters in word
                if (isalpha(c)) cleanWord += tolower(c); // Keep only alphabetic chars and convert to lowercase
            }
            if (cleanWord.length() < 3) continue; // Skip short words

            bool isStop = false; // Flag for stop word
            for (const string& sw : stopWords) { // Loop through stop words
                if (cleanWord == sw) { // Check if word is a stop word
                    isStop = true; // Set flag
                    break; // Exit loop
                }
            }
            if (isStop) continue; // Skip stop words

            int index = -1; // Variable for word index in vocab
            for (size_t i = 0; i < vocab.size(); ++i) { // Loop through vocabulary
                if (vocab[i] == cleanWord) { // Check if word exists
                    index = i; // Set index
                    break; // Exit loop
                }
            }

            if (index == -1) { // If word not found
                vocab.push_back(cleanWord); // Add to vocabulary
                repCounts.push_back(0); // Initialize Republican count
                demCounts.push_back(0); // Initialize Democrat count
                index = vocab.size() - 1; // Set index to new element
            }

            if (party == "Republican") { // If author is Republican
                repCounts[index]++; // Increment Republican count
            } else { // If author is Democrat
                demCounts[index]++; // Increment Democrat count
            }
        }
    }

    // 2. Identify Key Terms
    vector<string> keyTerms; // Vector to store key terms
    vector<string> keyTermParty; // Vector to store party associated with key terms
    
    // Find top Republican words
    for (int k = 0; k < 15; ++k) { // Loop 15 times to find top 15 words
        int maxDiff = -1; // Variable for max difference
        int bestIdx = -1; // Variable for best index
        for (size_t i = 0; i < vocab.size(); ++i) { // Loop through vocabulary
            int diff = repCounts[i] - demCounts[i]; // Calculate difference (Rep - Dem)
            bool picked = false; // Flag if word already picked
            for(const string& s : keyTerms) if(s == vocab[i]) picked = true; // Check if picked
            
            if (!picked && diff > maxDiff && (repCounts[i] + demCounts[i] > 5)) { // Check criteria
                maxDiff = diff; // Update max difference
                bestIdx = i; // Update best index
            }
        }
        if (bestIdx != -1) { // If a word was found
            keyTerms.push_back(vocab[bestIdx]); // Add to key terms
            keyTermParty.push_back("Republican"); // Add party association
        }
    }

    // Find top Democrat words
    for (int k = 0; k < 15; ++k) { // Loop 15 times to find top 15 words
        int maxDiff = -1; // Variable for max difference
        int bestIdx = -1; // Variable for best index
        for (size_t i = 0; i < vocab.size(); ++i) { // Loop through vocabulary
            int diff = demCounts[i] - repCounts[i]; // Calculate difference (Dem - Rep)
            bool picked = false; // Flag if word already picked
            for(const string& s : keyTerms) if(s == vocab[i]) picked = true; // Check if picked

            if (!picked && diff > maxDiff && (repCounts[i] + demCounts[i] > 5)) { // Check criteria
                maxDiff = diff; // Update max difference
                bestIdx = i; // Update best index
            }
        }
        if (bestIdx != -1) { // If a word was found
            keyTerms.push_back(vocab[bestIdx]); // Add to key terms
            keyTermParty.push_back("Democrat"); // Add party association
        }
    }

    cout << "Identified Key Political Terms:" << endl; // Print header
    for(size_t i=0; i<keyTerms.size(); ++i) { // Loop through key terms
        cout << left << setw(20) << keyTerms[i] << keyTermParty[i] << endl; // Print term and party
    }
    cout << endl; // Print empty line

    // 3. Analyze Tweets
    cout << "Analyzing tweets for alignment..." << endl; // Print status message
    
    int correctPredictions = 0; // Counter for correct predictions
    int totalPredictions = 0; // Counter for total predictions
    
    vector<int> senCorrect(senators.size(), 0); // Vector for correct predictions per senator
    vector<int> senTotal(senators.size(), 0); // Vector for total predictions per senator

    for (const auto& row : tweets) { // Loop through all tweets
        if (row.size() < 5) continue; // Skip invalid rows
        string actualParty = getParty(row[3]); // Get actual party
        string text = row[4]; // Get tweet text
        
        vector<string> tweetWords; // Vector to store words in tweet
        stringstream ss(text); // Create stringstream
        string word; // Declare word variable
        while(ss >> word) { // Loop through words
             string cleanWord = ""; // Variable for cleaned word
            for (char c : word) { // Loop through chars
                if (isalpha(c)) cleanWord += tolower(c); // Clean word
            }
            tweetWords.push_back(cleanWord); // Add to vector
        }

        double score = 0; // Initialize score
        int termsFound = 0; // Initialize terms found count

        for (size_t i = 0; i < tweetWords.size(); ++i) { // Loop through words in tweet
            string w = tweetWords[i]; // Get current word
            
            int termIdx = -1; // Variable for term index
            for(size_t k=0; k<keyTerms.size(); ++k) { // Loop through key terms
                if (keyTerms[k] == w) { // Check if word matches key term
                    termIdx = k; // Set index
                    break; // Exit loop
                }
            }

            if (termIdx != -1) { // If key term found
                double termVal = (keyTermParty[termIdx] == "Republican") ? 1.0 : -1.0; // Set value based on party
                
                // Check negation (window +/- 2)
                bool negated = false; // Flag for negation
                int start = (int)i - 2; // Start index for window
                int end = (int)i + 2; // End index for window
                if (start < 0) start = 0; // Bound check start
                if (end >= (int)tweetWords.size()) end = (int)tweetWords.size() - 1; // Bound check end

                for (int j = start; j <= end; ++j) { // Loop through window
                    if (j == (int)i) continue; // Skip current word
                    if (tweetWords[j] == "not" || tweetWords[j] == "no" || tweetWords[j] == "never") { // Check for negation words
                        negated = true; // Set flag
                    }
                }

                if (negated) termVal *= -1; // Flip value if negated
                
                score += termVal; // Add to score
                termsFound++; // Increment terms found
            }
        }

        if (termsFound > 0) { // If terms were found
            string predicted = (score > 0) ? "Republican" : (score < 0) ? "Democrat" : "Neutral"; // Predict party
            if (predicted != "Neutral") { // If not neutral
                totalPredictions++; // Increment total predictions
                if (predicted == actualParty) correctPredictions++; // Increment correct predictions if match
                
                // Update senator stats
                for(size_t s=0; s<senators.size(); ++s) { // Loop through senators
                    if (senators[s] == row[3]) { // Find matching senator
                        senTotal[s]++; // Increment total for senator
                        if (predicted == actualParty) senCorrect[s]++; // Increment correct for senator
                        break; // Exit loop
                    }
                }
            }
        }
    }

    cout << "Alignment Analysis Results by Senator:" << endl; // Print header
    cout << left << setw(20) << "Senator" << setw(15) << "Party" << setw(15) << "Accuracy" << endl; // Print table header
    cout << string(50, '-') << endl; // Print separator line
    
    for(size_t s=0; s<senators.size(); ++s) { // Loop through senators
        double acc = (senTotal[s] > 0) ? (double)senCorrect[s] / senTotal[s] * 100.0 : 0.0; // Calculate accuracy
        cout << left << setw(20) << senators[s] << setw(15) << getParty(senators[s]) << fixed << setprecision(1) << acc << "% (" << senCorrect[s] << "/" << senTotal[s] << ")" << endl; // Print stats
    }
    cout << endl; // Print empty line
    
    cout << "Overall Accuracy: " << (totalPredictions > 0 ? (double)correctPredictions / totalPredictions * 100.0 : 0.0) << "%" << endl; // Print overall accuracy
}
