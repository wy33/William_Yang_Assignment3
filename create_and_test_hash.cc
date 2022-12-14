// William Yang
// create_and_test_hash.cc: A test for linear probing, quadratic probing, and double hashing implementations.
// Fills hash table provided with a dictionary/words file and prints the hash table's attributes
// and then queries the hash table with a query file.

#include <fstream>
#include <iostream>
#include <string>

#include "linear_probing.h"
#include "double_hashing.h"
#include "quadratic_probing.h"

using namespace std;



// @hash_table: a hash table (can be linear, quadratic, or double)
// @words_filename: a filename of input words to construct the hash table
// @query_filename: a filename of input words to test the hash table
// A test function for hash table classes to track the attributes of
// each hash table implementation.
template <typename HashTableType>
void TestFunctionForHashTable(HashTableType &hash_table,
			      const string &words_filename,
			      const string &query_filename)
{
    hash_table.MakeEmpty();

    // Read words file.
    fstream words_file(words_filename);
    if (!words_file.is_open())
    {
        cerr << "Failed to read words file." << endl;
        exit(1);
    }

    string word;
    // Insert words into hash table.
    while (words_file >> word && !words_file.fail())
    {
        hash_table.Insert(move(word));
    }

    words_file.close();

    // Hash table attributes.
    cout << "number_of_elements: " << hash_table.Size() << endl;
    cout << "size_of_table: " << hash_table.Capacity() << endl;
    cout << "load_factor: " << hash_table.LoadFactor() << endl;
    cout << "collisions: " << hash_table.TotalCollisions() << endl;
    cout << "avg_collisions: " << hash_table.AverageCollisions() << "\n\n";

    // Read query.
    fstream query(query_filename);
    if (!query.is_open())
    {
        cerr << "Failed to read query file." << endl;
        exit(1);
    }

    // Query hash table.
    while (query >> word)
    {
        if(hash_table.Contains(word))
            cout << word << " Found " << hash_table.ProbesUsed() << endl;
        else
            cout << word << " Not_Found " << hash_table.ProbesUsed() << endl;
    }

    query.close();
}

// @argument_count: argc as provided in main
// @argument_list: argv as provided in imain
// Calls the specific testing function for hash table (linear, quadratic, or double).
int testHashingWrapper(int argument_count, char **argument_list)
{
    const string words_filename(argument_list[1]);
    const string query_filename(argument_list[2]);
    const string param_flag(argument_list[3]);
    int R = 89;
    if (argument_count == 5) {
	    const string rvalue(argument_list[4]);
	    R = stoi(rvalue); // Aborts program if R isn't a number.
    }

    if (param_flag == "linear") {
        HashTableLinear<string> linear_probing_table;
        TestFunctionForHashTable(linear_probing_table, words_filename, query_filename);
    } else if (param_flag == "quadratic") {
	    HashTable<string> quadratic_probing_table;
	    TestFunctionForHashTable(quadratic_probing_table, words_filename, query_filename);
    } else if (param_flag == "double") {
	    cout << "r_value: " << R << endl;
        HashTableDouble<string> double_probing_table(101, R);
        TestFunctionForHashTable(double_probing_table, words_filename, query_filename);
    } else {
	    cout << "Unknown tree type " << param_flag << " (User should provide linear, quadratic, or double)" << endl;
    }
    return 0;
}

// Sample main for program create_and_test_hash. DO NOT CHANGE IT.
// WE WILL NOT USE YOUR MAIN IN TESTING. DO NOT CODE FUNCTIONALITY INTO THE
// MAIN. WE WILL DIRECTLY CALL testHashingWrapper. ALL FUNCTIONALITY SHOULD BE
// THERE. This main is only here for your own testing purposes.
/*
int main(int argc, char **argv)
{
    if (argc != 4 and argc != 5) {
	cout << "Usage: " << argv[0]
	     << " <wordsfilename> <queryfilename> <flag>" << endl;
	cout << "or Usage: " << argv[0]
	     << " <wordsfilename> <queryfilename> <flag> <rvalue>" << endl;
	return 0;
    }

    testHashingWrapper(argc, argv);
    return 0;
}
*/
