#ifndef TRIESTATS_H
#define	TRIESTATS_H

#include <iostream>

#include "Counter.hpp"
#include "node_array/na_utils.h"

typedef enum { NODE_CNT, SYMBOL_CNT, OFFSET_CNT } STAT_TYPE;

/** Class that calculates various statistics on the trie. */
template <typename TNodeArray>
class TrieStats {
public:

    TrieStats(): nodes(0) {}

    void computeStats(const TNodeArray* n);
    void outputStats(ostream& stream, STAT_TYPE type);

private:

    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

    const TNodeArray* nodes;

    Counter<string> nodeCounter;
    Counter<TSymbol> symbolCounter;
    Counter<TIndex> offsetCounter;

};

template <typename TNodeArray>
void TrieStats<TNodeArray>::computeStats(const TNodeArray* n) {
    nodes = n;

    nodeCounter.reset();
    symbolCounter.reset();
    offsetCounter.reset();

    for (TIndex i = 0; i < nodes->getSize(); ++i) {
        nodeCounter.inc(nodeToString((*nodes)[i]));
        symbolCounter.inc((*nodes)[i].getSymbol());
        offsetCounter.inc((*nodes)[i].getSibling());
    }
}


template <typename TNodeArray>
void TrieStats<TNodeArray>::outputStats(ostream& stream, STAT_TYPE stat) {
    bool indexes = true;

    if (stat == NODE_CNT) { 
        stream << "node occurences" << endl;
        nodeCounter.output(stream, indexes);
    }
    else if (stat == OFFSET_CNT) {
        stream << "length occurences" << endl;
        offsetCounter.output(stream, indexes);
    }
    else if (stat == SYMBOL_CNT) {
        stream << "symbol occurences" << endl;
        symbolCounter.output(stream, indexes);
    }
}

#endif	/* TRIESTATS_H */

