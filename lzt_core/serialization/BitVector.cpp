#include "BitVector.h"

#include <cstdlib>
#include <sstream>

#include "BitPointer.h"
#include "util/constants.h"
#include "SerializationUtils.h"
#include "test/SerializationTest.h"

BitVector::BitVector(size_t sz): blocks(NULL), numBlocks(0) {
    resize(sz);
}

BitVector::~BitVector() { freeBlocks(); }

/** (Re)allocate memory for numOfBlocks blocks. */
void BitVector::reallocateBlocks() {
    blocks = (char *)realloc(blocks, numBlocks);    
}

/** Free blocks memory. */
void BitVector::freeBlocks() {
 if (blocks != NULL) {
     free(blocks);
     blocks = NULL;
     numBlocks = 0;
 }
}

/* Change size to enable vector to hold sz bits. */
void BitVector::resize(size_t sz) {
    //TODO write tests for resize
    size_t newNumBlocks;
    size_t bitsPerBlock = (BLOCK_SIZE * BITS_PER_CHAR);
    newNumBlocks = sz / bitsPerBlock;
    if (sz % bitsPerBlock > 0) newNumBlocks++;

    if (newNumBlocks != numBlocks) {
        numBlocks = newNumBlocks;
        reallocateBlocks();        
    }

    size = sz;
}

size_t BitVector::getSize() const { return size; }

string BitVector::toString() {
    ostringstream ss;
    ss << "size: " << getSize() << endl;
    for (size_t i = 0; i < getSize(); ++i) ss << (*this)[i] ? '1' : '0';
    
    return ss.str();
}

/** Serialize this object to output stream. */
void BitVector::writeToStream(ostream& stream) const {
    SerializationUtils::integerToStream(size, stream);
    //SerializationUtils::integerToStream(numBlocks, stream);
    stream.write(blocks, numBlocks);
}

/** Deserialize vector from stream to this object.  */
void BitVector::readFromStream(istream& stream) {
    freeBlocks();
    size_t sz = SerializationUtils::integerFromStream<size_t>(stream);
    resize(sz);
    stream.read(blocks, numBlocks);
}