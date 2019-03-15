/* 
 * File:   IndexGroupingTester.h
 * Author: dam1root
 *
 * Created on February 22, 2011, 12:30 PM
 */

#ifndef INDEXGROUPINGTESTER_H
#define	INDEXGROUPINGTESTER_H

#include <cassert>

#include "compress/lz_compressor/IndexGrouping.h"

template<typename TNodeArray>
class IndexGroupingTester {
    
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

public:

    IndexGroupingTester(TSymbol* alpha, int alphaSize);
    virtual ~IndexGroupingTester();

    void runTests();

private:

    typedef typename TNodeArray::Node TNode;

    TSymbol *alphabet;
    int alphabetSize;

    void testIndexGrouping();

};

template<typename TNodeArray>
IndexGroupingTester<TNodeArray>::IndexGroupingTester(TSymbol* alpha, int alphaSize):
    alphabet(alpha), alphabetSize(alphaSize) { }

template<typename TNodeArray>
IndexGroupingTester<TNodeArray>::~IndexGroupingTester() {
}

template<typename TNodeArray>
void IndexGroupingTester<TNodeArray>::runTests() {
    testIndexGrouping();
}

/**
 * Create array with nodes that differ only by symbols. After
 * grouping, positions with same symbols should be grouped together,
 * and withing a group they should be sorted by position.
 */
template<typename TNodeArray>
void IndexGroupingTester<TNodeArray>::testIndexGrouping() {
    const int nodesPerSymbol = 5;
    TNodeArray array;
    //TODO rjesi pomocu indeksa, tj. rijesi pitanje konverzije index <-> int
    array.createNodesAtEnd(alphabetSize * nodesPerSymbol);

    // fill node array with data

    // node index
    TIndex ni = 0;
    for (int i = 0; i < nodesPerSymbol; ++i)
        for (int j = 0; j < alphabetSize; ++j) {
            TNode n = array[ni++];
            n.setSymbol(alphabet[j]);
            n.setCow(true); n.setEow(false);
            n.setSibling(0);
        }

    // group
    IndexGrouping<TNodeArray> grouping(array);

    // calculate last position
    TIndex lastPos = 0;
    for (int i = 0; i < nodesPerSymbol * alphabetSize - 1; ++i ) lastPos++;

    CPPUNIT_ASSERT(grouping.getNextInGroup(lastPos) == 0);

    // test grouping of positions
    ni = 0;
    for (int i = 0; i < nodesPerSymbol; ++i)
        for (int j = 0; j < alphabetSize; ++j) {
            CPPUNIT_ASSERT(grouping.getNextInGroup(ni) != lastPos);

            // set k to index of the next node with the same symbol
            TIndex k = ni;
            for (int l = 0; l < alphabetSize; ++l) k++;

            // not the last node in the group
            if (i < nodesPerSymbol - 1) {
                /* this element should have no match because next node with
                    the same symbol is the last node */
                if (i == nodesPerSymbol - 2 && j == alphabetSize - 1)
                    CPPUNIT_ASSERT(grouping.getNextInGroup(ni) == 0);
                else
                    CPPUNIT_ASSERT(grouping.getNextInGroup(ni) == k);
            }
            // last node in the group
            else {
                CPPUNIT_ASSERT(grouping.getNextInGroup(ni) == 0);
            }

            ni++;
        }

}

#endif	/* INDEXGROUPINGTESTER_H */

