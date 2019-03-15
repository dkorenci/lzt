#include "SerializationUtils.h"

SerializationUtils::SerializationUtils() {
}

SerializationUtils::SerializationUtils(const SerializationUtils& orig) {
}

SerializationUtils::~SerializationUtils() {
}

/** Copy blocks of the bit sequence but exclude blocks at the end
 * with zero bits. This way min. number of blocks needed to hold the
 * bits is extracted.  */
BitBlocksArray SerializationUtils::getNonzeroBlocks(BitSequence bits) {
    BitBlocksArray array;
    int nz;
    for (nz = bits.NUM_OF_BLOCKS - 1; nz >= 0; nz--)
        if (isBlockZero(bits.blocks[nz]) == false) break;

    if (nz == -1) nz = 0;

    array.size = nz + 1;
    for (int i = 0; i < array.size; ++i)
        array.blocks[i] = bits.blocks[i];

    return array;
}

BitSequence SerializationUtils::fromNonzeroBlocks(BitBlocksArray array) {
    BitSequence bits; //bits.setFalse();
    for (int i = 0; i < array.size; ++i)
        bits.blocks[i] = array.blocks[i];

    // set remaining blocks to zero
    for (int i = array.size; i < BitSequence::NUM_OF_BLOCKS; ++i)
        bits.blocks[i] <<= BITS_PER_BLOCK;

    return bits;
}

bool SerializationUtils::isBlockZero(BitSequence::TBlockType block) {
    // set all the bits to 0 by shifting them out and compare with the block
    return ((block << BITS_PER_BLOCK) == block);
}

/** Boolean serialization. */
// TODO write without using integer serialization to save a byte
template <>
void SerializationUtils::integerToStream<bool>(bool i, ostream& stream) {
    unsigned char ch = i ? 1 : 0;
    integerToStream<unsigned char>(ch, stream);
}

/** Boolean deserialization. */
template <>
bool SerializationUtils::integerFromStream<bool>(istream& stream) {
    unsigned char ch = integerFromStream<unsigned char>(stream);
    bool b = (ch == 0) ? false : true;
}