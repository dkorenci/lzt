#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <fstream>
#include <climits>
#include <vector>
#include <sys/stat.h>

#include "dictionary/lztrie_dict/huffman/utest/HuffmanTest.h"
#include "serialization/test/BitVectorTest.h"
#include "serialization/BitVector.h"
#include "serialization/test/BitSequenceArrayTest.h"
#include "dictionary/lztrie_dict/HuffmanMapCreator.h"
#include "serialization/test/BitSequenceTest.h"
#include "dictionary/lztrie_dict/utest/HuffmanIndexMapTest.h"
#include "serialization/test/IntBitArrayTest.h"
#include "dictionary/lz_trie/LzTrieIterator.h"
#include "node_array/vector_array/VectorArray.h"
#include "dictionary/lz_trie/test/LzTrieTest.h"
#include "dictionary/lz_trie/test/LzTrieIteratorTest.h"
#include "dictionary/char_trie/test/WordIndexerTest.h"
#include "util/utest/FileReaderTest.h"
#include "dictionary/lztrie_dict/LzTrieDict.h"
#include "dictionary/lztrie_dict/LzTrieDictCreator.h"
#include "dictionary/lztrie_dict/WordPair.h"
#include "dictionary/lztrie_dict/utest/LzTrieDictTest.h"
#include "util/utest/TestCaseReaderTest.h"
#include "suffix_array/CharStringSA.h"
#include "compress/sa_compressor/SaCompressor.hpp"
#include "compress/utest/CompressorTest.h"
#include "compress/stats/TrieStats.hpp"
#include "compress/stats/StatCreator.hpp"
#include "vizualization/TrieToStringProjector.hpp"
#include "vizualization/NodeArrayStringWrapper.hpp"
#include "vizualization/NodeArrayVisualizer.hpp"
#include "node_array/concepts/NodeArrayStatic.h"
#include "vizualization/VisualisationRunner.hpp"
#include "compress/sa_compressor/SuffixStructCreator.hpp"
#include "compress/sa_compressor/LCPTreeCompressor.hpp"
#include "fsa_convert/FsaConverter.hpp"
#include "compress/sa_compressor/utest/SuffixStructTest.hpp"

using namespace std;

void printSA() {
//    const char *s = "acaaacatat";
    const char *s = "aabbaabb";
//    const char *s = "abcd0abcd1abcx2abcy3abcz4abcz5";

    CharStringSA sarray(s);
    sarray.printSA("suffix_array.txt");
//    sarray.computeLCPInt(false);
    sarray.computeLPF();
}

void testStats() {
    StatCreator<VectorArray<unsigned char, int> > cr(true, true, true);

    string files1 = "english.txt german.txt";
    string files2 = "english.txt german.txt french.txt russian.txt esp.dict polish.dict random.txt dimacs.txt";
    // a bug occurrs when lz-compressing files.txt
    string files3 = "english.txt german.txt french.txt russian.txt esp.dict polish.dict random.txt dimacs.txt";
    // a bug occurs at dimacs.txt
    string files4 = "english.txt german.txt french.txt russian.txt esp.dict polish.dict random.txt";

    string files5 = "english.txt german.txt french.txt russian.txt";

    string files6 = "esp.dict polish.dict random.txt dimacs.txt polishMorphDict.txt";

    string testLCP = "english.txt german.txt french.txt russian.txt esp.dict polish.dict";

   string allCompressionAlgos =
            "SQR SQRb LCP LCPb LCPr LCPbr "
            "LPF LPFb LPFr LPFbr SEQ SEQb SEQr SEQbr";

   //cr.makeStats("SQR LPF LPFr", "../dictionary_files", files5, "../statistics");
   //cr.makeStats("SQR LCP", "../dictionary_files", files5, "../statistics");
   //cr.makeStats("LCPd", "../dictionary_files", "french.txt", "../statistics");
   string lctFiles1 = "english.txt german.txt french.txt esp.dict polish.dict";
   string lctFiles2 = "russian.txt random.txt dimacs.txt";
   string standardDicts = "english.txt german.txt french.txt russian.txt esp.dict polish.dict random.txt dimacs.txt";
   
   //cr.makeStats("LCT", "../dictionary_files", "english.txt german.txt french.txt polish.dict", "../statistics");
   cr.makeStats("LCTm", "../dictionary_files", standardDicts , "../statistics");
}

