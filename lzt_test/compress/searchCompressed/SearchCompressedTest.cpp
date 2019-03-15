#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <set>

#include "SearchCompressedTest.h"
#include "dictionary/char_trie/Trie.h"
#include "compress/lz_compressor/LzCompressor.h"
#include "util/WordFileReader.h"
#include "node_array/na_utils.h"
#include "dictionary/lz_trie/LzTrie.h"
#include "util/utils.h"
#include "debug/lzt_test.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SearchCompressedTest);

SearchCompressedTest::SearchCompressedTest(): mutation(new char[MSIZE]) {
}

SearchCompressedTest::~SearchCompressedTest() {
    delete [] mutation;
}

void SearchCompressedTest::setUp() {
}

void SearchCompressedTest::tearDown() {
}


class LessThan {
public:
    inline bool operator()(const char* f, const char* s) {
        return (strcmp(f, s) < 0);
    }
};

void SearchCompressedTest::testContains() {
    // create a compressed trie from a dictionary
    WordFileReader<char> reader("../dictionary_files/french.txt");
    reader.readWords();

    Trie<VectorArray<char, int> > trie;
    LzCompressor<VectorArray<char, int> > comp;

    for (size_t i = 0; i < reader.getNumberOfWords(); ++i)
        trie.insertWord(reader.getWord(i));

    // assert that all the dictionary words are in the uncompressed trie
    for (int i = 0; i < reader.getNumberOfWords(); ++i)
        TEST_ASSERT_MESSAGE(trie.containsWord(reader.getWord(i)), reader.getWord(i));

    //return;
    // compresse trie
    VectorArray<char, int>& array = trie.exportNodeArray();
    //cout<< nodeArraySymbolsToString (array, 0, array.getSize()) << endl;

    comp.compressArray(array);
       
    //comp.compressArray(array);
    
    LzTrie<VectorArray<char, int> > lztrie(array);
    //cout<< nodeArraySymbolsToString (array, 0, array.getSize()) << endl;

    // assert that all the dictionary words are in the trie
    for (int i = 0; i < reader.getNumberOfWords(); ++i)
        TEST_ASSERT_MESSAGE(lztrie.containsWord(reader.getWord(i)), reader.getWord(i));

    return;

    // generate words not in the dictionary and assert they're not in the trie

    // create set of words from dict
    LessThan lt;
    set<char const *, LessThan> wordSet;
    for (size_t i = 0; i < reader.getNumberOfWords(); ++i) wordSet.insert( reader.getWord(i) );

    const char *alphabet = "abcdefghijklmnopqrstuvxyz1234567890";
    long avg = 0;   
    // mutate words from dict and test
    for (int i = 0; i < reader.getNumberOfWords(); ++i) {
        if ( sstringLength<char, int>(reader.getWord(i)) <= 2 ) continue;
        // generate a word not in the set, but similar to a word in the set
        int iter = 0;
        do {
            createMutation(reader.getWord(i), alphabet);
            iter++;
//            cout<<mutation<<"\n";
        } while ( wordSet.find(mutation) != wordSet.end() );
        avg += iter;

        // average number of neccessary mutations for last 500 words
//        if (i % 500 == 0) {
//            cout<<avg/500.<<"\n";
//            avg = 0;
//        }
        //cout<<mutation<<"\n";

        TEST_ASSERT_MESSAGE(trie.containsWord(mutation) == false, mutation);
    }
    
}

void SearchCompressedTest::createMutation(const char *word, const char *alphabet) {
    int len = sstringLength<char, int>(word), alphaLen = sstringLength<char, int>(alphabet);
    strcpy(mutation, word);
    len = sstringLength<char, int>(mutation);

    srand( time(NULL) );

    int c = rand() % 4;

    if (c == 1 || c == 2 || len <= 4) {
        // change two letters, on the average
        // for short letters, do only this to avoid mutation without effect
        for (int i = 0; i < len; ++i)
            if ((rand()/(double)RAND_MAX) <= 0.12) mutation[i] = alphabet[rand() % alphaLen ];
    }
    else if (c == 0) {
        // swap few pairs of chars
        for (int i = 0, sw = rand() % 2 + 2; i < sw; ++i) {
            int l1 = rand() % len,  l2 = rand() % len;
            char ch = mutation[l1];
            mutation[l1] = mutation[l2]; mutation[l2] = ch;
        }
    }
    else if (c == 3) {
        // truncate
        mutation[rand() % len + 1] = zeroSymbol<char>();
    }
}
