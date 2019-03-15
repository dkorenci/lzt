#ifndef COMPACTARRAYTEST_H
#define	COMPACTARRAYTEST_H

#include <string>
#include <sstream>

#include "dictionary/char_trie/Trie.h"
#include "dictionary/lz_trie/LzTrie.h"
#include "compress/lz_compressor/LzCompressor.h"
#include "util/WordFileReader.h"
#include "node_array/na_utils.h"
#include "debug/lzt_test.h"

#include "../CompactArray.h"
#include "../CompactArrayCreator.h"

class CompactArrayTest {

public:

    void simpleTests();
    void testWithDictionaries();
    void testSymbolArrayCreate();
    void testSymbolArraySerialize();

};

template <typename TNodeArray>
class CompactArrayTestTemplate {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

public:
    virtual ~CompactArrayTestTemplate() {};

    void testWithDictionaries();
    void simpleTests();

};

/** Read a number of small arrays, each for some special case of compactification,
 * and verifiy they get compacted correctly. */
template <typename TNodeArray>
void CompactArrayTestTemplate<TNodeArray>::simpleTests() {
    WordFileReader<TSymbol> reader("../lzt_core/node_array/compact_array/test/stringArrays.txt");
    reader.readWords();

    for (size_t i = 0; i < reader.getNumberOfWords(); ++i) {
        string strArray = reader.getWord(i);
        TNodeArray* array = nodeArrayFromString<TNodeArray>(strArray);
        CompactArrayCreator<TNodeArray> compactCreator(*array);
        CompactArray<TSymbol, TIndex>* compactArray = compactCreator.createCompactArray();
        string strCompArray = nodeArrayToString(*compactArray);

        ostringstream message; message << "i: " << i << endl;
        message << "uncompacted: " << strArray << endl;
        message << "compacted: " << strCompArray << endl;

        TEST_ASSERT_MESSAGE(strArray == strCompArray, message.str());

        delete array;
        delete compactArray;
    }
}

/** Test CompactNodeArray creation and access by creating compressed TNodeArray,
 * compacting it and comparing two array node by node. Also test that LZTrie
 * based on compact array contains all the words.  */
template <typename TNodeArray>
void CompactArrayTestTemplate<TNodeArray>::testWithDictionaries() {

    int const numOfDicts = 1;
    char const * dicts[] = { "../dictionary_files/test/smallDict.txt", "../dictionary_files/french.txt" };
    
    for (int i = 0; i < numOfDicts; ++i) {
        ostringstream message;
        message << "testWithDictionaries: " << dicts[i];

        WordFileReader<TSymbol> reader(dicts[i]);
        Trie<TNodeArray> trie;

        reader.readWords();
        for (size_t j = 0; j < reader.getNumberOfWords(); ++j) {
            TSymbol const * word = reader.getWord(j);
            trie.insertWord(word);
        }

        LzCompressor<TNodeArray> comp;
        TNodeArray& nodes = trie.exportNodeArray();
        comp.compressArray(nodes);        

        CompactArrayCreator<TNodeArray> compactCreator(nodes);
        CompactArray<TSymbol, TIndex>* compactArray = compactCreator.createCompactArray();

        // test nodeArray and compact node Array equality
        string nodesString = nodeArrayToString(nodes);
        string compactString = nodeArrayToString(*compactArray);
        TEST_ASSERT_MESSAGE(nodesString == compactString, message.str());

        // LZTrie based on compact array must contain all words
        LzTrie<CompactArray<TSymbol, TIndex> > lztrie(*compactArray);

        for (size_t j = 0; j < reader.getNumberOfWords(); ++j) {
            TSymbol const * word = reader.getWord(j);
            //TODO ovdje bi trebalo rjesiti genericki TSymbol* -> string
            ostringstream message2;
            message2 << message << endl << "word index: " << j;

            TEST_ASSERT_MESSAGE(lztrie.containsWord(word), message2.str() );
        }
    }

}


#endif	/* COMPACTARRAYTEST_H */