void manualComprStart() {
    // create node array from file
    string dict = "german.txt";
    string folder = "../dictionary_files/";
    WordFileReader<char> reader(folder + dict);
    WordList<char>* words = reader.getWords();    
    VectorArray<char, unsigned int>* array = getArray<VectorArray<char, unsigned int> >(*words);

     // call LZT compressor
    typedef VectorArray<char, unsigned int>  TNodeArray;
    SaCompressorConfig config; config.debug = false;
    SaCompressor<TNodeArray> sacompr(config);
    sacompr.compressArray(*array);

//    StatCreator<TNodeArray> statcr(false, true, true);
//    statcr.makeStats("LCT", folder, dict, "");

//    FsaConverter<TNodeArray> converter1(true);
//    TNodeArray* array1 = converter1.compressWords(folder+"english.txt");
//    delete array1;
//    FsaConverter<TNodeArray> converter2(true);
//    TNodeArray* array2 = converter2.compressWords(folder+"german.txt");
//    delete array2;

    // test result
    //StatCreator<TNodeArray>::testCompression("LCT", array, words, dict, true);

    // call lcptree compressor
//    LCPTreeCompressor<TNodeArray> lcptc;
//    lcptc.compressArray(*array);

    delete words;
    delete array;
}

void testMins() {
    CompressorTest ctest;
    ctest.testCompressors();
}

size_t getFileSize(const char* fname) {
    struct stat buf;
    stat(fname, &buf);
    return buf.st_size;
}

void serializeDictionaries() {
    string dictFolder = "../dictionary_files/split/";
    const int ND = 1; // number of dictionaries
    const char * dicts[] = {"pol_morph.txt", "english.txt.split", "german.txt.split",
        "french.txt.split", "russian.txt.split", "esp.dict.split",
        "polish.dict.split", "random.txt.split", "dimacs.txt.split"};

    typedef char TSymbol; typedef int TIndex;
    const TSymbol separator = '+';

    for (int i = 0; i < ND; ++i) {
        vector<WordPair<TSymbol> >* pairs =
            DictFileReader<TSymbol>::readWords(dictFolder+dicts[i], separator);

        // create dictionary
        LzTrieDictCreator<TSymbol, TIndex> dictCreator;
        LzTrieDict<TSymbol, TIndex> dict;
       
        int segSize = 1000;
        dictCreator.createDictionary(&dict, pairs, segSize);

        // write dictionary to file
        const char* dictFile = "dict.out";
        fstream output(dictFile, ios_base::out);
        dict.writeToStream(output);
        output.close();
        // write dictinary components to file
        const char* dictFile1 = "domain.out";
        fstream strdom(dictFile1, ios_base::out);
        const char* dictFile2 = "codomain.out";
        fstream strcod(dictFile2, ios_base::out);
        const char* dictFile3 = "mapping.out";
        fstream strmap(dictFile3, ios_base::out);                
        dict.writeToStreamDistinct(strdom, strcod, strmap);
        strdom.close(); strcod.close(); strmap.close();        

        cout << dicts[i] << "dict size: " << getFileSize(dictFile)
                << " domain size: " << getFileSize(dictFile1)
                << " codomain size: " << getFileSize(dictFile2)
                << " mapping size: " << getFileSize(dictFile3) << endl;
        
        DictFileReader<TSymbol>::freePairs(pairs);
    }
    
}

void calculateNodeSets() {
    string dictFolder = "../dictionary_files/";
    const int ND = 8; // number of dictionaries
    const char * dicts[] = { "english.txt", "german.txt",
        "french.txt", "russian.txt", "esp.dict",
        "polish.dict", "random.txt", "dimacs.txt", "pol_morph.txt"};

    typedef char TSymbol; typedef int TIndex;
    typedef VectorArray<TSymbol, TIndex> TNodeArray;
    const TSymbol separator = '|';

    for (int i = 0; i < ND; ++i) {
        string fileName = dictFolder + dicts[i];
        WordFileReader<TSymbol> reader(fileName);
        WordList<TSymbol>* words = reader.getWords();

        TNodeArray* nodes = getArray<TNodeArray>(*words);
        delete words;
        
        SuffixStructCreator<TNodeArray> suffixer;
        suffixer.setNodes(nodes);
        cout << dicts[i] << endl;
        int* ind = suffixer.indexNodes();
        delete [] ind;

        delete nodes;
    }

}

void testSuffixes() {
    SuffixStructTest test;   
    test.testArrayCreation();
}

int main(int argc, char** argv) {    
    //manualComprStart();
    testStats();
    //testSuffixes();
}