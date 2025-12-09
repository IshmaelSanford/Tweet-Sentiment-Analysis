
**CS 101 - Algorithmic Problem Solving I**
**Project - Fall 2025**

# Sentiment Analysis of US Senator Tweets

A US Senator has asked you to write a program to analyze his and his rivals' tweets. In this project you will analyze the following three datasets. 

**tweets.csv**: This file contains tweet data including tweet id, user id, datetime, senator name and tweet text of 14 senators. Each field is separated with "|" character. Tweet texts are already cleaned from whitespace characters, hashtags and https links. 

**positive-words.txt**: this file contains English words that convey positive emotions.

**negative-words.txt**: this file contains English words that convey negative emotions. 

## Part 1- (30 points)

Your job is to count the existence of positive and negative words in senator tweets and calculate a percentage by dividing in the total word count of each senator. You will be able to find out who is the most positive or negative senator. 

Sample output:
>(output using \t and right justified output stream)

Senator\tPositive %\tNegative %
Dan Sullivan\t7.24876\t4.28336
Cory Gardner\t12.1104\t2.46753
...

Helper Functions and Libraries:
**read_tweets_csv_file()**: this function creates a two-dimensional string vector, fills it with the tweet data from tweets.csv and returns it. Each row in the 2D vector contains tweet id, user id, datetime, senator name and tweet text.

**readEmotionFile(string path)**: this function takes the name of the emotion word file (e.g. positive-words.txt) and creates a one dimensional string vector filled with words from the parameter file and returns it. 

**Stemmer.h**: this header file contains a function called stemString(string word) that returns the stem of a word. 

**Step 1**. Create a function that takes the 2dD tweets vector as a parameter, creates a 1D string vector that includes the unique names of the senators and returns it. 

**Step 2**. Create a function that takes the 2D tweets vector, 1D senator name vector, positive words vector and negative words vector as parameters and calculates and prints the positive and negative words percentage for the parameter senator name. 

You may split a tweet text into words using the sstream library and the following code:

```cpp
stringstem ss(tweet);
string word;
while (ss >> word)
{
     cout << word << endl;
     // compare the word to words in positive and negative words vectors.
}
```

Before you can compare if the two words are equal, BOTH OF THEM MUST BE CONVERTED INTO THEIR STEMS. stemString() function from Stemmer.h takes a string parameter, converts it to its stem and returns it. Sample call:

```cpp
string s = stemString(searchWord);
```

**Step 3**. Create a main program to test your functions and produce the output. 

## PART II (30 points)

Pick two of the five capabilities and implement them (Each count as 15 points).

1. search for tweets between two dates for a specific senator and display the tweets.
2. Search for the most positive and most negative tweet for a specific senator and list them. 
3. Who is the most talkative senator in terms of number of tweets and average word count per tweet? 
4. Are the tweets about Joe Biden mostly positive or negative? Find mention of "Joe Biden" or "Biden" in the tweet texts. 
5. Find the texts that mention senators from the list? Are there any senators who talk about each other?

## EXTRA CREDIT (15 points)

Each student may implement an additional capability that he/she discovers, and thinks will be useful (NOT from the list above). The capability should be challenging enough to deserve the 15 points. 

## PRESENTATION (40 points)

The presentation will be a class discussion on the implementation methods used in the project code. Clear explanation of the design choices, the algorithms and the code structure will decide your presentation grade. 

What to submit: 
1. Your completed code properly commented.
2. Project report analyzing and discussing your results (2 pages)
> Cannot use 