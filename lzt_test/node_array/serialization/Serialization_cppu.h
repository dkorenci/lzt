#ifndef SERIALIZATION_CPPU_H
#define	SERIALIZATION_CPPU_H

#include <cppunit/extensions/HelperMacros.h>
#include "serialization/test/SerializationTest.h"

class Serialization_cppu : public CPPUNIT_NS::TestFixture, public SerializationTest {

    CPPUNIT_TEST_SUITE(Serialization_cppu);

    CPPUNIT_TEST(SerializationTest::testBitBlocksArray);
    CPPUNIT_TEST(SerializationTest::testIntegerSerialization);
    CPPUNIT_TEST(SerializationTest::testBitSequenceArray);

    CPPUNIT_TEST_SUITE_END();

public:
    Serialization_cppu() {};
    Serialization_cppu(const Serialization_cppu& orig) {};
    virtual ~Serialization_cppu() {};
    
private:

};

#endif	/* SERIALIZATION_CPPU_H */

