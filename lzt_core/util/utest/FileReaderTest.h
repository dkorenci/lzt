#ifndef FILEREADERTEST_H
#define	FILEREADERTEST_H

#include <vector>
#include <sstream>
#include <iostream>

#include "../WordFileReader.h"
#include "../DictFileReader.h"
#include "../WordEquality.h"
#include "dictionary/lztrie_dict/WordPair.h"
#include "dictionary/util/WordList.h"
#include "debug/lzt_test.h"
#include "util/utils.h"

using namespace std;

class FileReaderTest {
public:

    virtual ~FileReaderTest();

    void testWordFileReader();
    void testDictFileReader();

    template <typename TSymbol>
    void testDictReader();
    
private:

};

/** Test by reading pairs of words from a file with empty line
 * and degenerate cases, and asserting equality with expected result. */
template <typename TSymbol>
void FileReaderTest::testDictReader() {
    // read dictionary

    vector<WordPair<TSymbol> >* pairs =
            DictFileReader<TSymbol>::readWords("util/utest/dictFileReaderTest.txt", '|');

    // read expected results
    WordFileReader<TSymbol> reader("util/utest/dictFileReaderResults.txt");
    reader.readWords();
    WordList<TSymbol>* results = reader.getWords();

    ostringstream m;
    m << "num. pairs: " << pairs->size() << " expected: " << results->numberOfWords() / 2 << endl;

    // assert correctness of pairs size
    TEST_ASSERT_MESSAGE(pairs->size() == results->numberOfWords()/2, m.str());

    // assert correctness of pairs
    for (size_t i = 0; i < pairs->size(); ++i) {
        ostringstream m;
        m << "pair: " << (*pairs)[i].first << " " << (*pairs)[i].second 
                << " expected: " << (*results)[i*2] << " " << (*results)[i*2+1] << endl;

        TEST_ASSERT_MESSAGE(wordsEqual( (*pairs)[i].first, (*results)[i*2] ), m.str());
        TEST_ASSERT_MESSAGE(wordsEqual( (*pairs)[i].second, (*results)[i*2+1] ), m.str()); 
    }

    DictFileReader<TSymbol>::freePairs(pairs);
    delete results;

}

#endif	/* FILEREADERTEST_H */

