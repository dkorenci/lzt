#ifndef SERIALIZATION_H
#define	SERIALIZATION_H

#include <cstddef>

#include "util/constants.h"

size_t numberOfBlocks(size_t numberOfBits, size_t fieldSize);
size_t numberOfValues(int numberOfBits);
int numberOfBits(size_t numberOfValues);

/** Return a value of type T with all bits set to zero. */
template <typename T>
inline T zeroBits() {
    long unsigned int const ONE = 1;
    int numOfBits = sizeof(T) * BITS_PER_CHAR;

    T z;
    for (int i = 0; i < numOfBits; ++i)
        z &= ~(ONE << i);

    return z;
}

/** Get number of blocks, each blockSize chars large, required to
 * hold numberOfBits. */
inline size_t numberOfBlocks(size_t numberOfBits, size_t blockSize) {
    //TODO napisati efikasnije
    size_t nob = numberOfBits / (blockSize * BITS_PER_CHAR);
    if (numberOfBits % (blockSize * BITS_PER_CHAR) != 0) nob++;
    return nob;
}

/** Get number of different values representable by numberOfBits. */
inline size_t numberOfValues(int numberOfBits) {
    if (numberOfBits <= 0) return 0;
    size_t num = 1;
    return num << numberOfBits;
}

/** Calculate number of bits necessary to store numberOfValues different values. */
inline int numberOfBits(size_t numberOfValues) {
    int bits = 1; size_t vals = 2;
    while (vals < numberOfValues) {
        bits++;
        vals *= 2;
    }
    return bits;
}

/** Calculate min. number of bits that can store numbers [0,..,n].  */
inline int minBitsForNumber(size_t n) {
    return numberOfBits(n+1);
}

#endif	/* UTILS_H */

