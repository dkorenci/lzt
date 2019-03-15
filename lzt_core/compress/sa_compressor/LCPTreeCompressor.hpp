#ifndef LCPTREECOMPRESSOR_HPP
#define	LCPTREECOMPRESSOR_HPP

#include <stack>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cstdio>

#include "compress/ICompressor.h"
#include "SaCompressor.hpp"
#include "SuffixStructCreator.hpp"
#include "serialization/BitVector.h"
#include "NodesCompactifier.hpp"
#include "node_array/na_utils.h"
#include "util/etimer.h"

using namespace std;

struct LctCompressorConfig {
    LctCompressorConfig(): saFile(false), visualize(false), memSizes(false), pyramid(false) { }
   
    bool saFile; // load suffix array from file
    bool visualize; // print algorithm working data
    bool memSizes; // print sizes of memory structures
    bool pyramid; // use lcp pyramid instead of replacing position for compression
};

/** Compressor using augmented lcp interval tree structure to compress string. */
template <typename TNodeArray>
class LCPTreeCompressor : public ICompressor<TNodeArray> {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

public:

    LCPTreeCompressor();
    LCPTreeCompressor(LctCompressorConfig config);

    void compressArray(TNodeArray& array);

    string description();

private:

    void initData();
    void printSuffixArray();
    void freeMemory();
    void freeMemAfterCompress();
    void initDataBeforeCompress();
    void initBaseBoundaries();
    void createIntervalTree();
    void printIntervalTree();
    void printMemoryUsagePyramid();    
    void printMemoryUsage();
    void fillTreePositions();
    void initRepPos();
    void compressWithPyramid();
    void compressRepPos();
    void replaceMatchWithPointer(TIndex b, TIndex r, TIndex l, TIndex unrepl, bool closed);
    void replaceSequences(TIndex b, TIndex r, TIndex l);
    void compactArray();

    // A node in a lcp interval tree
    struct TLcpTreeNode {
            TLcpTreeNode() {}
            TLcpTreeNode(TIndex p, TIndex l): parent(p), lcp(l) {}                
            // index of the parent interval
            TIndex parent;
            // lcp depth for the node
            TIndex lcp;
    };
    // structure representing lcp interval data used in tree construction algorithm
    struct TLcpInterval {        
        TLcpInterval(TIndex lf, TIndex lc, TIndex ni):left(lf), lcp(lc), nodeIndex(ni) {}        
        TIndex left; // left boundary of the interval
        TIndex lcp;  // lcp value in the interval 
        TIndex nodeIndex; // index of the corresponding lcp tree node
    };   

    LctCompressorConfig config;

    // size of the array
    TIndex N;
    TNodeArray* nodes;
    // nodes of the lcp interval tree
    TLcpTreeNode *lcpTree;
    TIndex lcpTreeSize;
    TIndex *repPos;
    
    // suffix array and inverse suffix array
    TIndex* suffArray, * inverseSA;
    // longest common prefix array
    TIndex* lcp;
    // index of the deepest lcp interval/tree node at given suffix array position
    TIndex* terminalNode;
    // prevSibling[i] is the index of the node pointing to (*nodes)[i], or 0
    TIndex* prevSibling;

    // positions contained in lcp intervals
    TIndex* pyramid;
    TIndex* start, * end; // start and end of a node positions in the pyramid
    long numPos;

    // marker of replaced positions
    BitVector replaced, repStart, repEnd;

    long ptrCutoff, cutoffCnt;
    // number of closed pointers
    long numClosed;

    bool visualize;
    // stream for visualization data
    fstream vstream;

};

template <typename TNodeArray>
LCPTreeCompressor<TNodeArray>::LCPTreeCompressor() { }

template <typename TNodeArray>
LCPTreeCompressor<TNodeArray>::LCPTreeCompressor(LctCompressorConfig conf): config(conf) {}

