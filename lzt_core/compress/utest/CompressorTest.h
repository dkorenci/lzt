#ifndef COMPRESSORTEST_H
#define	COMPRESSORTEST_H

#include <string>
#include <iostream>
#include <sstream>

#include "debug/lzt_test.h"
#include "node_array/vector_array/VectorArray.h"
#include "util/TestCaseReader.h"
#include "util/WordFileReader.h"
#include "util/factory.h"
#include "dictionary/util/WordList.h"
#include "compress/sa_compressor/SaCompressor.hpp"
#include "dictionary/lz_trie/LzTrie.h"
#include "compress/stats/StatCreator.hpp"

class CompressorTest {
public:

    void testCases();
    void testUpdateMin();
    void testDictionaries(TSA_COMP_ALGO method);
    void testCompressors();

};

template <typename TSymbol, typename TIndex>
class CompressorTester {
public:   

    void testCases();
    void testDictionaries(TSA_COMP_ALGO meth);
    void testRandomGroups();
    void testSaCompression(TSA_COMP_ALGO meth, WordList<TSymbol>* words, string message);
    void testCompressors();
    void testUpdateMin();

private:
    typedef VectorArray<TSymbol, TIndex> TNodeArray;

};

/** Test all the compressors on small dictionaries. */
template <typename TSymbol, typename TIndex>
void CompressorTester<TSymbol, TIndex>::testCompressors() {
    string dictFolder = "test_files/";
    string dictList = "../lzt_core/compress/utest/dictList.txt";

    TestCaseReader<TSymbol> reader(dictList, 1);

    TEST_ASSERT(reader.readData());

    StatCreator<TNodeArray> statcr(false, true, true);
    string dicts = "";
    for (int i = 0; i < reader.getNumWords(0); ++i) {
         string dictFile = (char*)reader.getWord(0, i);
         dicts = dicts + dictFile + " ";
    }

    string allCompressionAlgos =
            "SQR SQRb LCP LCPb LCPr LCPbr "
            "LPF LPFb LPFr LPFbr SEQ SEQb SEQr SEQbr LCT LCTp FSA FSAc";

    statcr.makeStats(allCompressionAlgos, dictFolder, dicts, "");
}

/**
 * Test with compression cases covering border situations.
 */
template <typename TSymbol, typename TIndex>
void CompressorTester<TSymbol, TIndex>::testCases() {
    string casesFile = "../lzt_core/compress/sa_compressor/utest/testCases.txt";

    TestCaseReader<TSymbol> reader(casesFile, 1);

    TEST_ASSERT(reader.readData());

    SaCompressor<TNodeArray> compressor;

    do {        
        TNodeArray* array = getArray<TNodeArray>(*reader.getWords(0));
        compressor.compressArrayGr(*array);
    } while (reader.readData());

}

/** Test with larger dictionary files. */
template <typename TSymbol, typename TIndex>
void CompressorTester<TSymbol, TIndex>::testDictionaries(TSA_COMP_ALGO meth) {
    int N = 5;
    string dictFiles[7];

    dictFiles[0] = "../dictionary_files/english.txt";
    dictFiles[1] = "../dictionary_files/german.txt";
    dictFiles[5] = "../dictionary_files/dimacs.txt";
    dictFiles[6] = "../dictionary_files/files.txt";
    dictFiles[2] = "../dictionary_files/french.txt";    
    dictFiles[3] = "../dictionary_files/esp.dict";
    dictFiles[4] = "../dictionary_files/polish.dict";

    for (int i = 0; i < N; ++i) {
        WordFileReader<TSymbol> reader(dictFiles[i]);
        WordList<TSymbol>* words = reader.getWords();
        testSaCompression(meth, words, "testDictionaries: " + dictFiles[i]);
    }
}

