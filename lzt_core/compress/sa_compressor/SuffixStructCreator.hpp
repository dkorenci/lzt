#ifndef SACOMPRESSORBASE_HPP
#define	SACOMPRESSORBASE_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <map>

#include "SaIsCreator.hpp"

using namespace std;

/** Class that creates suffix array based structures. */
template <typename TNodeArray>
class SuffixStructCreator {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

public:
    SuffixStructCreator(bool saf = false);

    void setNodes(TNodeArray* n);
    TIndex* createSAwithSort();
    TIndex* createSuffixArray();
    TIndex* createLCPBruteForce();
    TIndex* createLCPArray();
    TIndex* createLPFArray();
    TIndex* createLPFPosArray();
    TIndex* createInverseSA();
    int* indexNodes();
    TIndex calcLcp(TIndex i1, TIndex i2);

private:

    static bool fileExists(string file);
    void readSuffixArray();
    void writeSuffixArray();

    TIndex N;
    int alphabetSize;

    TNodeArray* nodes;
    TIndex* suffArray;
    TIndex* inverseSA;
    TIndex* lcp;
    TIndex* lpf;
    TIndex* lpfPos;

    // flag indicating weather to read suffix array from file
    // and store it to file after it is constructed
    bool saFile;
    string saFileName;

    /** Comparator of indexes in the suffix array. */
    class SACompare {
    public:
        SACompare(TNodeArray& n): nodes(n), N(n.getSize()) {}
        bool operator()(TIndex i1, TIndex i2) {
            if (i1 == i2) return false;

            while (i1 < N && i2 < N) {
                if (nodes[i1] < nodes[i2]) return true;
                if (nodes[i2] < nodes[i1]) return false;

                i1++; i2++;
            }

            // last (fictional) character (at index N) is smaller than all the others
            if (i2 == N) return false;            
            else return true;
        }

    private:
        TIndex N;
        TNodeArray& nodes;

    };

    /* Position in the suffix array and position where
     * (currently) best lpf for that position starts. */
    struct TLpfInfo {
        TLpfInfo(TIndex p, TIndex lp): pos(p), lpfPos(lp) {}
        TIndex pos, lpfPos;
    };

};

template <typename TNodeArray>
SuffixStructCreator<TNodeArray>::
SuffixStructCreator(bool saf): saFile(saf), saFileName("suffix_array.txt") {}

/** Set node array for wich to calculate suffix structures and init data. */
template <typename TNodeArray>
void SuffixStructCreator<TNodeArray>::setNodes(TNodeArray* n) {
    nodes = n;
    suffArray = 0;
    inverseSA = 0;
    lcp = 0;
    lpf = 0;
    lpfPos = 0;
    N = nodes->getSize();
}

template <typename TNodeArray>
bool SuffixStructCreator<TNodeArray>::fileExists(string file) {
    ifstream ifile(file.c_str());
    return ifile.good();
}

template <typename TNodeArray>
void SuffixStructCreator<TNodeArray>::readSuffixArray() {
    ifstream ifile(saFileName.c_str());
    ifile >> N;
    suffArray = new TIndex[N+1];    
    for (TIndex i = 0; i <= N; ++i)
        ifile >> suffArray[i];
}

template <typename TNodeArray>
void SuffixStructCreator<TNodeArray>::writeSuffixArray() {
    fstream file(saFileName.c_str(), ios_base::out);
    file << N << endl;
    for (TIndex i = 0; i <= N; ++i)
        file << suffArray[i] << endl;
}

/** Create suffix array by brute force suffix sort. */
template <typename TNodeArray>
typename TNodeArray::Index* SuffixStructCreator<TNodeArray>::createSAwithSort() {
    if (saFile && fileExists(saFileName)) {
        readSuffixArray();
    }
    else {
        suffArray = new TIndex[N];
        for (TIndex i = 0; i < N; ++i) suffArray[i] = i;

        SACompare comp(*nodes);
        sort(suffArray, suffArray + N, comp);
        
        if (saFile) writeSuffixArray();
    }

    return suffArray;
}

/** Create suffix array using SAIS induced sorting algorithm */
template <typename TNodeArray>
typename TNodeArray::Index* SuffixStructCreator<TNodeArray>::createSuffixArray() {
    if (saFile && fileExists(saFileName)) {
        readSuffixArray();
    }
    else {
        int* indexes = indexNodes();
        suffArray = new TIndex[N+1];

        SA_IS<int, TIndex>(indexes, suffArray, N+1, alphabetSize);
        delete [] indexes;

        // last suffix (sentinel char) must be lexicographically smallest
        assert(suffArray[0] == N);
        // remove sentinel char
        for (TIndex i = 1; i <= N; ++i) suffArray[i-1] = suffArray[i];

        if (saFile) writeSuffixArray();
    }

    return suffArray;
}

/** Calculate the set of distinct array nodes and create an array with
 * nodes replaced by their ordinal in the set. */
