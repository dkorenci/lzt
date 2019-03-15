#ifndef NODEARRAYTEST_CPPU_H
#define	NODEARRAYTEST_CPPU_H

#include <cppunit/extensions/HelperMacros.h>
#include "node_array/test/NodeArrayTest.h"
#include "node_array/test/EnumArrayTest.h"

class NodeArrayTest_cppu : public CPPUNIT_NS::TestFixture, 
        public NodeArrayTest, public EnumArrayTest  {
public:

    CPPUNIT_TEST_SUITE(NodeArrayTest_cppu);

    CPPUNIT_TEST(testStringConversions);
    CPPUNIT_TEST(EnumArrayTest::testCompression);
    CPPUNIT_TEST(EnumArrayTest::testSerialization);

    CPPUNIT_TEST_SUITE_END();

public:
    
    NodeArrayTest_cppu() {};
    NodeArrayTest_cppu(const NodeArrayTest_cppu& orig) {};
    virtual ~NodeArrayTest_cppu() {};

private:

};

#endif	/* NODEARRAYTEST_CPPU_H */

