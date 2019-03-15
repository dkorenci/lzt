#include "NodeArrayTest.h"
#include "NodeArrayTester.h"
#include "node_array/vector_array/VectorArray.h"

CPPUNIT_TEST_SUITE_REGISTRATION(NodeArrayTest);

NodeArrayTest::NodeArrayTest() {
}

NodeArrayTest::~NodeArrayTest() {
}

void NodeArrayTest::setUp() {
}

void NodeArrayTest::tearDown() {
}

void NodeArrayTest::testVectorNodeArray() {
    NodeArrayTester<char, int, VectorArray<char, int> > nat;
    nat.runTests();
}
