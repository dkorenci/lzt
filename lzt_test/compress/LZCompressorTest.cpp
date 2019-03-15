#include "LZCompressorTest.h"
#include "LZCompressorTester.h"

CPPUNIT_TEST_SUITE_REGISTRATION(LZCompressorTest);

LZCompressorTest::LZCompressorTest() {
}

LZCompressorTest::~LZCompressorTest() {
}

void LZCompressorTest::setUp() {
}

void LZCompressorTest::tearDown() {
}

void LZCompressorTest::testLZCompressor() {
    LZCompressorTester<char, int> tester;
    tester.runTests();
}

