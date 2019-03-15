#ifndef TRIE_H
#define	TRIE_H

#include <cassert>
#include <cstdlib>
#include <string>
#include <iostream>

#include "dictionary/ILexicon.h"
#include "node_array/vector_array/VectorArray.h"
#include "node_array/types/symbol.h"
#include "node_array/na_utils.h"
#include "util/utils.h"

using namespace std;

/* Encapsulates operations on a character Trie.
 */
template <typename TNodeArray >
class Trie : public ILexicon<typename TNodeArray::Symbol> {
protected:

    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

public:

    Trie(TIndex capacity, bool enumerated = false);
    virtual ~Trie();
    TNodeArray& exportNodeArray();

    bool containsWord(const TSymbol* word);
    bool insertWord(const TSymbol* word);

    /* Insert a word into the trie, return true if the word wasn't
     * in the trie and was inserted. */
    template<typename TIterator>
    bool insert(TIterator word, TIndex length);

    TIndex getSize() const;
    bool isEmpty() const;

    const TNodeArray& getNodeArray() const;

protected:
    TNodeArray& nodes;

private:

    /* Describes the way string should be inserted in the trie and
     * if it's already inserted */
    enum InsertType {
        INSERTED, BRANCH, ADD_AT_END, PREFIX, EMPTY_TRIE
    };

    /* Data from searching a string of symbols in the trie
     * used for inserting a string */
    template<typename TIterator>
    struct InsertData {
        // node wich must be modified when inserting new word
        TIndex nodeIndex;
        // last processed symbol
        TIterator symbols;
        // no. of symbols to the end of the string, including the symbol
        // pointed to by symbols
        TIndex symbolsRem;
        // search result
        InsertType insertType;

        InsertData(InsertType r, TIndex ni, TIterator sy, TIndex syr) :
        insertType(r), nodeIndex(ni), symbols(sy), symbolsRem(syr) {
        }

        InsertData() {
        }

        void setData(InsertType r, TIndex ni, TIterator sy, TIndex syr) {
            insertType = r;
            nodeIndex = ni;
            symbols = sy;
            symbolsRem = syr;
        }

    };

    
private:
    
    bool nodesExported;

    template<typename TIterator>
    bool contains(TIterator word, TIndex length) const;

    void initNodeArray();

    /* Searches for a string within the trie. Return as search result
     * the data neccessary for inserting the string in the trie */
    template<typename TIterator>
    InsertData<TIterator> searchString(TIterator word, TIndex length) const;

    template<typename TIterator>
    void insertString(InsertData<TIterator> insertData);

    /* Add string to the end of the trie, by creating and initializing nodes.
     * No connecting of the added string is performed. */
    template<typename TIterator>
    void concatString(InsertData<TIterator> insertData);

    string arrayToString();
    /* if NodeArray's capacity isn't bigger than cap, resize it */
    //void ensureCapacity(TIndex cap);

};


// ********************* IMPLEMENTATION ********************* //

template <typename TNodeArray>
Trie<TNodeArray>::Trie(TIndex capacity = 0, bool enumerated) :
    nodes( *(new TNodeArray(capacity, enumerated)) ), nodesExported(false) {
    //TODO init nodes-a
}

template <typename TNodeArray>
Trie<TNodeArray>::~Trie() {
    if (nodesExported == false) delete &nodes;
}

template <typename TNodeArray>
typename TNodeArray::Index Trie<TNodeArray>::getSize() const {
    return nodes.getSize();
}

template <typename TNodeArray>
bool Trie<TNodeArray>::isEmpty() const {
    return nodes.getSize() == 0;
}

/** Exports nodes to outside world. Nodes can be altered
 * so trie acts as if it had no data. Deallocation responsibility
 * is also exported.
 * //TODO profini ovo ponasanje */
template <typename TNodeArray>
TNodeArray& Trie<TNodeArray>::exportNodeArray() {
    nodesExported = true;
    return nodes;
}

template <typename TNodeArray>
const TNodeArray& Trie<TNodeArray>::getNodeArray() const {
    return nodes;
}

template <typename TNodeArray>
bool Trie<TNodeArray>::containsWord(const TSymbol* word) {
    //TODO make search search null-terminated string without length
    TIndex length = sstringLength<TSymbol, TIndex>(word);
    InsertData<const TSymbol*> info = searchString(word, length);
    if (info.insertType == INSERTED) return true;
    else return false;
}

