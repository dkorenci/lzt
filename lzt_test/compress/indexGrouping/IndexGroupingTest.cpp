/*
 * File:   IndexGroupingTest.cpp
 * Author: dam1root
 *
 * Created on Feb 22, 2011, 11:29:28 AM
 */

#include "IndexGroupingTest.h"
#include "IndexGroupingTester.h"
#include "node_array/vector_array/VectorArray.h"

CPPUNIT_TEST_SUITE_REGISTRATION(IndexGroupingTest);

IndexGroupingTest::IndexGroupingTest() {
}

IndexGroupingTest::~IndexGroupingTest() {
}

void IndexGroupingTest::setUp() {
}

void IndexGroupingTest::tearDown() {
}

void IndexGroupingTest::testGrouping() {
    char a[] = {'a','b','c','d'};
    IndexGroupingTester<VectorArray<char, int> > igt(a, 4);
    igt.runTests();
}

