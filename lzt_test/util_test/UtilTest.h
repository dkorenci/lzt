#ifndef UTILTEST_H
#define	UTILTEST_H

#include <cppunit/extensions/HelperMacros.h>

#include "util/utest/FileReaderTest.h"
#include "util/utest/TestCaseReaderTest.h"

class UtilTest : public CPPUNIT_NS::TestFixture, public FileReaderTest,
        public TestCaseReaderTest {

    CPPUNIT_TEST_SUITE(UtilTest);

    CPPUNIT_TEST(TestCaseReaderTest::test);

    CPPUNIT_TEST(FileReaderTest::testWordFileReader);
    CPPUNIT_TEST(FileReaderTest::testDictFileReader);

    CPPUNIT_TEST_SUITE_END();

};

#endif	/* UTILTEST_H */

