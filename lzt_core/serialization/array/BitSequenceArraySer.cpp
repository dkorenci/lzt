#include "BitSequenceArraySer.h"
#include "serialization/SerializationUtils.h"
#include <cassert>

/** Serializes BitSequenceArray to output stream. */
void BitSequenceArraySer::arrayToStream(BitSequenceArray const & array, ostream& stream) {
    // write integer members
    SerializationUtils::integerToStream(array.numOfBlocks, stream);
    SerializationUtils::integerToStream(array.numOfSequences, stream);
    SerializationUtils::integerToStream(array.bitsPerSequence, stream);
    
    // write the array
    stream.write(array.getBlocks(), array.getNumOfBlocks());
}

//TODO make the method write directly to an array
/** Deserializes BitSequenceArray from input stream. */
void BitSequenceArraySer::arrayFromStream(BitSequenceArray &array, istream& stream) {
    array.freeBlocks();

    // read integer members
    array.numOfBlocks = SerializationUtils::integerFromStream<size_t>(stream);
    array.numOfSequences = SerializationUtils::integerFromStream<size_t>(stream);
    array.bitsPerSequence = SerializationUtils::integerFromStream<size_t>(stream);

    // read the array
    array.allocateBlocks();
    stream.read(array.blocks, array.numOfBlocks);    
}
