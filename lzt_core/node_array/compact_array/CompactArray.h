#ifndef COMPACTARRAY_H
#define	COMPACTARRAY_H

#include <cstddef>
#include <cassert>
#include <iostream>

#include "utils.h"
#include "CompactArrayNode.h"
#include "serialization/BitSequenceArray.h"
#include "serialization/serialization.h"
#include "CompactSymbolArray.h"

/** Implementation of const NodeArray concept.
 * It's a node array in wich only distinct nodes are stored and the array is a
 * sequence of indexes pointing to these nodes. Eow and Cow flags are not
 * stored, they are calculated from index position. */
template <typename TSymbol, typename TIndex>
class CompactArray {

public:
       
    virtual ~CompactArray();

    typedef TSymbol Symbol;
    typedef TIndex Index;
    typedef CompactArrayNode<TSymbol, TIndex> NodeConst;

    //TODO promjeniti tip parametra u size_t
    NodeConst operator[](TIndex i) const;
    TIndex getSize() const;

    template <typename TNodeArray> friend class CompactArrayCreator;
    template <typename TS, typename TI> friend class CompactArraySerializer;

private:

    CompactArray(bool enumerated = false);
    CompactArray(size_t numOfDistinct, size_t numOfNodes, bool enumerated = false);

    static const int NUM_OFFSETS = 4;

    void reserveNodeSpace(size_t size);
    void setFlagOffsets(size_t flagOffsets[NUM_OFFSETS]);
    void setNodeIndex(size_t position, size_t nodeIndex);

    int flagsFromPosition(size_t p) const;

    void printIndexes() const;

    size_t numOfDistinct;
    size_t numOfNodes;
    size_t bitsPerIndex;

    // true if the numOfWords is stored for each node
    bool enumerated;

    /* Cow and eow flags are deduced from index, these are the borders of
     * blocks of nodes with same flag combinations. For each combination (coded
     * as number in [0,3]), nodes with that flag values lie in the interval
     * [ flagOffsets[flags], flagOffsets[flags+1] - 1 ]. */
    size_t flagOffsets[NUM_OFFSETS];

    BitSequenceArray array;    
    BitSequenceArray siblings;
    // number of words a node contains
    BitSequenceArray numOfWords;
    CompactSymbolArray<TSymbol> symbols;
    
};

int numberOfBits(size_t numberOfValues);

/** Default constructor, safe to use only for deserialization. */
template <typename TSymbol, typename TIndex>
CompactArray<TSymbol, TIndex>::CompactArray(bool e)
: numOfDistinct(0), numOfNodes(0), bitsPerIndex(0), enumerated(e) { }

template <typename TSymbol, typename TIndex>
CompactArray<TSymbol, TIndex>::CompactArray(size_t distinct, size_t nodes, bool e)
: numOfDistinct(distinct), numOfNodes(nodes), enumerated(e),
   bitsPerIndex(numberOfBits(distinct)), array(nodes, bitsPerIndex)
{ }

template <typename TSymbol, typename TIndex>
CompactArray<TSymbol, TIndex>::~CompactArray() { }

template <typename TSymbol, typename TIndex>
TIndex CompactArray<TSymbol, TIndex>::getSize() const {
    return numOfNodes;
}

template <typename TSymbol, typename TIndex>
void CompactArray<TSymbol, TIndex>::printIndexes() const {
    for (size_t i = 0; i < numOfNodes; ++i) {
        size_t index = fromBitSequence<size_t>(array[i]);
        cout<<index<<endl;
    }
}

template <typename TSymbol, typename TIndex>
inline CompactArrayNode<TSymbol, TIndex>
CompactArray<TSymbol, TIndex>::operator[](TIndex i) const {
    CompactArrayNode<TSymbol, TIndex> node;
    // decode node-table index of a node
    BitSequence indexBits = array[i];
    size_t index = fromBitSequence<size_t>(indexBits, bitsPerIndex);

    // get silbing and symbol data from the table
    node.sibling = numberFromBits<TIndex>(siblings[index], siblings.getSequenceSize());
    // TODO TIndex type operation (conversion to size_t)
    node.symbol = symbols[(size_t)index];

    // calculate flag values
    int flags = flagsFromPosition(index);
    node.eow = intFlagsEow(flags);
    node.cow = intFlagsCow(flags);

    // get numberOfWords if array is enumerated
    if (enumerated) {
        node.numOfWords = numberFromBits<TIndex>(numOfWords[index], numOfWords.getSequenceSize());
        node.enumerated = true;
    }
    else node.enumerated = false;

    return node;
}

/** Set nodeIndex on position i in the BitSequenceArray, wich means
 * that i-th node in the array will be the distinct node at position nodeIndex */
template <typename TSymbol, typename TIndex>
void CompactArray<TSymbol, TIndex>::setNodeIndex(size_t i, size_t nodeIndex) {    
    array.setSequence(i, toBitSequence(nodeIndex, bitsPerIndex));
}

/** Calculate int representation of eow-cow, from position
 * in distinct nodes array.  */
template <typename TSymbol, typename TIndex>
int CompactArray<TSymbol, TIndex>::flagsFromPosition(size_t p) const {
    for (int i = 0; i < NUM_OFFSETS; ++i) {
        size_t start = flagOffsets[i], end;
        // calculate end of range for current flags
        if (i < NUM_OFFSETS - 1) end = flagOffsets[i+1];
        else end = numOfDistinct;

        if (start <= p && p < end) return i;
    }
    // flags must be calculated
    assert(false);
}

template <typename TSymbol, typename TIndex>
void CompactArray<TSymbol, TIndex>::setFlagOffsets(size_t offsets[NUM_OFFSETS]) {
    for (int i = 0; i < NUM_OFFSETS; ++i) flagOffsets[i] = offsets[i];
}

//template <typename T>
//BitSequence toBitSequence(T t);

#endif	/* COMPACTARRAY_H */

