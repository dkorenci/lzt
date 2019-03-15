#ifndef WORDLISTTEST_H
#define	WORDLISTTEST_H

#include <strstream>

#include "../WordList.h"

#include "debug/lzt_test.h"
#include "util/WordFileReader.h"
#include "util/TestCaseReader.h"
#include "util/WordEquality.h"

class WordListTest {
public:

    void testEquals();
    void testSort();
    void testConstruction();

};

template <typename TSymbol>
class WordListTester {

public:

    void testEquals() {
        const int numOfTests = 2;
        char const * testFiles[] = {"../lzt_core/dictionary/util/test/unequalTests.txt",
                                    "../lzt_core/dictionary/util/test/equalTests.txt"};
        for (int i = 0; i < numOfTests; ++i) {
            TestCaseReader<TSymbol> reader(testFiles[i], 2);
            bool read = false;
            int j = 1;
            while (reader.readData()) {
                read = true;
                bool equal = (bool)i;

                WordList<TSymbol>* list1 = reader.getWords(0);
                WordList<TSymbol>* list2 = reader.getWords(1);

                ostringstream m;
                m << "file: " << testFiles[i] << " testCase: " << j << endl;
                TEST_ASSERT_MESSAGE((*list1 == *list2) == equal, m.str());

                delete list1;
                delete list2;
                j++;
            }
            TEST_ASSERT(read);
        }
    }

    void testSort() {
        const int numOfTests = 2;
        char const * testFiles[] = { "../lzt_core/dictionary/util/test/sortTests.txt",
                                     "../lzt_core/dictionary/util/test/sortTestBig.txt"};
        WordEquality<TSymbol> equal;
        for (int i = 0; i < numOfTests; ++i) {
            TestCaseReader<TSymbol> reader(testFiles[i], 2);
            bool read = false;
            int j = 1;
            while (reader.readData()) {
                read = true;
                WordList<TSymbol>* list1 = reader.getWords(0);
                WordList<TSymbol>* list2 = reader.getWords(1);

                list1->sort();

                ostringstream m;
                m << "file: " << testFiles[i] << " testCase: " << j << endl;

                TEST_ASSERT_MESSAGE(list1->numberOfWords() == list2->numberOfWords(), m.str());
                
                for (size_t k = 0; k < list1->numberOfWords(); ++k) {
                    ostringstream m2; m2 << m.str();
                    m2 <<"k: "<<k<<" list1[k] "<<(*list1)[k]<<" list2[k] "<<(*list2)[k]<<endl;
                    
                    TEST_ASSERT_MESSAGE( equal( (*list1)[k], (*list2)[k] ), m2.str() );
                }                

                delete list1;
                delete list2;
                j++;
            }
            TEST_ASSERT(read);
        }
    }

};

#endif	/* WORDLISTTEST_H */

