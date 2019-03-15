#ifndef LZCOMPRESSOR_H
#define	LZCOMPRESSOR_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cassert>
#include <cstdlib>

#include "compress/ICompressor.h"
#include "IndexGrouping.h"
#include "node_array/types/symbol.h"
#include "node_array/na_utils.h"
#include "util/utils.h"
#include "LzCompressorDebugger.h"
#include "vizualization/NodeArrayVisualizer.hpp"
#include "vizualization/NodeArrayToDot.hpp"

using namespace std;

template <typename TNodeArray>
class IndexGrouping;

struct LzCompressorConfig {
    LzCompressorConfig():  pointerBreak(false), visualize(false), graphViz(false) {}
    bool pointerBreak;
    bool visualize;
    bool graphViz;
};

template <typename TNodeArray>
class LzCompressor : public ICompressor<TNodeArray> {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    //TODO do const operations with NodeConst?
    typedef typename TNodeArray::Node TNode;

public:

    LzCompressor();
    LzCompressor(LzCompressorConfig conf);
    
    virtual ~LzCompressor() {};

    void compressArray(TNodeArray& na);

    string description();

    template <typename nodearray> 
    friend class LzCompressorDebugger;

private:

    void initAlgorithmData();
    void clearData();

    bool isReplaced(TIndex i);
    void setReplaced(TIndex i);

    bool isBaseBorder(TIndex i);
    void setBaseBorder(TIndex i);

    TSymbol findMaxMatch(TIndex i, TIndex j);
    TIndex getReplacedSegmentEnd(TIndex i);
    void replaceMatchedPosition(TIndex b, TIndex r, TSymbol match);
    TIndex markReplacedAndGetSegmentEnd(TIndex r, TSymbol match);
    TIndex stepThroughArray(TIndex b, TSymbol s);
    TIndex calculateExpandedLength(TIndex p, TSymbol match);
    void correctPointers();
    void compactArray();
    void correctReplacedSizes();

    //for debug
    LzCompressorDebugger<TNodeArray> debug;

    TNodeArray* nodes;
    //number of nodes in the array
    TIndex N;
    //number of nodes int he array before the compression
    TIndex uncompressedN;
    vector<bool>* replaced;
    vector<bool>* baseBorder;

    // ps[i] is index of a node wich points to node i, if such exists
    TIndex* prevSibling;
    TIndex* repSegmentEnd;
    // full length of a sequence replaced by a pointer
    TIndex* expandedLength;
    IndexGrouping<TNodeArray> *indexGrouping;

    LzCompressorConfig config;
    // stream for visualization data
    fstream vstream;

};

template <typename TNodeArray>
LzCompressor<TNodeArray>::LzCompressor():  debug(*this) {
    config.pointerBreak = false;
}

template <typename TNodeArray>
LzCompressor<TNodeArray>::LzCompressor(LzCompressorConfig conf):
    debug(*this), config(conf) {
}

template <typename TNodeArray>
string LzCompressor<TNodeArray>::description() {
    string desc = "sqr";
    desc.append(".b:");
    desc.append(config.pointerBreak?"1":"0");
    return desc;
}

