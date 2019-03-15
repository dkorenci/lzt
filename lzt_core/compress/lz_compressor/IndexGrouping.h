#ifndef INDEXGROUPING_H
#define	INDEXGROUPING_H

#include <vector>
#include <algorithm>
#include <cstdio>

#include "node_array/types/symbol.h"

using namespace std;

/**
 * Groups (indexes of) locations in a node array, when compressing
 * for each location the remainder of the group is being scanned
 * to find redirection candidates.
 */
template <typename TNodeArray>
class IndexGrouping {

private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

    TIndex *suffixArray;
    // location of a node in a suffix array
    TIndex* saLocation;

    TNodeArray& nodeArray;

    // TODO rijesi s manje memorije
    // is a position in suffix array end of group
    vector<bool> isGroupEnd;


public:

    IndexGrouping(TNodeArray& nodes):
        nodeArray(nodes), suffixArray(NULL), saLocation(NULL) {
            makeGroups();
        }

    virtual ~IndexGrouping() {
        delete [] suffixArray;
        delete [] saLocation;
    }

    TIndex getNextInGroup(TIndex i) {
        TIndex i1 = saLocation[i]; TIndex i2 = i1; i2++;

        // index is at the end of suf. array
        if (i2 == nodeArray.getSize()) return 0;
        // check weather index is last in it's group
        if ( isGroupEnd[i1] == false )
            return suffixArray[i2];
        else
            return 0;
    }

private:

    void makeGroups() {
        suffixArray = new TIndex[nodeArray.getSize()];
        saLocation = new TIndex[nodeArray.getSize()];
        isGroupEnd.resize(nodeArray.getSize(), false);

        for (TIndex i = 0; i < nodeArray.getSize(); i++)
            suffixArray[i] = i;

        SuffixComparator comp(nodeArray);
        sort(suffixArray, suffixArray + nodeArray.getSize(), comp);

        for (TIndex i = 0; i < nodeArray.getSize(); i++)
            saLocation[suffixArray[i]] = i;

        // mark group ends
        for (TIndex i = 0; i < nodeArray.getSize(); i++) {
            TIndex next = i; next++;
            if (next == nodeArray.getSize()) continue;
            // check if first two chars don't match
            if (equal(suffixArray[i], suffixArray[next]) == false)
                isGroupEnd[i] = true;
        }

//        for (int i = 0; i < nodeArray.getSize(); ++i)
//            printf("%c ", nodeArray[suffixArray[i]].getSymbol());
//        printf("\n");

    }

    // compare first two nodes at two positions in the array
    bool equal(TIndex i1, TIndex i2) {
        if (i1 == i2) return true;

        const int len = 2;
        for (int i = 0; i < len; ++i) {
            TNode n1 = nodeArray[i1], n2 = nodeArray[i2];

            if (n1 < n2) return false;
            if (n2 < n1) return false;

            i1++; i2++;

            /* check if we hit the end of array, but before
               len nodes were compared, wich means one sequence
               cannot be of length len and differs from the other */
            if (i < len-1) {
                if (i1 == nodeArray.getSize()) return false;
                if (i2 == nodeArray.getSize()) return false;
            }
        }

        return true;
    }

    class SuffixComparator {
    public:

        SuffixComparator(TNodeArray &narray):
            nodes(narray) {}

        /* compare sequences (of length 2) of nodes starting at given indexes,
           return true iff seq1 < seq2 */
        inline bool operator()(TIndex s1, TIndex s2) const {
            if (s1 == s2) return false;

            const int len = 2;
            for (int i = 0; i < len; ++i) {
                TNode n1 = nodes[s1], n2 = nodes[s2];

                if (n1 < n2) return true;
                if (n2 < n1) return false;
                
                s1++; s2++;

                /* check if we hit the end of array, but before
                   len nodes were compared, wich means one sequence
                   cannot be of length len and is less than the other */
                if (i < len-1) {
                    if (s1 == nodes.getSize()) return true;
                    if (s2 == nodes.getSize()) return false;
                }
            }

            /* original s1 and s2 should be compared, but relation
             * holds if both indexes are increased for the same amount
             */
            return (s1 < s2);
        }

    private:

        TNodeArray &nodes;
       
    };

};

#endif	/* INDEXGROUPING_H */