template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::compressArray(TNodeArray& array) {
    startEvent("trie_compression");
    nodes = &array;
    N = nodes->getSize();
    initData();
    
    startEvent("interval_tree_construction");
    createIntervalTree();
    endEvent("interval_tree_construction");
    
    if (config.pyramid == true) {
        startEvent("pyramid_construction");
        fillTreePositions();
        endEvent("pyramid_construction");    
    }
    else {
        startEvent("reppos_construction");
        initRepPos();
        endEvent("reppos_construction");        
    }
    
    if (config.memSizes) {
        if (config.pyramid == true) printMemoryUsagePyramid();
        else printMemoryUsage();
    }
    
    startEvent("lz_compression");
    if (config.pyramid == true) compressWithPyramid();    
    else compressRepPos();
    endEvent("lz_compression");
    
    startEvent("compactification");
    compactArray();
    endEvent("compactification");
    
    freeMemory();
    endEvent("trie_compression");
}

template <typename TNodeArray>
string LCPTreeCompressor<TNodeArray>::description() {
    string desc = "LCT";
    if (config.pyramid) desc = desc + "p";
    return desc;
}

template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::createIntervalTree() {
    // init to largest possible size
    // todo: prove that N is upper bound for number of intervals
    lcpTree = (TLcpTreeNode *)malloc(N * sizeof(TLcpTreeNode));
    if (config.pyramid) {
        start = (TIndex *)malloc(N * sizeof(TIndex));
        end = (TIndex *)malloc(N * sizeof(TIndex));
    }
    // start size is 1 to include the base interval at the beginning
    lcpTreeSize = 1;
    lcpTree[0].lcp = lcpTree[0].parent = 0;
    numPos = 0;
    
    stack<TLcpInterval> lcpStack;
    TLcpInterval base(0, 0, 0);
    lcpStack.push(base);
    // init terminalNode
    for (TIndex i = 0; i < N; ++i) terminalNode[i] = 0;

    for (TIndex i = 1; i <= N; ++i) {
        TIndex leftBoundary = i - 1;
        TIndex newNodeIndex;
        bool intervalClosed = false;

        while (lcp[i] < lcpStack.top().lcp) {
            // the interval being closed
            TLcpInterval closedInt = lcpStack.top(); lcpStack.pop();
            // set interval boundaries
            leftBoundary = closedInt.left;                         
            TIndex rightBoundary = i - 1;            
            // update terminalNode array with current interval/node
            for (TIndex j = leftBoundary; j <= rightBoundary; ++j)
                if (terminalNode[j] == 0) terminalNode[j] = closedInt.nodeIndex;

            // create new tree node corresponding to the closed interval
            TLcpTreeNode newNode;
            newNodeIndex = closedInt.nodeIndex;
            newNode.lcp = closedInt.lcp;
            newNode.parent = lcpStack.top().nodeIndex;
            lcpTree[newNodeIndex] = newNode;
            intervalClosed = true;                                    
            // if interval has compressible lcp, and pyramid compression is used,
            // calculate indexes of the node positions in the pyramid array
            if (config.pyramid && closedInt.lcp >= 2) {
                TIndex intSize = rightBoundary - leftBoundary + 1;
                // calculate start and end of the new node positions in the pyramid
                start[newNodeIndex] = numPos;
                end[newNodeIndex] = numPos + intSize - 1;
                // increase position index for the number of reserved positions
                numPos += intSize;
            }             
        }

        if (lcp[i] > lcpStack.top().lcp) {
            // create new interval, nodeIndex is first free position in the lcpTree
            TLcpInterval newInt(leftBoundary, lcp[i], lcpTreeSize++);
            lcpStack.push(newInt);
            // if an interval was just closed in the previous loop,
            // than the interval just opened is it's parent
            if (intervalClosed) {
                lcpTree[newNodeIndex].parent = lcpStack.top().nodeIndex;
            }
        }
    }
//    cout << endl << endl;
    assert(lcpTreeSize <= N);
    // resize lcpTree array to actual number of nodes
    lcpTree = (TLcpTreeNode *)realloc(lcpTree, lcpTreeSize * sizeof(TLcpTreeNode));
    if (config.pyramid) {
        start = (TIndex *)realloc(start, lcpTreeSize * sizeof(TIndex));
        end = (TIndex *)realloc(end, lcpTreeSize * sizeof(TIndex));
    }
    
    delete [] lcp;
    
    // write terminal lcp tree node indexes corresponding to suffix array index
    // of the string position, and delete inverse suffix array
    for (TIndex i = 0; i < N; ++i) inverseSA[i] = terminalNode[inverseSA[i]];
    delete [] terminalNode;                           
    terminalNode = inverseSA;                    
}

