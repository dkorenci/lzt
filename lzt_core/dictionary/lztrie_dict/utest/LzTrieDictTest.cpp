#include "LzTrieDictTest.h"

LzTrieDictTest::~LzTrieDictTest() {
}

void LzTrieDictTest::testDictionary() {
    LzTrieDictTester<char, unsigned int> tester(false);
    tester.test();
}

void LzTrieDictTest::testSerialization() {
    LzTrieDictTester<char, unsigned int> tester(true);
    tester.test();
}