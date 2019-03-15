#ifndef COMPACTARRAYSER_CPPU_H
#define	COMPACTARRAYSER_CPPU_H

#include <cppunit/extensions/HelperMacros.h>
#include "serialization/array/test/CompactArraySerTest.h"

class CompactArraySer_cppu : public CPPUNIT_NS::TestFixture, public CompactArraySerTest {

    CPPUNIT_TEST_SUITE(CompactArraySer_cppu);

    CPPUNIT_TEST(CompactArraySerTest::testWithDictionaries);

    CPPUNIT_TEST_SUITE_END();

public:
    virtual ~CompactArraySer_cppu() {}

private:

};

#endif	/* COMPACTARRAYSER_CPPU_H */

