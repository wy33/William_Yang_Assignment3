// YOUR NAME.
// spell_check.cc: A simple spell checker.

#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <map>
#include <cctype>
#include <vector>
#include <algorithm>

// You can change to quadratic probing if you
// haven't implemented double hashing.
#include "double_hashing.h"
#include "linear_probing.h"
#include "quadratic_probing.h"
using namespace std;

// You can add more functions here.



// Shows how the map that is eventually produced, used to print the required answers
void printHighChangeables(const map<string, vector<string>>& adjacentWords, int minWords = 15) {

    // Steps through the map to print and entrues that are pairs consisting of a word and vector of words
    // const references are used to replace complicated expressions and avoid making unneeded copies
    for (auto& entry : adjacentWords) {
        const vector<string>& words = entry.second; // const

        if (words.size() >= minWords) {
            cout << entry.first << " (" << words.size() << "):";
            for (auto& str : words) {
                cout << " " << str;
            }
            cout << endl;
        }
    }
}

string removePunctuation(const string& word) {
    string copy = word;
    if (ispunct(copy[copy.size() - 1]))
        copy.erase(copy.end() - 1);
    if (ispunct(copy[0]))
        copy.erase(copy.begin());
    return copy;
}

string toLower(const string& word)
{
    string copy = word;
    transform(copy.begin(), copy.end(), copy.begin(), ptr_fun<int, int>(tolower));
    return copy;
}

bool equalsIgnoreCase(const string& lhs, const string& rhs)
{
    return toLower(lhs) == toLower(rhs);
}

// Test to see if two words are the same except one character
bool oneCharOff(const string& word1, const string& word2) {

    // if they are not the same length then they cannot be the same except one character
    if (word1.length() != word2.length())
        return false;

    int diffs = 0; // count of differences

    // traversing through the word
    for (int i = 0; i < word1.length(); i++)
        //checking character by character of word if they are equal
        if (word1[i] != word2[i])
            // return false if difference is more than 1
            if (++diffs > 1)
                return false;

    // boolean expression that should evaluate to true
    return diffs == 1;
}

// Test to see if two words are the same except two characters
bool twoCharsOff(const string& word1, const string& word2) {

    // if they are not the same length then they cannot be the same except two characters
    if (word1.length() != word2.length())
        return false;

    int diffs = 0; // count of differences

    // traversing through the word
    for (int i = 0; i < word1.length(); i++)
        //checking character by character of word if they are equal
        if (word1[i] != word2[i])
            // return false if difference is more than 1
            if (++diffs > 1)
                return false;

    // boolean expression that should evaluate to true
    return diffs == 2;
}

// Returns the index of the first instance where two words have different characters
// starting from the given position.
// Meant to be a follow-up call from twoCharsOff() function after the two strings are
// confirmed to have 2 differing characters each.
int differingCharPosition(const string& word1, const string& word2,const int& position) {


    for (int i = position; i < word1.length(); i++)
    {
        if (word1[i] != word2[i])
            return i;
    }
    return -1;  // Cannot determine position in which they differ
}

// Swaps the chars of a word given the two positions of the chars.
void swapChars(string& word, const int& pos1, const int& pos2) {
    char temp = move(word[pos1]);
    word[pos1] = move(word[pos2]);
    word[pos2] = move(temp);
}


bool oneCharMissing(const string& word1, const string& word2) {/////////////maybe obsolete, perhaps a function that returns an alternative is best
    if (abs((int)word1.length() - (int)word2.length()) != 1)
        return false;


}

// Adds a character to the word and tests if it is in the dictionary,
// if it is, push to alternatives vector.
// Try to insert a-z in all positions.
void tryAlphabetChars(vector<string>& alts, const string& word, HashTableDouble<string>& dictionary) {
    string temp = word;
    char char_insert;
    for (size_t i = 0; i < word.size(); i++)
    {
        // Insert a-z (97-122 ASCII values) into temp at position i and
        // test if it matches a dictionary word, if so, push_back to
        // alts vector.
        for (int j = 97; j <= 122; j++)
        {
            char_insert = j;
            temp.insert(temp.begin() + i, char_insert);
            if (dictionary.Contains(temp))
            {
                alts.push_back(temp);
            }
            temp.erase(temp.begin() + i);
        }
    }
}

