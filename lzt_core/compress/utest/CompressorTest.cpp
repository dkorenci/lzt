#include "CompressorTest.h"

void CompressorTest::testCases() {
    CompressorTester<char, unsigned int> tester;
    tester.testCases();
}

void CompressorTest::testDictionaries(TSA_COMP_ALGO method) {
    CompressorTester<char, unsigned int> tester;
    tester.testDictionaries(method);
}

void CompressorTest::testCompressors() {
    CompressorTester<char, unsigned int> tester;
    tester.testCompressors();
}

void CompressorTest::testUpdateMin() {
    CompressorTester<char, unsigned int> tester;
    tester.testUpdateMin();
}