#ifndef LZTRIEDICTCREATOR_H
#define	LZTRIEDICTCREATOR_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>

#include "LzTrieDict.h"
#include "WordPair.h"
#include "dictionary/char_trie/Trie.h"
#include "node_array/vector_array/VectorArray.h"
#include "node_array/compact_array/CompactArray.h"
#include "node_array/compact_array/CompactArrayCreator.h"
#include "compress/lz_compressor/LzCompressor.h"
#include "dictionary/lztrie_dict/HuffmanMapCreator.h"
#include "dictionary/lztrie_dict/HuffmanIndexMap.h"
#include "util/WordComparator.h"
#include "serialization/BitVector.h"
#include "util/utils.h"
#include "compress/sa_compressor/LCPTreeCompressor.hpp"

using namespace std;

template <typename TSymbol, typename TIndex>
class LzTrieDictCreator {
public:
    
    virtual ~LzTrieDictCreator() {};
    
    void createDictionary(LzTrieDict<TSymbol, TIndex>* dict, 
            vector<WordPair<TSymbol> >* wordPairs,
            size_t segmentSize = DEFAULT_SEG_SIZE, bool old=false);
      
private:

    typedef VectorArray<TSymbol, TIndex> TNodeArray;
    typedef CompactArray<TSymbol, TIndex> TCompactArray;

    // dictionary being created
    LzTrieDict<TSymbol, TIndex>* dict;
    // pairs of words from which dictionary is created
    vector<WordPair<TSymbol> >* pairs;

    static const size_t DEFAULT_SEG_SIZE = 30;

    // important parameter of created huffmanIndexMap
    size_t segmentSize;

    void createIndex(bool first, bool old);
    void createHuffmanIndexMap();
    TNodeArray* buildTrie(bool first);

};

/** Create dictionary from pairs of words. */
template <typename TSymbol, typename TIndex> void LzTrieDictCreator<TSymbol, TIndex>::
createDictionary(LzTrieDict<TSymbol,TIndex>* d, vector<WordPair<TSymbol> >* p, 
                 size_t segSize, bool old) {
    dict = d; pairs = p; segmentSize = segSize;
    // create index of second members of pairs
    createIndex(false, old);
    // create index of first members of pairs
    createIndex(true, old);
    createHuffmanIndexMap();
}

/** Create uncompressed trie from either first or second members pairs array. */
template <typename TSymbol, typename TIndex>
typename LzTrieDictCreator<TSymbol, TIndex>::TNodeArray*
LzTrieDictCreator<TSymbol, TIndex>::buildTrie(bool first) {    
    TSymbol** words = new TSymbol*[pairs->size()];

    // project pairs to words array and sort it
    for (size_t i = 0; i < pairs->size(); ++i) {
        TSymbol* word;
        if (first) word = (*pairs)[i].first;
        else word = (*pairs)[i].second;

        words[i] = word;
    }

    WordComparator<TSymbol> compare;
    sort(words, words + pairs->size(), compare);

    // build trie
    Trie<TNodeArray> trie(0, true);
    for (size_t i = 0; i < pairs->size(); ++i)
        trie.insertWord(words[i]);

    TNodeArray* array =  &(trie.exportNodeArray());
    
    WordIndexer<TNodeArray, TIndex> indexer(*array);
    indexer.enumerate();

    delete [] words;

    return array;
}

/** Create index from either first or second members of word pairs, depending
 * on the parameter first. Index is created by sorting pairs lexicographicaly
 * by first or second member, and constructing lz-compressed trie dict->words for
 * first or dict->defs for second member. */
template <typename TSymbol, typename TIndex>
void LzTrieDictCreator<TSymbol, TIndex>::createIndex(const bool first, bool old) {
    TNodeArray* array = buildTrie(first);
    
    LzCompressor<TNodeArray> lzCompressor;
    LCPTreeCompressor<TNodeArray> lcptCompressor;
    if (old) lzCompressor.compressArray(*array);
    else lcptCompressor.compressArray(*array);

    CompactArrayCreator<TNodeArray> compacter(*array);
    CompactArray<TSymbol, TIndex>* compactArray = compacter.createCompactArray();

    delete array;

    TCompactArray** dictArray;
    WordIndexer<TCompactArray, LzTrieIterator<TCompactArray> >** dictIndex;
    LzTrie<TCompactArray> **dictTrie;

    // map array, index, and trie to either doman or codomain varaibles
    if (first) {
        dictArray = &(dict->domain);
        dictIndex = &(dict->domainIndex);
        dictTrie = &(dict->domainTrie);
    }
    else {
        dictArray = &(dict->codomain);
        dictIndex = &(dict->codomainIndex);        
    }
    // set array, and trie only for first members of pairs (domain words)
    *dictArray = compactArray;
    if (first) { 
        // deleting a trie also deletes the node array it is attached to
        if (*dictTrie != 0) delete *dictTrie;        
        *dictTrie = new LzTrie<TCompactArray>(*compactArray);
    }
    // set index
    if (*dictIndex != 0) delete *dictIndex;
    *dictIndex = new WordIndexer<TCompactArray, LzTrieIterator<TCompactArray> >(*compactArray);
}

/** Encode mapping of first to second members of pairs by mapping their
 * indexes, where index is a position in a lexicographicaly ordered array. */
template <typename TSymbol, typename TIndex>
void LzTrieDictCreator<TSymbol, TIndex>::createHuffmanIndexMap() {
    // uncompressed trie and indexer used to get indexes of codomain words
    TNodeArray* codomainTrie = buildTrie(false);
    WordIndexer<TNodeArray, TIndex> codomainIndexer(*codomainTrie);

    /* Sort lexicographicaly by first members. Sorting is stable so that
     * order of indexes (of second members) mapped from an index (of a first member)
     * is preserved. */
    WordPairCompare<TSymbol> compare(bool);
    // gcc produces an error if template arguments are not given explictily
    stable_sort<typename vector<WordPair<TSymbol> >::iterator,
                WordPairCompare<TSymbol> >(pairs->begin(), pairs->end(), compare);

    const size_t N = pairs->size();
    /* Copy mapping stored in pairs array to structures that will be
     * input for HuffmanMapCreator. */
    BitVector endings(N);
    TIndex* indexes = new TIndex[N];
    for (size_t i = 0; i < N; ++i) {
       typename WordIndexer<TNodeArray, TIndex>::WordSearchResult
        wsearch = codomainIndexer.getIndex((*pairs)[i].second);

       assert(wsearch.exists);
       indexes[i] = wsearch.index;

       bool sequenceEnd;
       // check if the sequence of pairs that have the same first member ends
       if (i == N - 1) sequenceEnd = true;
       else {
           if (wordsEqual((*pairs)[i].first, (*pairs)[i+1].first))
               sequenceEnd = false;
           else
               sequenceEnd = true;
       }
       endings.setBit(i, sequenceEnd);
    }

    delete codomainTrie;

    HuffmanMapCreator<TIndex> mapCreator(indexes, N, endings, segmentSize);
    mapCreator.createHuffmanMap(&(dict->mapping));

    delete [] indexes;
}

#endif	/* LZTRIEDICTCREATOR_H */

