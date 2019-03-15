#ifndef BITVECTOR_H
#define	BITVECTOR_H

#include <cstddef>
#include <string>

#include "BitSequence.h"

using namespace std;

/** Class for compact storage of bits. */
class BitVector {
public:

    BitVector(size_t size = 0);
    virtual ~BitVector();

    size_t getSize() const;

    bool operator[](size_t bitIndex) const;
    void setBit(size_t bitIndex, bool bit);

    BitSequence getSequence(size_t startBit, int seqSize) const;
    void setSequence(size_t startBit, BitSequence seq, int seqSize);

    void resize(size_t sz);

    string toString();

    void writeToStream(ostream& stream) const;
    void readFromStream(istream& stream);

private:    

    // size in bits
    size_t size;
    // number of blocks for storage of bits
    size_t numBlocks;

    static int const BLOCK_SIZE = sizeof(char);
    static char const ONE = 1;

    char* blocks;   

    void reallocateBlocks();
    void freeBlocks();

};

inline bool BitVector::operator[](size_t i) const {
    BitPointer bp(1, i);
    return (bool)(blocks[bp.blockIndex] & ONE << bp.bitIndex);
}

inline void BitVector::setBit(size_t i, bool bit) {
    BitPointer bp(1, i);
    if (bit) blocks[bp.blockIndex] |= (ONE << bp.bitIndex);
    else blocks[bp.blockIndex] &= ~(ONE << bp.bitIndex);
}

/** Copy seqSize bits (or maximum number of bits that a sequence can hold
 * if seqSize is larger) from vector to a BitSequence. Vector bits are
 * copied starting from position startBit until seqSize bits are copied or
 * end of vector occurs.  All other bits of the sequence are set to 0. */
inline BitSequence BitVector::getSequence(size_t startBit, int seqSize) const {
    BitSequence seq; seq.setFalse();
    if (seqSize > seq.getNumberOfBits()) seqSize = seq.getNumberOfBits();

    BitPointer bp(1, startBit);
    for (int i = 0; i < seqSize && startBit < size; ++i, ++startBit) {
        bool bit = (bool)(blocks[bp.blockIndex] & ONE << bp.bitIndex);
        seq.setBit(i, bit);
        bp.increment();
    }

    return seq;
}

/** seqSize bits (or max. number of bits the sequence can hold) from
 * BitSequence are copied to vector, starting from startBit going until
 * seqSize bits are copied or end of vector is reached. */
inline void BitVector::setSequence(size_t startBit, BitSequence seq, int seqSize) {
    if (seqSize > seq.getNumberOfBits()) seqSize = seq.getNumberOfBits();

    BitPointer bp(1, startBit);
    for (int i = 0; i < seqSize && startBit < size; ++i, ++startBit) {
        bool bit = seq[i];

        if (bit) blocks[bp.blockIndex] |= (ONE << bp.bitIndex);
        else blocks[bp.blockIndex] &= ~(ONE << bp.bitIndex);

        bp.increment();
    }
}

#endif	/* BITVECTOR_H */

