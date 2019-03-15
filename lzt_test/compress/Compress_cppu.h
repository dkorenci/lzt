#ifndef COMPRESS_CPPU_H
#define	COMPRESS_CPPU_H

#include <cppunit/extensions/HelperMacros.h>

#include "compress/utest/CompressorTest.h"
#include "compress/sa_compressor/utest/SuffixStructTest.hpp"

class Compress_cppu : public CPPUNIT_NS::TestFixture, 
        public CompressorTest, public SuffixStructTest {
public:

    CPPUNIT_TEST_SUITE(Compress_cppu);

    CPPUNIT_TEST(testCompressors);
    CPPUNIT_TEST(testArrayCreation);

    CPPUNIT_TEST_SUITE_END();

private:

};

#endif	/* COMPRESS_CPPU_H */

