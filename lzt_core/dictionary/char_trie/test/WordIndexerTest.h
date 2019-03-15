#ifndef ENUMTRIETEST_H
#define	ENUMTRIETEST_H

#include <sstream>
#include <iostream>

#include "../WordIndexer.h"
#include "util/TestCaseReader.h"
#include "util/factory.h"
#include "debug/lzt_test.h"
#include "util/utils.h"
#include "node_array/na_utils.h"
#include "util/WordFileReader.h"
#include "dictionary/lz_trie/LzTrieIterator.h"

using namespace std;

class WordIndexerTest {

private:
    typedef VectorArray<char, unsigned int> TNodeArray;
    typedef TNodeArray::Index TIndex;
    typedef LzTrieIterator<TNodeArray> TIterator;

public:
    void testWordCounting();
    void testGetWord();
    void testGetIndex();
    void testWithDictionary();

};

/** Helper class that creates node array based on iterator type:
 * lz-compressed array for LzTrieIterator and uncompressed for other
 * types. In a normal case, client that picks the iterator also controls
 * the (un)compressednes of the array, but in order to use same test code for
 * both kind of iterators, arrays have to be created automaticaly.  */
template <typename TNodeArray, typename TIterator>
class ArrayGetter {
public:
    static TNodeArray* get(WordList<typename TNodeArray::Symbol>& words) {
        return getEnumArray<TNodeArray>(words);
    }
};

/** Partial specialization for LzTrieIterators that yields lz-compressed arrays. */
template <typename TNodeArray>
class ArrayGetter<TNodeArray, LzTrieIterator<TNodeArray> > {
public:
    static TNodeArray* get(WordList<typename TNodeArray::Symbol>& words) {
        return getLzArray<TNodeArray>(words, true);
    }
};


template <typename TNodeArray, typename TIterator>
class EnumTrieTester {
    
private:
    
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Symbol TSymbol;

public:

    /** Test if the numWords field in nodes of a trie is correct after trie
     * enumeration. */
    void testWordCounting() {
        TestCaseReader<TSymbol> reader("dictionary/char_trie/test/wordCounting.txt", 2);
        bool read = false;
        while (reader.readData()) {
            read = true;
            WordList<TSymbol>* inputs = reader.getWords(0);
            TNodeArray* nodes = getEnumArray<TNodeArray>(*inputs);

            TEST_ASSERT(reader.getNumWords(1) == nodes->getSize());
            for (int i = 0; i < reader.getNumWords(1); ++i) {
                stringstream ss(reader.getWord(1, i));
                int numWords; ss >> numWords;

                stringstream m;                
                m << "Number of words in trie: " << (*nodes)[i].getNumWords()
                  << " expected number of words: " << numWords << " node index: " << i;
                TEST_ASSERT_MESSAGE(numWords == (*nodes)[i].getNumWords() , m.str());
            }

            delete inputs;
            delete nodes;
        }
        TEST_ASSERT(read);
    }

    void testGetWord() {
        TestCaseReader<TSymbol> reader("dictionary/char_trie/test/wordIndexing.txt", 2);
        bool read = false;
        int c = 1;
        while (reader.readData()) {
            read = true;
            WordList<TSymbol>* inputs = reader.getWords(0);

            TNodeArray* array = ArrayGetter<TNodeArray, TIterator>::get(*inputs);
            WordIndexer<TNodeArray, TIterator> indexer(*array);
            
//            cout<<nodeArrayToString(*array)<<endl;
//            for (int i = 0; i < array->getSize(); ++i) cout << (*array)[i].getNumWords() << " ";
//            cout<<endl;
            
            // assert all the words are retrieved correctly by index
            for (int i = 0; i < reader.getNumWords(0); ++i) {
                TSymbol* word = indexer.getWord(i);

                stringstream m;
                m << "test case #: " << c << " word index: " << i <<
                     " word: " << word << " expected: " << (*inputs)[i];

                TEST_ASSERT_MESSAGE(wordsEqual(word, (*inputs)[i]), m.str());
                delete word;
            }
            // assert that indexes out of range result in 0 pointers
            int N = reader.getNumWords(0);
            for (int i = N; i <= 2 * N; ++i) {
                TSymbol* word = indexer.getWord(i);

                stringstream m;
                m << "test case #: " << c << " word index: " << i
                  << " word: " << (word == 0 ? "null" : "not null")
                  << " expected: " << "null";

                TEST_ASSERT_MESSAGE(word == 0, m.str());
            }

            c++;

            delete inputs;
            delete array;
        }
        TEST_ASSERT(read);
    }

    void testGetIndex() {
        TestCaseReader<TSymbol> reader("dictionary/char_trie/test/wordIndexing.txt", 2);
        bool read = false;
        int c = 1;
        while (reader.readData()) {
            read = true;
            WordList<TSymbol>* inputs = reader.getWords(0);

            TNodeArray* array = ArrayGetter<TNodeArray, TIterator>::get(*inputs);
            WordIndexer<TNodeArray, TIterator> indexer(*array);
            typename WordIndexer<TNodeArray, TIterator>::WordSearchResult result;

            typename TNodeArray::Index ind = 0;
            for (int i = 0; i < reader.getNumWords(0); ++i) {
                result = indexer.getIndex(reader.getWord(0, i));

                stringstream m;
                m << "test case #: " << c << " word: " << reader.getWord(0, i)
                  << " calculated index: " << result.index << " expected: " << ind;

                TEST_ASSERT_MESSAGE(result.exists, m.str());
                TEST_ASSERT_MESSAGE(ind == result.index, m.str());
                
                ++ind;                
            }

            // the "results" are the words that must not be in the trie
            for (int i = 0; i < reader.getNumWords(1); ++i) {
                result = indexer.getIndex(reader.getWord(1, i));

                stringstream m;
                m << "test case #: " << c << " word: " << reader.getWord(1, i);

                TEST_ASSERT_MESSAGE(result.exists == false, m.str());
            }

            c++;

            delete inputs;
            delete array;
        }

        TEST_ASSERT(read);
    }

    /** Test index to word mapping with big dictionaries. */
    void testWithDictionary() {
        const char * dicts[] = {"english_small.txt", "polishMorph_small.txt"};
        int numDicts = 2;

        for (int i = 0; i < numDicts; ++i) {
            string dictFile = "test_files/"; dictFile += dicts[i];
            // create enumerated trie from dictionary
            WordFileReader<TSymbol> reader(dictFile);
            WordList<TSymbol>* words = reader.getWords();
            words->sort();

            TNodeArray* array = ArrayGetter<TNodeArray, TIterator>::get(*words);
            WordIndexer<TNodeArray, TIterator> indexer(*array);
            typename WordIndexer<TNodeArray, TIterator>::WordSearchResult result;
            
            /* For each word, assert that the trie returns the right index
             * and the right word for correct index. */
            TIndex ind = 0;
            for (size_t j = 0; j < words->numberOfWords(); ++j, ++ind) {
                const TSymbol *word = (*words)[j];
                // trie index and word
                result  = indexer.getIndex(word);
                TSymbol *tword = indexer.getWord(ind);

                stringstream m;
                m << "dictionary: " << dicts[i] << " word: " << word << " index: " << ind << endl
                        << " trie word: " << tword << " trie index: " << result.index;

                TEST_ASSERT_MESSAGE(wordsEqual(word, tword), m.str());
                TEST_ASSERT_MESSAGE(result.exists == true, m.str());
                TEST_ASSERT_MESSAGE(result.index == ind, m.str());
            }

            delete words;
            delete array;
        }

    }

};

#endif	/* ENUMTRIETEST_H */

