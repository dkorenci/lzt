#include "HuffmanIndexMapTest.h"

HuffmanIndexMapTest::~HuffmanIndexMapTest() {
}

void HuffmanIndexMapTest::testMap() {
    HuffIndexMapTester<int> tester;
    tester.testMap("small_special_cases.txt", false);
    tester.testMap("big_rand_map.txt", false);
}

void HuffmanIndexMapTest::testSerialization() {
    HuffIndexMapTester<int> tester;
    tester.testMap("small_special_cases.txt", true);
    tester.testMap("big_rand_map.txt", true);
}