/** For each interval in LCP interval tree, write positions contained within
 * the interval in the sorted order. */
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::fillTreePositions() {
    // temporary array for current position in the interval being filled
    TIndex* posIndex = new TIndex[lcpTreeSize];
    // allocate memory for the positions
    pyramid = new TIndex[numPos];
    for (TIndex i = 0; i < lcpTreeSize; ++i) posIndex[i] = start[i];
    // main loop, iterates through node array positions
    for (TIndex i = 0; i < N; ++i) {        
        // deepest node index at current suffix array position
        TIndex lcpNode = terminalNode[i];
        // descent to the root of the tree and add current position
        // to the intervals of the nodes
        while (lcpTree[lcpNode].lcp >= 2) {
            pyramid[posIndex[lcpNode]++] = i;
            lcpNode = lcpTree[lcpNode].parent;
        }
    }

    // check that all the intervals with lcp >= 0 are fully filled
    for (TIndex i = 0; i < lcpTreeSize; ++i) {
        if (lcpTree[i].lcp >= 2) assert(posIndex[i] == end[i]+1);
    }
    delete [] posIndex;
}

template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::initRepPos() {
    // temporary array for current position in the interval being filled
    repPos = new TIndex[lcpTreeSize];    
    for (TIndex i = 0; i < lcpTreeSize; ++i) repPos[i] = N;
    // main loop, iterates through node array positions
    for (TIndex i = 0; i < N; ++i) {        
        // deepest node index at current suffix array position
        TIndex lcpNode = terminalNode[i];
        // descent to the root of the tree and update repPos with current position        
        while (lcpTree[lcpNode].lcp >= 2) {
            if (repPos[lcpNode] == N) repPos[lcpNode] = i;
            lcpNode = lcpTree[lcpNode].parent;
        }
    }
}

template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::printIntervalTree() {
    int maxHeight = 0;
    int *nodeHeight = new int[lcpTreeSize];

    // calculate node heights
    for (int i = 0; i < lcpTreeSize; ++i) {
        nodeHeight[i] = 0;
        TLcpTreeNode node = lcpTree[i];
        while (node.lcp >= 2) {
            nodeHeight[i]++;
            if (node.lcp <= 2) break;
            node = lcpTree[node.parent];
        }
        if (nodeHeight[i] > maxHeight) maxHeight = nodeHeight[i];
    }
    // 2d array to print the pyramid indexes to
    int **treePrint = new int*[N];
    for (int i = 0; i < N; ++i) treePrint[i] = new int[maxHeight];

    for (int i = 0; i < N; ++i)
    for (int j = 0; j < maxHeight; ++j) treePrint[i][j] = -1;

    //print suffix array
    for (int i = 0; i < N; ++i) cout << setw(3) << i;
    cout << endl;
    for (int i = 0; i < N; ++i) cout << setw(3) << suffArray[i];
    cout << endl << endl;

    // print the tree to the array
    // iterate over suffix array indexes
    for (int i = 0; i < N; ++i) {
        int node = terminalNode[i];        
        if (lcpTree[node].lcp < 2) continue;        

        int h = nodeHeight[node] - 1;
        while (lcpTree[node].lcp >= 2) {
            treePrint[i][h] = pyramid[start[node]++];
            if (lcpTree[node].lcp <= 2) break;
            node = lcpTree[node].parent;
            h--;
        }

    }

    for (int j = maxHeight-1; j >= 0; --j) {
        for (int i = 0; i < N; ++i)
            if (treePrint[i][j] == -1) cout << setw(3) << " ";
            else cout << setw(3) << treePrint[i][j];
        cout << endl;
    }

    // free memory
    for (int i = 0; i < N; ++i) delete [] treePrint[i];
    delete [] treePrint;

    delete [] nodeHeight;
}

