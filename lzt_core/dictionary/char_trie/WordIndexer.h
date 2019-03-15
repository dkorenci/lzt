#ifndef ENUMTRIE_H
#define	ENUMTRIE_H

#include <cassert>
#include <vector>
#include <iostream>

#include "Trie.h"
#include "node_array/types/symbol.h"
#include "node_array/na_utils.h"

/** Index to word and word to index mapping on enumerated (lz)trie. Indexer
 * also enumerates (non-lz) trie to init word indexing data. */
template <typename TNodeArray, typename TIterator>
class WordIndexer {

private:
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::NodeConst TNodeConst;

public:
    WordIndexer(TNodeArray& nodes);
    virtual ~WordIndexer() {};

    void enumerate();

    // Result of the getIndex query
    struct WordSearchResult {
        bool exists;
        TIndex index;
    };

    WordSearchResult getIndex(const TSymbol* word) const;
    TSymbol* getWord(TIndex i) const;

private:

    TNodeArray& nodes;

    TIndex recursiveEnum(TIndex nodeIndex);

};

template <typename TNodeArray, typename TIterator>
WordIndexer<TNodeArray, TIterator>::WordIndexer(TNodeArray& n) : nodes(n) {}

/** Set numWords for each node to be number of words in the trie that contains
 * that node. NodeArray must be enumerated and non-compressed. */
template <typename TNodeArray, typename TIterator>
void WordIndexer<TNodeArray, TIterator>::enumerate() {
    // call recursive enumeration for each first level node
    TIndex i = 0;
    while (true) {
        recursiveEnum(i);

        if (nodes[i].getSibling() == 0) break;
        else i += nodes[i].getSibling();
    }
}

/** Calculate, set and return number of words that contain a node with given index. */
template <typename TNodeArray, typename TIterator>
typename TNodeArray::Index WordIndexer<TNodeArray, TIterator>::recursiveEnum(TIndex i) {
    typedef typename TNodeArray::Node TNode;
    
    TIndex numWords = 0;
    TNode node = nodes[i];
    // there is a word that ends with this node
    if (node.getEow()) numWords++;

    if (node.getCow()) { // there are children, visit them
        ++i; // first child is the next node
        while (true) {
            numWords += recursiveEnum(i);
            if (nodes[i].getSibling() == 0) break;
            else i += nodes[i].getSibling();
        }
    }

    if (node.getSibling() == 0) node.setNumWords(0);
    else node.setNumWords(numWords);

    return numWords;
}

/** For a zero-based index i, get word i-th word in the
 * enumerated trie i.e. i-th word in lexicographical order. If i is larger
 * than the number of words in the trie, return 0. */
template <typename TNodeArray, typename TIterator> typename TNodeArray::Symbol*
WordIndexer<TNodeArray, TIterator>::getWord(typename TNodeArray::Index wi) const {
    // we work with const node array to allow only const operations
    const TNodeArray& nodes = this->nodes;
    vector<TSymbol> buff;
    // node index
    TIterator ni = IterInit<TIterator, TNodeArray>::get(nodes);
    /* In every iteration, wi is 0-based word index among words in a subtree
     * containing current node and all its children and siblings visited by dfs. */
    while(true) {
        TNodeConst node = nodes[ni];
        while (node.getSibling() != 0 && node.getNumWords() <= wi) {
            wi -= node.getNumWords();
            ni += node.getSibling();
            node = nodes[ni];
        }
        
        buff.push_back(node.getSymbol());
        
        // check is the word is found
        if (node.getEow()) {
            // this condition must happen if trie is correctly structured
            if (wi == 0) break;
            else wi--;
        }
        // move to node's child
        if (node.getCow()) ++ni;
        else {
            /* End of trie traversal and word not found,
             * index was greater then number of word in the trie. */           
            return 0;
        }

        assert(ni < nodes.getSize());
        assert(wi >= 0);
    }

    // copy buffer to new word and return
    TSymbol* word = new TSymbol[buff.size()+1];    
    for (size_t i = 0; i < buff.size(); ++i) word[i] = buff[i];
    word[buff.size()] = zeroSymbol<TSymbol>();
    return word;
}

/** Get zero-based index of a word in a lexicographicaly sorted set of words
 * storen in enumerated trie. Returns nodes.getSize() if word is not found. */
template <typename TNodeArray, typename TIterator>
typename WordIndexer<TNodeArray, TIterator>::WordSearchResult
WordIndexer<TNodeArray, TIterator>::getIndex(const typename TNodeArray::Symbol* word) const {
    // we work with const node array to allow only const operations
    const TNodeArray& nodes = this->nodes;
    
    WordSearchResult fail; fail.exists = false;
    WordSearchResult success; success.exists = true;
    // empty word is not in the trie
    if (word[0] == zeroSymbol<TSymbol>()) return fail;

    // node index
    TIterator ni = IterInit<TIterator, TNodeArray>::get(nodes);
    // word index
    TIndex wi = 0;
    // symbol index
    TIndex si = 0;

    while(true) {
        TNodeConst node = nodes[ni];
        // pick a right sibling branch, modifying word index
        while (node.getSibling() != 0 && node.getSymbol() != word[si]) {
            wi = wi + node.getNumWords();
            ni += node.getSibling();
            node = nodes[ni];
        }

        if (node.getSymbol() == word[si]) {            
            if (node.getEow()) {
                if (word[si+1] == zeroSymbol<TSymbol>()) {
                    success.index = wi;
                    return success;
                }
                else wi++;
            }            
            if (node.getCow()) {
                si++;
                if (word[si] == zeroSymbol<TSymbol>()) return fail;
                ++ni;
            }
            else return fail;
        }
        else return fail;        
    }

    success.exists = true;
    success.index = wi;

    return success;
}

#endif	/* ENUMTRIE_H */

