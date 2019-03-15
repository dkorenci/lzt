#include "BitVectorTest.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include "debug/lzt_debug.h"
#include "serialization/serialization.h"
#include "serialization/BitSequence.h"
#include "util/TempFile.h"

BitVectorTest::~BitVectorTest() {
}


void BitVectorTest::testAccess() {
    testSameBit(1, false);
    testSameBit(1, true);
    testSameBit(9, false);
    testSameBit(9, true);
    testSameBit(1913, false);
    testSameBit(1913, true);

    testReadWrite(100, 7, 4);
    testReadWrite(1113, 23, 5);
    testReadWrite(2041, 31, 27);
    testReadWrite(129143, 29, 14);
}

void BitVectorTest::testSequenceIO() {
    testWithBinNumbers(2);
    testWithBinNumbers(16);
    testWithBinNumbers(1024);
    testWithBinNumbers(32768);
}

void BitVectorTest::testResize() {
    
}

void BitVectorTest::testSerialize() {
    testSerWithBinNumber(2047);
    testSerWithBinNumber(32767);
}

/** Test by filling BitVector of given size with bit and
 * asserting that. */
void BitVectorTest::testSameBit(size_t size, bool bit) {
    BitVector bvector(size);

    for (size_t i = 0; i < size; ++i) bvector.setBit(i, bit);
    for (size_t i = 0; i < size; ++i) {
        ostringstream m;
        m << "i: " << i << " bit: " << bit << endl;
        TEST_ASSERT_MESSAGE(bvector[i] == bit, m.str());
    }
}

/** Test that vector is correctly filled with bits, v[i] being set
 * ti true if (i % mod) is less than limit, false othervise */
void BitVectorTest::testReadWrite(size_t size, int mod, int limit) {
    BitVector bvector(size);

    for (size_t i = 0; i < size; ++i) {
        bool bit = (i % mod) < limit;
        bvector.setBit(i, bit);
    }

    for (size_t i = 0; i < size; ++i) {
        bool bit = (i % mod) < limit;

        ostringstream m;
        m << "i: " << i << " mod: " << mod << " limit: "
                << limit << " bit: " << bit << endl;
        
        TEST_ASSERT_MESSAGE(bvector[i] == bit, m.str());
    }
}

/** Fill vector with binary representations of numbers [0,..,maxNumber> using
 * minimum number of bits necessary to represent all numbers.
 * Return that number of bits. */
int BitVectorTest::fillVectorWithNumbers(BitVector* bvector, size_t maxNumber) {
    const int numBits = numberOfBits(maxNumber);
    bvector->resize(numBits * maxNumber);    

    for (size_t i = 0; i < maxNumber; ++i) {
        BitSequence bits = numberToBits(i);
        bvector->setSequence(i*numBits, bits, numBits);
    }

    return numBits;
}

/** Test BitSequence I/O with binary 
 * representations of numbers from [0..maxNumber>. */
void BitVectorTest::testWithBinNumbers(size_t maxNumber) {    
    BitVector bvector;
    const int numBits = fillVectorWithNumbers(&bvector, maxNumber);

    for (size_t i = 0; i < maxNumber; ++i) {
        ostringstream m;
        m << "maxNumber: " << maxNumber 
          << " numBits: " << numBits << " i: " << i;

        BitSequence bits = bvector.getSequence(i*numBits, numBits);

        size_t num = numberFromBits<size_t>(bits);
        m << " num: " << num;

        TEST_ASSERT_MESSAGE(num == i, m.str());
    }
}

/** Fill vector with binary representation of numbers from [0..maxNumber>,
 * write to file, read and assert equality. */
void BitVectorTest::testSerWithBinNumber(size_t maxNumber) {
    BitVector bvector;
    const int numBits = fillVectorWithNumbers(&bvector, maxNumber);
    TempFile file;
    // serialize
    fstream fstr(file.getName());
    bvector.writeToStream(fstr);
    fstr.close();
    // deserialize
    BitVector bvectorDeser;
    fstr.open(file.getName());
    bvectorDeser.readFromStream(fstr);
    fstr.close();

    ostringstream vectors;
    vectors << bvector.toString() << endl << bvectorDeser.toString() << endl;

    TEST_ASSERT_MESSAGE(bvector.getSize() == bvectorDeser.getSize(), vectors.str());

    for (size_t i = 0; i < bvector.getSize(); ++i) {
        ostringstream m;
        m << "mistmatch index: " << i << endl;
        TEST_ASSERT_MESSAGE(bvector[i] == bvectorDeser[i], vectors.str() + m.str());
    }

}