/** Print detailed data for peak memory usage during fillPositions. */
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::printMemoryUsagePyramid() {
    cout << "MEMORY" << endl;
    cout << "trieSize: " << N << endl;
    cout << "lcpTreeNodes: " << lcpTreeSize << endl;
    cout << "pyramidSize: " << numPos << endl;
    long pyramidBytes = (long)numPos * sizeof(TIndex); // sortedPos
    // lcpTree, posIndex, start, end
    long treeBytes = (long)lcpTreeSize * (sizeof(TLcpTreeNode) + 3 * sizeof(TIndex));
    // terminalNode, inverseSA
    long auxStructBytes = 2 * (long)N * sizeof(TIndex);
    cout << "pyramidBytes: " << pyramidBytes << endl;
    cout << "treeBytes: " << treeBytes << endl;
    cout << "auxStructBytes: " << auxStructBytes << endl;
    cout << "sumBytes: " << (pyramidBytes + treeBytes + auxStructBytes) << endl;
}

/** Print data for memory usage during createIntervalTree. */
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::printMemoryUsage() {
    cout << "MEMORY" << endl;
    // terminalNode, inverseSA, lcp, lcpTree
    long peakBytes = 5 * (long)N * sizeof(TIndex);
    cout << "peakBytes: " << peakBytes << endl;
}

template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::compressWithPyramid() {
    initDataBeforeCompress();
    ptrCutoff = 0; cutoffCnt = 0; numClosed = 0;
    for  (TIndex i = 0; i < N; ++i) {
        if (replaced[i]) continue;        
        // rep is the position being replacead
        // base is the (potential) target of pointer redirection 
        TIndex rep = i, base;
        // index of the deepest interval at a given position
        TIndex intIndex = terminalNode[i];
        // lenght of longest replaceable sequence at rep and base positions
        TIndex longestRep;
        while (lcpTree[intIndex].lcp >= 2) {
            // flags causing actions of moving up the interval tree            
            bool goLevelUp = false;
            // all the position within the interval are used
            if ( start[intIndex] > end[intIndex] ) {
                goLevelUp = true;
            }
            else {
                // this is the potential redirection target (base sequence)
                base = pyramid[start[intIndex]];
                if (base >= rep) {
                    goLevelUp = true;
                }
                else {
                    if (replaced[base]) {
                        // move forward one position
                        start[intIndex]++;
                    }
                    // replacement is possible
                    else {
                        longestRep = lcpTree[intIndex].lcp;
                        // location past the lcp-length subsequence starting at
                        // base is replaced, which means that a pointer starting
                        // in that subsequence spans past the end, so the replaceable
                        // lenght has to be shortened
                        if (replaced[base + longestRep]) {
                            // go backwards to the first unreplaced,
                            // which is the pointer
                            while (replaced[base + longestRep] && longestRep > 0) {
                                longestRep--;
                            }
                            assert((*nodes)[base + longestRep].isPointer());
                            // at this point longestRep is the length of longest replaceable seq.
                            TIndex parent = lcpTree[intIndex].parent;
                            if (longestRep > 1 && longestRep >= lcpTree[parent].lcp) {
                                // move forward one position
                                start[intIndex]++;
                                // proceed with replacement rep -> base
                                break;
                            }
                            else {
                                // move forward one position, and continue
                                // searching for base pointer
                                start[intIndex]++;                                
                            }
                            
                        }
                        else {
                            // proceed with replacement rep -> base
                            break;
                        }
                    }
                }
            }

            if (goLevelUp) {
                intIndex = lcpTree[intIndex].parent;
            }
        }
        if (lcpTree[intIndex].lcp < 2) continue;
        // do the replacement
        replaceSequences(base, rep, longestRep);
    }    
    freeMemAfterCompress();
//    cout << "pointer cutoff: " << ptrCutoff << " ";
//    cout << "cutoff/N: " << setprecision(3) << ptrCutoff / (float)N << " ";
//    cout << "number of cutoffs: " << cutoffCnt << endl;
}