//TODO this method should return void and copress the array
template <typename TNodeArray>
void LzCompressor<TNodeArray>::compressArray(TNodeArray& na) {
    nodes = &na; N = nodes->getSize();
    uncompressedN = N;
    initAlgorithmData();

    if (config.visualize) {
        NodeArrayVisualizer<TNodeArray> visualizer(*nodes);
        vstream << "start: " << endl;
        visualizer.printArray(vstream);
    }

    int cnt = 0; int step = 1; int fcnt = 0;

    for (TIndex i = 0; i < N; i++) {
        if (isReplaced(i)) continue;

        for(TIndex j = indexGrouping->getNextInGroup(i);
            j != 0; j = indexGrouping->getNextInGroup(j) ) {

            if (isReplaced(j)) continue;
            if (isBaseBorder(j)) continue;
            
            TSymbol mmatch = findMaxMatch(i, j);

            if (mmatch)
                replaceMatchedPosition(i, j, mmatch);

            if (config.visualize && mmatch) {
                NodeArrayVisualizer<TNodeArray> visualizer(*nodes);
                vstream << "i j: " << i << " " << j << endl;
                visualizer.printArray(vstream);
            }

            if (config.graphViz && mmatch) {
                cnt++;
                if (cnt % step == 0) {
                    fcnt++;
                    NodeArrayToDot<TNodeArray> graphViz(nodes);
                    char number[20];
                    sprintf(number,"%d",fcnt);
                    string fname = "frames3/trie"; fname += number; fname += ".dot";
                    graphViz.write(fname);
                }
            }
        }
    }

    //TODO odmah dealocirati memoriju koja nije potrebna    
    correctPointers();
    compactArray();
    correctReplacedSizes();
    clearData();
}

/**
 * Find the number match such that array[i,match] == array[j,match] and
 * all other trie and compression constraints are satisfied for both
 * subsequences. array[k,l] means start at k and go until l of normal
 * nodes or pointers (only first level, ie not pointers inside 
 * subsequences replaced by a pointer) is reached.
 */
template <typename TNodeArray>
typename TNodeArray::Symbol LzCompressor<TNodeArray>::findMaxMatch(TIndex i, TIndex j) {
    assert(i < j);
    /* ovaj assert ne mora biti zadovoljen ako na
     * dvije na pocetku iste pozicije budu upisani razliciti pointeri 
     * assert( ((*nodes)[i]) == ((*nodes)[j]) ); */
    //TODO novi zahtjev na tip
    /* number of characters matched, excluding the first one.
     * for that reason match is not increased after the first chars of
     * subsequences are matched. */
    TSymbol match = 0;
    TIndex startj = j;

    /* largest index of a sibling pointed to by a node from array[j,match];
     * that is matched */
    TIndex maxSibling = 0;
    /* linked to maxSibling, this is the max. possible index of the
     * so that array[j, j ++ maxClosedMatch] remains closed with respect to
     * sibling pointers */
    TSymbol maxClosedMatch;

    //flag used to end the main loop in the next cycle
    bool endLoop = false;
    // flag is true if matching is at the first chars of sequences
    bool firstPosition = true;

    while (true) {        
        assert(i < startj);
        // check if nodes are identical
        TNode n1 = (*nodes)[i], n2 = (*nodes)[j];
        if (n1 != n2) break;
        /* if there's a sibling pointer to the node being replaced,
         * then if pointing node is outside the sequence, matching stops and
         * the node is not replaced, except if it's at the beginning of the
         * sequence  */
        if (prevSibling[j] != 0) {            
            if (startj < j && prevSibling[j] < startj) break;
        }

        // TODO staviti ovo izvan glavne petlje?
        /* if node to be replaced has a sibling we have to make sure
         * the sibling is included in the replaced sequence, or is just
         * one position after the sequence's end, so we do the bookkeeping  */
        if (n2.isPointer() == false)
        if (n2.getSibling() != 0) {
            if (config.pointerBreak) break;
            
            TIndex si = j + n2.getSibling();
            if (maxSibling < si) {
                maxSibling = si;
                /* this is the current max match, without the node at j.
                 * this match is taken if max.match at the end ends before
                 * one position before maxSibling. Not completely correct since
                 * if j == startj this match will be 0, same as
                 * for the j == startj+1, but it doesn't matter since match
                 * must be at least 2 for the replacement to happen.  */
                maxClosedMatch = match;
            }
        }
        /* current end of replaced sequence is a base sequence border, so
         * this must be last replaced character, lest a base sequence in
         * question is expanded with nodes not originaly in it, rendering
         * the previous replacement incorrect */
        if (isBaseBorder(j)) endLoop = true;

        /* Move to the next positions to match.
         * If the nodes are pointers, skip all replaced elements */
        if (n1.isPointer()) {
            assert(n2.isPointer());
            i = getReplacedSegmentEnd(i);
            j = getReplacedSegmentEnd(j);
            assert((startj < i) == false);
        }
        else {
            i++;
            j++;
        }

        /* all constraints are satisfied, increase the match.
         * match is not increased if first characters of
         * subsequences match, that must be true.
         * So match after this loop is the number of matching chars-1 */
        if (firstPosition == false) match++;
        else firstPosition = false;
        // matching sequences length is maximum we can encode in TSymbol
        if (match == maxSymbol<TSymbol>()) break;

        // end of node array is reached
        if (j == N) break;
        // first subsequence has reached first char of the second subsequence
        if (i == startj) break;
        // if for a some reason previous node match must be the last
        if (endLoop) break;
    }

    /* check if we have a sibling that is out of the matched range.
     * at the end, j is one position after the last char of replaced sequence. */
    if (j < maxSibling) match = maxClosedMatch;

    return match;
}