template <typename TNodeArray>
int* SuffixStructCreator<TNodeArray>::indexNodes() {
    map<TNode, int> inodes; // map of nodes to their ordinal number

    for (TIndex i = 0; i < N; ++i)
        inodes[(*nodes)[i]] = 0;    

    typename  map<TNode, int>::iterator it; int i = 0;
    for (it = inodes.begin(); it != inodes.end(); ++it, ++i) {
        it->second = i;
    }

    alphabetSize = i;

    int* indexArray = new int[N+1];
    for (TIndex i = 0; i < N; ++i) {
        indexArray[i] = inodes[(*nodes)[i]]+1;
    }
    indexArray[N] = 0;

    return indexArray;    
}

template <typename TNodeArray>
typename TNodeArray::Index* SuffixStructCreator<TNodeArray>::createInverseSA() {
    if (suffArray == 0) createSuffixArray();

    inverseSA = new TIndex[N];
    for (TIndex i = 0; i < N; ++i)
        inverseSA[suffArray[i]] = i;

    return inverseSA;
}

template <typename TNodeArray>
typename TNodeArray::Index* SuffixStructCreator<TNodeArray>::createLCPBruteForce() {
    lcp = new TIndex[N+1];

    //fstream lcpout("lcp.txt", ios_base::out);

    lcp[0] = 0;
    for (TIndex i = 1; i < N; ++i) {
        lcp[i] = calcLcp(suffArray[i-1], suffArray[i]);
        //lcpout << lcp[i] << " " << suffArray[i] << endl;
    }
    lcp[N] = 0;

    return lcp;
}

/** Create LCP array using Kasai(et.al.)'s algorithm. */
template <typename TNodeArray>
typename TNodeArray::Index* SuffixStructCreator<TNodeArray>::createLCPArray() {
    lcp = new TIndex[N+1];
    createInverseSA();

    TIndex l = 0, sai;
    for (TIndex i = 0 ; i < N ; ++i) {
        sai = inverseSA[i];
        if (sai > 0) {
            TIndex k = suffArray[sai-1];
            while ( i+l < N && k+l < N && (*nodes)[i+l] == (*nodes)[k+l] ) l++;
        }
        else {
            l = 0;
        }
        lcp[sai] = l;
        l = l > 1 ? l-1 : 0;
    }
    lcp[N] = 0;

    return lcp;
}

/** Given indexes of two suffixes in the node array, calculate their
 * longest common prefix (non-overlapping). */
template <typename TNodeArray>
typename TNodeArray::Index SuffixStructCreator<TNodeArray>::calcLcp(TIndex i1, TIndex i2) {
    if (i2 < i1) { TIndex t = i1; i1 = i2; i2 = t; }

    TIndex lcp = 0;

    while (i2 < N && (*nodes)[i1] == (*nodes)[i2]) {
        i1++; i2++; lcp++;
    }

    return lcp;
}

/** Calculate longest previous factors for all the positions
 * in the array using suffix array and lcp array. */
template <typename TNodeArray>
typename TNodeArray::Index* SuffixStructCreator<TNodeArray>::createLPFArray() {
    lpf = new TIndex[N];
    lpfPos = new TIndex[N];
    /* Indicator of no previous factor, just for valitation, it
     * should occur only when lpf is set to 0 for a position, could
     * produce an error with a large array. */
    const TIndex NO_PREV = INT_MAX;
    stack<TLpfInfo> S;
    TLpfInfo init(0, NO_PREV); S.push(init);
    lcp[N] = 0;

    for (TIndex i = 1; i <= N; ++i) {
        // data for current position that will be pushed after processing
        TLpfInfo newpos(i, NO_PREV);

        while (S.empty() == false) {
            bool end = (i == N);
            TIndex top = S.top().pos;
            if ( (end || suffArray[i] < suffArray[top]) ||
                 (suffArray[i] > suffArray[top]) && (lcp[i] <= lcp[top]) ) {

                if (end || suffArray[i] < suffArray[top]) {
                    if (lcp[top] > lcp[i]) {
                        lpf[suffArray[top]] = lcp[top];
                        lpfPos[suffArray[top]] = S.top().lpfPos;
                    }
                    else {
                        lpf[suffArray[top]] = lcp[i];
                        lpfPos[suffArray[top]] = suffArray[i];
                    }
                    lcp[i] = min(lcp[top], lcp[i]);
                }
                else {
                    lpf[suffArray[top]] = lcp[top];
                    lpfPos[suffArray[top]] = S.top().lpfPos;

                    if (lpf[suffArray[top]] == 0)
                        assert(lpfPos[suffArray[top]] == NO_PREV);
                }
            }
            else {
                newpos.lpfPos = suffArray[top];
                break;
            }

            S.pop();

            if (i < N && S.empty()) {
                assert(lcp[i] == 0);
            }
        }

        if (i < N) {
            S.push(newpos);
        }
    }

    double lpfAvg = 0;
    //validation
    for (TIndex i = 0; i < N; ++i) {
        if (lpf[i] != 0) {
            assert(lpfPos[i] < i);
            assert(calcLcp(lpfPos[i], i) == lpf[i]);
        }
        lpfAvg += lpf[i];
    }

    return lpf;
}

template <typename TNodeArray>
typename TNodeArray::Index* SuffixStructCreator<TNodeArray>::createLPFPosArray() {
    if (lpf == 0) createLPFArray();
    return lpfPos;
}

#endif	/* SACOMPRESSORBASE_HPP */

