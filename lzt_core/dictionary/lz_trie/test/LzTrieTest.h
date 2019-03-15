#ifndef LZTRIETEST_H
#define	LZTRIETEST_H

#include <cstring>
#include <string>
#include <cstddef>

#include "../LzTrie.h"
#include "util/WordFileReader.h"
#include "util/TestCaseReader.h"
#include "node_array/vector_array/VectorArray.h"
#include "dictionary/util/WordList.h"
#include "util/factory.h"
#include "node_array/na_utils.h"
#include "debug/lzt_test.h"
#include "util/utils.h"
#include "util/regex.h"

class LzTrieTest {
public:
    void testListCases();
    void testListLarge();
private:

};

template <typename TSymbol, typename TIndex>
class LzTrieTester {

public:
    LzTrieTester();
    void testListCases();
    void testListLarge();

private:

    typedef VectorArray<TSymbol, TIndex> TNodeArray;

    static const int numOfCases = 1;
    char const * caseFiles[numOfCases];

    static const int numOfLarge = 2;
    char const *largeFiles[numOfLarge][2];

};

template <typename TSymbol, typename TIndex>
LzTrieTester<TSymbol, TIndex>::LzTrieTester() {
    caseFiles[0] = "../lzt_core/dictionary/lz_trie/test/listTestCase1.txt";

    largeFiles[0][0] = "../lzt_core/dictionary/lz_trie/test/listTestLarge1.txt";
    largeFiles[0][1] = "../lzt_core/dictionary/lz_trie/test/listQueriesLarge1.txt";

    largeFiles[1][0] = "../lzt_core/dictionary/lz_trie/test/listTestLarge2.txt";
    largeFiles[1][1] = "../lzt_core/dictionary/lz_trie/test/listQueriesLarge2.txt";
}

/** Test listing on a small special cases. */
template <typename TSymbol, typename TIndex>
void LzTrieTester<TSymbol, TIndex>::testListCases() {
    for (int i = 0; i < numOfCases; ++i) {
        TestCaseReader<TSymbol> reader(caseFiles[i], 2);
        TEST_ASSERT(reader.readData());
        WordList<TSymbol>* words= reader.getWords(0);
        // create compressed array from input words
        TNodeArray* array = getLzArray<TNodeArray>(*words);
        // check the result is as expected
        string arrayStr = nodeArrayToString(*array);
        const TSymbol* expected = reader.getWord(1, 0);
        // check that compression result is as expected
        TEST_ASSERT_MESSAGE(wordLength(expected) == arrayStr.size(), arrayStr);
        for (size_t i = 0; i < wordLength(expected); ++i)
            TEST_ASSERT(arrayStr[i] == expected[i]);
        
        delete words;

        // test listing cases
        LzTrie<TNodeArray> lzTrie(*array);
        while(reader.readData()) {
            TSymbol prefix[1000];
            getPrefixBeforeStar(reader.getWord(0, 0), prefix);
            WordList<TSymbol>* listing = lzTrie.getWordsByPrefix(prefix);

            if (reader.getNumWords(1) == 0) {
                TEST_ASSERT(listing == NULL);
                delete listing;
                continue;
            }
            
            //wordListToStream<TSymbol>(listing, cout);

            WordList<TSymbol>* expected = reader.getWords(1);
            TEST_ASSERT(*expected == *listing);

            delete listing;
            delete expected;
        }


        //delete array;
    }
}

/** Test listing on a larger dictionary. */
template <typename TSymbol, typename TIndex>
void LzTrieTester<TSymbol, TIndex>::testListLarge() {
    for (int i = 0; i < numOfLarge; ++i) {
        // create trie
        WordFileReader<TSymbol> fileReader(largeFiles[i][0]);
        WordList<TSymbol>* words = fileReader.getWords();
        
//        for (int j = 0; j < words->numberOfWords(); ++j)
//            cout<<(*words)[j]<<endl;

        LzTrie<TNodeArray>* trie = getLzTrie<TNodeArray>(*words);

        TestCaseReader<TSymbol> caseReader(largeFiles[i][1], 2);
        bool read = false;
        while (caseReader.readData()) {
            read = true;
            TSymbol prefix[1000];
            getPrefixBeforeStar(caseReader.getWord(0, 0), prefix);

            WordList<TSymbol>* expected = caseReader.getWords(1);
            WordList<TSymbol>* list = trie->getWordsByPrefix(prefix);

            if (caseReader.getNumWords(1) == 0) {
                TEST_ASSERT(list == NULL);
            }
            else {
               //wordListToStream<TSymbol>(list, cout);
               TEST_ASSERT(*expected == *list);
            }

            delete expected;
            delete list;
        }
        TEST_ASSERT(read);

        delete trie;
        delete words;
    }
}

#endif	/* LZTRIETEST_H */

