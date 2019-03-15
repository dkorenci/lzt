#include "HuffmanDecoder.h"
#include <iostream>

#include "serialization/SerializationUtils.h"
#include "serialization/array/BitSequenceArraySer.h"

HuffmanDecoder::~HuffmanDecoder() {}

/** Put decoder in a state before reading the first bit. */
void HuffmanDecoder::reset() {
    endOfCode = false;
    length = 0;
    value = 0;
}

/** Process the next bit from the coded sequence of bits. */
void HuffmanDecoder::processBit(bool bit) {
    length++;
    value = 2 * value + (bit ? 1 : 0);    
    if (value >= firstCode[length-1]) endOfCode = true;
}

/** Return true if the last processed bit was an end of a symbol code. */
bool HuffmanDecoder::isEndOfCode() {
    return endOfCode;
}

/** Return symbol whose code ended with last processed bit. */
BitSequence HuffmanDecoder::getSymbol() {
    size_t l = length, v = value;
    reset();
    return symbolTable[ lengthOffset[l-1] + (v - firstCode[l-1]) ];
}

int HuffmanDecoder::bitsPerSymbol() {
    return symbolTable.getSequenceSize();
}

/** Serialize decoder to stream. */
void HuffmanDecoder::writeToStream(ostream& stream) const {
    BitSequenceArraySer::arrayToStream(symbolTable, stream);
    firstCode.writeToStream(stream);
    lengthOffset.writeToStream(stream);
}

/** Deserialize decoder from stream. */
void HuffmanDecoder::readFromStream(istream& stream) {
    BitSequenceArraySer::arrayFromStream(symbolTable, stream);
    firstCode.readFromStream(stream);
    lengthOffset.readFromStream(stream);
}