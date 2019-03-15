#include <cstdlib>

#include "BitSequenceArray.h"

/** Create empty array. */
BitSequenceArray::BitSequenceArray()
: bitsPerSequence(0), numOfSequences(0), numOfBlocks(0), blocks(NULL) {

}

BitSequenceArray::BitSequenceArray(size_t size, int bitsPerSeq): blocks(NULL) {
    changeFormat(size, bitsPerSeq);
}

BitSequenceArray::BitSequenceArray(const BitSequenceArray& src)
: numOfBlocks(src.numOfBlocks), numOfSequences(src.numOfSequences),
  bitsPerSequence(src.bitsPerSequence) {
    freeBlocks();
    allocateBlocks();
    for (size_t i = 0; i < numOfBlocks; ++i) blocks[i] = src.blocks[i];
}

/** Resize the array so that it can store newSize sequences, keeping the
 * values of the sequences up to the lesser of two sizes. */
void BitSequenceArray::resize(size_t newSize) {
    numOfSequences = newSize;
    size_t numOfBits = numOfSequences * bitsPerSequence;
    numOfBlocks = numberOfBlocks(numOfBits, BLOCK_SIZE);
    blocks = (char *)realloc(blocks, numOfBlocks);
}

/** Change size and/or bitsPerSequence to new values, reallocating if
 * necessary, if the bitsPerSeq changed, values in the new array are undefinded,
 * else the array is just resized. */
void BitSequenceArray::changeFormat(size_t size, int bitsPerSeq) {
    freeBlocks();
    bitsPerSequence = bitsPerSeq; numOfSequences = size;
    size_t numOfBits = numOfSequences * bitsPerSequence;
    numOfBlocks = numberOfBlocks(numOfBits, BLOCK_SIZE);
    allocateBlocks();
}

BitSequenceArray& BitSequenceArray::operator=(const BitSequenceArray& rhs) {
    if (this == &rhs) return *this;

    numOfBlocks = rhs.numOfBlocks;
    numOfSequences = rhs.numOfSequences;
    bitsPerSequence = rhs.bitsPerSequence;

    freeBlocks();
    allocateBlocks();
    for (size_t i = 0; i < numOfBlocks; ++i) blocks[i] = rhs.blocks[i];

    return *this;
}

BitSequenceArray::~BitSequenceArray() {
    freeBlocks();
}

/** Reallocate memory for numOfBlocks blocks. */
void BitSequenceArray::allocateBlocks() {
    blocks = (char *)malloc(numOfBlocks);
}

/** Free blocks memory. */
void BitSequenceArray::freeBlocks() {
 if (blocks != NULL) {
     free(blocks);
     blocks = NULL;
 }
}

/** Set all bits in the array to 0. */
void BitSequenceArray::nullArray() {
    for (size_t i = 0; i < numOfBlocks; ++i)
        blocks[i] = zeroBits<char>();    
}

/** Set i-th bit sequence to seq, i is zero-based. */
void BitSequenceArray::setSequence(size_t index, BitSequence seq) {
    // pointer to the bit in the array where first bit of the seq will be stored
    BitPointer bp(1, index * bitsPerSequence);

    for (int i = 0; i < bitsPerSequence; ++i) {
        bool bit = seq[i];
        
        if (bit)
            blocks[bp.blockIndex] |= (ONE << bp.bitIndex);
        else
            blocks[bp.blockIndex] &= ~(ONE << bp.bitIndex);

        bp.increment();
    }
}

char const * BitSequenceArray::getBlocks() const {
    return blocks;
}

size_t BitSequenceArray::getNumOfBlocks() const {
    return numOfBlocks;
}



BSAEquals::BSAEquals(const BitSequenceArray& a): array(a) {
    bits = array.getSequenceSize();
}

bool BSAEquals::operator()(size_t i1, size_t i2) const {
    return array[i1].equals(array[i2], bits);
}

