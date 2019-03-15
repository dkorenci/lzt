#ifndef SACOMPRESSOR_HPP
#define	SACOMPRESSOR_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stack>
#include <queue>
#include <cstdlib>
#include <climits>

#include "SuffixStructCreator.hpp"
#include "compress/ICompressor.h"
#include "serialization/BitVector.h"
#include "node_array/types/symbol.h"
#include "node_array/na_utils.h"
#include "vizualization/NodeArrayVisualizer.hpp"
#include "NodesCompactifier.hpp"

using namespace std;

/** Type describing compression algorithm type for
 * suffix array based compressors. */
enum TSA_COMP_ALGO {
    SEQ_PAIRWISE, LCP_INT, LCP_INT_GR, LPF
};

enum TGROUP_ORDERING {
    RANDOM, WEIGHT, INTERFERENCE
};

struct SaCompressorConfig {
    SaCompressorConfig():
        algorithm(LCP_INT), recReplace(false), debug(false), pointerBreak(false),
            NMIN(5), grOrder(RANDOM), grShuffles(0), visualize(false) { }

    ~SaCompressorConfig() {

    }

    string algoDescription() {
        string desc;

        if (algorithm == SEQ_PAIRWISE) desc = "seq";
        else if (algorithm == LCP_INT) desc = "int";        
        else if (algorithm == LPF) desc = "lpf";
        else if (algorithm == LCP_INT_GR) {
            desc = "int_gr";
            if (grOrder == RANDOM) desc.append(".r");
            else if (grOrder == WEIGHT) desc.append(".w");
            else if (grOrder == INTERFERENCE) desc.append(".i");
        }
        else desc = "?";

        desc.append(".r:");
        desc.append(recReplace?"1":"0");

        desc.append(".b:");
        desc.append(pointerBreak?"1":"0");

        return desc;
    }

    // compression algorithm to use
    TSA_COMP_ALGO algorithm;
    // weather to use recursive replacement algorithm
    bool recReplace;
    // write debug data if true

    // debug flag
    bool debug;
    
    bool pointerBreak;

    // visualization output flag
    bool visualize;

    // number of minimal postions for replacement
    int NMIN;
    
    // method to order 2-groups
    TGROUP_ORDERING grOrder;
    // number of times to randomly shuffle groups and attempt compression
    size_t grShuffles;
};

/** Suffix array compressor, compresses a NodeArray in linear time
 * using suffix array. */
template <typename TNodeArray>
class SaCompressor : public ICompressor<TNodeArray> {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;
    
public:

    SaCompressor();
    SaCompressor(SaCompressorConfig config);
    virtual ~SaCompressor();

    string description();

    void compressArray(TNodeArray& array);
    void printCompressStats();
    
    void compressArraySeq(TNodeArray& array);
    void compressArrayGr(TNodeArray& array);

    void compressArrayInt(TNodeArray& array);
    //void compressArrayIntMins(TNodeArray& array);
    void compressArrayIntGr(TNodeArray& array);

    void compressArrayLPF(TNodeArray& array);

    long getIntervalTreeArea() { return intervalTreeArea; }

    template <typename TSymbol, typename TIndex> friend class CompressorTester;

public:
    // for TReplaceResult, reason why subsequence comparison ended
    // sequence being replaced is reffered to as repseq, sequence to
    // wich the replacing pointer is dirrected is called baseseq
    enum TBREAK_REASON {
        // no break
        NO_BREAK,
        // sibling pointer out of repseq
        OUT_POINTER, 
        // sibling pointer to repseq
        IN_POINTER,
        // first char of baseseq is replaced
        B_REPLACED,
        // first char of repseq is replaced
        R_REPLACED,
        // repseq is alredy replacing some other sequence and that interferes
        // with replacement
        R_REPLACING,
        // pointer in repseq incompatible with the corresponding position in baseseq
        POINTER,
        // shortening in replaceSequencesLcp
        SHORTENED,
        // other reason
        OTHER
    };

    struct TReplaceStats {

        void init() {
            breakFreq[OUT_POINTER] = 0;
            breakFreq[IN_POINTER] = 0;
            breakFreq[R_REPLACING] = 0;
            breakFreq[R_REPLACED] = 0;
            breakFreq[B_REPLACED] = 0;
            breakFreq[POINTER] = 0;
            breakFreq[SHORTENED] = 0;
            breakFreq[OTHER] = 0;

            breakWeight[OUT_POINTER] = 0;
            breakWeight[IN_POINTER] = 0;
            breakWeight[R_REPLACING] = 0;
            breakWeight[R_REPLACED] = 0;
            breakWeight[B_REPLACED] = 0;
            breakWeight[POINTER] = 0;
            breakWeight[SHORTENED] = 0;
            breakWeight[OTHER] = 0;
        }

        map<TBREAK_REASON, size_t> breakFreq;
        map<TBREAK_REASON, size_t> breakWeight;
    };

private:

    // data about the replacement of two sequences
    struct TReplaceResult {
        TReplaceResult(): numReplaced(0), matchLength(0) {}
        /** Number of symbols that are replaced by pointers and can be excluded
         * from the string, does not include the first symbol that becomes pointer.
         * For replacement aabx <--- aaby => aabx <--- (p)y, numReplaced = 2. */
        TSymbol numReplaced;
        /** Length of the entire replacement, in chars, minus 1 (not including
         * the char that is the pointer), ie number of saved chars.
         * For aabx <--- aaby => aabx <--- (p)y , matchLength = 2   */
        TIndex matchLength;

        /** Percent of the unreplaced characters in [ rep, rep + LCP >
         * That is left unreplaced after the replacement. */
        float replacePercentage;

        TBREAK_REASON breaker;
    };

    // descriptor of a group, i.e. sequence of consecutive
    // suffixes (in the suffix array) that share first two symbols
    struct TGroup {
        // index of starting suffix
        TIndex start;
        // length of group
        TIndex length;
        // some measure of weight
        double weight;
    };

    // compare two groups by weight
    class GroupCompareW {
    public:
        bool operator()(TGroup g1, TGroup g2) {
            return g1.weight > g2.weight;
        }
    };

    // Compare two indexes by groups they represent.
    class IndCompareGr {
    public:
        IndCompareGr(TNodeArray* n): nodes(n), N(nodes->getSize()) {}

        bool operator()(TIndex g1, TIndex g2) const {
            if (g1 == g2) return false;

            if ((*nodes)[g1] < (*nodes)[g2]) return true;
            if ((*nodes)[g2] < (*nodes)[g1]) return false;

            g1++; g2++;
            if (g1 == N) return true;
            if (g2 == N) return false;

            if ((*nodes)[g1] < (*nodes)[g2]) return true;
            else return false;            
        }

    private:
        TNodeArray* nodes;
        TIndex N;
    };

    /* Structs for lcp-interval compression
     * open interval, currently on the main stack. */
    struct TLcpIntOpen {
        TLcpIntOpen(TIndex l, TIndex lf, TIndex m, TIndex sub):
            lcp(l), left(lf), min(m), subInt(sub) {}

        // lcp and suffix array index of the interval start
        TIndex lcp, left;
        // min. node array index among the members of the interval
        TIndex min;
        // number of closed subintervals on the subinterval stack
        TIndex subInt;

        void updateMin(TIndex m) {
            if (min > m) min = m;
        }

    };

    /* Structs for lcp-interval compression, describes an open interval,
     * currently on the main stack, plus a chosen number of minimal position
     * from the interval. */
    struct TLcpIntOpenM {
        TLcpIntOpenM(TIndex l, TIndex lf, int nm):
            lcp(l), left(lf), nmin(nm), smin(0), subInt(0) {
            if (nmin > MAX_MINS) nmin = MAX_MINS;
        }

        static const int MAX_MINS = 15;

        // number of min positions to store, and the number of stored positions
        int nmin; int smin;
        TIndex mins[MAX_MINS];

        // lcp and suffix array index of the interval start
        TIndex lcp, left;