/** Test with larger dictionary files. */
template <typename TSymbol, typename TIndex>
void CompressorTester<TSymbol, TIndex>::testRandomGroups() {
    int N = 3, NUM_GROUP_SHUFFLE = 3;
    int nshuff[] = {1500, 300, 300};
    string dictFiles[7];

    dictFiles[3] = "../dictionary_files/english.txt";
    dictFiles[0] = "../dictionary_files/german.txt";
    dictFiles[5] = "../dictionary_files/dimacs.txt";
    dictFiles[6] = "../dictionary_files/files.txt";
    dictFiles[2] = "../dictionary_files/french.txt";
    dictFiles[1] = "../dictionary_files/esp.dict";
    dictFiles[4] = "../dictionary_files/polish.dict";
    
    for (int i = 0; i < N; ++i) {
        WordFileReader<TSymbol> reader(dictFiles[i]);
        WordList<TSymbol>* words = reader.getWords();

        size_t minSz = 1000000000;

        for (int j = 0; j < nshuff[i]; ++j) {
            TNodeArray* array = getArray<TNodeArray>(*words);

            SaCompressorConfig config;
            config.algorithm = LCP_INT_GR;
            config.grOrder = RANDOM;
            config.recReplace = true;

            SaCompressor<TNodeArray> compressor(config);

            compressor.compressArray(*array);

            if (array->getSize() < minSz) minSz = array->getSize();

            delete array;

//            string message = dictFiles[i];
//            message = message + " ,compression method: " + config.algoDescription();
//            LzTrie<TNodeArray> lzTrie(*array);
//            TSymbol emptyPrefix[1]; emptyPrefix[0] = zeroSymbol<TSymbol>();
//            WordList<TSymbol>* result = lzTrie.getWordsByPrefix(emptyPrefix);
//            bool wordsEq = (*result == *words);
//            if (wordsEq == false) {
//                fstream bugWords("bug_trie_words.txt", fstream::out);
//                for (size_t i = 0; i < result->numberOfWords(); ++i)
//                    bugWords << (*result)[i] << endl;
//
//                bugWords.close();
//
//
//                TEST_ASSERT_MESSAGE(*result == *words, message);
//            }
        }

        size_t lcpSize;
        {
            TNodeArray* array = getArray<TNodeArray>(*words);
            SaCompressorConfig config;
            config.algorithm = LCP_INT;
            config.recReplace = true;

            SaCompressor<TNodeArray> compressor(config);
            compressor.compressArray(*array);
            lcpSize = array->getSize();

            delete array;
        }

      TNodeArray* sqCompArr = getLzArray<TNodeArray>(*words);
      cout << "min.gr_comp.size: " << minSz
              << " lcp-int size: " << lcpSize 
              << " lz.size: " << sqCompArr->getSize() << endl;

      delete sqCompArr;

    }
}

template <typename TSymbol, typename TIndex>
void CompressorTester<TSymbol, TIndex>::
testSaCompression(TSA_COMP_ALGO meth, WordList<TSymbol>* words, string message) {
    TNodeArray* array = getArray<TNodeArray>(*words);
    TIndex trieSize = array->getSize();

    SaCompressorConfig config;
    config.algorithm = meth;
    config.recReplace = true;

     SaCompressor<TNodeArray> compressor(config);

     compressor.compressArray(*array);
  
    message = message + " , compression method: " + config.algoDescription();

    cout << message << endl;
   // compressor.printCompressStats();
    
    LzTrie<TNodeArray> lzTrie(*array);

    TSymbol emptyPrefix[1]; emptyPrefix[0] = zeroSymbol<TSymbol>();
    WordList<TSymbol>* result = lzTrie.getWordsByPrefix(emptyPrefix);

    TNodeArray* sqCompArr = getLzArray<TNodeArray>(*words);

    cout << "comp.size: " << array->getSize() << " lz.size: " << sqCompArr->getSize() << endl;

    bool wordsEq = (*result == *words);
    if (wordsEq == false) {
        fstream bugWords("bug_trie_words.txt", fstream::out);
        for (size_t i = 0; i < result->numberOfWords(); ++i)
            bugWords << (*result)[i] << endl;

        bugWords.close();


        TEST_ASSERT_MESSAGE(*result == *words, message);
    }
}

template <typename TSymbol, typename TIndex>
void CompressorTester<TSymbol, TIndex>::testUpdateMin() {
    /* Each test case consists of 2 sets, first is a string NM N n1 ... nN
     * where NM is number of mins in the tested interval followed by N and N numbers
     * to be inserted as mins. Second is a string M m1 ... mM
     * that holds expected state of smin value and mins array after insertion. */

    string casesFile = "../lzt_core/compress/utest/minsTestCases.txt";
    TestCaseReader<TSymbol> reader(casesFile, 2);

    int cc = 1;
    while (reader.readData()) {
        stringstream in(reader.getWord(0, 0)), out(reader.getWord(1, 0));
        

        TIndex NM, N;
        in >> NM >> N;
        typename SaCompressor<TNodeArray>::TLcpIntOpenM ival(0,0,NM);

        for (TIndex i = 0; i < N; ++i) {
            TIndex min;
            in >> min;
            ival.updateMin(min);
        }

        TIndex M; out >> M;
        stringstream mess;
        mess << "case: " << cc << " M: " << M << " smin: " << ival.smin << " array: ";
        for (int i = 0; i < ival.smin; ++i) mess << ival.mins[i] << " ";
        mess << endl;

        TEST_ASSERT_MESSAGE(ival.smin == M, mess.str());
        for (TIndex i = 0; i < M; ++i) {
            TIndex m; out >> m;
            stringstream mess2;
            mess2 << "i: " << i << " m: " << m;
            TEST_ASSERT_MESSAGE(m == ival.mins[i], mess.str() +  mess2.str());
        }

        cc++;
    }

}

#endif	/* COMPRESSORTEST_H */

