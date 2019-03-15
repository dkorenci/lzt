#ifndef NODEARRAYTEST_H
#define	NODEARRAYTEST_H

#include <cppunit/extensions/HelperMacros.h>

class NodeArrayTest : public CPPUNIT_NS::TestFixture {
    
    CPPUNIT_TEST_SUITE(NodeArrayTest);

    CPPUNIT_TEST(testVectorNodeArray);

    CPPUNIT_TEST_SUITE_END();

public:

    NodeArrayTest();
    virtual ~NodeArrayTest();
    void setUp();
    void tearDown();

private:
    void testVectorNodeArray();
    
};

#endif	/* NODEARRAYTEST_H */