template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::compressRepPos() {
    initDataBeforeCompress();
    ptrCutoff = 0; cutoffCnt = 0; numClosed = 0;
    for  (TIndex i = 0; i < N; ++i) {
        if (replaced[i]) continue;        
        // rep is the position being replacead
        // base is the (potential) target of pointer redirection 
        TIndex rep = i, base;
        // index of the deepest interval at a given position
        TIndex intIndex = terminalNode[i];
        // lenght of longest replaceable sequence at rep and base positions
        TIndex longestRep;
        while (lcpTree[intIndex].lcp >= 2) {
            // flags causing actions of moving up the interval tree            
            bool goLevelUp = false;
            // this is the potential redirection target (base sequence)
            base = repPos[intIndex]; assert(base != N);
            if ( !(base < rep) ) {
                goLevelUp = true;
            }
            else {
                if (replaced[base]) {                    
                    repPos[intIndex] = rep;
                    goLevelUp = true;
                }
                // replacement is possible
                else {
                    longestRep = lcpTree[intIndex].lcp;
                    if (longestRep > maxSymbol<TSymbol>()) longestRep = maxSymbol<TSymbol>();                    
                    // location past the lcp-length subsequence starting at
                    // base is replaced, which means that a pointer starting
                    // in that subsequence spans past the end, so the replaceable
                    // length has to be shortened
                    if (replaced[base + longestRep]) {
                        // go backwards to the first unreplaced,
                        // which is the pointer
                        while (replaced[base + longestRep] && longestRep > 0) {
                            longestRep--;
                        }
                        assert((*nodes)[base + longestRep].isPointer());
                        // at this point longestRep is the length of longest replaceable seq.
                        TIndex parent = lcpTree[intIndex].parent;
                        if (longestRep > 1 && longestRep >= lcpTree[parent].lcp) {
                            // move forward one position
                            repPos[intIndex] = rep;
                            // proceed with replacement rep -> base
                            break;
                        }
                        else {
                            repPos[intIndex] = rep;
                            goLevelUp = true;
                        }
                    }
                    else {
                        // proceed with replacement rep -> base
                        break;
                    }
                }
            }

            if (goLevelUp) {
                intIndex = lcpTree[intIndex].parent;
            }                                    
        }
        if (lcpTree[intIndex].lcp < 2) continue;
        // do the replacement
        replaceSequences(base, rep, longestRep);
        // on ancestor nodes, replace repPos[] values with rep
        // repPos is replaced or rep should yield better replacement
        intIndex = lcpTree[intIndex].parent;
        while (lcpTree[intIndex].lcp >= 2) {
            if (replaced[repPos[intIndex]]) repPos[intIndex] = rep;
            else {
                TIndex lcp = lcpTree[intIndex].lcp;
                if (replaced[repPos[intIndex] + lcp] && !replaced[rep + lcp]) {
                    repPos[intIndex] = rep;
                }
            }
            intIndex = lcpTree[intIndex].parent;
        }
    }
    freeMemAfterCompress();
//    cout << "pointer cutoff: " << ptrCutoff << " ";
//    cout << "cutoff/N: " << setprecision(3) << ptrCutoff / (float)N << " ";
//    cout << "number of cutoffs: " << cutoffCnt << endl;
}