/**
 * For a pointer node, return index of a node right after
 * the end of replaced segment.
 *
 * @param i index of a pointer node
 */
template <typename TNodeArray>
inline typename TNodeArray::Index
LzCompressor<TNodeArray>::getReplacedSegmentEnd(TIndex i) {
    assert( ((*nodes)[i]).isPointer() );
    assert(repSegmentEnd[i] != 0);
    return repSegmentEnd[i];
}

/**
 * After max. match is found for two subsequences starting at b and r,
 * this procedure writes pointer data to r and marks the subsequence
 * starting at r as replaced.
 *
 * @param b base subsequence
 * @param r replaced subsequence
 * @param match max. match
 */
template <typename TNodeArray>
void LzCompressor<TNodeArray>::replaceMatchedPosition(TIndex b, TIndex r, TSymbol match) {
    assert(r < N); assert(b < N); assert(b < r);
    //debug.logReplacement(b, r, match, "before replacing");
    // these operations must be done before new pointer is written to position r    
    expandedLength[r] = calculateExpandedLength(r, match);
    TIndex end = markReplacedAndGetSegmentEnd(r, match);
    repSegmentEnd[r] = end;
    // write new pointer data
    TNode n = (*nodes)[r];    
    n.setPointer();
    n.setSymbol(match);
    n.setSibling(b);    
    // TODO zahtjev na tip
    // mark last node of the base sequence as base border
    TIndex lastBaseChar = stepThroughArray(b, match);
    setBaseBorder(lastBaseChar);
    //debug.logReplacement(b, r, match, "after replacing");
}

/** Before position r is set to pointer that replaces match 1st level
 * characters (match does not include the first char!), this function is called
 * to mark those chars as replaced and returns position after
 * the last char in the replaced segment. */
template <typename TNodeArray>
typename TNodeArray::Index LzCompressor<TNodeArray>::
markReplacedAndGetSegmentEnd(TIndex i, TSymbol match) {
    // first char is also processed, so match+1 steps must be made
    // because match can be a maximum number that fits in the TSymbol,
    // we can't increase it, so we add counter increase delay
    bool firstChar = true;
    //TODO zahtjev na tipove
    for (TSymbol count = 0; count != match; ) {
        assert(isReplaced(i) == false);
        assert(i < N);
        /* set all but first char as replaced, because first position
         * will become a new pointer */
        if (firstChar == false) setReplaced(i);
        
        TNode n = (*nodes)[i];
        if (n.isPointer()) i = getReplacedSegmentEnd(i);
        else i++;

        if (firstChar == false) count++;
        else firstChar = false;
    }
    return i;
}

/**
 * Start from a position in an array and move forward for a
 * number of chars. If chars are pointers, all the replaced
 * chars have to be skipped in one step.
 * 
 * @param b beginning index
 * @param s
 * @return position after s steps are iterated
 */
