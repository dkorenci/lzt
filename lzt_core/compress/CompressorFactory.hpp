#ifndef COMPRESSORFACTORY_HPP
#define	COMPRESSORFACTORY_HPP

#include <string>

#include "ICompressor.h"
#include "NonCompressor.hpp"
#include "lz_compressor/LzCompressor.h"
#include "sa_compressor/SaCompressor.hpp"
#include "sa_compressor/LCPTreeCompressor.hpp"
#include "fsa_convert/FsaConverter.hpp"

using namespace std;

/** Class for creating ICompressor instances. */
template <typename TNodeArray>
class CompressorFactory {
    
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

public:

    static ICompressor<TNodeArray>* createFromDesc(string desc);

};

/** Create ICompressor from string description.
 * string format: AlgoId[r][d]
 * AlgoId is a 3 uppercase letter code, with following code mappings:
 * NOC - no compressor, just pass the array through, for comparison
 * SQR - square algorithm, LCP - lcp intervals,
 * SEQ - sequential pairwise, LPF - largest previous factor,
 * LCT - lcp interval tree
 * Options are:
 * for SQR, LCP, SEQ, LPF algorithms:
 * 'v' to visualize node array and aux. data during compression
 * for non-SQR:
 * 'r' specifies recursion in replace (has no effect on SQR),
 * 'd' specifies debug output.
 * for SQR:
 * 'b' for pointer breaks
 * 'g' output graphviz
 * Return 0 if compressor is incorrectly specified.
 * FSA - build FSA and convert it to LZT */
template <typename TNodeArray>
ICompressor<TNodeArray>* CompressorFactory<TNodeArray>::createFromDesc(string desc) {
    if (desc.length() < 3) return 0;
    string algoId = desc.substr(0,3);

    if (algoId.compare("NOC") == 0) {
        return new NonCompressor<TNodeArray>();
    }
    else if (algoId.compare("SQR") == 0) {
        LzCompressorConfig conf;

        if (desc.length() > 3) {
            string switches = desc.substr(3);
            if (switches.find('b') != string::npos) conf.pointerBreak = true;
            if (switches.find('v') != string::npos) conf.visualize = true;
            if (switches.find('g') != string::npos) conf.graphViz = true;
        }

        return new LzCompressor<TNodeArray>(conf);
    }
    else if (algoId.compare("LCT") == 0) {
        LctCompressorConfig conf;
        if (desc.length() > 3) {
            string switches = desc.substr(3);
            if (switches.find('f') != string::npos) conf.saFile = true;
            if (switches.find('v') != string::npos) conf.visualize = true;
            if (switches.find('m') != string::npos) conf.memSizes = true;
            if (switches.find('p') != string::npos) conf.pyramid = true;
        }

        return new LCPTreeCompressor<TNodeArray>(conf);
    }
    else if (algoId.compare("FSA") == 0) {
        bool compress = false;
        if (desc.length() > 3) {
            string switches = desc.substr(3);
            if (switches.find('c') != string::npos) compress = true;
        }       
        return new FsaConverter<TNodeArray>(compress);
    }
    else {
        SaCompressorConfig conf;

        if (algoId.compare("LCP") == 0) conf.algorithm = LCP_INT;
        else if (algoId.compare("LPF") == 0) conf.algorithm = LPF;
        else if (algoId.compare("SEQ") == 0) conf.algorithm = SEQ_PAIRWISE;
        else if (algoId.compare("LCG") == 0) conf.algorithm = LCP_INT_GR;

        if (desc.length() > 3) {
            string switches = desc.substr(3);
            if (switches.find('r') != string::npos) conf.recReplace = true;
            if (switches.find('d') != string::npos) conf.debug = true;
            if (switches.find('b') != string::npos) conf.pointerBreak = true;
            if (switches.find('w') != string::npos) conf.grOrder = WEIGHT;
            if (switches.find('i') != string::npos) conf.grOrder = INTERFERENCE;
            if (switches.find('v') != string::npos) conf.visualize = true;
        }

        return new SaCompressor<TNodeArray>(conf);
    }
}

#endif	/* COMPRESSORFACTORY_HPP */