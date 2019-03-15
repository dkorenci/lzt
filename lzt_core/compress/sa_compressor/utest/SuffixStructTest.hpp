#ifndef SUFFIXSTRUCTTEST_HPP
#define	SUFFIXSTRUCTTEST_HPP

#include <string>

#include "util/TestCaseReader.h"
#include "debug/lzt_test.h"
#include "util/factory.h"
#include "util/WordFileReader.h"
#include "debug/lzt_test.h"
#include "../SuffixStructCreator.hpp"


using namespace std;

class SuffixStructTest {
public:    
    void testArrayCreation();

};

template <typename TNodeArray>
class SuffixStructTester {

private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

public:
    void testArrayCreation(bool lcp);

};

/** Test creation of either LCP array or Suffix Array, depending on lcp parameter. */
template <typename TNodeArray>
void SuffixStructTester<TNodeArray>::testArrayCreation(bool lcp) {
    string dictFolder = "test_files/";
    string dictList = "../lzt_core/compress/sa_compressor/utest/suffixTestDicts.txt";

    TestCaseReader<TSymbol> reader(dictList, 1);

    TEST_ASSERT(reader.readData());

    string dicts = "";
    for (int i = 0; i < reader.getNumWords(0); ++i) {
         string dictFile = (char*)reader.getWord(0, i);
         dictFile = dictFolder + dictFile;
         WordFileReader<TSymbol> reader(dictFile);
         WordList<TSymbol>* words = reader.getWords();

         TNodeArray* nodes = getArray<TNodeArray>(*words);

         SuffixStructCreator<TNodeArray> ssc;
         ssc.setNodes(nodes);
         // array created with brute force method and sophisticated algorithm
         TIndex* arrayBF, * array;

         if (lcp) {
            ssc.createSuffixArray();
            arrayBF = ssc.createLCPBruteForce();
            array = ssc.createLCPArray();
         }
         else {
            arrayBF = ssc.createSAwithSort();
            array = ssc.createSuffixArray();
         }
         
         string message = dictFile + " ";
         message = message + (lcp?"lcpArray":"suffixArray");

         TIndex maxIndex = lcp ? nodes->getSize() : nodes->getSize()-1;

         for (TIndex i = 0; i <= maxIndex; ++i) {
             if (array[i] != arrayBF[i]) {
                 cout << nodes->getSize() << endl;
                for (int j = 0; j <= maxIndex; ++j) cout << array[j] << " ";
                cout << endl;
                for (int j = 0; j <= maxIndex; ++j) cout << arrayBF[j] << " ";
                cout << endl;
                TEST_ASSERT_MESSAGE(array[i] == arrayBF[i], message);
             }
         }

         delete [] array;
         delete [] arrayBF;
         delete nodes;
    }
}

#endif	/* SUFFIXSTRUCTTEST_HPP */

