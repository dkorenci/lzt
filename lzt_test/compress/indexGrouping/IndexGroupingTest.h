/*
 * File:   IndexGroupingTest.h
 * Author: dam1root
 *
 * Created on Feb 22, 2011, 11:29:28 AM
 */

#ifndef INDEXGROUPINGTEST_H
#define	INDEXGROUPINGTEST_H

#include <cppunit/extensions/HelperMacros.h>

class IndexGroupingTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(IndexGroupingTest);

    CPPUNIT_TEST(testGrouping);

    CPPUNIT_TEST_SUITE_END();

public:
    IndexGroupingTest();
    virtual ~IndexGroupingTest();
    void setUp();
    void tearDown();

private:
    void testGrouping();

};

#endif	/* INDEXGROUPINGTEST_H */