// Removes a character from the word and tests if it is in the dictionary,
// if it is, push to alternatives vector.
// Try removing a char in all positions (removes only one char from word).
void tryRemoveAChar(vector<string>& alts, const string& word, HashTableDouble<string>& dictionary) {
    string temp = word;
    char char_removed;
    for (size_t i = 0; i < word.size(); i++)
    {
        char_removed = temp[i];
        temp.erase(temp.begin() + i);
        if (dictionary.Contains(temp))
        {
            alts.push_back(temp);
        }
        temp.insert(temp.begin() + i, char_removed);
    }
}

void trySwappingChars(vector<string>& alts, const string& word, HashTableDouble<string>& dictionary) {
    string temp = word;
    for (size_t i = 0; i < word.size()/* -1 */; i++)
    {
        for (size_t j = i + 1; j < word.size(); j++)
        {
            swapChars(temp, i, j);
            if (dictionary.Contains(temp))
            {
                alts.push_back(temp);
            }
            swapChars(temp, i, j);
        }
    }
}

void printAlternatives(const vector<string>& alts, const string& incorrect_word, const char& case_letter) {

    for (size_t i = 0; i < alts.size(); i++)
    {
        cout << "** " << incorrect_word << " -> " << alts[i] << " ** case " << case_letter << endl;

    }
}

// Creates and fills double hashing hash table with all words from
// dictionary_file
HashTableDouble<string> MakeDictionary(const string& dictionary_file) {
    HashTableDouble<string> dictionary_hash;

    fstream dictionary_file_stream(dictionary_file);
    if (!dictionary_file_stream.is_open())
    {
        cerr << "Failed to read dictionary file." << endl;
        exit(1);
    }

    // Fill dictionary_hash.
    string word;
    while (dictionary_file_stream >> word && !dictionary_file_stream.fail())
    {
        dictionary_hash.Insert(move(word));
    }
    dictionary_file_stream.close();

    return dictionary_hash;
}

// For each word in the document_file, it checks the 3 cases for a word being
// misspelled and prints out possible corrections
void SpellChecker(HashTableDouble<string>& dictionary, const string &document_file) {

    fstream document_file_stream(document_file);
    if (!document_file_stream.is_open())
    {
        cerr << "Failed to read document file." << endl;
        exit(1);
    }

    string input_word;
    string lowercase_word;
    vector<string> alternative_words;
//    map<string, vector<string>> wordsOneChar;
//    vector<string> words;
    // 
    while (document_file_stream >> input_word && !document_file_stream.fail())
    {
        ///////////////////////////set word to lower case for comparisons
        input_word = removePunctuation(input_word);
        lowercase_word = toLower(input_word);
        if (dictionary.Contains(lowercase_word)) ////////////////////need to make Contains Function const as well as FindPos since hashtabledouble is const
        {/////////////discussion board allows removal of const keyword in parameter
            cout << input_word << " is CORRECT" << endl;
            continue;
        }

        cout << input_word << " is INCORRECT" << endl;

        tryAlphabetChars(alternative_words, lowercase_word, dictionary);
        printAlternatives(alternative_words, lowercase_word, 'A');
        alternative_words.clear();
        tryRemoveAChar(alternative_words, lowercase_word, dictionary);
        printAlternatives(alternative_words, lowercase_word, 'B');
        alternative_words.clear();
        trySwappingChars(alternative_words, lowercase_word, dictionary);
        printAlternatives(alternative_words, lowercase_word, 'C');
        alternative_words.clear();

        // Print out alternative words.
 //       for (size_t i = 0; i < alternative_words.size(); i++)
 //       {
 //           cout << alternative_words[i] << " case" << endl;
//
  //      }
//        alternative_words.clear();
        /////////////////// fill words vector with adjacent words
//        wordsOneChar[word] = words;
//        printHighChangeables(wordsOneChar, 5);
    }



    document_file_stream.close();
}

// @argument_count: same as argc in main
// @argument_list: save as argv in main.
// Implements
int testSpellingWrapper(int argument_count, char** argument_list) {
    const string document_filename(argument_list[1]);
    const string dictionary_filename(argument_list[2]);
    
    // Call functions implementing the assignment requirements.
    HashTableDouble<string> dictionary = MakeDictionary(dictionary_filename);
    SpellChecker(dictionary, document_filename);

    return 0;
}

// Sample main for program spell_check.
// WE WILL NOT USE YOUR MAIN IN TESTING. DO NOT CODE FUNCTIONALITY INTO THE
// MAIN. WE WILL DIRECTLY CALL testSpellingWrapper. ALL FUNCTIONALITY SHOULD BE
// THERE. This main is only here for your own testing purposes.
/*
int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <document-file> <dictionary-file>" << endl;
        return 0;
    }
    
    testSpellingWrapper(argc, argv);
    
    return 0;
}
*/
