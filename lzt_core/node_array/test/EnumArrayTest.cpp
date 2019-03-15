#include "EnumArrayTest.h"
#include "node_array/vector_array/VectorArray.h"

void EnumArrayTest::testCompression() {
    EnumArrayTester<VectorArray<char, unsigned int> > tester;
    tester.testCompression();
}

void EnumArrayTest::testSerialization() {
    EnumArrayTester<VectorArray<char, unsigned int> > tester;
    tester.testSerialization();
}