template <typename TNodeArray> typename TNodeArray::Index
LzCompressor<TNodeArray>::stepThroughArray(TIndex b, TSymbol steps) {
    TIndex i = b;
    //TODO zahtjev na tipove
    for (TSymbol count = 0; count != steps; count++) {
        TNode n = (*nodes)[i];
        if (n.isPointer()) i = getReplacedSegmentEnd(i);
        else i++;
    }
    return i;
}

/** Before position p is replaced by a pointer, calculate length of this
 * replaced sequence when fully expanded to characters. */
template <typename TNodeArray> typename TNodeArray::Index
LzCompressor<TNodeArray>::calculateExpandedLength(TIndex p, TSymbol match) {
    TIndex length = 0;
    // first char is also processed, so match+1 steps must be made
    // because match can be a maximum number that fits in the TSymbol, 
    // we can't increase it, so we add counter increase delay
    bool firstChar = true;
    //TODO zahtjev na tip
    for (TSymbol count = 0; count != match; ) {
        TNode n = (*nodes)[p];
        if (n.isPointer()) {
            assert( expandedLength[p] != 0 );
            assert( expandedLength[p] < N );
            assert( expandedLength[p] >= 0);
            //TODO novi zahtjev na tip
            length += expandedLength[p];
            p = getReplacedSegmentEnd(p);
        }
        else {
            length++;
            p++;
        }

        if (firstChar) firstChar = false;
        else count++;
        
    }
    return length;
}

/** Correct sibling offsets and compression pointers. Indexes of nodes that
 * are pointed to change when subsequences are replaced with single pointer
 * nodes. */
template <typename TNodeArray>
void LzCompressor<TNodeArray>::correctPointers() {
    // numOfReplaced[i] == no. of replaced nodes up to and including index i
    //TIndex* numOfReplaced = new TIndex[N];
    TIndex* numOfReplaced = (TIndex *)malloc(N * sizeof(TIndex));

    TIndex r = 0;
    for (TIndex i = 0; i < N; ++i) {
        if (isReplaced(i)) r++;
        numOfReplaced[i] = r;
    }

    for (TIndex i = 0; i < N; ++i) {
            if (isReplaced(i)) continue;
            TNode n = (*nodes)[i];

            bool check = ( n.getSibling() != 0 );

            if (n.isPointer())
                n.setSibling(n.getSibling() - numOfReplaced[n.getSibling()]);
            else if (n.getSibling() != 0)
                n.setSibling(n.getSibling() -
                    (numOfReplaced[i+n.getSibling()] - numOfReplaced[i]));

            if (check) {
                assert(0 <= n.getSibling());
                assert(n.getSibling() < N);
            }
            
        }

    free((void *)numOfReplaced);
    //delete [] numOfReplaced;
}

/**
 * Remove all the replaced elements from the array.
 * Also move expandedLength array members (maps nodes to numbers),
 * this info will be use later.
 */
template <typename TNodeArray>
void LzCompressor<TNodeArray>::compactArray() {
    TIndex newI = 0;
    for (TIndex oldI = 0; oldI < N; oldI++) {
        if (isReplaced(oldI)) continue;

        if (newI != oldI) {
            (*nodes)[newI].set((*nodes)[oldI]);
            expandedLength[newI] = expandedLength[oldI];
        }
        newI++;
    }
    (*nodes).resize(newI);
    N = newI;
}

/**
 * When a replacement occurs in the middle of a replacing sequence, corresponding
 * replaced sequence pointers have wrong size field. These sizes have to be
 * corrected.
 */
