#include "LzTrieIteratorTest.h"

LzTrieIteratorTest::~LzTrieIteratorTest() {
}

void LzTrieIteratorTest::test() {
    LzTrieIterTester<VectorArray<char, int > > tester;
    tester.test();
}