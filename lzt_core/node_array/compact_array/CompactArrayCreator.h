#ifndef COMPACTARRAYCREATOR_H
#define	COMPACTARRAYCREATOR_H

#include <set>
#include <algorithm>
#include <cstddef>
#include <cassert>

#include "utils.h"
#include "CompactArray.h"
#include "serialization/BitSequenceArray.h"
#include "serialization/BitSequence.h"
#include "serialization/serialization.h"
#include "serialization/SerializationUtils.h"
#include "util/utils.h"

using namespace std;

/** Creates a compact node array from a node array. */
template <typename TNodeArray>
class CompactArrayCreator {
    
public:

    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

    CompactArrayCreator(TNodeArray const & nodeArray);
    virtual ~CompactArrayCreator();

    CompactArray<TSymbol, TIndex>* createCompactArray();

private:

    TNodeArray const & nodeArray;
    CompactArray<TSymbol, TIndex>* compactArray;

    static const int NUM_OFFSETS = 4;

    // indexes of all distinct nodes
    TIndex* distinctInd;
    size_t numOfDistinct;
    size_t flagOffsets[NUM_OFFSETS];

    void selectDistinctIndexes();
    void sortIndexes();
    void calculateFlagOffsets();
    void copySymbols();
    void copySiblings();
    void copyNumOfWords();
    void copyNodeIndexes();

    class NodeIndexCompare {
    public:
        NodeIndexCompare(TNodeArray const & na): nodes(na) {}

        inline bool operator()(TIndex i1, TIndex i2) const {
            return nodes[i1] < nodes[i2];
        }

    private:
        TNodeArray const & nodes;
    };

    /** Strict weak ordering, compares two node indexes,
     * first by flags and than by other node data members. */
    class IndexOrdering {
    public:
        IndexOrdering(TNodeArray const & na): nodes(na) {}

        bool operator()(TIndex i1, TIndex i2) const {
            typename TNodeArray::NodeConst n1 = nodes[i1], n2 = nodes[i2];
            int flags1 = nodeFlagsToInt(n1.getEow(), n1.getCow()),
                flags2 = nodeFlagsToInt(n2.getEow(), n2.getCow());

            if (flags1 < flags2) return true;
            if (flags1 > flags2) return false;

            return n1 < n2;
        }

    private:
        TNodeArray const & nodes;
    };

};

template <typename TNodeArray>
CompactArrayCreator<TNodeArray>::CompactArrayCreator(TNodeArray const & na)
: nodeArray(na) { }

template <typename TNodeArray>
CompactArrayCreator<TNodeArray>::~CompactArrayCreator() { }

template <typename TNodeArray>
CompactArray<typename TNodeArray::Symbol, typename TNodeArray::Index>*
CompactArrayCreator<TNodeArray>::createCompactArray() {    
    // preprocess data
    selectDistinctIndexes();
    sortIndexes();

    // Create and fill the compact array.
    //TODO zahtjev na tip
    compactArray =
     new CompactArray<TSymbol, TIndex>
     (numOfDistinct, (size_t)nodeArray.getSize(), nodeArray.isEnumerated());

    // Store cow and eow flags of distinct nodes using offsets.
    calculateFlagOffsets();
    compactArray->setFlagOffsets(flagOffsets);

    // Copy the rest of node data.
    copySymbols();
    copySiblings();
    if (nodeArray.isEnumerated()) copyNumOfWords();

    // Copy entire array by storing indexes of nodes in the distinct node table.
    copyNodeIndexes();

    return compactArray;
}

/** Extract indexes of all distinct nodes. */
template <typename TNodeArray>
void CompactArrayCreator<TNodeArray>::selectDistinctIndexes() {
    NodeIndexCompare comp(nodeArray);
    typedef set<TIndex, NodeIndexCompare> TIndexSet;
    TIndexSet indexSet(comp);

    for (TIndex i = 0; i < nodeArray.getSize(); ++i) indexSet.insert(i);

    numOfDistinct = indexSet.size();
    distinctInd = new TIndex[numOfDistinct];

    typename TIndexSet::iterator it; size_t i = 0;
    for (it = indexSet.begin(); it != indexSet.end(); ++it)
        distinctInd[i++] = *it;    
}

