#ifndef HUFFMANDECODERCREATOR_H
#define	HUFFMANDECODERCREATOR_H

#include <cstddef>

#include "HuffmanDecoder.h"
#include "HuffmanCoder.h"
#include "serialization/BitSequenceArray.h"
#include "serialization/BitSequence.h"

/** Creates HuffmanDecoder for given symbols and their frequencies. */
class HuffmanCodecCreator {
public:

    HuffmanCodecCreator(size_t numSymbols, const size_t* freqs);
    virtual ~HuffmanCodecCreator();

    void createDecoder(HuffmanDecoder* dec, const BitSequenceArray& symbols);
    void createCoder(HuffmanCoder* coder);

private:

    // number of symbols
    size_t N;
    // size of the heap used for calculating huff. code lengths
    size_t H;
    // max. length of a simbol code
    int maxLen;
    // frequencies of the symbols
    const size_t *freqs;
    // length of symbol codes
    size_t *lengths;
    // number of symbols with code of some length
    size_t *numLengths;
    // array for calculation of the lengths of huffman codes
    size_t *A;
    // beginning of a sequence of cannonical huff. codes with given length
    size_t *firstCode;


    void calcLengths();
    void calcStartCodes();

    void heapSift(size_t i);

};

#endif	/* HUFFMANDECODERCREATOR_H */

