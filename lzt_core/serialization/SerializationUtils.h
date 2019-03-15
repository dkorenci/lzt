#ifndef SERIALIZATIONUTILS_H
#define	SERIALIZATIONUTILS_H

#include "serialization.h"
#include "BitSequence.h"
#include "BitSequenceArray.h"
#include <cassert>
#include <cstddef>
#include <iostream>

/** Struct with an array that can hold all the block of the BitSequence,
 * and with size of that array. */
struct BitBlocksArray {
    BitSequence::TBlockType blocks[BitSequence::NUM_OF_BLOCKS];
    size_t size;
};

class SerializationUtils {
public:
    SerializationUtils();
    SerializationUtils(const SerializationUtils& orig);
    virtual ~SerializationUtils();
    
    static BitBlocksArray getNonzeroBlocks(BitSequence bits);
    static BitSequence fromNonzeroBlocks(BitBlocksArray blocks);
    
    template <typename T>
    static void positiveIntegersToBits(const T* ints, size_t num, BitSequenceArray& bitArray);

    template <typename T>
    static void integerToStream(T integer, ostream& stream);

    template <typename T>
    static T integerFromStream(istream& stream);

private:

    static const int BITS_PER_BLOCK = BitSequence::BLOCK_SIZE * BITS_PER_CHAR;

    static bool isBlockZero(BitSequence::TBlockType block);

};

//TODO write generic serialization of integers
/** Serialize integral data type to output stream. */
template <typename T>
void SerializationUtils::integerToStream(T integer, ostream& stream) {
    // serialize integer
    BitBlocksArray blocks =
            SerializationUtils::getNonzeroBlocks(toBitSequence<T>(integer));
    // serialize the number of integer blocks
    BitBlocksArray numOfBlocksSer =
            SerializationUtils::getNonzeroBlocks( toBitSequence<size_t>(blocks.size) );

//    cout<<(int)integer<<" "<<blocks.size<<" "<<numOfBlocksSer.size<<endl
//        <<toBitSequence<size_t>(blocks.size).toString()<<endl;
    assert(numOfBlocksSer.size == 1);

    // write serialized data to stream
    stream.write(numOfBlocksSer.blocks, 1);
    stream.write(blocks.blocks, blocks.size);
}

/** Deserialize integral data type from output stream. */
template <typename T>
T SerializationUtils::integerFromStream(istream& stream) {
    // deserialize the size of the serialized integer
    BitBlocksArray numOfBlocksSer;
    numOfBlocksSer.size = 1;
    stream.read(numOfBlocksSer.blocks, numOfBlocksSer.size);
    size_t numOfBlocks =
            fromBitSequence<size_t>(SerializationUtils::fromNonzeroBlocks( numOfBlocksSer ));

    BitBlocksArray blocks;
    blocks.size = numOfBlocks;
    stream.read(blocks.blocks, blocks.size);

    return fromBitSequence<T>( SerializationUtils::fromNonzeroBlocks( blocks ) );
}

/** Copy an array of positive numbers to BitSequenceArray. */
template <typename T> void SerializationUtils::
positiveIntegersToBits(const T* ints, size_t num, BitSequenceArray& bitArray) {
    // Calculate number of bits needed to store largest integer.
    int maxBits = 1;
    for (size_t i = 0; i < num; ++i) {
        BitSequence bits = numberToBits( ints[i] );
        int numOfBits = bits.maxNonzeroBit() + 1;
        if (numOfBits > maxBits) maxBits = numOfBits;
    }
    // Copy bit representations of numbers to BitSequenceArray.
    bitArray.changeFormat(num, maxBits);
    for (size_t i = 0; i < num; ++i) {
        BitSequence bits = numberToBits( ints[i] );
        bitArray.setSequence(i, bits);
    }
}

#endif	/* SERIALIZATIONUTILS_H */

