#include <sstream>
#include <iostream>

#include "BitSequenceArrayTest.h"
#include "../serialization.h"

void BitSequenceArrayTest::testAccess() {
    // array with one bit seqneces
    readWriteNumbers(NULL, numberOfValues(1), 1);
    // numbers are same size as array's blocks
    readWriteNumbers(NULL, numberOfValues(BITS_PER_CHAR), BITS_PER_CHAR);
    // numbers are have one bit less than array blocks, so they span across blocks.
    readWriteNumbers(NULL, numberOfValues(BITS_PER_CHAR - 1), BITS_PER_CHAR - 1);
    // more span across blocks
    readWriteNumbers(NULL, numberOfValues(5), 5);
    readWriteNumbers(NULL, numberOfValues(13), 13);
    
    int arraySize = 10000;
    // test with bit sequences whose size is multiple of block size
    readWriteNumbers(NULL, arraySize, BITS_PER_CHAR * 2);
    readWriteNumbers(NULL, arraySize, BITS_PER_CHAR * 5);

    arraySize = 20000;
    // some large ugly bit sequences
    readWriteNumbers(NULL, arraySize, 79);
    readWriteNumbers(NULL, arraySize, 113);
}

void BitSequenceArrayTest::testChangeFormat() {
    BitSequenceArray* array = new BitSequenceArray();

    // test array enlargement
    for (int i = 1; i < 15; ++i) {
        array->changeFormat(numberOfValues(i), i);
        readWriteNumbers(array, numberOfValues(i), i);
    }

    // create large array
    array->changeFormat(numberOfValues(20), 20);
    // test array when size is falling
    for (int i = 15; i > 0; --i) {
        array->changeFormat(numberOfValues(i), i);
        readWriteNumbers(array, numberOfValues(i), i);
    }

    delete array;
 }

void BitSequenceArrayTest::testResize() {
    readWriteNumberResize(113, 3, 15, 10);
    readWriteNumberResize(129, 5, -11, 10);
    readWriteNumberResize(1000, 8, 100, 6);
    readWriteNumberResize(5000, 13, 1000, 3);
    readWriteNumberResize(3000, 11, -1000, 2);
    readWriteNumberResize(70417, 21, 40000, 1);
    readWriteNumberResize(60217, 23, -10000, 1);
}

/** Fill an array, cyclically, with binary representations of all representable
 * by B bits. Resize the array filling only newly allocated positions, if any,
 * and assert the array holds the right numbers. */
void BitSequenceArrayTest::
readWriteNumberResize(long initSize, int B, long resizeStep, int numSteps) {
    BitSequenceArray array(initSize, B);
    // number of integers representable by B bits
    long N = 1;
    for (int i = 0; i < B; ++i) N *= 2;
    // fill array
    for (long i = 0; i < initSize; ++i) {
        long num = i % N;
        BitSequence bits = numberToBits(num);
        array.setSequence(i, bits);
    }

    ostringstream m;
    m << "initSize: " << initSize << " B: " << B << " N: " << N
            << " step: " << resizeStep << " numSteps: " << numSteps << endl;
    // test resizing
    long size = initSize;
    for (int i = 0; i < numSteps; ++i) {
        long oldSize = size;
        size += resizeStep;
        if (size <= 0) break;
        array.resize(size);
        // fill newly created space with bits
        if (size >= oldSize) {
        for (long j = oldSize; j < size; ++j) {
            long num = j % N;
            BitSequence bits = numberToBits(num);
            array.setSequence(j, bits);
        }
        }
        // check that correct number are in the array
        for (long j = 0; j < size; ++j) {
            BitSequence bits = array[j];
            long num = numberFromBits<long>(bits, B);

            ostringstream m2;
            m2 << "size: " << size << " j: " << j
                    << " expected: " << j % N << " num: " << num <<  endl;

            TEST_ASSERT_MESSAGE(num == j % N, m.str() + m2.str());
        }
    }
}

/** Write and read BitSequences of size numberSize interpreting them as
 * binary form of natural numbers in range [0, arraySize> .
 * Largest number representable by numberSize bits must not be less than arraySize */
void BitSequenceArrayTest::readWriteNumbers(BitSequenceArray* array, size_t arraySize, int numberSize) {
    bool created = false;

    if (array == NULL) {
        array = new BitSequenceArray(arraySize, numberSize);
        created = true;
    }    
    
    // create debug message header
    ostringstream ss; 
    ss << "readWriteNumbers(), arraySize: " << arraySize << " numberSize: " << numberSize << endl;
    string header = ss.str();

    // write numbers
    for (size_t i = 0; i < arraySize; ++i) {
        size_t number = i;
        BitSequence bits;
        // write number to bits
        bits.setFalse();
        for (int bi = 0; bi < numberSize; ++bi) {
            bool bit = (bool) (number & (ONE << bi));
            bits.setBit(bi, bit);
        }

        array->setSequence(i, bits);
    }

    // read numbers
    for (size_t i = 0; i < arraySize; ++i) {
        BitSequence bits = (*array)[i];
        // read number from bits
        size_t number = 0, shift = 1;
        for (int bi = 0; bi < numberSize; ++bi) {
            size_t bit = bits[bi] ? 1 : 0;
            if (bit) number += shift * bit;
            
            shift *= 2;
        }

        ostringstream ss; ss << "i: " << i << ", number: " << number;
        TEST_ASSERT_MESSAGE(number == i, header + ss.str());
        
    }

    if (created) delete array;
}

