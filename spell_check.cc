// William Yang
// spell_check.cc: A simple spell checker.
// Checks the provided document file for spelling mistakes.
// If a word is correct, prints CORRECT.
// If a word is incorrect, prints INCORRECT and offers alternative words
// from the provided dictionary file.

#include <fstream>
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>

#include "double_hashing.h"
#include "linear_probing.h"
#include "quadratic_probing.h"

using namespace std;



// Removes punctuation at the beginning and end of word.
string removePunctuation(const string& word)
{
    string copy = word;
    if (word.size() > 1) // Ensures copy will have at least one character when returning.
        if (ispunct(copy[copy.size() - 1]))
            copy.erase(copy.end() - 1);
    if (word.size() > 1) // Ensures copy will have at least one character when returning.
        if (ispunct(copy[0]))
            copy.erase(copy.begin());
    return copy;
}

// Returns the lowercase version of word.
string toLower(const string& word)
{
    string copy = word;
    transform(copy.begin(), copy.end(), copy.begin(), ptr_fun<int, int>(tolower));
    return copy;
}

// Swaps the chars of a word given the two positions.
void swapChars(string& word, const int& pos1, const int& pos2)
{
    char temp = move(word[pos1]);
    word[pos1] = move(word[pos2]);
    word[pos2] = move(temp);
}

// Adds a character to the word and tests if it is in the dictionary,
// if it is, push to alternatives vector.
// Try to insert a-z in all positions.
void tryAlphabetChars(vector<string>& alts, const string& word, HashTableDouble<string>& dictionary)
{
    string temp = word;
    char char_insert;
    for (size_t i = 0; i < word.size() + 1; i++) // +1 to test inserting a-z at the end of word
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
void tryRemoveAChar(vector<string>& alts, const string& word, HashTableDouble<string>& dictionary)
{
    string temp = word;
    char char_removed;
    for (size_t i = 0; i < word.size(); i++)
    {
        char_removed = temp[i]; // Needed to put removed character back into temp.
        temp.erase(temp.begin() + i);
        if (dictionary.Contains(temp))
        {
            if (alts.empty())
                alts.push_back(temp);
            else if (alts.back() != temp)
                alts.push_back(temp);
        }
        temp.insert(temp.begin() + i, char_removed);
    }
}

// Swaps adjacent characters in given word and tests if it is in the dictionary,
// if it is, push to alternatives vector.
void trySwappingChars(vector<string>& alts, const string& word, HashTableDouble<string>& dictionary)
{
    string temp = word;
    for (size_t i = 0; i < word.size() -1; i++)
    {
        swapChars(temp, i, i + 1);
        if (dictionary.Contains(temp))
        {
            alts.push_back(temp);
        }
        swapChars(temp, i, i + 1);
    }
}

// Print the alternative words of the incorrect word
// as well as the case letter associated with correction type.
void printAlternatives(const vector<string>& alts, const string& incorrect_word, const char& case_letter)
{
    for (size_t i = 0; i < alts.size(); i++)
        cout << "** " << incorrect_word << " -> " << alts[i] << " ** case " << case_letter << endl;
}

// Creates and fills double hashing hash table with all words from
// dictionary_file
HashTableDouble<string> MakeDictionary(const string& dictionary_file)
{
    HashTableDouble<string> dictionary_hash;

    // Read dictionary file.
    fstream dictionary_file_stream(dictionary_file);
    if (!dictionary_file_stream.is_open())
    {
        cerr << "Failed to read dictionary file." << endl;
        exit(1);
    }

    string word;
    // Fill dictionary_hash.
    while (dictionary_file_stream >> word && !dictionary_file_stream.fail())
    {
        dictionary_hash.Insert(move(word));
    }

    dictionary_file_stream.close();

    return dictionary_hash;
}

// For each word in the document_file, it checks the 3 cases for a word being
// misspelled and prints out possible corrections
void SpellChecker(HashTableDouble<string>& dictionary, const string &document_file)
{
    // Read document file.
    fstream document_file_stream(document_file);
    if (!document_file_stream.is_open())
    {
        cerr << "Failed to read document file." << endl;
        exit(1);
    }

    string input_word;
    vector<string> alternative_words;
    // Check for corrections of input word from document file.
    while (document_file_stream >> input_word && !document_file_stream.fail())
    {
        // Remove punctuations at the beginning and end of input word
        // and set word to lower case for comparisons.
        input_word = toLower(removePunctuation(input_word));

        // If word is in dictionary, print:
        // <word> is CORRECT
        if (dictionary.Contains(input_word))
        {
            cout << input_word << " is CORRECT" << endl;
            continue;
        }

        // Word does not exist in dictionary, print:
        // <word> is INCORRECT
        cout << input_word << " is INCORRECT" << endl;

        // Find alternatives for the incorrect word from the dictionary
        // and print them.
        tryAlphabetChars(alternative_words, input_word, dictionary);
        printAlternatives(alternative_words, input_word, 'A');
        alternative_words.clear();
        tryRemoveAChar(alternative_words, input_word, dictionary);
        printAlternatives(alternative_words, input_word, 'B');
        alternative_words.clear();
        trySwappingChars(alternative_words, input_word, dictionary);
        printAlternatives(alternative_words, input_word, 'C');
        alternative_words.clear();

    }

    document_file_stream.close();
}

// @argument_count: same as argc in main
// @argument_list: save as argv in main.
// Implements
int testSpellingWrapper(int argument_count, char** argument_list)
{
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
int main(int argc, char** argv)
{
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <document-file> <dictionary-file>" << endl;
        return 0;
    }
    
    testSpellingWrapper(argc, argv);
    
    return 0;
}
*/