        // number of closed subintervals on the subinterval stack
        TIndex subInt;

    public:

        void updateMin(TIndex min) {
            // all min positions are filled and the new min
            // is greater then the largest min stored
            if (smin == nmin && min > mins[nmin-1]) return;

            // find the position to insert the new min
            int ins = 0;
            for (; min > mins[ins] && ins < smin; ++ins);

            // if the new min is equal to some existent, return
            if (ins < smin && min == mins[ins]) return;

            // calculate last position in the mins array with new min added
            int last;
            if (smin < nmin) last = smin++;
            else last = nmin-1;

            // shift mins in the direction ins -> last
            for (int pos = last; pos > ins; --pos)
                mins[pos] = mins[pos-1];

            mins[ins] = min;
        }

    };


    struct TLcpIntClosed {
        TLcpIntClosed(TIndex l, TIndex r, TIndex m): left(l), right(r), min(m) {}

        // boundaries of the interval (suffix array indexes)
        TIndex left, right;
        // min. node array index in the subinterval
        TIndex min;
    };


    /* Position in the suffix array and position where
     * (currently) best lpf for that position starts. */
    struct TLpfInfo {
        TLpfInfo(TIndex p, TIndex lp): pos(p), lpfPos(lp) {}
        TIndex pos, lpfPos;
    };


    // compression configuration
    SaCompressorConfig conf;
    // stream for debug data
    fstream dstream;

    // stream for visualization data
    fstream vstream;

    // size of the (suffix) array
    TIndex N;
    TNodeArray* nodes;
    // copy of the uncompressed node array
    TNodeArray* nodesCopy;
    SuffixStructCreator<TNodeArray> suffCreator;
    TIndex* suffArray;
    TIndex* inverseSA;
    // longest common prefix
    // TODO TSymbol is enough
    TIndex* lcp;
    // longest previous factor and its position
    TIndex* lpf;
    TIndex* lpfPos;
    // number of suffix groups
    TIndex NG;
    // start indexes of groups
    TGroup* groups;
    // marks start positions of substrings pointed to by a pointer
    // (replacing substrings)
    BitVector repStart;
    // marks end positions of substring pointed to by a pointer
    BitVector repEnd;
    // marks positions replaced by pointers
    BitVector inPointer;
    /* index of a sibling node pointing to a node at given position, 
     * or 0 if there is none */
    TIndex* prevSibling;
    // replace sequences recursion depth
    int depth;
    // average number of replacing sequences per interval, statistic
    double replSeqAvg;
    long intervalTreeArea;

    TReplaceStats repStats;

    void initAlgorithmData();
    void nullArrays();

    void initGroups();
    void orderGroupsByWeight();
    void orderGroups();
    int calculateSavings(TIndex i1, TIndex i2);
    int minLpfTiling(TIndex start, TIndex end);
    void testGroups();

    bool sameGroup(TIndex i1, TIndex i2);

    TReplaceResult replaceSeqFilter(TIndex i1, TIndex i2, TIndex lcp, bool rec = true);
    int approxReplaceable(TIndex pos, TIndex l);
    int approxReplaceableLcp(TIndex pos, TIndex l);
    TReplaceResult replaceSequences(TIndex i1, TIndex i2, TIndex lcp, bool rec = true);
    void replaceSequencesLcp(TIndex b, TIndex r, TIndex lcp);
    TIndex chooseReplacePosition(TLcpIntOpenM interval);
    TIndex computeMaxReplacingSeq(TIndex pos);

    void replaceMatchWithPointer(TIndex i1, TIndex i2, TSymbol m);

    void compactArray();

    TIndex calcLcp(TIndex i1, TIndex i2);
    void freeMemory();
    
    template <typename T>
    void freeNotNull(T*);

    void printSuffixStructures();

};

template <typename TNodeArray>
SaCompressor<TNodeArray>::SaCompressor() {
    nullArrays();
    conf.algorithm = LCP_INT;
    conf.recReplace = false;
}

template <typename TNodeArray>
SaCompressor<TNodeArray>::~SaCompressor() {

}

