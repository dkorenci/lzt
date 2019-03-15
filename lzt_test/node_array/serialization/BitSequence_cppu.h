#ifndef BITSEQUENCE_CPPU_H
#define	BITSEQUENCE_CPPU_H

#include <cppunit/extensions/HelperMacros.h>
#include "serialization/test/BitSequenceTest.h"
#include "serialization/test/BitSequenceArrayTest.h"
#include "serialization/test/IntBitArrayTest.h"
#include "serialization/test/BitVectorTest.h"

class BitSequence_cppu : public CPPUNIT_NS::TestFixture,
public BitSequenceTest, public BitSequenceArrayTest, 
        public IntBitArrayTest, public BitVectorTest {

    CPPUNIT_TEST_SUITE(BitSequence_cppu);

    CPPUNIT_TEST(BitSequenceTest::testAccess);
    CPPUNIT_TEST(BitSequenceTest::testMaxNonzero);
    CPPUNIT_TEST(BitSequenceTest::testTransform);
    CPPUNIT_TEST(BitSequenceTest::testShift);
    CPPUNIT_TEST(BitSequenceTest::testPositiveNums);
    CPPUNIT_TEST(BitSequenceTest::testEquals);

    CPPUNIT_TEST(BitSequenceArrayTest::testChangeFormat);
    CPPUNIT_TEST(BitSequenceArrayTest::testAccess);
    CPPUNIT_TEST(BitSequenceArrayTest::testResize);

    CPPUNIT_TEST(IntBitArrayTest::testExtremes);
    CPPUNIT_TEST(IntBitArrayTest::testRandom);
    CPPUNIT_TEST(IntBitArrayTest::testSerialization);

    CPPUNIT_TEST(BitVectorTest::testAccess);
    CPPUNIT_TEST(BitVectorTest::testSequenceIO);
    CPPUNIT_TEST(BitVectorTest::testSerialize);
    
    CPPUNIT_TEST_SUITE_END();

public:
    BitSequence_cppu() {};
    BitSequence_cppu(const BitSequence_cppu& orig) {};
    virtual ~BitSequence_cppu() {};
private:

};

#endif	/* BITSEQUENCE_CPPU_H */

