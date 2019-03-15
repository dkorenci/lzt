#ifndef LZCOMPRESSORTEST_H
#define	LZCOMPRESSORTEST_H

#include <cppunit/extensions/HelperMacros.h>

class LZCompressorTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(LZCompressorTest);

    CPPUNIT_TEST(testLZCompressor);

    CPPUNIT_TEST_SUITE_END();

public:
    LZCompressorTest();
    virtual ~LZCompressorTest();
    void setUp();
    void tearDown();

private:
    void testLZCompressor();
};

#endif	/* LZCOMPRESSORTEST_H */

