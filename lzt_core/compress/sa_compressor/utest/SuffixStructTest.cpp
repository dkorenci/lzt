#include "SuffixStructTest.hpp"

#include "node_array/vector_array/VectorArray.h"

void SuffixStructTest::testArrayCreation() {
    SuffixStructTester<VectorArray<unsigned char, int> > tester;
    tester.testArrayCreation(true);
    tester.testArrayCreation(false);
}
