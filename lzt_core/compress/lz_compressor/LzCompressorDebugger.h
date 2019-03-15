/* 
 * File:   LzCompressorDebugger.h
 * Author: dam1root
 *
 * Created on April 5, 2011, 11:05 AM
 */

#ifndef LZCOMPRESSORDEBUGGER_H
#define	LZCOMPRESSORDEBUGGER_H

#include <cstddef>
#include <algorithm>
#include <fstream>

#include "LzCompressor.h"
#include "node_array/types/symbol.h"
#include "node_array/na_utils.h"
#include "util/utils.h"

//forward declaration
template <typename TNodeArray> class LzCompressor;


template <typename TNodeArray>
class LzCompressorDebugger {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

public:

    LzCompressorDebugger(LzCompressor<TNodeArray>& c);

    //LzCompressorDebugger(const LzCompressorDebugger& orig);

    virtual ~LzCompressorDebugger() {};

    void dumpData(int N, int ints[], const char* messages[], const char *fileName);
    void dumpNodeArray(const char *fileName);
    void checkExpandedLength();
    string arrayToString();

    //TODO vidi dali je potrebno
    // poziva se da bi se instancirale template funkcije, da ih se vidi iz debuggera
    void instantiateTemplates();

    string * getCRSData(TIndex i, TIndex j, TIndex length, int newMatch);
    void logReplacement(TIndex b, TIndex r, TSymbol match, string message);
    string getReplacementData(TIndex b, TIndex r, TSymbol match);
    void logNodeArray(TIndex seg[][2], size_t nr, string message);
    string getNodeArraySegments(TIndex seg[][2], size_t nr);

    // functions for viewing data state from debugger sessions
    const char * nodesToString();
    const char * nodesToString(TIndex b, TIndex e);
    const char * symbolsToString(TIndex b, TIndex e);
    const char * expLenToString(size_t b, size_t e);


private:

    LzCompressor<TNodeArray>& C;

    // index of a node replacements to whose neighborhood are logged
    static TIndex const baseRepToLog = 929004;
    // offset that defines neighbourhood 
    static TIndex const baseRepToLogOff = 200;
    
    ofstream logStream;

};

template <typename TNodeArray>
LzCompressorDebugger<TNodeArray>::LzCompressorDebugger(LzCompressor<TNodeArray> & c)
: C(c), logStream("tmp/debug.txt") { }

template <typename TNodeArray>
void LzCompressorDebugger<TNodeArray>::
dumpData(int n, int ints[], const char * messages[], const char *fileName) {
    ofstream file(fileName);
    for (int i = 0; i < n; ++i) {
        if (messages != NULL) file<<messages[i]<<" "<<ints[i]<<endl;
        else file<<ints[i]<<endl;
    }
    file<<flush;
}

template <typename TNodeArray>
void LzCompressorDebugger<TNodeArray>::dumpNodeArray(const char *fileName) {
    ofstream file(fileName);
    file<<nodesToString(*C.nodes);
    file<<flush;
}

template <typename TNodeArray>
void LzCompressorDebugger<TNodeArray>::checkExpandedLength() {
    bool correct = true;
    for (TIndex i = 0; i < C.N; i++) {
        if ( C.expandedLength[i] == 0) continue;
        if ( C.expandedLength[i] >= C.uncompressedN ) {
            cout<<i<<"N";
            correct = false;
        }
        if ( C.expandedLength[i] < 0) {
            cout<<i<<"-";
            correct = false;
        }
    }
    cout<<endl;
    assert(correct);
}

/** Format data describing a state correctReplacedSizes method. */
template <typename TNodeArray>
string * LzCompressorDebugger<TNodeArray>::
getCRSData(TIndex i, TIndex j, TIndex length, int newMatch) {
    stringstream ss;
    TIndex r = (*C.nodes)[i].getSibling();
    // TODO zahtjev na tip
    const TIndex O =  ((int)maxSymbol<TSymbol>()+1) * 2;
    TIndex offi = min(i + O, C.N);
    TIndex offr = min(r + O, C.N);
    ss << "replaced: " << i << " replacing: " << r << " array size: " << C.N << endl;
    ss << "correction stopped with index: " << j 
       << " length: " << length << " newMatch: " << newMatch << endl;
    ss << "nodes @ replaced: " << subNodeArrayToString(*C.nodes, i, offi) << endl;
    ss << "lengths @ replaced: " << subArrayToString(C.expandedLength, i, offi) << endl;
    ss << "nodes @ replacing: " << subNodeArrayToString(*C.nodes, r, offr) << endl;
    ss << "lengths @ replacing: " << subArrayToString(C.expandedLength, r, offr) << endl;
    ss << "node array: " << nodeArrayToString(*C.nodes) << endl;
    ss << "lengths: " << subArrayToString(C.expandedLength, 0, C.N) << endl;
    return new string(ss.str());
}

