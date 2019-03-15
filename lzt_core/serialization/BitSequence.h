#ifndef BITSEQUENCE_H
#define	BITSEQUENCE_H

#include <cstddef>
#include <string>
#include <sstream>

#include "debug/lzt_test.h"
#include "util/constants.h"
#include "BitPointer.h"
#include "serialization.h"

using namespace std;

class SerializationUtils;
class BitBlocksArray;

/** Efficient bit array for small number of bits. */
class BitSequence {
    
public:
    
    typedef char TBlockType;
    friend class BitBlocksArray;

    BitSequence();
    BitSequence(const BitSequence& orig);
    virtual ~BitSequence();

    bool operator[](int i) const;
    void operator<<=(int shift);
    void operator|=(BitSequence bs);
    bool operator==(BitSequence bs) const;
    bool equals(BitSequence bs, int numBits = -1) const;

    void setBit(int i, bool bit);
    int getSize() const;
    int maxNonzeroBit() const;
    TBlockType const * getBlocks() const;
    static int getNumberOfBits();
    void setFalse();
    string toString() const;

    friend class SerializationUtils;

private:

    // constant for setting the blocks to zero
    static const TBlockType ZERO_BLOCK = 0;
    static const int NUM_OF_BLOCKS = 16;
    static const int BITS_PER_BLOCK = sizeof(TBlockType) * BITS_PER_CHAR;

     // size of a field, in bytes
    static const int BLOCK_SIZE = sizeof(TBlockType);
    // number of bits the sequence can hold
    static const int NUM_OF_BITS = NUM_OF_BLOCKS * BLOCK_SIZE * BITS_PER_CHAR;
    
    // represent one for bitwise operations
    static const TBlockType ONE = 1;

    // bits are stored as an array of fields
    TBlockType blocks[NUM_OF_BLOCKS];
    
};

inline bool BitSequence::operator[](int i) const {
    BitPointer bp(BLOCK_SIZE, i);
    TBlockType block = blocks[bp.blockIndex];
    bool bit = (block >> bp.bitIndex) & ONE;
    return bit;
}

inline void BitSequence::setBit(int i, bool bit) {
    BitPointer bp(BLOCK_SIZE, i);
    if (bit)
        blocks[bp.blockIndex] |= (ONE << bp.bitIndex);
    else
        blocks[bp.blockIndex] &= (~(ONE << bp.bitIndex));
}

/** Returns max. index i such that bit[i] == true, or -1 if all bits are 0. */
inline int BitSequence::maxNonzeroBit() const {
    int h = 0;
    for (int i = NUM_OF_BLOCKS - 1; i >= 0; --i) {
        TBlockType block = blocks[i];
        // if there is nonzero bit in the block
        if (block != (block << BITS_PER_BLOCK) ) {
            //TODO ova logika ovisi o nacinu indeksiranja bitova, koji
            // je djelomicno implementiran u BitPointer-u
            for (int j = BITS_PER_BLOCK - 1; j >= 0; --j)
                if (block >> j & ONE) {
                    return i * BITS_PER_BLOCK + j;
                }
        }
    }
    return -1;
}

/** Set all bits to false. */
inline void BitSequence::setFalse() {
    for (int i = 0; i < NUM_OF_BLOCKS; ++i)
        blocks[i] = ZERO_BLOCK;
}

/** Copies bits of a value of type T to BitSequence. Only the first numOfBits
 * bits are copied, other are undefined. If numOfBits is -1, max possible
 * number of bits is copied and other bits are set to 0. Bit are read using
 * bitwise operators. This function only works for integral types.
 */
template <typename T>
inline BitSequence toBitSequence(T t, int numOfBits = -1) {
    long unsigned int const ONE = 1;
    BitSequence bits;

    if (numOfBits == -1) {
        bits.setFalse();
        // calculate number of bits we work with
        int typeBits = sizeof(t) * BITS_PER_CHAR;
        int maxBits = BitSequence::getNumberOfBits();
        numOfBits = maxBits > typeBits ? typeBits : maxBits;
    }

    for (int i = 0; i < numOfBits; ++i) {
        bool bit = (t >> i) & ONE;
        bits.setBit(i, bit);
    }

    return bits;
}

/** Copies bits from BitSequence to a value of type T, using bitwise
 * operators to set bits of T. Only the first numOfBits bits are copied,
 * other bits are set to 0. This function only works for integral types. */
template <typename T>
inline T fromBitSequence(BitSequence bits, int numOfBits = -1) {
    //long unsigned int const ONE = 1;
    T const ONE = 1;

    if (numOfBits == -1) {
         // calculate number of bits we work with
        int typeBits = sizeof(T) * BITS_PER_CHAR;
        int maxBits = BitSequence::getNumberOfBits();
        numOfBits = maxBits > typeBits ? typeBits : maxBits;
    }

    // init return value - all bits to 0
    T val; val &= 0ul;
    for (int i = 0; i < numOfBits; ++i) {
        if (bits[i]) val |= ONE << i;
        else val &= ~(ONE << i);
    }

    return val;
}

// TODO introduce parameter B that inicates how many bits will be used - 
// so only this bits have to be cleared.
/** Returns binary representation of a modulo of an integer number,
 * ignoring signum of the number. */
template <typename T>
inline BitSequence numberToBits(T number) {
    BitSequence bits;
    bits.setFalse();

    int i = 0;
    while (number != 0) {
        bool bit;

        if (number % 2 == 0) bit = false;
        else bit = true;

        bits.setBit(i, bit);
        i++;
        number /= 2;
    }
    return bits;
}

/** Interpret max bits (bits[0..max-1] if max != 0, all bits otherwise)
 * as a binary number and convert to a positive or negative number,
 * depending on signum parameter. */
template <typename T>
inline T numberFromBits(BitSequence bits, int max = 0, bool negative = false) {
    T number = 0;  T exp2;
    
    if (negative) exp2 = -1;
    else exp2 = 1;

    if (max == 0) max = bits.maxNonzeroBit() + 1;

    for (int i = 0; i < max; ++i ) {
        if (bits[i]) number += exp2;

        exp2 *= 2;
    }
    return number;
}

#endif	/* BITSEQUENCE_H */

