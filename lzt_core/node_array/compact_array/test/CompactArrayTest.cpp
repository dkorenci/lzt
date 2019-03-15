#include "CompactArrayTest.h"
#include "CompactSymbolTester.h"

void CompactArrayTest::testWithDictionaries() {
    CompactArrayTestTemplate<VectorArray<char, int> > test;
    test.testWithDictionaries();
}

void CompactArrayTest::simpleTests() {
    CompactArrayTestTemplate<VectorArray<char, int> > test;
    test.simpleTests();
}

void CompactArrayTest::testSymbolArrayCreate() {
    //TODO ove konstante za minimume i maksimume bi trebale biti posebno definirane
    {
        CompactSymbolTester<char> tester(-128, 127, 300, false);
        tester.testCreate();
    }
    {
        CompactSymbolTester<char> tester(-128, 127, 200000, false);
        tester.testCreate();
    }
    {
        CompactSymbolTester<short> tester(-20000, -19900, 1100, true);
        tester.testCreate();
    }
    {
        CompactSymbolTester<unsigned int> tester(199999900, 200000000, 2100, true);
        tester.testCreate();
    }
}

void CompactArrayTest::testSymbolArraySerialize() {
    {
        CompactSymbolTester<unsigned int> tester(199999900, 200000000, 2100, true);
        tester.testSerialize();
    }
    {
        CompactSymbolTester<char> tester(-128, 127, 300, false);
        tester.testSerialize();
    }
    {
        CompactSymbolTester<short> tester(-20000, -19900, 1100, true);
        tester.testSerialize();
    }
}