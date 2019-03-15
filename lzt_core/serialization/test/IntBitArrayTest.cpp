#include "IntBitArrayTest.h"

#include <climits>

void IntBitArrayTest::testRandom() {
    int len = 200;

    testWithRandomInts<char>(len, CHAR_MIN, CHAR_MAX);
    testWithRandomInts<unsigned char>(len, 0, UCHAR_MAX);

    len = 5000;

    testWithRandomInts<short>(len, SHRT_MIN, SHRT_MAX);
    testWithRandomInts<unsigned short>(len, 0, USHRT_MAX);

    testWithRandomInts<int>(len, INT_MIN, INT_MAX);
    testWithRandomInts<unsigned int>(len, 0, UINT_MAX);
    
    testWithRandomInts<long>(len, LONG_MIN, LONG_MAX);
    testWithRandomInts<unsigned long>(len, 0, ULONG_MAX);
}

void IntBitArrayTest::testExtremes() {
    int len = 100;

    testWithSample<char>(len, CHAR_MIN, 0, CHAR_MAX);
    testWithSample<short>(len, SHRT_MIN, 0, SHRT_MAX);
    testWithSample<int>(len, INT_MIN, 0, INT_MAX);
    testWithSample<long>(len, LONG_MIN, 0, LONG_MAX);

    testWithSample<unsigned char>(len, 0, UCHAR_MAX/2, UCHAR_MAX);
    testWithSample<unsigned short>(len, 0, USHRT_MAX/2, USHRT_MAX);
    testWithSample<unsigned int>(len, 0, UINT_MAX/2, UINT_MAX);
    testWithSample<unsigned long>(len, 0, ULONG_MAX/2, ULONG_MAX);

}

void IntBitArrayTest::testSerialization() {
    int len = 2000, range = 500;
    testSerializeWithRange<int>(len, INT_MIN, INT_MIN + range);
    testSerializeWithRange<int>(len, INT_MAX - range, INT_MAX);

    testSerializeWithRange<long>(len, LONG_MIN, LONG_MIN + range);
    testSerializeWithRange<long>(len, LONG_MAX - range, LONG_MAX);

    testSerializeWithRange<unsigned long>(len, ULONG_MAX - range, ULONG_MAX);
}
