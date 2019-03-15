#ifndef COMPACTARRAYTEST_CPPU_H
#define	COMPACTARRAYTEST_CPPU_H

#include <cppunit/extensions/HelperMacros.h>
#include "node_array/compact_array/test/CompactArrayTest.h"

class CompactArrayTest_cppu : public CPPUNIT_NS::TestFixture, public CompactArrayTest {
public:

    CPPUNIT_TEST_SUITE(CompactArrayTest_cppu);

    CPPUNIT_TEST(testSymbolArrayCreate);
    CPPUNIT_TEST(testSymbolArraySerialize);
    CPPUNIT_TEST(simpleTests);
    //TODO make test with a smaller dictionary
    CPPUNIT_TEST(testWithDictionaries);

    CPPUNIT_TEST_SUITE_END();

public:
    
    CompactArrayTest_cppu() {};
    CompactArrayTest_cppu(const CompactArrayTest_cppu& orig) {};
    virtual ~CompactArrayTest_cppu() {};

private:

};

#endif	/* COMPACTARRAYTEST_CPPU_H */

