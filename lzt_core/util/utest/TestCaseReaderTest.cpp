#include "TestCaseReaderTest.h"

template <>
char* getWord<char>(fstream& stream) {
    string s;
    stream >> s;
    return wordClone(s.c_str());
}

void TestCaseReaderTest::test() {
    TestCaseReaderTester<char> tester;
    tester.runTests();
}
