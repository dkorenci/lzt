#include "HuffmanCoder.h"
#include "serialization/BitSequence.h"

HuffmanCoder::HuffmanCoder(): codes(NULL), lengths(NULL) {}

HuffmanCoder::~HuffmanCoder() {
    if (codes != NULL) delete [] codes;
    if (lengths != NULL) delete [] lengths;
}

/** Return huffman code for symbol with index i. */
HuffmanCode HuffmanCoder::getCode(size_t i) {
    HuffmanCode hcode;
    hcode.length = lengths[i];
    int l = lengths[i]; size_t c = codes[i];
    /* Write binary representation of c to first l bits of hcode.code,
     * with least significant bit at rightmost position, and pad with zeroes
     * to the left if neccessary. I.E. for c = 2 and l = 4, write 0010.
     * Only first l bits are set. */
    for (int i = l-1; i >= 0; --i) {
        bool bit;
        if (c > 0) bit = (c % 2 == 1 ? true : false);
        else bit = false;
        hcode.code.setBit(i, bit);
        c /= 2;
    }    

    return hcode;
}

/** Get length of huffman code with index i. */
int HuffmanCoder::getLength(size_t i) {
    return lengths[i];
}
