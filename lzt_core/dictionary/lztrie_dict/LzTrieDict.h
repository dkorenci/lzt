#ifndef LZTRIEDICT_H
#define	LZTRIEDICT_H

#include <vector>
#include <iostream>
#include <cassert>

#include "node_array/compact_array/CompactArray.h"
#include "dictionary/lz_trie/LzTrie.h"
#include "dictionary/char_trie/WordIndexer.h"
#include "dictionary/lztrie_dict/HuffmanIndexMap.h"
#include "dictionary/lz_trie/LzTrieIterator.h"
#include "serialization/array/CompactArraySerializer.h"
#include "dictionary/util/WordList.h"
#include "WordPair.h"
#include "util/utils.h"

using namespace std;

/** Dictionary based on two LzTries for storing words and IndexMap that maps
 * (word) indexes to indexes using Huffman coding to encode differences between
 * consecutive indexes. */
template <typename TSymbol, typename TIndex>
class LzTrieDict {
public:

    LzTrieDict();
    virtual ~LzTrieDict();

    vector<TSymbol*> lookupWord(const TSymbol* word);
    vector<WordPair<TSymbol> > findByPrefix(const TSymbol* word);

    void readFromStream(istream& stream);
    void writeToStream(ostream& stream) const;
    void writeToStreamDistinct(ostream& str1, ostream& str2, ostream& str3) const;

    template <typename TS, typename TI> friend class LzTrieDictCreator;

private:

    // node array type used to store words in a compact form
    typedef CompactArray<TSymbol, TIndex> TCompactArray;
    // indexer of compact, lz-compressed array
    typedef WordIndexer<TCompactArray, LzTrieIterator<TCompactArray> > TCompactIndexer;

    // words that are mapped from, stored as an lz-trie
    TCompactArray *domain;
    // words that are mapped to, stored as an lz-trie
    TCompactArray *codomain;

    // indexer of words that are mapped from
    TCompactIndexer* domainIndex;
    // indexer of words that are mapped to
    TCompactIndexer* codomainIndex;

    LzTrie<TCompactArray> *domainTrie;

    // mapping between the words
    HuffmanIndexMap<TIndex> mapping;

};

template <typename TSymbol, typename TIndex>
LzTrieDict<TSymbol, TIndex>::LzTrieDict()
: domain(0), codomain(0), domainIndex(0), codomainIndex(0), domainTrie(0) { }

template <typename TSymbol, typename TIndex>
LzTrieDict<TSymbol, TIndex>::~LzTrieDict() {
    if (domainTrie != 0) delete domainTrie;
    if (codomain != 0) delete codomain;
    if (domainIndex != 0) delete domainIndex;
    if (codomainIndex != 0) delete codomainIndex;
}

/** Return sequence of words (terminated by a NULL pointer) in the dictionary
 * associated with given word. */
template <typename TSymbol, typename TIndex>
vector<TSymbol*> LzTrieDict<TSymbol, TIndex>::lookupWord(const TSymbol* word) {
    // get index of the word
    vector<TSymbol*> result;
    typename TCompactIndexer::WordSearchResult wsearch = domainIndex->getIndex(word);

    // if word is not in the domain, return empty vector
    if (wsearch.exists == false) return result;

    // get indexes of all associated words
    vector<TIndex> indexes = mapping.getIndexes(wsearch.index);
    
    // get words for associated indexes
    typename vector<TIndex>::iterator it;
    for (it = indexes.begin(); it != indexes.end(); ++it)       
        result.push_back( codomainIndex->getWord(*it) );
    
    return result;
}

/** Return all the words in the dictionary that are associated with a word
 * with a given prefix. Duplicate words can occur. */
//TODO write test for findByPrefix
template <typename TSymbol, typename TIndex>
vector<WordPair<TSymbol> > LzTrieDict<TSymbol, TIndex>::findByPrefix(const TSymbol* prefix) {
    WordList<TSymbol>* words =
        domainTrie->getWordsByPrefix(prefix);

    vector<WordPair<TSymbol> > result;
    // no prefix found, return empty result
    if (words == 0) return result;

    for (size_t i = 0; i < words->numberOfWords(); ++i) {
        //cout<<(*words)[i]<<endl;
        typename TCompactIndexer::WordSearchResult
            wsearch = domainIndex->getIndex( (*words)[i] );
        assert(wsearch.exists);
        // get indexes of all associated words
        vector<TIndex> indexes = mapping.getIndexes(wsearch.index);
        // get words for associated indexes and store to result
        typename vector<TIndex>::iterator it;
        for (it = indexes.begin(); it != indexes.end(); ++it) {
            WordPair<TSymbol> pair;
            pair.first = wordClone( (*words)[i] );
            pair.second = codomainIndex->getWord(*it);
            result.push_back(pair);
        }
    }

    delete words;

    return result;
}

template <typename TSymbol, typename TIndex>
void LzTrieDict<TSymbol, TIndex>::writeToStream(ostream& stream) const {
    CompactArraySerializer<TSymbol, TIndex> serd(domain);
    serd.arrayToStream(stream);
    CompactArraySerializer<TSymbol, TIndex> serc(codomain);
    serc.arrayToStream(stream);

    mapping.writeToStream(stream);
}

// For measuring sizes of dictionary components, quick and dirty solution. 
template <typename TSymbol, typename TIndex>
void LzTrieDict<TSymbol, TIndex>::
writeToStreamDistinct(ostream& str1, ostream& str2, ostream& str3) const {
    CompactArraySerializer<TSymbol, TIndex> serd(domain);
    serd.arrayToStream(str1);
    CompactArraySerializer<TSymbol, TIndex> serc(codomain);
    serc.arrayToStream(str2);

    mapping.writeToStream(str3);
}

template <typename TSymbol, typename TIndex>
void LzTrieDict<TSymbol, TIndex>::readFromStream(istream& stream) {
    //free memory
    // deleting the trie must also delete associated node array
    if (domainTrie != 0) delete domainTrie;
    if (codomain != 0) delete codomain;
    if (domainIndex != 0) delete domainIndex;
    if (codomainIndex != 0) delete codomainIndex;
    
    CompactArraySerializer<TSymbol, TIndex> ser;
    domain = ser.arrayFromStream(stream);
    domainTrie = new LzTrie<TCompactArray>(*domain);

    codomain = ser.arrayFromStream(stream);

    domainIndex =
    new TCompactIndexer(*domain);

    codomainIndex =
    new TCompactIndexer(*codomain);

    mapping.readFromStream(stream);
}

#endif	/* LZTRIEDICT_H */