/** Replace subsequence starting at rep with a subsequence starting at b.
 * len is longest match length. */
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::replaceSequences(const TIndex b, const TIndex r, const TIndex len) {    
    /*
    assumptions on the replacement context following from the algorithm:
    1. all the positions with index >= rep are unreplaced, no pointers
    2. all the positions with index >= rep are not pointed to, which means that
     * replacement can be done without taking into account the interference with
     * existing pointers
    3. each pointer starting in [b, b+len-1] does not stretch beyond b+len-1
     */
    assert(b < r); assert(len >= 2);

    /* The entire replacement procedure is run twice: first with unlimited replacement
     * length and second time, if the replacement fails, with limited length. */
    bool unlimitedLen = true;
    for (int loop = 0; loop < 2; ++loop) {
    // index just before sibling pointer in replaced sequence, and position
    // to which silbing pointer points
    TIndex siblingStart = 0, siblingEnd = 0;
    // true if i is within a (maximal) sibling pointer
    bool siblingPointer = false;
    // start index of a pointer
    TIndex pointerStart = 0;
    // true if i is within a pointer in the base sequence
    bool basePointer = false;
    // i is offset of current nodes being compared
    TIndex i = 0;
    // number of unreplaced positions in [b+1, b+i]
    TIndex unrepl = 0;
    // flag indicating that the loop must end in next iteration
    bool end = false;
    for (i = 0; i < len; ++i) {
        // invariant: i == 0 || [r, r + i - 1] is the range for correct replacement
        // (not necessarily the longest)
        if (end) break;
        // end of trie
        if (r + i == N) break;
        // base seqence must not overlap with replaced
        if (b + i == r) break;
        // i (length of a match - 1) must fit in the symbol field
        if (unlimitedLen == false && unrepl > maxSymbol<TSymbol>()) break;
        // start of a replacing sequence, can only be at the start of a pointer
        if (repStart[r+i] && i > 0) break;
        // end of a replacing sequence, must be the last char of the replacement
        if (repEnd[r+i]) end = true;

        // there must not be a pointer from a position before the replaced sequence
        // to any position inside a replaced sequence except the first position
        if (i > 0 && prevSibling[r + i] != 0) {
            if (prevSibling[r + i] < r ) break;            
        }

        assert(r + i < N);
        assert(b + i < r);

        TNode n1 = (*nodes)[b+i], n2 = (*nodes)[r+i];

        if (i > 0 && replaced[b+i] == false) unrepl++;

        if (siblingPointer && i + 1 >= siblingEnd) {
            siblingPointer = false;
            siblingEnd = 0;
        }
        // if this node has a sibling pointer and it points to a posititon
        // higher then highest position pointed to by a sibling pointer
        if (n2.isPointer() == false)
        if (n2.getSibling() > 1 && siblingEnd < i + n2.getSibling()) {
            siblingEnd = i + n2.getSibling();
            siblingStart = i;
            siblingPointer = true;
        }

        if (basePointer && replaced[b+i] == false) basePointer = false;
        if (n1.isPointer() && replaced[b+i] == false) {
            pointerStart = i;            
            basePointer = true;
        }
    }
    assert(i);
    // set i to the last position of the replacement
    i--;
    // check if replacement should be shortened to a position before a
    // sibling pointer or a pointer in base sequence
    TIndex oldI = i;
    if (siblingPointer) {
        // check if the segment before sibling pointer is to short
        if (siblingStart <= 1) i = 0;
        else {
            i = siblingStart - 1;
            // position before the sibling pointer is in a base sequence pointer
            if ((*nodes)[b+i].isPointer() || replaced[b+i]) {
                // and this position is not the end of the pointer
                if (replaced[b+i+1]) {
                    cutoffCnt++;
                    TIndex beforeCutoff = i;
                    // point i to the start of the pointer
                    //while ((*nodes)[b+i].isPointer() == false) { i--; }
                    while (replaced[b+i]) { i--; }                    
                    assert(i >= 0);
                    assert((*nodes)[b+i].isPointer());
                    // set i to a position before pointer, or to 0 if there is
                    // not enough characters for replacement
                    if (i > 1) i--;
                    else i = 0;
                    // if position at i is in a base sequence pointer or sibling pointer,
                    // it must be at the end of these pointers: base pointers do not
                    // overlap and if i is in the middle of some other sibling pointer than
                    // that sibling pointer would encompass the current, which is not possible
                    ptrCutoff += (beforeCutoff - i);
                }
            }
        }        
    }
    else {
        // position is in a base pointer and it is not the its last position
        if (basePointer && replaced[b+i+1]) {
            cutoffCnt++;
            // set i to 0 or to the position before the base pointer if the
            // replacement will be long enough (at least 2 chars).
            TIndex beforeCutoff = i;
            if (pointerStart <= 1) i = 0;
            else i = pointerStart - 1;
            ptrCutoff += (beforeCutoff - i);
            // if is a position before the base pointer and it is in another
            // base sequence or sibling pointer than it must be at the end
            // of these pointer: base pointers do not overlap and if i is
            // in the middle of a sibling pointer than this pointer would
            // stretch past the end of the base pointer, which is not the case
            // because siblingPointer == false
        }
    }

    // after the basic replacemnt procedure, which yields length for correct
    // replacement, we have to consider if the replacement is a closed pointer
    // and if not, weather the length is longer than maxSymbol
    if (i > 0) {
        bool closedPointer = true;
        // if last char of base sequence is replaced,
        // determine if the sequence is closed by checking if the pointer at the
        // end of the sequence is closed
        if (replaced[b+i]) {
            TIndex pi = i;
            // find the position of the pointer
            while (pi >= 0 && replaced[b+pi]) pi--;
            TNode n = (*nodes)[b+pi];
            assert(n.isPointer());
            if (n.getSymbol() > 0) closedPointer = false;
        }
        else {
            TNode n = (*nodes)[b+i];            
            if (n.isPointer()) {
                // this can only be a closed pointer
                // (that was in the array before compression started)
                if (n.getSymbol() != 0) {
                    cout << i << " " << oldI << endl;
                    cout << nodeArraySuffShortToString(*nodes, b+i) << endl;
                    for (int j = 0; j < 50; ++j) cout << replaced[b+i+j];
                    cout << endl;
                    cout << nodeArraySuffShortToString(*nodes, n.getSibling()) << endl;
                    assert(n.getSymbol() == 0);
                }
            }
            else {
                // if there is a continuation after the last char of base sequence,
                // the replacement is not closed
                if (n.getCow()) closedPointer = false;
            }
        }
        // if there is a sibling pointer inside base sequence
        // pointing out of the sequence, replacement is not closed 
        if (prevSibling[b+i+1] != 0 && prevSibling[b+i+1] >= b) {
            closedPointer = false;
        }

        if (closedPointer) {
            replaceMatchWithPointer(b, r, i, 0, true);
            break;
        }
        else {
            unrepl = 0;
            for (TIndex j = 1; j <= i; ++j) if (replaced[b+j] == false) unrepl++;
            
            if (unrepl > maxSymbol<TSymbol>()) {
                unlimitedLen = false;
                continue;
            }
            else {
                replaceMatchWithPointer(b, r, i, unrepl, false);
                break;
            }
        }
    }
    else break;

    }
}