/** Log data relevant for pointer replacement. */
template <typename TNodeArray>
void LzCompressorDebugger<TNodeArray>::logReplacement(TIndex b, TIndex r, TSymbol match, string message) {
    if ( (b >= baseRepToLog && b <= baseRepToLog + baseRepToLogOff ) == false )
        return;
    
    logStream << message << endl;
    logStream << getReplacementData(b, r, match);
    logStream << "nodes: " << endl << nodeArrayToString(*C.nodes) 
                << endl << endl;
    logStream.flush();
}

/** Get data relevant for pointer replacement. */
template <typename TNodeArray>
string LzCompressorDebugger<TNodeArray>::getReplacementData(TIndex b, TIndex r, TSymbol match) {
    stringstream ss;
    const TIndex O =  1000;
    TIndex offb = min(b + O, C.N);
    TIndex offr = min(r + O, C.N);

    ss << "replacing: " << b << " replaced: " << r << " match: " <<  symbolToInt(match) << endl;
    ss << "nodes @ replaced: " << endl << subNodeArrayToString(*C.nodes, r, offr) << endl;
    ss << "isReplaced @ replaced: " << endl <<  subArrayToString(*C.replaced , r, offr) << endl;
    ss << "nodes @ replacing: " << endl << subNodeArrayToString(*C.nodes, b, offb) << endl;
    ss << "isReplaced @ replacing: " << endl <<  subArrayToString(*C.replaced , b, offb) << endl;

    return ss.str();
}

/** Log a number of node array ranges and the node array itself */
template <typename TNodeArray>
void LzCompressorDebugger<TNodeArray>::logNodeArray(TIndex seg[][2], size_t ns, string message) {
    logStream << message << endl;
    logStream << "segments: " << endl;
    logStream << getNodeArraySegments(seg, ns);
    logStream << "nodeArray: " << endl;
    logStream << nodeArrayToString(*C.nodes) << endl << endl;
    logStream.flush();
}

/** Get a number of nodeArray segments. */
template <typename TNodeArray>
string LzCompressorDebugger<TNodeArray>::getNodeArraySegments(TIndex seg[][2], size_t ns) {
    stringstream ss;
    for (size_t i = 0; i < ns; ++i) {
        TIndex c = seg[i][0]; TIndex off = min(c + seg[i][1], C.N);
        ss << "nodes[" << c << "," << off << "> :" << endl;
        ss << subNodeArrayToString(*C.nodes, c, off) << endl;
    }
    return ss.str();
}

/**
 * To call certain functions from within the debugging session, for watches,
 * the have to be instantiated first. 
 */
template <typename TNodeArray>
void LzCompressorDebugger<TNodeArray>::instantiateTemplates() {
    nodesToString();
    //TODO zahtjev na tip
    TIndex b = 0, e = 0;
    symbolsToString(b, e);
    nodesToString(b, e);
    expLenToString(0, 0);
}

template <typename TNodeArray>
const char * LzCompressorDebugger<TNodeArray>::nodesToString() {
    return nodeArrayToString(*C.nodes).c_str();
}

template <typename TNodeArray>
const char * LzCompressorDebugger<TNodeArray>::nodesToString(TIndex b, TIndex e) {
    return subNodeArrayToString(*C.nodes, b, e).c_str();
}

template <typename TNodeArray>
const char * LzCompressorDebugger<TNodeArray>::symbolsToString(TIndex b, TIndex e) {
    return nodeArraySymbolsToString(*C.nodes, b, e).c_str();
}

template <typename TNodeArray>
const char * LzCompressorDebugger<TNodeArray>::expLenToString(size_t b, size_t e) {
    return subArrayToString(C.expandedLength, b, e).c_str();
}

#endif	/* LZCOMPRESSORDEBUGGER_H */

