#ifndef LZCOMPRESSORTESTER_H
#define	LZCOMPRESSORTESTER_H

#include <cstring>
#include <fstream>

#include "LZCompressorTest.h"
#include "dictionary/char_trie/Trie.h"
#include "node_array/vector_array/VectorArray.h"
#include "util/TestCaseReader.h"
#include "node_array/na_utils.h"
#include "compress/lz_compressor/LzCompressor.h"

template <typename TSymbol, typename TIndex>
class LZCompressorTester {

public:

    void runTests();

    virtual ~LZCompressorTester() {}

private:

    void test(string fileName);

};

template <typename TSymbol, typename TIndex>
void LZCompressorTester<TSymbol, TIndex>::runTests() {
    test("../lzt_test/compress/testFiles/simpleTests.txt");
}

template <typename TSymbol, typename TIndex>
void LZCompressorTester<TSymbol, TIndex>::test(string fileName) {
    TestCaseReader<TSymbol> reader(fileName, 2);
    bool read = false;

    int testCaseCounter = 0;
    while (reader.readData()) {
        read = true;
        Trie<VectorArray<TSymbol, TIndex> > trie;

        for (int i = 0; i < reader.getNumWords(0); ++i)
            trie.insertWord(reader.getWord(0, i));

        string uncompressed =
        nodeArrayToString(trie.exportNodeArray());

        // compress node array
        LzCompressor<VectorArray<TSymbol, TIndex> > comp;        
        VectorArray<TSymbol, TIndex>& nodes = trie.exportNodeArray();
        comp.compressArray( nodes );


        string compressed =
        nodeArrayToString( nodes );

        string expected = reader.getWord(1, 0);

        ostringstream ss; ss << testCaseCounter;
        string message =
                "file: "+ fileName + ", test no.: " + ss.str() + "\n" +
                "uncompressed: " + uncompressed + "\n" +
                "result: " + compressed + "\n" + "expected: " + expected + "\n";
        
        CPPUNIT_ASSERT_MESSAGE(message, compressed == expected);
        testCaseCounter++;
    }
    
    CPPUNIT_ASSERT(read);
}

#endif	/* LZCOMPRESSORTESTER_H */