/** Do actual pointer replacement and bookkeeping. Redirect sequence [r, r+l] to [b, b+l]. */
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::replaceMatchWithPointer(TIndex b, TIndex r, TIndex l, TIndex unrepl, bool closed) {
    if (config.visualize) {
        vstream << "base: " << b << " repl: " << r << " len: " << l+1 
                << " unrepl: " << unrepl << " closed: " << closed << endl;
        vstream << nodeArraySuffShortToString(*nodes, b) << endl;
        vstream << nodeArraySuffShortToString(*nodes, r) << endl;
    }

    TNode n = (*nodes)[r];
    TNode bn = (*nodes)[b];

    // true if new pointer can be redirected to where the pointer at b points to
    bool redirect = false;
    if (bn.isPointer()) {
        if (closed == false) {
            if (unrepl == 0) {
                // unrepl == 0, nodes[b] must be a pointer, and redirection happens
                redirect = true;
                assert(bn.isPointer());
                assert(bn.getSymbol() > 0);
            }
        }
        else if (bn.getSymbol() == 0) {
            // closed pointer points to the beginning of other closed pointers, 
            // they replace the same sequences
            redirect = true;
            // assert that the closed pointer at b spans the entire sequence [b, b+l],
            // otherwise it is a bug
            TIndex i;
            for (i = 1; i <= l && replaced[b+i]; ++i);
            
            if (i != l+1) {
                cout << "non-matching base" << endl;
                cout << "base: " << b << " repl: " << r << " len: " << l+1 << " i: " << i << endl;
                cout << nodeArraySuffShortToString(*nodes, b) << endl;
                cout << nodeArraySuffShortToString(*nodes, r) << endl;
                assert(false);
            }
        }
    }
    
    if (redirect) {
        n.setSibling(bn.getSibling());
        n.setSymbol(bn.getSymbol());
    }
    else {
        if (closed == false) {
            assert(unrepl > 0);
            n.setSymbol(unrepl);
        }
        else n.setSymbol(l);

        n.setSibling(b);
    }
    
    n.setPointer();
    
    if (n.getSymbol() == 0) numClosed++;
    for (TIndex i = r + 1; i <= r + l; ++i) replaced.setBit(i, true);
}