/** Sort distinct indexes, first by eow-cow flags and than by other data members. */
template <typename TNodeArray>
void CompactArrayCreator<TNodeArray>::sortIndexes() {
    IndexOrdering lessThan(nodeArray);
    sort(distinctInd, distinctInd + numOfDistinct, lessThan);
}

/** Distinct nodes are sorted by cow and eow flags, as first criteria.
 * Here the starting indexes of flag combinations 01, 10 and 11 are calculated */
template <typename TNodeArray>
void CompactArrayCreator<TNodeArray>::calculateFlagOffsets() {
    int oldFlags = 0; flagOffsets[0] = 0;
    for (size_t i = 0; i < numOfDistinct; ++i) {
        typename TNodeArray::NodeConst n = nodeArray[distinctInd[i]];
        int flags = nodeFlagsToInt(n.getEow(), n.getCow());
        if (flags != oldFlags) {
            flagOffsets[flags] = i;
            for (size_t j = oldFlags+1; j < flags; ++j) flagOffsets[j] = i;
            oldFlags = flags;
        }
        // we've reached the last flag
        if (flags == NUM_OFFSETS - 1) break;
    }
    // mark nonexisting flags
    for (size_t i = oldFlags + 1; i < NUM_OFFSETS; ++i)
        flagOffsets[i] = numOfDistinct + 1;
}

/** Copy symbols from distinct nodes to compact array. */
template <typename TNodeArray>
void CompactArrayCreator<TNodeArray>::copySymbols() {
    // fill an array with distinct symbols
    TSymbol* distinctSymbols = new TSymbol[numOfDistinct];
    for (size_t i = 0; i < numOfDistinct; ++i) {
        typename TNodeArray::NodeConst n = nodeArray[distinctInd[i]];
        distinctSymbols[i] = n.getSymbol();
    }
    // create compact array
    compactArray->symbols.createFromArray(distinctSymbols, numOfDistinct);

    delete [] distinctSymbols;
}

/** Copy siblings from distinct nodes to compact array. */
template <typename TNodeArray>
void CompactArrayCreator<TNodeArray>::copySiblings() {
    // Store values from distinct nodes to array.
    TIndex* dist = new TIndex[numOfDistinct];
    for (size_t i = 0; i < numOfDistinct; ++i)
        dist[i] = nodeArray[distinctInd[i]].getSibling();
    // Copy sibling pointers to BitSequenceArray.
    SerializationUtils::positiveIntegersToBits<TIndex>
                        (dist, numOfDistinct, compactArray->siblings);

    delete [] dist;
}

/** Copy numOfWords from distinct nodes to compact array. */
template <typename TNodeArray>
void CompactArrayCreator<TNodeArray>::copyNumOfWords() {
    // Store values from distinct nodes to array.
    TIndex* dist = new TIndex[numOfDistinct];
    for (size_t i = 0; i < numOfDistinct; ++i)
        dist[i] = nodeArray[distinctInd[i]].getNumWords();
    // Copy sibling pointers to BitSequenceArray.
    SerializationUtils::positiveIntegersToBits<TIndex>
                        (dist, numOfDistinct, compactArray->numOfWords);

    delete [] dist;
}

/** For each node in nodeArray, find it's distinct index and copy
 * it to compact array at the position of the node. */
template <typename TNodeArray>
void CompactArrayCreator<TNodeArray>::copyNodeIndexes() {
    IndexOrdering lessThan(nodeArray);
    //TODO TIndex ili size_t
    for (TIndex i = 0; i < nodeArray.getSize(); ++i) {
        // run STL binary search to find distinct index of the node
        TIndex* di = lower_bound(distinctInd, distinctInd + numOfDistinct, i, lessThan);
        assert(nodeArray[*di] == nodeArray[i]);
        compactArray->setNodeIndex(i, (size_t)(di - distinctInd));
    }
}


#endif	/* COMPACTARRAYCREATOR_H */