//TODO makni in due time
template <typename TNodeArray> template<class Titerator>
bool Trie<TNodeArray>::contains(Titerator symbols, TIndex length) const {
    //TODO eksplicitni length ili zeroSymbol na kraju?
    InsertData<Titerator> info = searchString(symbols, length);
    if (info.insertType == INSERTED) return true;
    else return false;
}

template <typename TNodeArray>
bool Trie<TNodeArray>::insertWord(const TSymbol* word) {
    TIndex length = sstringLength<TSymbol, TIndex>(word);
    return insert(word, length);
}

/** Try to insert a string of symbols given by an iterator, return true iff
 * it is inserted in the trie. */
template <typename TNodeArray> template<class Titerator>
bool Trie<TNodeArray>::insert(Titerator symbols, TIndex length) {
    InsertData<Titerator> info = searchString(symbols, length);
    if (info.insertType == INSERTED) return false;
    else {
        insertString(info);
        return true;
    }
}

template <typename TNodeArray> template<class Titerator>
void Trie<TNodeArray>::insertString(InsertData<Titerator> idata) {
    //ensureCapacity(idata.symbolsRem + this->getSize());
    if (idata.insertType == EMPTY_TRIE) {
        concatString(idata);
    } else if (idata.insertType == PREFIX) {
        nodes[idata.nodeIndex].setEow(true);
    } else if (idata.insertType == ADD_AT_END) {
        /* if this is the insert case, first symbol is the
        last symbol matched with trie node, and it must not
        be inserted, so it is discarded */
        assert(idata.symbolsRem > 1);
        // node for wich the last match occured must be at the end of trie
        assert(idata.nodeIndex == this->getSize() - 1);

        nodes[idata.nodeIndex].setCow(true);
        idata.symbolsRem--;
        idata.symbols++;
        concatString(idata);
    } else if (idata.insertType == BRANCH) {
        nodes[idata.nodeIndex].setSibling(this->getSize() - idata.nodeIndex);
        concatString(idata);
    }

}

/**
 * Append nodes containing given symbols to the trie's end
 */
template <typename TNodeArray> template<class Titerator>
void Trie<TNodeArray>::concatString(InsertData<Titerator> idata) {
    TIndex lastNode = nodes.getSize();
    nodes.createNodesAtEnd(idata.symbolsRem);

    for (; idata.symbolsRem; idata.symbolsRem--) {
        // get new node at the end of node array, inc. array size
        TNode n = nodes[lastNode++];

        n.setSymbol(*idata.symbols++);
        n.setSibling(0);

        // last symbol
        if (idata.symbolsRem == 1) {
            n.setEow(true);
            n.setCow(false);
        }// more symbols remaining
        else {
            n.setEow(false);
            n.setCow(true);
        }
    }
}

template <typename TNodeArray> template<class Titerator>
struct Trie<TNodeArray>::InsertData<Titerator>
Trie<TNodeArray>::searchString(Titerator symbols, TIndex length) const {
    InsertData<Titerator> result;

    // emtpy string is in the trie by default
    if (length == 0) {
        result.setData(INSERTED, 0, symbols, 0);
        return result;
    }

    // trie is empty
    if (this->isEmpty()) {
        result.setData(EMPTY_TRIE, 0, symbols, length);
        return result;
    }

    // index of the current position in the trie
    TIndex nodeIndex = 0;
    // index of the current symbol within the string
    TIndex symbolIndex = 0;
    InsertType insertType;

    while (true) {
        // current trie node
        TNode node = nodes[nodeIndex];
        // match between trie symbol and string symbol
        if (node.getSymbol() == *symbols) {
            // not the end of string
            if (symbolIndex < length - 1) {
                // more chars in current trie branch (continuation)
                if (node.getCow()) {
                    nodeIndex++;
                    symbolIndex++;
                    symbols++;
                }// end of trie word
                else {
                    insertType = ADD_AT_END;
                    break;
                }
            }// end of string
            else {
                if (node.getEow()) {
                    insertType = INSERTED;
                    break;
                }
                else {
                    insertType = PREFIX;
                    break;
                }
            }
        }// mismatch between trie symbol and string symbol
            else {
                if (node.getSibling() == 0) {
                    insertType = BRANCH;
                    break;
                }
                else {
                    nodeIndex = nodeIndex + node.getSibling();
                }
            }
    }

    result.setData(insertType, nodeIndex, symbols, length - symbolIndex);
    return result;
}

/** for debugging */
template <typename TNodeArray>
string Trie<TNodeArray>::arrayToString() {
    return
    nodeArrayToString<TSymbol, TIndex, VectorArray<TSymbol, TIndex> >(nodes);
}

#endif	/* TRIE_H */