/**
 * Remove all the replaced elements from the array.
 */
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::compactArray() {
    NodesCompactifier<TNodeArray> comp(nodes, replaced, false);
    comp.correctPointersAndSiblings();
    comp.compactArray();
    //comp.correctPointerLengths();
}

/** Allocate memory and init data strucutres. */
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::initData() {
    SuffixStructCreator<TNodeArray> suffCreator(config.saFile);
    suffCreator.setNodes(nodes);
    //suffArray = suffCreator.createSAwithSort();
    startEvent("suffix_array_creation");
    suffArray = suffCreator.createSuffixArray();
    endEvent("suffix_array_creation");
    //printSuffixArray();
    inverseSA = suffCreator.createInverseSA();
    startEvent("lcp_array_creation");
    lcp = suffCreator.createLCPArray();
    endEvent("lcp_array_creation");
    
    delete [] suffArray;    
    terminalNode = new TIndex[N];
    
    if (config.visualize) vstream.open("vis_lcptree.txt", ios::out);
}

template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::printSuffixArray() {
    FILE* saFile = fopen("suffix_array.txt", "w");
    for (TIndex i = 0; i <= N; ++i) {
        if (suffArray[i] != N) fprintf(saFile,"%d\n", suffArray[i]);
    }
    fclose(saFile);
}

/** Mark start and end positions of sequences to which pointers point.
  Assumption is that all the pointers are closed.*/
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::initBaseBoundaries() {
    for (TIndex i = 0; i < N; ++i) {
        if ((*nodes)[i].isPointer()) {
            assert((*nodes)[i].getSymbol() == 0); // assert closed pointer
            TIndex p = (*nodes)[i].getSibling();
            if (repStart[i]) continue;
            repStart.setBit(p, true);            
            // target of the sibling pointer with largest index
            TIndex maxSibling = 0;
            while (true) {
                // pointer (closed) must be the end of a closed sequence
                if ((*nodes)[p].isPointer()) break;
                else {
                    TIndex sib = (*nodes)[p].getSibling();
                    if (sib != 0 && p + sib > maxSibling) maxSibling = p + sib;
                    if ((*nodes)[p].getCow() == false && maxSibling <= p) break;
                }
                p++;
            }
            repEnd.setBit(p, true);
        }
    }
}

/** Init data necessary just for the compression process, put in a separate
 * method to lower maximum amount of allocated memory, as some memory will be
 * freed after fillPositions. */
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::initDataBeforeCompress() {
    // initialize marker of replaced positions
    replaced.resize(N);
    repStart.resize(N);
    repEnd.resize(N);
    for (TIndex i = 0; i < N; ++i) {
        replaced.setBit(i, false);
        repStart.setBit(i, false);
        repEnd.setBit(i, false);
    }
    initBaseBoundaries();
    // prevSilbing
    prevSibling = new TIndex[N];
    for (TIndex i = 0; i < N; i++) prevSibling[i] = 0;

    for (TIndex i = 0; i < N; i++) {
        if ((*nodes)[i].isPointer() == false && (*nodes)[i].getSibling() != 0) {
            assert(i+((*nodes)[i]).getSibling() < N);
            prevSibling[i+((*nodes)[i]).getSibling()] = i;
        }
    }
}

// free all the memory that is not needed after compress()
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::freeMemAfterCompress() {    
    delete [] terminalNode;

    free(lcpTree);
    if (config.pyramid == true) {
        delete [] pyramid;
        delete [] start;
        delete [] end;
    }
    else {                    
        delete [] repPos;
    }        
    delete [] prevSibling;
}

// free memory at the end of the entire compression process
template <typename TNodeArray>
void LCPTreeCompressor<TNodeArray>::freeMemory() {
    replaced.resize(0);
    if (config.visualize) vstream.close();
}

#endif	/* LCPTREECOMPRESSOR_HPP */