template <typename TNodeArray>
SaCompressor<TNodeArray>::SaCompressor(SaCompressorConfig config): conf(config) {
    nullArrays();
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::nullArrays() {
    suffArray = 0;
    inverseSA = 0;
    lcp = 0;
    prevSibling = 0;
    groups = 0;
    lpf = 0;
    lpfPos = 0;
}

template <typename TNodeArray>
string SaCompressor<TNodeArray>::description() {
    return conf.algoDescription();
}

/** Compress array using group sorting by some criteria. */
template <typename TNodeArray>
void SaCompressor<TNodeArray>::compressArrayGr(TNodeArray& narray) {
    nodes = &narray; N = narray.getSize();    
    initAlgorithmData();
    initGroups();
    orderGroupsByWeight();
    depth = 0;

    for (TIndex i = 0; i < NG; ++i) {
        TIndex j = groups[i].start;
        for (TIndex k = 0; j < N-1 && k < groups[i].length-1; ++k, ++j) {
            if (inPointer[suffArray[j+1]] == false && inPointer[suffArray[j]] == false)
            if (lcp[j] > 1) {
                replaceSeqFilter(suffArray[j], suffArray[j+1], lcp[j], conf.recReplace);
                if (conf.debug) dstream  <<endl;
            }

             //try replacement j <- j+2
            if (groups[i].length > 2 && k < groups[i].length - 2) {
                if (inPointer[suffArray[j+2]] == false && inPointer[suffArray[j]] == false) {
                    TIndex lcp2 = lcp[j];
                    if (lcp2 > lcp[j+1]) lcp2 = lcp[j+1];

                    if (lcp2 > 1) {
                        replaceSeqFilter(suffArray[j], suffArray[j+2], lcp2, conf.recReplace);
                        if (conf.debug) dstream  <<endl;
                    }
                }
            }
        }
    }

    if (conf.debug) dstream   << endl << nodeArrayToString(*nodes) << endl;

    compactArray();

    if (conf.debug) dstream   << endl << nodeArrayToString(*nodes) << endl;

    freeMemory();
}

/** Returns true if suffixes of node array at positions i1 and i2 are 
 * in the same group (first two symbols of each suffix are the same). */
template <typename TNodeArray>
inline bool SaCompressor<TNodeArray>::sameGroup(TIndex i1, TIndex i2) {
    if (i1 > i2) { TIndex t = i1; i1 = i2; i2 = t; }
    if (i2 == N - 1) return false;
    if ((*nodes)[i1] != (*nodes)[i2]) return false;
    if ((*nodes)[i1+1] != (*nodes)[i2+1]) return false;
    return true;
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::initGroups() {
    queue<TGroup> grq;
    TIndex grSize = 1, gi = 0, grStart = 0;
    // compute groups
    for (TIndex i = 1; i < N; ++i) {
        if (lcp[i] >= 2) grSize++;        
        else {
            if (grSize > 1) { // close group
                TGroup newGr;
                newGr.start = grStart;
                newGr.length = grSize;
                grq.push(newGr);
            }
            grSize = 1;
            grStart = i;
        }
    }
    // add currently open group, if it exists
    if (grSize > 1) {
        TGroup newGr;
        newGr.start = grStart;
        newGr.length = grSize;
        grq.push(newGr);
    }

    NG = grq.size();
    groups = new TGroup[NG];
    gi = 0;
    while (grq.empty() == false) {
        groups[gi] = grq.front();
        groups[gi].weight = 0;
        gi++;
        grq.pop();
    }
}

/** Calculate number of consecutive replacements needed to replace all
 * the positions in substring [start, end], when a replacement of
 * substring at position i is of length lpf[i]. */
template <typename TNodeArray>
inline int SaCompressor<TNodeArray>::minLpfTiling(TIndex start, TIndex end) {
    if (start > end) {
        assert(start == end+1);
        return 0;
    }
    TIndex e = start;
    int tile = 0;
    while (e <= end) {
        tile++;
        if (lpf[e] != 0) e += lpf[e];
        else e++;
    }
    return tile;
}

/** Calculate approximate savings obtained when substring at i1 is pointed
 * to an optimal position before substring at i2 is pointed. */
template <typename TNodeArray>
int SaCompressor<TNodeArray>::calculateSavings(TIndex i1, TIndex i2) {
    assert(i1 < i2);
    TIndex i1e = i1 + lpf[i1] - 1, i2e = i2 + lpf[i2] - 1;
    // replacements do not overlap
    if (i1e < i2) return 0;
    // number of pointers when a position is replaced first
    int i1First, i2First;
    // max. i1 replacement encompasses max. i2 replacement
    if (i2e <= i1e) {
        i1First = 1;
        i2First = 2 + minLpfTiling(i2e+1, i1e);
    }
    else {
        i1First = 1 + minLpfTiling(i1e+1, i2e);
        i2First = minLpfTiling(i1, i2-1)+1;
    }

    return i2First - i1First;
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::orderGroups() {
    /* Maping of nodes[] array index to index of corresponding TGroup.
     * Array indexes are compared by group equality.  */
    IndCompareGr indComp(nodes);
    map<TIndex, TIndex, IndCompareGr> gmap(indComp);
    for (TIndex i = 0; i < NG; ++i) {
        assert(gmap.count(suffArray[groups[i].start]) == 0);
        gmap[suffArray[groups[i].start]] = i;
    }
    // data for currently active groups
    struct TempGr {
        TIndex arrayPos;
        TIndex groupPos;
    };
        
    class TWrapAround {
    public:
        const int NUM_ACTIVE;

        TWrapAround(): NUM_ACTIVE(maxSymbol<TSymbol>()) {}

        inline int operator()(int i) const {
           return (i < NUM_ACTIVE - 1 ? i + 1 : 0 );
        }
    };

    const TWrapAround wrap;

    TempGr activeGr[wrap.NUM_ACTIVE];

    /** First and one past last index of the active groups and number
     * of active groups, ie active groups are in range [activeStart, activeEnd> */
    int activeStart = 0, activeEnd = 0, numActive = 0;
    for (TIndex i = 0; i < N; ++i) {
        TempGr currGroup;
        // if an index does not represent a group, continue
        if (gmap.count(i) == 0) continue;
        currGroup.groupPos = gmap[i];
        currGroup.arrayPos = i;
        cout << numActive << " ";
        for (int j = activeStart; j != activeEnd; j = wrap(j)) {
            int sav = calculateSavings(activeGr[j].arrayPos, i);
            groups[activeGr[j].groupPos].weight += sav;
            //groups[currGroup.groupPos].weight -= sav;
        }

        // add currGroup to the end of activeGr        
        activeGr[activeEnd] = currGroup;
        activeEnd = wrap(activeEnd);

        if (numActive < wrap.NUM_ACTIVE) numActive++;
        else activeStart = wrap(activeStart);

    }
    cout << endl;

    double av = 0;
    for (TIndex i = 0; i < NG; ++i) av += groups[i].weight;

    cout << av / NG << endl;

    // sort groups by group weights
    GroupCompareW comp;

    sort(groups, groups + NG, comp);

    for (TIndex i = 0; i < NG; ++i) cout << groups[i].weight << " ";
    cout << endl;
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::orderGroupsByWeight() {
   // calculate group weights
    for (TIndex i = 0; i < NG; ++i) {
        groups[i].weight = 0;
        TIndex j = groups[i].start, count = 1;
        for (; j < N-1 && sameGroup(suffArray[j], suffArray[j+1]); ++j) {
            count++;
            // max. match length
            TIndex mm = lcp[j];
            if (mm > (int)(maxSymbol<TSymbol>()) + 1) {
                mm = (int)(maxSymbol<TSymbol>()) + 1;
            }

            // add to group weight max. savings for pointer replacement with
            // suffArray[j] and suffArray[j+1]
            if (mm > 0) groups[i].weight += (mm - 1);
        }

        groups[i].weight /= (double) count;
    }

    // sort groups by group weights
    GroupCompareW comp;

    sort(groups, groups + NG, comp);
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::testGroups() {
    BitVector inGroup(N);
    for (int i = 0; i < N; ++i) inGroup.setBit(i, false);

    for (TIndex i = 0; i < NG; ++i) {
        TIndex j = groups[i].start;
        inGroup.setBit(suffArray[j], true);
        for (TIndex k = 0; j < N-1 && k < groups[i].length-1; ++k, ++j) {
            inGroup.setBit(suffArray[j+1], true);
        }
    }

    int gcount = 0;
    for (int i = 0; i < N; ++i) if (inGroup[i]) gcount++;

    cout << "N: " << N << " gcount: " << gcount << endl;

    return;

    int n = N; if (n > 200) n = 200;
    cout << "START OF SUFFIX ARRAY: " << endl;
    for (int i = 0; i < n; ++i)
        cout << nodeArraySuffShortToString(*nodes, suffArray[i]) << endl;
    
    cout << endl;

    cout << "START OF GROUPS: " << endl;
    for (TIndex i = 0; i < 10; ++i) {
        TIndex j = groups[i].start;
        cout << nodeArraySuffShortToString(*nodes, suffArray[j]) << endl;
        for (; j < N-1 && sameGroup(suffArray[j], suffArray[j+1]); ++j) {
            cout << nodeArraySuffShortToString(*nodes, suffArray[j+1]) << endl;
        }
        cout << endl;
    }

}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::printSuffixStructures() {
    for (TIndex i = 1; i <= N; ++i)
        cout << lcp[i] << " ";
    cout << endl;
}

/** Compress array using algorithm for listing lcp intervals. */
template <typename TNodeArray>
void SaCompressor<TNodeArray>::compressArrayInt(TNodeArray& narray) {
    nodes = &narray; N = narray.getSize();
    initAlgorithmData();
    const int NUM_MIN = 10;
   
    stack<TLcpIntOpenM> mainStack;
    stack<TLcpIntClosed> subIntStack;

    TLcpIntOpenM base(0, 0, NUM_MIN);
    mainStack.push(base);

    intervalTreeArea = 0;
    replSeqAvg = 0;
    long numberOfIntervals = 0;
    //printSuffixStructures();

    for (TIndex i = 1; i <= N; ++i) {
        TIndex leftBoundary = i - 1;
        // flag indicating that the closed interval has to be transfered to 
        // the next interval to be opened
        bool transferClosed = false; TIndex transferedMin;
        bool intervalClosed = false;

        while (lcp[i] < mainStack.top().lcp) {
            intervalClosed = true;
            // the interval being closed
            TLcpIntOpenM ival = mainStack.top();
            leftBoundary = ival.left;
            // new interval is created, update total interval tree area
            if (ival.lcp >= 2)
                intervalTreeArea += ((i - 1) - ival.left + 1);

            // process interval
            {                
                // right boundary of the unprocessed interval elements
                TIndex right = i - 1;
                // position within currenly processed interval that the positions will target
                TIndex replacePos = chooseReplacePosition(ival);
                bool rightNegative = false;
                // process all the closed subintervals (on the subInt stack)
                // subintervals are processed in decreasing order by their left boundary
                // because they were put on stack in increasing order by left boundary
                for (int j = 0; j < ival.subInt; ++j) {
                    TLcpIntClosed sub = subIntStack.top();
                    subIntStack.pop();
                    // replace suffixes after the current subinterval, if any
                    if (rightNegative == false && replacePos != N) {
                    for (TIndex k = right; k > sub.right; --k) {                        
                        replaceSequencesLcp(replacePos, suffArray[k], ival.lcp);
                        // to prevent overflow if k is unsigned integer
                        if (k == 0) break;
                    }
                    }

                    // replace subinterval's position that was target of replacements
                    if (sub.min != N)
                    replaceSequencesLcp(replacePos, sub.min, ival.lcp);

                    // if sub.left is zero and it is an unsigned integer, 
                    // overflow will occur so we have to check
                    if (sub.left == 0) rightNegative = true;
                    else {
                        right = sub.left - 1;
                        rightNegative = false;
                    }
                }
                
                // replace the leftmost unreplaced positions, if any
                if (replacePos != N && right >= ival.left && rightNegative == false) {
                    for (TIndex j = right; j >= ival.left; --j) {
                        replaceSequencesLcp(replacePos, suffArray[j], ival.lcp);
                        // to prevent overflow if j is unsigned integer
                        if (j == 0) break;
                    }
                }

                // close the interval and put it on the subinterval stack                
                mainStack.pop();
                // update the (open) parent interval
                TLcpIntOpenM& parent = mainStack.top();
                // close this interval and put it on the subInt stack
                TLcpIntClosed closed(ival.left, i-1, replacePos);
                subIntStack.push(closed);

                /* If the top interval is about to be closed or continued,
                 * than the current closed interval is assigned to it. */
                if (lcp[i] <= mainStack.top().lcp) {                    
                    parent.subInt++;
                    // update parents min index with closed interval's replace index
                    if (replacePos != N)
                    parent.updateMin(replacePos);
                }
                else {
                    /* new interval is about to be opened that encompasses
                     * the closed interval so the closed interval has to be added
                     * to this new interval */
                    transferClosed = true;
                    transferedMin = replacePos;
                }
            }
        }

        /* We're at the exact lcp-level of the top
         * interval, just update minimal index for the interval. */
        if (lcp[i] == mainStack.top().lcp) {
            TLcpIntOpenM& topInt = mainStack.top();
            // update minimal element in the interval
            if (inPointer[suffArray[i]] == false)
                topInt.updateMin(suffArray[i]);

            continue;
        }

        if (lcp[i] > mainStack.top().lcp) {
            // create new interval
            TLcpIntOpenM newInt(lcp[i], leftBoundary, NUM_MIN);
            // update minimal position with current position
            if (inPointer[suffArray[i]] == false)
                newInt.updateMin(suffArray[i]);

            // new interval contains postions leftBoundary and i, so
            // left boundary has to be considered as a candidate for min position
            // !! pitanje je da li je dobro ukljuciti i leftBoundary
            // posto se preklapa sa prethodnim zatvorenim intervalom
            if (intervalClosed == false) {
                if (inPointer[suffArray[leftBoundary]] == false)
                    newInt.updateMin(suffArray[leftBoundary]);
            }

            mainStack.push(newInt);
            numberOfIntervals++;

            if (transferClosed) {
                mainStack.top().subInt++;

                if (transferedMin != N)
                mainStack.top().updateMin(transferedMin);
            }
        }
    }    

//    cout << "number of lcp intervals: " << numberOfIntervals << endl;
//    replSeqAvg /= numberOfIntervals;
//    cout << "average number of positions per interval: " << setprecision(5) << replSeqAvg << endl;

    compactArray();

    freeMemory();

    //printCompressStats();
}

/** Compress array using algorithm for listing lcp intervals, running the
 * algorithm separately for each of the 2-groups. Order of group processing
 * is important. */
template <typename TNodeArray>
void SaCompressor<TNodeArray>::compressArrayIntGr(TNodeArray& narray) {
    nodes = &narray; N = narray.getSize();
    initAlgorithmData();
    initGroups();

    // number of compressions for different group orderings
    size_t numTries = 0;
    if (conf.grOrder == RANDOM) { 
        srand ( time(NULL) );
        numTries = conf.grShuffles;
        random_shuffle(groups, groups + NG);
    }
    else if (conf.grOrder == WEIGHT) {
        orderGroupsByWeight();
        numTries = 1;
    }
    else if (conf.grOrder == INTERFERENCE) orderGroups();

    for (TIndex g = 0; g < NG; ++g) {
        //
        stack<TLcpIntOpen> mainStack;
        stack<TLcpIntClosed> subIntStack;

        TIndex i = groups[g].start;

        TLcpIntOpen base(0, 0, 0, 0);
        mainStack.push(base);

        for (TIndex i = groups[g].start+1, l = 1; l <= groups[g].length + 1 ; ++i, ++l) {
            TIndex leftBoundary = i - 1;

            /* We're at the exact lcp-level of the top
             * interval, just update minimal index for the interval. */
            if (lcp[i] == mainStack.top().lcp) {
                TLcpIntOpen& topInt = mainStack.top();
                if (suffArray[i] < topInt.min)
                    topInt.min = suffArray[i];

                continue;
            }

            while (lcp[i] < mainStack.top().lcp) {
                // the interval being closed
                TLcpIntOpen ival = mainStack.top();
                leftBoundary = ival.left;
                // process interval
                {

                    // index of the last interval element not processed yet
                    TIndex last = ival.left;
                    // process all the closed subintervals (on the subInt stack)
                    for (int j = 0; j < ival.subInt; ++j) {
                        TLcpIntClosed sub = subIntStack.top();
                        subIntStack.pop();
                        // replace suffixes before the current subinterval, if any
                        for (TIndex k = last; k < sub.left; ++k)
                            replaceSeqFilter(ival.min, suffArray[k], ival.lcp, conf.recReplace);

                        // replace subinterval
                        replaceSeqFilter(ival.min, sub.min, ival.lcp, conf.recReplace);

                        last = sub.right + 1;
                    }
                    // right boundary of the currently closed interval;
                    TIndex right = i - 1;
                    // replace the rightmost unreplaced suffixes, if any
                    if (last < right) {
                        for (TIndex j = last; j <= right; ++j)
                            replaceSeqFilter(ival.min, suffArray[j], ival.lcp, conf.recReplace);
                    }

                    // close the interval and put it on the subinterval stack
                    mainStack.pop();
                    // update the (open) parent interval
                    TLcpIntOpen& parent = mainStack.top();
                    // this variable is used to avoid a gcc bug
                    TIndex m = ival.min;
                    if (m < parent.min) parent.min = m;

                    // close this interval and put it on the subInt stack
                    TLcpIntClosed closed(ival.left, right, ival.min);
                    subIntStack.push(closed);
                    parent.subInt++;
                }
            }

            if (lcp[i] > mainStack.top().lcp) {
                TLcpIntOpen newInt(lcp[i], leftBoundary, suffArray[i], 0);
                mainStack.push(newInt);
            }
        }

    }
    
    compactArray();
    freeMemory();
}

///** Compress array using algorithm for listing lcp intervals. */
//template <typename TNodeArray>
//void SaCompressor<TNodeArray>::compressArrayIntMins(TNodeArray& narray) {
//    nodes = &narray; N = narray.getSize();
//    initAlgorithmData();
//    stack<TLcpIntOpenM> mainStack;
//    stack<TLcpIntClosed> subIntStack;
//
//    TLcpIntOpenM base(0, 0, 0, 0);
//    mainStack.push(base);
//
//    // number of minimal position to store per interval
//    const int NMIN = conf.NMIN;
//
//    for (TIndex i = 1; i <= N; ++i) {
//        TIndex leftBoundary = i - 1;
//
//        /* We're at the exact lcp-level of the top
//         * interval, just update minimal index for the interval. */
//        if (lcp[i] == mainStack.top().lcp) {
//            TLcpIntOpenM& topInt = mainStack.top();
//            topInt.updateMin(suffArray[i]);
//
//            continue;
//        }
//
//        while (lcp[i] < mainStack.top().lcp) {
//            // the interval being closed
//            TLcpIntOpenM ival = mainStack.top();
//            leftBoundary = ival.left;
//            // process interval
//            {
//                // calculate best minimal position, the one that can
//                // replace longest sequences if char match
//                TIndex minPos, bestMin, bestLength;
//                minPos = bestMin = ival.mins[0];
//                bestLength = computeMaxReplacingSeq(bestMin);
//
//                for (int j = 1; j < ival.smin; ++j) {
//                    TIndex len = computeMaxReplacingSeq(ival.mins[j]);
//                    if (len > bestLength) {
//                        bestLength = len;
//                        bestMin = ival.mins[j];
//                    }
//                }
//
//                // index of the last interval element not processed yet
//                TIndex last = ival.left;
//                // process all the closed subintervals (on the subInt stack)
//                for (int j = 0; j < ival.subInt; ++j) {
//                    TLcpIntClosed sub = subIntStack.top();
//                    subIntStack.pop();
//                    // replace suffixes before the current subinterval, if any
//                    for (TIndex k = last; k < sub.left; ++k)
//                        if (suffArray[k] > bestMin)
//                        replaceSeqFilter(bestMin, suffArray[k], ival.lcp, conf.recReplace);
//
//                    // replace subinterval
//                    if (sub.min > bestMin)
//                    replaceSeqFilter(bestMin, sub.min, ival.lcp, conf.recReplace);
//
//                    last = sub.right + 1;
//                }
//
//                //TODO could lcp sent to replace reduce compression?
//                // right boundary of the currently closed interval;
//                TIndex right = i - 1;
//                // replace the rightmost unreplaced suffixes, if any
//                if (last < right) {
//                    for (TIndex j = last; j <= right; ++j)
//                    if (suffArray[j] > bestMin)
//                        replaceSeqFilter(bestMin, suffArray[j], ival.lcp, conf.recReplace);
//                }
//
//                // replace min positions smaller than bestMin with minPos
//                if (bestMin != minPos) {
//                    for (int j = 1; j < ival.smin; ++j)
//                    if (ival.mins[j] <= bestMin)
//                        replaceSeqFilter(minPos, ival.mins[j], ival.lcp, conf.recReplace);
//                }
//
//                // close the interval and put it on the subinterval stack
//                mainStack.pop();
//
//                // update the (open) parent interval
//                TLcpIntOpenM& parent = mainStack.top();
//                // TODO could another position be better to pass to parent?
//                parent.updateMin(minPos);
//
//                // close this interval and put it on the subInt stack
//                TLcpIntClosed closed(ival.left, right, minPos);
//                subIntStack.push(closed);
//                parent.subInt++;
//            }
//        }
//
//        if (lcp[i] > mainStack.top().lcp) {
//            TLcpIntOpenM newInt(lcp[i], leftBoundary, suffArray[i], NMIN);
//            mainStack.push(newInt);
//        }
//    }
//
//    compactArray();
//    freeMemory();
//}

/** Compute length of maximal subsequence starting at pos that can
 * replace some other subsequence that is a same string. This length
 * must be the same as one computed in replaceSequences() in case all
 * the characters is subsequences math, except this procedure breaks at
 * pointers. */
template <typename TNodeArray> typename SaCompressor<TNodeArray>::TIndex
SaCompressor<TNodeArray>::computeMaxReplacingSeq(TIndex pos) {
    TIndex i = 0;

    while (true) {
        // end of trie
        if (pos + i == N) break;
        // i (length of a match - 1) must fit in the symbol field
        if (i > maxSymbol<TSymbol>()) break;
        // replaced sequence overlaps with start of sequence pointed to by a pointer
        if (repStart[pos + i]) {
            // start of a replacing sequence, must be a first element of a pointer
            if (i > 0) break;
        }
        // replaced sequence overlaps with end of sequence pointed to by a pointer
        if (repEnd[pos + i]) {
            // in all the other cases pos+i is 1 past the last replaceable character
            // in this case pos+i is the last char, so we increase i to keep the property
            i++;
            break;
        }
        // there must not be a pointer from a position before the replaced sequence
        // to any position inside a replaced sequence except the first position
        if (prevSibling[pos + i] != N)
            if (prevSibling[pos + i] < pos && i > 0) break;

        if ((*nodes)[i].isPointer()) break;

        i++;
    }

    // length=end-start+1, i == pos+(i-1) - pos + 1
    return i;
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::compressArray(TNodeArray& array) {
    repStats.init();
    if (conf.debug) dstream.open("debug.txt", ios::out);
    if (conf.visualize) vstream.open("vis_compr.txt", ios::out);

    if (conf.algorithm == SEQ_PAIRWISE)
        compressArraySeq(array);
    else if (conf.algorithm == LCP_INT)
        compressArrayInt(array);   
    else if (conf.algorithm == LPF)
        compressArrayLPF(array);
    else if (conf.algorithm == LCP_INT_GR)
        compressArrayIntGr(array);

    if (conf.debug) dstream.close();
    if (conf.visualize) vstream.close();
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::printCompressStats() {
     //OUT_POINTER, IN_POINTER, REPLACED, POINTER, OTHER
    cout << "out_pointer: freq = " << repStats.breakFreq[OUT_POINTER]
            << " weight = " << repStats.breakWeight[OUT_POINTER] << endl;
    cout << "in_pointer: freq = " << repStats.breakFreq[IN_POINTER]
            << " weight = " << repStats.breakWeight[IN_POINTER] << endl;
    cout << "rep replacing: freq = " << repStats.breakFreq[R_REPLACING]
            << " weight = " << repStats.breakWeight[R_REPLACING] << endl;
    cout << "r_replaced: freq = " << repStats.breakFreq[R_REPLACED]
            << " weight = " << repStats.breakWeight[R_REPLACED] << endl;
    cout << "base_repl: freq = " << repStats.breakFreq[B_REPLACED]
            << " weight = " << repStats.breakWeight[B_REPLACED] << endl;
    cout << "pointer: freq = " << repStats.breakFreq[POINTER]
            << " weight = " << repStats.breakWeight[POINTER] << endl;
    cout << "shortened: freq = " << repStats.breakFreq[SHORTENED]
            << " weight = " << repStats.breakWeight[SHORTENED] << endl;
    cout << "other: freq = " << repStats.breakFreq[OTHER]
            << " weight = " << repStats.breakWeight[OTHER] << endl;
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::compressArraySeq(TNodeArray& narray) {
    nodes = &narray; N = narray.getSize();
    initAlgorithmData();
    depth = 0;
    int cnt = 1000;
    
    for (TIndex i = 1; i <= N-1; ++i) {
        if (inPointer[suffArray[i-1]] == false && inPointer[suffArray[i]] == false)
        if (lcp[i] > 1) {
            TReplaceResult result =
                replaceSeqFilter(suffArray[i-1], suffArray[i], lcp[i], conf.recReplace);
            
            int maxSavings = lcp[i] - 1;
            float savingRatio = (double)result.numReplaced / maxSavings;
            int savingCategory;
            if (savingRatio < .3) savingCategory = 1;
            else if (savingRatio < .6) savingCategory = 2;
            else savingCategory = 3;

            if (conf.debug) dstream << "saving cat: " << savingCategory
                    << " saving ratio: " << savingRatio << endl << endl;
        }
        
//        if (i < N - 2) {
//            TIndex lcp2 = lcp[i];
//            if (lcp2 > lcp[i+1]) lcp2 = lcp[i+1];
//
//            if (inPointer[suffArray[i+2]] == false && inPointer[suffArray[i]] == false)
//            if (lcp2 > 1) {
//                replaceSequences(suffArray[i], suffArray[i+2], lcp2);
//                if (conf.debug) dstream  <<endl;
//            }
//        }
    }

    if (conf.debug) dstream   << endl << nodeArrayToString(*nodes) << endl;

    compactArray();

    if (conf.debug) dstream   << endl << nodeArrayToString(*nodes) << endl;

    freeMemory();
}

/** Compress array with an algorithm that replaces like an
 *  LZ-factorization based on LPF array. */
template <typename TNodeArray>
void SaCompressor<TNodeArray>::compressArrayLPF(TNodeArray& narray) {    
    nodes = &narray; N = narray.getSize();
    initAlgorithmData();
    lpf = suffCreator.createLPFArray();
    lpfPos = suffCreator.createLPFPosArray();

    if (conf.visualize) {
        vstream << "start: " << endl;
        NodeArrayVisualizer<TNodeArray> visualizer(*nodes);
        visualizer.printArray(vstream);
    }

    for (TIndex i = 0; i < N; ) {
        if (lpf[i] >= 2) {
            replaceSequencesLcp(lpfPos[i], i, lpf[i]);
            TNode n = (*nodes)[i];
            if (n.isPointer()) {
                i += n.getSymbol()+1;
            }
            else i++;
        }
        else i++;
    }

    compactArray();

    freeMemory();
}

/** Convenience method that does additional tests on replace parameters and
 * calls replace if none of them fail. */
template <typename TNodeArray>
inline typename SaCompressor<TNodeArray>::TReplaceResult
SaCompressor<TNodeArray>::replaceSeqFilter(TIndex b, TIndex r, TIndex lcp, bool rec) {
    TReplaceResult result;
    bool baseReplaced = false;

    if (b == r) {
        result.breaker = OTHER;
        return result;
    }
    if (inPointer[b]) {
        result.breaker = B_REPLACED;
        result.numReplaced = 0;
        baseReplaced = true;
    }
    if (inPointer[r]) {
        result.breaker = R_REPLACED;
        return result;
    }

    if (lcp < 2) return result;

    if (b > r) { TIndex t = b; b = r; r = t; }
    
    // calculate number of replaceable symbols and pointer in [r, r+lcp> range
    int replaceable = approxReplaceable(r, lcp);

    if (baseReplaced == false) {
        result = replaceSequences(b, r, lcp, rec);
    }

    // refresh replacement statistics
    repStats.breakFreq[result.breaker]++;
    // number of unreplaced nodes
    int ommited;
    if (replaceable == 0) {
        ommited = 0;
    }
    else {
        if (replaceable > result.numReplaced)
            ommited = replaceable - result.numReplaced;
        else 
            ommited = 0;

        result.replacePercentage = result.numReplaced / (float)replaceable;
    }
    repStats.breakWeight[result.breaker] += ommited;

    if (conf.debug && replaceable != 0) {
        dstream << setprecision(4) << result.replacePercentage << endl;
    }

    if (conf.visualize) {
        vstream << "rep: base = " << b << " rep = " << r << " numRepl = " 
                    << (int)result.numReplaced << " lcp = " << lcp << endl;
        vstream << nodeArraySuffShortToString(*nodes, r) << endl;
    }

    return result;
}

/** Approximate number of replaceable nodes in
 * range [pos, pos+l> with replaceSequencesLcp method. */
template <typename TNodeArray>
inline int SaCompressor<TNodeArray>::approxReplaceableLcp(TIndex b, TIndex len) {
     // break reason
     bool end = false; int i;
     for (i = 0; i < len; ++i) {
         if (end) break;
        // end of trie
        if (b + i == N) break;
        // i (length of a match - 1) must fit in the symbol field
        if (i > maxSymbol<TSymbol>()) break;

        TNode n1 = (*nodes)[b+i];

        // start of base pointer, put it on stack
        if (n1.isPointer() && inPointer[b+i] == false) {
            if (i + n1.getSymbol() >= len) {
                break;
            }
            else i = i + n1.getSymbol();
        }

    }
    return i;
}

/** Approximate number of replaceable nodes int range [pos, pos+l> */
template <typename TNodeArray>
inline int SaCompressor<TNodeArray>::approxReplaceable(TIndex pos, TIndex l) {
    int rep = 0, i = 0;
    bool end = false;

    while (true) {
        if (end) break;

        if (i >= l) break;
        
        if (pos + i == N) break;
        // i (length of a match - 1) must fit in the symbol field
        if (i > maxSymbol<TSymbol>()) break;
        // replaced sequence overlaps with start of sequence pointed to by a pointer
        if (repStart[pos + i] && i > 0) break;        
        // replaced sequence overlaps with end of sequence pointed to by a pointer
        if (repEnd[pos + i]) end = true;

        if (prevSibling[pos + i] != N)
            if (prevSibling[pos + i] < pos && i > 0) {
                break;
            }

        if (i > 0) rep++;

        if ((*nodes)[pos+i].isPointer()) i += ((*nodes)[pos+i].getSymbol() + 1);
        else i++;
    }

    return rep;
}

/** Try to replace substrings of nodes starting at positions b and r with
 * longest common prefix lcp. Sequence starting at larger index is replaced
 * with sequence starting at smaller index.
 * Return number of replacements achieved by replacement.
 * If rec is true, continue replacemets recursively. */
template <typename TNodeArray> typename SaCompressor<TNodeArray>::TReplaceResult
SaCompressor<TNodeArray>::replaceSequences(TIndex b, TIndex r, TIndex lcp, bool rec) {
    assert(b != r);
    
    depth++;            
    // make b < r
    if (b > r) { TIndex t = b; b = r; r = t; }

//    if (conf.debug) {
//        dstream << "b: " << b << " r: " << r << " depth: " << depth
//                << " lcp: " << lcp << endl;
//        dstream << nodeArraySuffShortToString(*nodes, b) << endl;
//        dstream << nodeArraySuffShortToString(*nodesCopy, b) << endl;
//        dstream << nodeArraySuffShortToString(*nodes, r) << endl;
//        dstream << nodeArraySuffShortToString(*nodesCopy, r) << endl;
//    }

    // try to replace r (replaced) with b (base)

    TReplaceResult result;
    result.breaker = OTHER;

    // maximum match without sibling pointers pointing out of replaced sequence
    TIndex maxClosedMatch;
    // number of symbols inside max closed match
    TIndex maxClosedIndex = 0;
    // max. position to wich a sibling pointer form replaced sequence points
    TIndex maxSibling = 0;
    // i is offset of current nodes being compared, m is number of matched nodes
    TIndex i = 0, m = 0;
    // flag to indicate termination of the sequence at the beginning of the next loop cycle
    bool end = false;
    // flag to indicate ne necessity to continue with replacement recursively
    bool continueRec = false;
    // new indexes for recursive replacement
    TIndex newB, newR, newLcp, newOffset;

    while (true) {
        if (end == true) break;

        // end of trie
        if (r + i == N) break;
        // base seqence must not overlap with replaced
        if (b + i == r) break;
        // i (length of a match - 1) must fit in the symbol field
        if (i > maxSymbol<TSymbol>()) break;
        // replaced sequence overlaps with start of sequence pointed to by a pointer
        if (repStart[r + i]) {
            // start of a replacing sequence, must be a first element of a pointer
            if (i > 0) {
                result.breaker = R_REPLACING;
                break;
            }
        }
        // replaced sequence overlaps with end of sequence pointed to by a pointer
        if (repEnd[r+i]) {
            result.breaker = R_REPLACING;
            end = true;
        }
        // there must not be a pointer from a position before the replaced sequence
        // to any position inside a replaced sequence except the first position
        if (prevSibling[r + i] != N) {
            if (prevSibling[r + i] < r && i > 0) {
                result.breaker = IN_POINTER;
                break;
            }
        }

        assert(r + i < N); 
        assert(b + i < r);

        TNode n1 = (*nodes)[b+i], n2 = (*nodes)[r+i];

        if (n1.isPointer() == false && n2.isPointer() == false) {
            if (n1 == n2) {
                if (n2.getSibling() != 0 && conf.pointerBreak) {
                    result.breaker = OUT_POINTER;
                    break;
                }

                if (n2.getSibling() != 0 && maxSibling < i + n2.getSibling()) {
                    maxSibling = i + n2.getSibling();                    
                    
                    maxClosedIndex = i;                    
                    maxClosedMatch = m;
                }
                i++; m++; 
                continue;
            }
            else break;
        }

        if (n1.isPointer() == true && n2.isPointer() == true) {
            bool pointersEqual;
            if (n1 == n2) pointersEqual = true;
            else pointersEqual = false;
//            else {
//                if (n1.getSymbol() == n2.getSymbol()) {
//                    // pointers are equal if they are of the same length
//                    // and fall within the lcp range
//                    if (i + n1.getSymbol() <= lcp - 1) pointersEqual = true;
//                    else pointersEqual = false;
//                }
//                else pointersEqual = false;
//            }
            //TODO case when end == true
            if (pointersEqual) {
                // check if the length of replaced sequence extended to the
                // pointer end is greater than the number that fits in symbol field
                // and that end of the pointer is not end of replaced sequence
                // all other constraints (from the start of the loop) should not
                // be violated
                if ( repEnd[r + i + n2.getSymbol()] == false &&
                     i + n2.getSymbol() <= maxSymbol<TSymbol>() ) {
                    // now we can continue setting i to next length, with all
                    // the constraints for i-1 satisfied
                    i += n2.getSymbol() + 1; m++;
                    continue;
                }
                else break;
            }
            else {
                result.breaker = POINTER;
                break;
            }
        }

        // TODO try to replace sequence at [b+i] with n2.getSibling
        // sequence starting at r+i is already replaced
        if (n1.isPointer() == false && n2.isPointer() == true) {
            // check that n2 does not point to n1
            if (n2.getSibling() != b + i)
            {
            // check if sequences at positions b+i and r+i match in enough nodes
                if (lcp - i >= 2) {
                    continueRec = true;
                    // calculate new lcp
                    newLcp = lcp - i;
                    if (n2.getSymbol()+1 < newLcp) newLcp = n2.getSymbol()+1;
                    
                    // new replace positions
                    newB = b + i;
                    newR = n2.getSibling();

                    newOffset = i + n2.getSymbol() + 1;
                }
            }
            else {
                // TODO
                // pointer at n2 could be merged with the pointer
                // that will be created for the first part of the sequences
            }

            result.breaker = POINTER;
            break;
        };

        if (n1.isPointer() == true && n2.isPointer() == false) {
            // check if sequences at positions b+i and r+i match in enough nodes
            if (lcp - i >= 2) {
                continueRec = true;
                // calculate new lcp
                newLcp = lcp - i;
                if (n1.getSymbol()+1 < newLcp) newLcp = n1.getSymbol()+1;
                // new replace positons
                newB = n1.getSibling();
                newR = r + i;

                newOffset = i + n1.getSymbol() + 1;
            }

            result.breaker = POINTER;
            break;
        }

    }

    // if there's a sibling pointer past the end of matched sequence + 1
    // then fall back to the sequence match before that pointer
    if (maxSibling != 0 && i < maxSibling) {
        if (maxClosedIndex == 0) {
            assert(maxClosedMatch == 0);
            result.matchLength = 0;
            i = 0;
        }
        else {
            result.matchLength = maxClosedIndex;
            i = maxClosedIndex - 1;
        }
        
        m = maxClosedMatch;
    }
    else {
        result.matchLength = i;
        // i is the first position to which match could not be extended
        // so it must be decreased to denote last match position
        if (i > 0) i--;
    }

    if (i == 0) assert(m < 2);

    result.numReplaced = 0;
    // enough consecutive nodes are matched for pointer replacement
    if (m >= 2) {
        assert(i >= 1);
        replaceMatchWithPointer(b, r, i);
        //if (conf.debug) dstream << nodeArraySuffShortToString(*nodes, r) << endl;

        result.numReplaced = (m-1);        
    }
    //else if (conf.debug) dstream << "no replacement" << endl;
   
    if (rec && continueRec) {
        if (inPointer[newB] == false && inPointer[newR] == false) {
            TReplaceResult recResult = replaceSequences(newB, newR, newLcp);
            result.numReplaced += recResult.numReplaced;
            result.matchLength += recResult.matchLength;
        }

        /* Try to replaceme suffixes after the end of the pointer, with the same
         * offset, i.e. for aa(p)xxy, aaxxxy this would try to start new replacement
         * at 'y'. This can collide with prevoius replacement ('x' in the second
         * sequence with position pointed to by (p)), so for now is lef out. */
//        if (lcp - newOffset >= 2) {
//            if (b + newOffset < N && r + newOffset < N)
//            if (inPointer[b + newOffset] == false
//                && inPointer[r + newOffset] == false)
//                result += replaceSequences(b + newOffset, r + newOffset, lcp - newOffset);
//        }

    }

    depth--;

    return result;
}

/** For given interval, choose best position for replacement target. */
template <typename TNodeArray> typename SaCompressor<TNodeArray>::TIndex
SaCompressor<TNodeArray>::chooseReplacePosition(TLcpIntOpenM ival) {
    replSeqAvg += ival.smin;
    if (ival.smin == 0) return N;
    int maxReplaceLen = -1; int maxReplaceIndex;
    for (int i = 0; i < ival.smin; ++i) {
        if (inPointer[ival.mins[i]] == false) {
            int approx = approxReplaceableLcp(ival.mins[i], ival.lcp);
            if (approx > maxReplaceLen) {
                maxReplaceLen = approx;
                maxReplaceIndex = i;
            }            
        }
    }

    if (maxReplaceLen >= 2)  return ival.mins[maxReplaceIndex];
    else return N;
}

/** Replace sequences without char-by-char comparison, taking maximal possible
 * replacement that is less than or equal to lcp. */
template <typename TNodeArray> void
SaCompressor<TNodeArray>::replaceSequencesLcp(TIndex b, TIndex r, TIndex len) {
    if (b >= r) return; if (len < 2) return;
    if (inPointer[b] || inPointer[r]) {
        if (inPointer[b]) {
            repStats.breakWeight[B_REPLACED] += len;
            repStats.breakFreq[B_REPLACED]++;
        }
        return;
    }
    /*
    !!! The following assumptions can be violated:
    1. all the positions with index >= rep are unreplaced, no pointers
    2. all the positions with index >= rep are not pointed to, which means that
     * replacement can be done without taking into account the interference with
     * existing pointers
    3. each pointer starting in [b, b+len-1] does not stretch beyond b+len-1
     */
    /** struct describing an interval in the node array that can
     * represent pointer range or a silbing pointer start and end */
    typedef struct { TIndex start, end; } TInterval;
    const int MAXSTACK = maxSymbol<TSymbol>();
    // stacks for pointers in base sequence, pointers in replaced sequence,
    // and sibling pointers in replaced sequence
    TInterval basePtr[MAXSTACK]; TIndex bpCnt = 0;
    TInterval repPtr[MAXSTACK]; TIndex rpCnt = 0;
    TInterval sibPtr[MAXSTACK]; TIndex spCnt = 0;

    // index just before sibling pointer in replaced sequence, and position
    // to which silbing pointer points
    TIndex siblingEnd = 0;
    // i is offset of current nodes being compared
    TIndex i = 0;
    // flag indicating that the main loop should break in the next step
    bool end = false;

    // break reason
    string reason = "full replace";
    TBREAK_REASON breakReason = NO_BREAK;

    for (i = 0; i < len; ++i) {
        if (end) break;
        // end of trie
        if (r + i == N) {            
            reason = "end of trie";
            breakReason = OTHER;
            break;
        }
        // base seqence must not overlap with replaced
        if (b + i == r) {
            reason = "base touched rep";
            breakReason = OTHER;
            break;
        }
        // i (length of a match - 1) must fit in the symbol field
        if (i > maxSymbol<TSymbol>()) {
            reason = "max symbol";
            breakReason = OTHER;
            break;
        }

        if (repStart[r + i]) {
            // start of a replacing sequence, must be a first element of a pointer
            if (i > 0) { 
                reason = "rep start";
                breakReason = R_REPLACING;
                break;
            }
        }
        // replaced sequence overlaps with end of sequence pointed to by a pointer
        if (repEnd[r+i]) {
            reason = "rep end";
            breakReason = R_REPLACING;
            end = true;
        }

        // there must not be a pointer from a position before the replaced sequence
        // to any position inside a replaced sequence except the first position
        if (i > 0 && prevSibling[r + i] != N) {
            if (prevSibling[r + i] < r ) {
                reason = "prev sibling";
                breakReason = IN_POINTER;
                break;
            }
        }

        assert(r + i < N);
        assert(b + i < r);

        TNode n1 = (*nodes)[b+i], n2 = (*nodes)[r+i];

        // if this node has a sibling pointer and it points to a posititon
        // higher then highest position pointed to by a sibling pointer
        if (n2.isPointer() == false) {
        if (n2.getSibling() != 0 && (siblingEnd < i + n2.getSibling())) {
            siblingEnd = i + n2.getSibling();
            sibPtr[spCnt].start = i;
            sibPtr[spCnt].end = siblingEnd;
            spCnt++;
        }
        }
        // start of base pointer, put it on stack
        if (n1.isPointer() && inPointer[b+i] == false) {
            // pointer stretches beyond max. replacement length
            if (i + n1.getSymbol() >= len) {
                reason = "base pointer";
                breakReason = POINTER;
                break;
            }

            basePtr[bpCnt].start = i;
            basePtr[bpCnt].end = i + n1.getSymbol();
            bpCnt++;
        }
        // start of replaced pointer, put it on stack
        if (n2.isPointer() && inPointer[r+i] == false) {
            // pointer stretches beyond max. replacement length
            if (i + n2.getSymbol() >= len) {
                reason = "rep pointer";
                breakReason = POINTER;
                break;
            }
            
            repPtr[rpCnt].start = i;
            repPtr[rpCnt].end = i + n2.getSymbol();
            rpCnt++;
        }
    }
    // set i to the last position of the replacement
    if (i > 0) i--;
    TIndex oldI = i;
    
    bool writeCut = false;
    //if (b == 3382 && r == 193940) writeCut = true;

    if (breakReason != NO_BREAK) {
        TIndex loss = len - (i+1);
        repStats.breakWeight[breakReason] += loss;
        repStats.breakFreq[breakReason]++;
    }

    bool interfere = true;
    while (interfere && i > 0) {
        // remove from stacks all the pointers that are not relevant anymore
        if (bpCnt) while (bpCnt && basePtr[bpCnt-1].start > i) bpCnt--;
        if (rpCnt) while (rpCnt && repPtr[rpCnt-1].start > i) rpCnt--;
        if (spCnt) while (spCnt && sibPtr[spCnt-1].start > i) spCnt--;

        interfere = false;
        // resolve interferences with pointers
        {
            // base pointer
            if (bpCnt && i < basePtr[bpCnt-1].end) {
                if (writeCut) {
                    cout << "baseCut: " << basePtr[bpCnt-1].start << " " << basePtr[bpCnt-1].end << endl;
                }
                interfere = true;
                if (basePtr[bpCnt-1].start == 0) i = 0;
                else i = basePtr[bpCnt-1].start-1;
                bpCnt--;
                continue;
            }
            // replaced pointer
            if (rpCnt && i < repPtr[rpCnt-1].end) {
                if (writeCut) {
                    cout << "repCut: " << repPtr[rpCnt-1].start << " " << repPtr[rpCnt-1].end << endl;
                }
                interfere = true;
                if (repPtr[rpCnt-1].start == 0) i = 0;
                else i = repPtr[rpCnt-1].start-1;
                rpCnt--;
                continue;
            }
            // sibling pointer
            if (spCnt && i < sibPtr[spCnt-1].end - 1) {
                if (writeCut) {
                    cout << "sibCut: " << sibPtr[spCnt-1].start << " " << sibPtr[spCnt-1].end << endl;
                }
                interfere = true;
                if (sibPtr[spCnt-1].start == 0) i = 0;
                else i = sibPtr[spCnt-1].start-1;
                spCnt--;
            }
        }
    }

    // loss caused by replacement shortening in the last while loop
    TIndex loss = (oldI - i);
    if (loss > 0) {
        repStats.breakWeight[SHORTENED] += loss;
        repStats.breakFreq[SHORTENED]++;
    }

    if (conf.debug) {
        float ratio = (i+1) / (float)len;
        dstream << "ratio: " << setprecision(3) << ratio << " unrep: " << len - (i+1) 
                << " b: " << b << " r: " << r << " replen: " << i+1
                << " old replen: " << oldI+1 << " lcp: " << len << " reason: " << reason <<  endl;
        dstream << nodeArraySuffShortToString(*nodes, b) << endl;
        dstream << nodeArraySuffShortToString(*nodesCopy, b) << endl;
        dstream << nodeArraySuffShortToString(*nodes, r) << endl;
        dstream << nodeArraySuffShortToString(*nodesCopy, r) << endl;
    }

    if (i > 0) {        
        replaceMatchWithPointer(b, r, i);
    }
}

/** Set node r to pointer that points to b, replaced length being m.
 * Do all the bookkeeping. */
template <typename TNodeArray> void SaCompressor<TNodeArray>::
replaceMatchWithPointer(TIndex b, TIndex r, TSymbol m) {    
    TNode n = (*nodes)[r];
    TNode bn = (*nodes)[b];
    n.setPointer();
    // base node is a pointer with exact length m, so r can be redirected
    // to the location where this node points
    if (bn.isPointer() && bn.getSymbol() == m) {
        n.setSibling(bn.getSibling());
    }
    else {
        // else point r to base
        n.setSibling(b);        
        //mark beginning and end of the replacing sequence
        repStart.setBit(b, true);
        repEnd.setBit(b + m, true);
    }

    n.setSymbol(m);
    for (TIndex i = r + 1; i <= r + m; ++i) inPointer.setBit(i, true);
}

/** init data structures neccessary for comprssion */
template <typename TNodeArray>
void SaCompressor<TNodeArray>::initAlgorithmData() {
    suffCreator.setNodes(nodes);
    //suffArray = suffCreator.createSAwithSort();
    suffArray = suffCreator.createSuffixArray();
    lcp = suffCreator.createLCPArray();

    depth = 0;

    nodesCopy = new TNodeArray(*nodes);

//    if (conf.debug) dstream   << nodeArrayToString(*nodes) << endl;
//    for (int i = 0; i < N; ++i) if (conf.debug) dstream   << suffArray[i] << "," << lcp[i]<< " ";
//    if (conf.debug) dstream   << endl;

    // rep border
    repStart.resize(N);
    repEnd.resize(N);
    inPointer.resize(N+1); inPointer.setBit(N, false);
    for (TIndex i = 0; i < N; i++) {
        repStart.setBit(i, false);
        repEnd.setBit(i, false);
        inPointer.setBit(i, false);
    }

    // prevSilbing
    prevSibling = new TIndex[N];
    for (TIndex i = 0; i < N; i++) prevSibling[i] = N;

    for (TIndex i = 0; i < N; i++) {
        if ((*nodes)[i].getSibling() != 0) {
            assert(i+((*nodes)[i]).getSibling() < N);
            prevSibling[i+((*nodes)[i]).getSibling()] = i;
        }
    }
}

/**
 * Remove all the replaced elements from the array.
 */
template <typename TNodeArray>
void SaCompressor<TNodeArray>::compactArray() {
    NodesCompactifier<TNodeArray> comp(nodes, inPointer);
    comp.correctPointersAndSiblings();
    comp.compactArray();
    //comp.correctPointerLengths();
}

/** Frees memory pointer to by pointer if it's not null. */
template <typename TNodeArray> template <typename T>
void SaCompressor<TNodeArray>::freeNotNull(T* pointer) {
    if (pointer != 0) delete [] pointer;
}

template <typename TNodeArray>
void SaCompressor<TNodeArray>::freeMemory() {
    freeNotNull(suffArray);
    freeNotNull(inverseSA);
    freeNotNull(lcp);
    freeNotNull(prevSibling);
    freeNotNull(groups);
    freeNotNull(lpf);
    freeNotNull(lpfPos);

    delete nodesCopy;
    repStart.resize(0);
    repEnd.resize(0);
    inPointer.resize(0);
}

#endif	/* SACOMPRESSOR_HPP */

