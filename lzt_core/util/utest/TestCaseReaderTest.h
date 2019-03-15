#ifndef TESTCASEREADERTEST_H
#define	TESTCASEREADERTEST_H

#include <cctype>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "util/TestCaseReader.h"
#include "util/utils.h"
#include "debug/lzt_test.h"

using namespace std;

class TestCaseReaderTest {
public:
    void test();
private:

};

template <typename TSymbol>
class TestCaseReaderTester {
public:

    /** Structure with information about test file location and expected
     * results after reading the file with TestCaseReader. */
    struct TestCase {
        /** Name of the file with data that, when read by TestCaseReader,
         * should match expected results. */
        string fileName;
        size_t numSets;

        // expected results

        size_t numCases;
        // sizes of all sets in all cases
        size_t **setSizes;
        
        // words in all sets of all cases
        TSymbol ****words;
    };

    typedef typename vector<TestCase>::iterator Titer;

    void runTests();

private:

    vector<TestCase> getCases();
    void runCase(TestCase testCase);
    void freeCase(TestCase testCase);

};


/** Read a word from a line in a file.
 * Method is implemented as a function, not a method, to enable template specialization */
template <typename TSymbol> 
TSymbol* getWord(fstream& stream);

template <typename TSymbol>
void TestCaseReaderTester<TSymbol>::runTests() {
    vector<TestCase> cases = getCases();
    for (Titer it = cases.begin(); it != cases.end(); ++it) {
        runCase(*it);
        freeCase(*it);
    }
}

/** Open a testCase.fileName file, read it with TestCaseReader and
 * assert that it produces the same data that is written in the testCase. */
template <typename TSymbol>
void TestCaseReaderTester<TSymbol>::runCase(TestCase testCase) {    
    TestCaseReader<TSymbol> reader(testCase.fileName, testCase.numSets);
    size_t numCases = 0;

    while (reader.readData()) {
        for (int i = 0; i < reader.getNumSets(); ++i) {
            // compare numbers of words in the set
            ostringstream m;
            m << "file: " << testCase.fileName << endl 
              << "expected set size: " << testCase.setSizes[numCases][i] << endl
              << "read set size: " << reader.getNumWords(i) << endl;

            TEST_ASSERT_MESSAGE(
             testCase.setSizes[numCases][i] == reader.getNumWords(i), m.str());

            // compare exact words
            m << "words (expected|read) : " << endl;
            for (int j = 0; j < testCase.setSizes[numCases][i]; ++j)
                m << testCase.words[numCases][i][j] << "|" << reader.getWord(i, j) << endl;

            for (int j = 0; j < testCase.setSizes[numCases][i]; ++j) {
                TEST_ASSERT_MESSAGE(
                 wordsEqual(testCase.words[numCases][i][j], reader.getWord(i, j)), m.str());
            }            
        }

        numCases++;        
    }

    ostringstream m;
    m << "file: " << testCase.fileName << endl << "expected cases: "
            << testCase.numCases << "read cases: " << endl << numCases;
    TEST_ASSERT_MESSAGE(numCases == testCase.numCases, m.str());
}

/** Read TestCases from the file. File consists of lines non-empty string
 * without whitespaces, except for EOL's. First there's a line with number
 * of TestCases. Than for each TestCase, file with TestCaseReader data,
 * number of cases and number of sets the reader should produce.
 * Than follow cases (cases in TestCaseReader terminology, not to be
 * confused with TestCases) that contain sets. Each set contains number
 * of words followed by the words. */
template <typename TSymbol>
vector<typename TestCaseReaderTester<TSymbol>::TestCase>
TestCaseReaderTester<TSymbol>::getCases() {
    fstream file("util/utest/caseReaderTestCases.txt");
    vector<TestCase> cases;

    size_t N;
    // number of TestCase instances in the file
    file >> N;
    for (size_t i = 0; i < N; ++i) {
        TestCase tcase;
        file >> tcase.fileName;
        file >> tcase.numCases;
        file >> tcase.numSets;

        tcase.setSizes = new size_t* [tcase.numCases];
        tcase.words = new TSymbol*** [tcase.numCases];

        for (size_t j = 0; j < tcase.numCases; ++j) {
            tcase.setSizes[j] = new size_t[tcase.numSets];
            tcase.words[j] = new TSymbol** [tcase.numSets];
            for (size_t k = 0; k < tcase.numSets; ++k) {
                file >> tcase.setSizes[j][k];

                if (tcase.setSizes[j][k] > 0)
                    tcase.words[j][k] = new TSymbol* [tcase.setSizes[j][k]];

                for (size_t l = 0; l < tcase.setSizes[j][k]; ++l)
                    tcase.words[j][k][l] = getWord<TSymbol>(file);
            }
        }

        cases.push_back(tcase);
    }

    return cases;
}

/** Free all dynamicaly allocated memory in a test case. */
template <typename TSymbol>
void TestCaseReaderTester<TSymbol>::freeCase(TestCase testCase) {
    for (size_t i = 0; i < testCase.numCases; ++i) {
        for (size_t j = 0; j < testCase.numSets; ++j) {
            for (size_t k = 0; k < testCase.setSizes[i][j]; ++k)
                delete [] testCase.words[i][j][k];

            if (testCase.setSizes[i][j] > 0)
                delete [] testCase.words[i][j];
        }
        
        delete [] testCase.words[i];

        delete [] testCase.setSizes[i];        
    }
    delete [] testCase.words;
    delete [] testCase.setSizes;
}


#endif	/* TESTCASEREADERTEST_H */

