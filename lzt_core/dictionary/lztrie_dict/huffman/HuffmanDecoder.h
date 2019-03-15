#ifndef HUFFMANDECODER_H
#define	HUFFMANDECODER_H

#include <cstddef>
#include <iostream>

#include "serialization/BitSequenceArray.h"
#include "serialization/BitSequence.h"
#include "serialization/IntBitArray.h"

class HuffmanCodecCreator;
class HuffmanTest;

/** Class that encapsulates all the data for decoding of canonnical
 * huffman codes: symbol table and first codes for length. Data is
 * represented in a compact and serializable way.
 * Client should call start() before processing, call processBit()
 * for each bit in coded sequence, check if a bit isEndOfCode()
 * and if yes, call getSymbol().
 * Instances of HuffmanDecoder are created via HuffmanCodecCreator. */
class HuffmanDecoder {
public:

    virtual ~HuffmanDecoder();

    void reset();
    void processBit(bool bit);
    bool isEndOfCode();
    BitSequence getSymbol();
    int bitsPerSymbol();

    void writeToStream(ostream& stream) const;
    void readFromStream(istream& stream);

    friend class HuffmanCodecCreator;
    friend class HuffmanTest;

private:

    /* Values of the first cannonical huffman codes for given length,
     * indexes are zero based. */
    IntBitArray<size_t> firstCode;
    /* Start index (in the symbolTable array) for symbols whose
     * codes are of given length, indexes are zero based. */
    IntBitArray<size_t> lengthOffset;

    BitSequenceArray symbolTable;

    // indicated if end of symbol code was reached for last processed bit
    bool endOfCode;
    // number of bits processed for symbol code being currently processed
    size_t length;
    // numeric value a processed bits
    size_t value;

};

#endif	/* HUFFMANDECODER_H */

