#ifndef FACTORY_H
#define	FACTORY_H

#include <fstream>
#include <cstddef>

#include "dictionary/util/WordList.h"
#include "dictionary/char_trie/Trie.h"
#include "dictionary/lz_trie/LzTrie.h"
#include "dictionary/char_trie/WordIndexer.h"
#include "compress/lz_compressor/LzCompressor.h"
#include "node_array/compact_array/CompactArray.h"
#include "node_array/compact_array/CompactArrayCreator.h"
#include "serialization/array/CompactArraySerializer.h"
#include "compress/sa_compressor/LCPTreeCompressor.hpp"
#include "vizualization/NodeArrayVisualizer.hpp"
#include "node_array/na_utils.h"

using namespace std;

template <typename TNodeArray>
Trie<TNodeArray>* getTrie(WordList<typename TNodeArray::Symbol>& words, bool enumerated = false) {
    Trie<TNodeArray>* trie = new Trie<TNodeArray>(0, enumerated);

    for (size_t i = 0; i < words.numberOfWords(); ++i)
        trie->insertWord(words[i]);

    return trie;
}

/** Construct and return node array from a list of words. */
template <typename TNodeArray>
TNodeArray* getArray(WordList<typename TNodeArray::Symbol>& words) {
    Trie<TNodeArray>* trie = getTrie<TNodeArray>(words);
    TNodeArray* array = &(trie->exportNodeArray());
    delete trie;
    return array;
}

/** Construct and return enumerated node array from a list of words. */
template <typename TNodeArray>
TNodeArray* getEnumArray(WordList<typename TNodeArray::Symbol>& words) {
    Trie<TNodeArray>* trie = getTrie<TNodeArray>(words, true);
    cout << "trie created" << endl << flush;
    TNodeArray* array = &(trie->exportNodeArray());
    delete trie;

    WordIndexer<TNodeArray, typename TNodeArray::Index> windex(*array);
    windex.enumerate();    
    
    return array;
}

template <typename TNodeArray>
TNodeArray* getLzArray(WordList<typename TNodeArray::Symbol>& words, bool enumerated = false) {
    TNodeArray* array;
    if (enumerated) array = getEnumArray<TNodeArray>(words);
    else array = getArray<TNodeArray>(words);
    cout << "created array" << endl << flush;

    LzCompressor<TNodeArray> compressor;
    compressor.compressArray(*array);
    return array;
}

template <typename TNodeArray>
TNodeArray* printArray(TNodeArray& na) {
    NodeArrayVisualizer<TNodeArray> navis(na);
    cout << "-------------------" << endl;
    navis.printArray(cout);                
    cout << "-------------------" << endl;
    cout << nodeArrayToString(na) << endl;
    cout << "-------------------" << endl;
}

template <typename TNodeArray>
TNodeArray* getLzArrayLCT(WordList<typename TNodeArray::Symbol>& words, bool enumerated = false) {
    TNodeArray* array;
    if (enumerated) array = getEnumArray<TNodeArray>(words);
    else array = getArray<TNodeArray>(words);
    //LzCompressor<TNodeArray> compressor;
    LCPTreeCompressor<TNodeArray> compressor;
    compressor.compressArray(*array);    
    return array;
}

template <typename TNodeArray>
LzTrie<TNodeArray>* getLzTrie(WordList<typename TNodeArray::Symbol>& words, bool enumerated = false) {
    LzTrie<TNodeArray>* trie =
            new LzTrie<TNodeArray>(*getLzArray<TNodeArray>(words, enumerated));
    return trie;
}

/** Get Compact array constructed from lz-compressed TNodeArray */
template <typename TNodeArray>
CompactArray<typename TNodeArray::Symbol, typename TNodeArray::Index> *
getCompactedLzArray(WordList<typename TNodeArray::Symbol>& words, bool enumerated = false) {
    TNodeArray* nodes = getLzArray<TNodeArray>(words, enumerated);

    CompactArrayCreator<TNodeArray> creator(*nodes);
    CompactArray<typename TNodeArray::Symbol, typename TNodeArray::Index> *carray =
            creator.createCompactArray();

    delete nodes;
    return carray;
}

template <typename TSymbol, typename TIndex>
LzTrie<CompactArray<TSymbol, TIndex> > * getLzTrieFromCompressedFile(string file) {
    CompactArraySerializer<TSymbol, TIndex> ser;
    fstream stream(file.c_str());
    if (stream.good()) {
        CompactArray<TSymbol, TIndex>* array = ser.arrayFromStream(stream);

        LzTrie<CompactArray<TSymbol, TIndex> >* lzTrie =
                new LzTrie<CompactArray<TSymbol, TIndex> >(*array);

        return lzTrie;
    }
    else return NULL;
}

template <typename TSymbol, typename TIndex>
CompactArray<TSymbol, TIndex>* getCompactArrayFromCompressedFile(string file) {
    CompactArraySerializer<TSymbol, TIndex> ser;
    fstream stream(file.c_str());
    if (stream.good()) {
        CompactArray<TSymbol, TIndex>* array = ser.arrayFromStream(stream);
        return array;
    }
    else return NULL;
}

#endif	/* FACTORY_H */