#ifndef BITPOINTER_H
#define	BITPOINTER_H

#include <cstddef>

#include "util/constants.h"

/** Describes a location of a bit in an array of bit blocks,
 * indexes are zero based. */
class BitPointer {
public:

    size_t blockIndex;
    // position of a bit inside the block
    int bitIndex;

    /** Create a pointer for an array of bit blocks, each blockSize chars wide,
     * pointing to a bit at bitPosition. */
    BitPointer(int blockSize, size_t bitPosition) {
        bitsPerBlock = blockSize * BITS_PER_CHAR;
        blockIndex = bitPosition / bitsPerBlock;
        bitIndex = bitPosition % bitsPerBlock;
    }

    /** Increment bit pointer by 1. */
    inline void increment() {
        bitIndex++;
        if (bitIndex == bitsPerBlock) {
            bitIndex = 0;
            blockIndex++;
        }
    }

    /** Decrement bit pointer by 1. Decremented zero position remains zero. */
    inline void decrement() {
        if (bitIndex != 0) {
            bitIndex--;
        }
        else {
            if (blockIndex == 0) return;
            else {
                blockIndex--;
                bitIndex = bitsPerBlock-1;
            }
        }

    }

    inline bool operator<(BitPointer bp) {
        if (blockIndex < bp.blockIndex) return true;
        if (blockIndex > bp.blockIndex) return false;
        if (bitIndex < bp.bitIndex) return true;
        return false;
    }

private:
    int bitsPerBlock;
};

#endif	/* BITPOINTER_H */

