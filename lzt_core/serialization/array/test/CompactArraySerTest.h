#ifndef COMPACTARRAYSERTEST_H
#define	COMPACTARRAYSERTEST_H

#include <fstream>
#include <string>

#include "../CompactArraySerializer.h"
#include "dictionary/char_trie/Trie.h"
#include "dictionary/lz_trie/LzTrie.h"
#include "node_array/compact_array/CompactArrayCreator.h"
#include "util/TempFile.h"
#include "util/WordFileReader.h"
#include "compress/lz_compressor/LzCompressor.h"

using namespace std;

class CompactArraySerTest {
public:

    void testWithDictionaries();


};

template <typename TNodeArray>
class CompactArraySerTemplate {

private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

public:
    CompactArraySerTemplate() {};
    virtual ~CompactArraySerTemplate() {};

    void testWithDictionaries();

};

template <typename TNodeArray>
void CompactArraySerTemplate<TNodeArray>::testWithDictionaries() {

    int const numOfDicts = 1;
    char const * dicts[] = { "../dictionary_files/test/smallDict.txt",
                             "../dictionary_files/french.txt"};

    for (int i = 0; i < numOfDicts; ++i) {
        //TODO factor out Trie/NodeArray creation from dictionaries
        WordFileReader<TSymbol> reader(dicts[i]);
        Trie<TNodeArray> trie;

        reader.readWords();
        for (size_t j = 0; j < reader.getNumberOfWords(); ++j) {
            TSymbol const * word = reader.getWord(j);
            trie.insertWord(word);
        }
        //TODO fali delete nodes
        LzCompressor<TNodeArray> comp;
        TNodeArray& nodes = trie.exportNodeArray();
        comp.compressArray(nodes);

        CompactArrayCreator<TNodeArray> compactCreator(nodes);
        CompactArray<TSymbol, TIndex>* compactArray = compactCreator.createCompactArray();

        TempFile file;

        fstream stream(file.getName());
        CompactArraySerializer<TSymbol, TIndex> ser(compactArray);
        ser.arrayToStream(stream);
        stream.close();

        stream.open(file.getName());
        CompactArray<TSymbol, TIndex>* compArrayDeser = ser.arrayFromStream(stream);

        // test deserialzied and original node Array equality
        ostringstream message;
        message << "testWithDictionaries: " << dicts[i] << endl;
        string compArrayStr = nodeArrayToString(*compactArray);
        string compArrayDeserStr = nodeArrayToString(*compArrayDeser);
        message << "string array: " << compArrayStr << endl
                << "string array deser: " << compArrayDeserStr << endl;
        TEST_ASSERT_MESSAGE(compArrayStr == compArrayDeserStr, message.str());

        // LZTrie based on deserialized compact array must contain all words
        LzTrie<CompactArray<TSymbol, TIndex> > lztrie(*compArrayDeser);

        for (size_t j = 0; j < reader.getNumberOfWords(); ++j) {
            TSymbol const * word = reader.getWord(j);
            //TODO ovdje bi trebalo rjesiti genericki TSymbol* -> string
            ostringstream message2;
            message2 << message << endl << "word index: " << j;

            TEST_ASSERT_MESSAGE(lztrie.containsWord(word), message2.str() );
        }

        delete compactArray;
    }
}

#endif	/* COMPACTARRAYSERTEST_H */