template <typename TNodeArray>
void LzCompressor<TNodeArray>::correctReplacedSizes() {
    for (TIndex i = 0; i < N; i++) {
        TNode n1 = (*nodes)[i];
        if (n1.isPointer()) {
            TIndex length = 0, j = n1.getSibling();
            TSymbol newMatch = 0;
            while (length < expandedLength[i]) {
                assert(newMatch <= maxSymbol<TSymbol>());

                if (length) newMatch++;
                TNode n2 = (*nodes)[j];
                if (n2.isPointer()) {
                    assert( expandedLength[j] != 0 );
                    length += expandedLength[j];
                }
                else {
                    assert( expandedLength[j] == 0 );
                    length++;
                }
                j++;
            }

            if (length != expandedLength[i]) {
                fstream bug("bug.txt", ios::out);
                TIndex j = n1.getSibling();
                bug << "i: " << i << " j: " << j <<
                        " length: " << length << " exp. len: " << expandedLength[i] << endl;
                bug << "j: " << nodeArraySuffShortToString(*nodes, j) << endl;
                bug << "i: " << nodeArraySuffShortToString(*nodes, i) << endl;
                bug << "array: " << nodeArrayToString(*nodes) << endl;
                for (TIndex k = j; k <= j + 256; ++k) 
                    bug << "el["<<k<<"]: " << expandedLength[k] << endl;
                bug.close();
                
                assert( length == expandedLength[i] );
            }
            assert( newMatch );                        
            assert( newMatch <= n1.getSymbol() );

            if (newMatch < n1.getSymbol())
                n1.setSymbol(newMatch);
        }
    }
}

/** init data structures neccessary for comprssion */
template <typename TNodeArray>
void LzCompressor<TNodeArray>::initAlgorithmData() {
    indexGrouping = new IndexGrouping<TNodeArray>(*nodes);

    replaced = new vector<bool>;
    replaced->reserve(N);
    replaced->resize(N, false);

    baseBorder = new vector<bool>;
    baseBorder->reserve(N);
    baseBorder->resize(N, false);

    repSegmentEnd = new TIndex[N];
    for (TIndex i = 0; i < N; i++) repSegmentEnd[i] = 0;

    // prevSilbing
    prevSibling = new TIndex[N];
    long j = 0;
    for (TIndex i = 0; i < N; i++, j++)
        prevSibling[j] = 0;

    for (TIndex i = 0; i < N; i++) {
        if ((*nodes)[i].getSibling() != 0) {
            assert(i+((*nodes)[i]).getSibling() < N);
            prevSibling[i+((*nodes)[i]).getSibling()] = i;
        }
    }

    expandedLength = new TIndex[N];
    for (TIndex i = 0; i < N; i++) expandedLength[i] = 0;

    if (config.visualize) vstream.open("vis_compr.txt", ios::out);
}

/** free all memory allocated for compression process */
template <typename TNodeArray>
void LzCompressor<TNodeArray>::clearData() {
    //TODO postepeno oslobadanje memorije, kako nestaje potreba za njom?
    delete indexGrouping;
    delete replaced;
    delete baseBorder;
    delete [] prevSibling;
    delete [] repSegmentEnd;
    delete [] expandedLength;

    if (config.visualize) vstream.close();
}

/** weather or not a node is in a sequence replaced by a pointer */
template <typename TNodeArray>
inline bool LzCompressor<TNodeArray>::isReplaced(TIndex i) {
    return (*replaced)[i];
}

template <typename TNodeArray>
inline void LzCompressor<TNodeArray>::setReplaced(TIndex i) {
    (*replaced)[i] = true;
}

/** weather or not a node is the last node of a base sequence
 * (a sequence pointed to by a pointer, ie replacing some other sequence) */
template <typename TNodeArray>
inline bool LzCompressor<TNodeArray>::isBaseBorder(TIndex i) {
    return (*baseBorder)[i];
}

template <typename TNodeArray>
inline void LzCompressor<TNodeArray>::setBaseBorder(TIndex i) {
    (*baseBorder)[i] = true;
}

#endif	/* LZCOMPRESSOR_H */

