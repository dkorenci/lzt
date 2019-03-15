#include <cstdlib>

#include "BitSequence.h"

BitSequence::BitSequence() { }

BitSequence::BitSequence(const BitSequence& orig) {
    for (int i = 0; i < NUM_OF_BLOCKS; ++i)
        blocks[i] = orig.blocks[i];
}

BitSequence::~BitSequence() { }

/** Shift bits for shift positions towards higher indexes.
   Set bits newly shifted in to false. */
void BitSequence::operator<<=(int shift) {
    // check if we shift out entire sequence
    if (shift >= NUM_OF_BITS) {
        setFalse();
        return;
    }
    if (shift <= 0) return;

    // position to wich bits will be copied
    BitPointer high(BLOCK_SIZE, NUM_OF_BITS - 1);
    // position from wich bits will be copied
    BitPointer low(BLOCK_SIZE, NUM_OF_BITS -1 - shift);
    // position to wich first bit will be moved, all lower bits will be 0;
    BitPointer last(BLOCK_SIZE, shift);

    for (int i = 0; i < NUM_OF_BITS; ++i) {
        if (high < last) {
            // set bit to 0
            blocks[high.blockIndex] &= (~(ONE << high.bitIndex));
        }
        else {
            // copy bit from low to high position
            bool lowBit = (blocks[low.blockIndex] >> low.bitIndex) & ONE;
            
            if (lowBit)
                blocks[high.blockIndex] |= (ONE << high.bitIndex);
            else
                blocks[high.blockIndex] &= (~(ONE << high.bitIndex));
        }

        high.decrement();
        low.decrement();
    }
}

/** Perform bitwise or operation with bits from bs. */
void BitSequence::operator|=(BitSequence bs) {
    for (int i = 0; i < NUM_OF_BLOCKS; ++i)
        blocks[i] |= bs.blocks[i];
}

bool BitSequence::operator==(BitSequence bs) const {
    for (int i = 0; i < NUM_OF_BLOCKS; ++i)
        if (blocks[i] != bs.blocks[i]) return false;
    
    return true;
}

/** Return true if two sequences match in first numBits, or all bits
 * if numBits == -1. */
bool BitSequence::equals(BitSequence bs, int numBits) const {
    if (numBits >= NUM_OF_BITS) numBits = -1;

    // number of blocks that are fully contained within numBits
    int fullBlocks;
    if (numBits == -1) fullBlocks = NUM_OF_BLOCKS;
    else fullBlocks = numBits / BITS_PER_BLOCK;

    for (int i = 0; i < fullBlocks; ++i)
        if (blocks[i] != bs.blocks[i]) return false;

    if (numBits == -1) return true;

    // check the bits that fall out of the checked bytes
    
    // num of bits that have to be individualy checked
    int remBits = numBits % BITS_PER_BLOCK;
    // set pointer to first bit after the checked bytes
    BitPointer bp(BLOCK_SIZE, fullBlocks * BITS_PER_BLOCK);
    for (int i = 0; i < remBits; ++i) {
        bool bit1 = (blocks[bp.blockIndex] >> bp.bitIndex) & ONE;
        bool bit2 = (bs.blocks[bp.blockIndex] >> bp.bitIndex) & ONE;
        if (bit1 != bit2) return false;
    }

    return true;
}

/** Return capacity in chars, should be char == 1 byte. */
//int BitSequence::getCapacity() {
//    return NUM_OF_BLOCKS * BLOCK_SIZE;
//}

/** Return capacity in bits. */
int BitSequence::getNumberOfBits() {
    return NUM_OF_BITS;
}

/** Return the array where bits are stored. */
BitSequence::TBlockType const * BitSequence::getBlocks() const {
    return blocks;
}

string BitSequence::toString() const {
    ostringstream ss;
    for (size_t i = 0; i < getNumberOfBits(); ++i)
        ss << (*this)[i];
    return ss.str();
}
