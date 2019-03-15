#include "BitSequenceTest.h"

#include <iostream>
#include <sstream>

#include "util/utils.h"

#include "../BitSequence.h"
#include "../serialization.h"


void BitSequenceTest::testAccess() {
    
    BitSequence bs;
    int const size = BitSequence::getNumberOfBits();
   
    for (int i = 0; i < size; ++i)
        bs.setBit(i, (bool)((i % 7) % 2 == 0) ) ;

    for (int i = 0; i < size; ++i)
        TEST_ASSERT_MESSAGE( bs[i] == (bool)((i % 7) % 2 == 0 ), "" ) ;

    bs.setFalse();
    for (int i = 0; i < size; ++i)
        TEST_ASSERT( bs[i] == false);

}

void BitSequenceTest::testTransform() {
    size_t const numTests = 10000;
    long min = -1000000000, max = 1000000000;
    randomSeed();
    for (size_t i = 0; i < numTests; ++i) {
        long r = getRandomNumber(min, max);

        BitSequence bits = toBitSequence(r);
        long r2 = fromBitSequence<long>(bits);

        ostringstream m;
        m << "r: " << r << bits.toString() << endl;
        TEST_ASSERT_MESSAGE(r == r2, m.str());
    }

}

/** Write random positive longs to BitSequence,
 * transform back and assert equality. */
//TODO add negative numbers test and test extremes
void BitSequenceTest::testPositiveNums() {
    size_t const numTests = 10000;
    long min = 0, max = 1000000000;
    randomSeed();
    // test when numbers are written to and read from the entire bit sequence
    for (size_t i = 0; i < numTests; ++i) {
        long r = getRandomNumber(min, max);

        BitSequence bits = numberToBits(r);
        long r2 = numberFromBits<long>(bits);

        ostringstream m; 
        m << "r: " << r << " r2: " << r2 << " maxnz: " << bits.maxNonzeroBit()
          << endl << bits.toString()<<endl;
        TEST_ASSERT_MESSAGE(r == r2, m.str());
    }
    // test when numbers are written to and read from only first B bits
    int B = 23; size_t numOfVals = numberOfValues(B);
    for (size_t i = 0; i < numTests; ++i) {
        size_t r = getRandomNumber<size_t>(0, numOfVals-1);

        BitSequence bits = numberToBits(r);
        size_t r2 = numberFromBits<long>(bits, B);

        ostringstream m;
        m << "B: " << B << " r: " << r << " r2: " << r2
          << " maxnz: " << bits.maxNonzeroBit() << endl << bits.toString()<<endl;
        TEST_ASSERT_MESSAGE(r == r2, m.str());
    }
}

/** Shift all possible combination of numOfBits bits from the beginning of
 * BitSequence to last possible place and assert the shift is correct.  */
void BitSequenceTest::testShift() {
    int numOfBits = 7, N = 1;
    for (int i = 0; i < numOfBits; ++i) N *= 2;

    for (size_t i = 1; i < N; ++i) {
        BitSequence base = toBitSequence(i);
        // we do all shifts of bits at positions [0, numOfBits>
        for (int j = 0; j < BitSequence::getNumberOfBits() - numOfBits; ++j) {
            BitSequence bs = toBitSequence(i);
            bs <<= j;

            // check that first numOfBits bits are shifted correctly for j placed
            for (int k = 0; k < numOfBits; ++k) {
                if (base[k] != bs[j + k]) {
                    ostringstream m; m << "i: " << i << " j: " << j << " k: " << k << endl;
                    TEST_ASSERT_MESSAGE(base[k] == bs[j + k], m.str());
                }
            }

            // check that first j bits are 0
            for (int k = 0; k < j; ++k) {
                if (bs[k] == true) {
                    ostringstream m; m << "i: " << i << " j: " << j << " k: " << k << endl;
                    TEST_ASSERT_MESSAGE(bs[k] == false, m.str());
                }
            }
        }

    }
}

/** Set all possible positions to 1 while others are 0 and assert this
 * is maxNonzero bit. */
void BitSequenceTest::testMaxNonzero() {
    for (int i = -1; i < BitSequence::getNumberOfBits(); ++i) {
        BitSequence bits; bits.setFalse();
        if (i != -1) bits.setBit(i, true);
        
        ostringstream m; m << "i: " << i << endl;
        TEST_ASSERT_MESSAGE(bits.maxNonzeroBit() == i, m.str());
    }
}

/** Test equals of first b bits, b from 0..MAX_BIT by setting
 * b bits of two sequences to equal values and other bits to different values
 * and asserting bits1.equals(bits2, k).  */
void BitSequenceTest::testEquals() {
    BitSequence bits1, bits2;
    randomSeed();
    for (int b = 0; b <= BitSequence::getNumberOfBits(); ++b) {
        
        // set first b bits to equal
        for (int i = 0; i < b; ++i) {
            int r = getRandomNumber<int>(0, 100);
            bool bit = r < 50 ? false : true;
            bits1.setBit(i, bit);
            bits2.setBit(i, bit);
        }
        // set other bits to different values
        for (int i = b; i < BitSequence::getNumberOfBits(); ++i) {
            bits1.setBit(i, false);
            bits2.setBit(i, true);
        }

        ostringstream m; m << bits1.toString() << endl << bits2.toString() << endl;
        TEST_ASSERT_MESSAGE(bits1.equals(bits2, b), m.str());
    }
}