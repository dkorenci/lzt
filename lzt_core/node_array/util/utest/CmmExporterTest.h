#ifndef CMMEXPORTERTEST_H
#define	CMMEXPORTERTEST_H

#include <cstdlib>
#include <sstream>

#include "node_array/vector_array/VectorArray.h"
#include "../CmmExporter.h"
#include "util/factory.h"
#include "util/WordFileReader.h"
#include "dictionary/util/WordList.h"
#include "dictionary/lz_trie/LzTrie.h"

class CmmExporterTest {
public:

    void test();

};

template <typename TNodeArray>
class CmmExporterTester {
private:
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Symbol TSymbol;
    
public:

    /** For each test dictionary, lz compress it, export trie and compare
     * word list with Ristov's tool run with exported trie. */
    void test() {
        const char * dicts[] = {"english.txt", "french.txt"};
        int numDicts = 1;

        string exportedArray = "rjecnik.cmm";
        string cmmWordsFile = "list.txt";
        string listCommand = "node_array/util/utest/search_trie > " + cmmWordsFile;        

        for (int i = 0; i < numDicts; ++i) {
            string dictFile = "../dictionary_files/"; dictFile += dicts[i];
            // create enumerated trie from dictionary
            WordFileReader<TSymbol> reader(dictFile);
            WordList<TSymbol>* words = reader.getWords();
            words->sort();
            TNodeArray* array = getLzArray<TNodeArray>(*words);
                    //&((getTrie<TNodeArray>(*words))->exportNodeArray());            
            CmmExporter<TNodeArray>::writeToFile(*array, exportedArray);
            
            int s = system(listCommand.c_str());

            WordFileReader<TSymbol> reader2(cmmWordsFile);
            WordList<TSymbol>* cmmWords = reader2.getWords();

            stringstream m; m << "dictionary: " << dicts[i];

            TEST_ASSERT_MESSAGE(*cmmWords == *words, m.str());

            string remArray = "rm ";

            s = system((remArray + exportedArray).c_str());
            s = system((remArray + cmmWordsFile).c_str());

            delete array;
            delete words;
            delete cmmWords;
        }

    }
};

#endif	/* CMMEXPORTERTEST_H */

