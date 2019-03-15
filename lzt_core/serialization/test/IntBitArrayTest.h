#ifndef INTBITARRAYTEST_H
#define	INTBITARRAYTEST_H

#include <cstddef>
#include <sstream>
#include <fstream>

#include "../IntBitArray.h"
#include "debug/lzt_test.h"
#include "util/utils.h"
#include "util/TempFile.h"

class IntBitArrayTest {
public:

    void testExtremes();
    void testRandom();
    void testSerialization();

private:
    template <typename T>
    void testWithArray(T* ints, size_t len);

    template <typename T>
    T* getRandomArray(size_t len, T min, T max);

    template <typename T>
    void testWithRandomInts(size_t len, T min, T max);

    template <typename T>
    T* getArrayFromSample(size_t len, T* sample, size_t sampleLen);

    template <typename T>
    void testWithSample(size_t len, T s1, T s2, T s3);

    template <typename T>
    void testSerializeWithRange(size_t len, T min, T max);

};

template <typename T>
void IntBitArrayTest::testWithRandomInts(size_t len, T min, T max) {
    T* array = getRandomArray<T>(len, min, max);
    testWithArray(array, len);
    delete [] array;
}

/** Construct IntBitArray from an array of integers and 
 * assert (member by member) that two arrays are equal. */
template <typename T>
void IntBitArrayTest::testWithArray(T* ints, size_t len) {
    IntBitArray<T> bitArray(ints, len);
    stringstream m;
    m << "len: " << len << " bitArray length: " << bitArray.getSize();
    TEST_ASSERT_MESSAGE(len == bitArray.getSize(), m.str());

    stringstream array;
    for (size_t i = 0; i < len; ++i)
        array << "i: " << i << " ints[i]: " << ints[i] << " bitArray[i]: " << bitArray[i] << endl;

    for (size_t i = 0; i < len; ++i) {
        stringstream m2;
        m2 << "failed i: " << i << endl;

        if (ints[i] != bitArray[i])
            TEST_ASSERT_MESSAGE(ints[i] == bitArray[i], array.str() + m2.str());
    }
}

/** Generate random array of integers of type T. */
template <typename T>
T* IntBitArrayTest::getRandomArray(size_t len, T min, T max) {
    randomSeed();
    T* ints = new T[len];
    
    for (int i = 0; i < len; ++i) ints[i] = getRandomNumber<T>(min, max);

    return ints;
}

/** Return array of length len filled with repeated copies of sample array. */
template <typename T>
T* IntBitArrayTest::getArrayFromSample(size_t len, T* sample, size_t sampleLen) {
    T* array = new T[len];
    size_t s = 0;
    for (size_t i = 0; i < len; ++i) {
        array[i] = sample[s++];
        if (s == sampleLen) s = 0;
    }
    return array;
}

/** Test with array of length len with values taken from three sample values. */
template <typename T>
void IntBitArrayTest::testWithSample(size_t len, T s1, T s2, T s3) {
    T* sample = new T[3];
    sample[0] = s1; sample[1] = s2; sample[2] = s3;
    T* array = getArrayFromSample(len, sample, 3);
    
    testWithArray(array, len);

    delete array;
    delete sample;
}

/** Test by filling an array of length len with getArrayFromSample where
 * sample is [min, max>, then writing to file, reading and asserting equality. */
template <typename T>
void IntBitArrayTest::testSerializeWithRange(size_t len, T min, T max) {    
    // create array
    T* sample = new T[max-min];    
    for (T i = 0, v = min; v < max; ++v, ++i) sample[i] = v;
    T* array = getArrayFromSample(len, sample, max-min);
    delete [] sample;

    IntBitArray<T> bitArray(array, len);
    delete [] array;

    TempFile file;
    // serialize
    fstream fstr(file.getName());
    bitArray.writeToStream(fstr);
    fstr.close();
    // deserialize
    IntBitArray<T> bitArrayDeser;
    fstr.open(file.getName());
    bitArrayDeser.readFromStream(fstr);
    fstr.close();

    // assert equality
    ostringstream m;
    m << bitArray.toString() << bitArrayDeser.toString();

    TEST_ASSERT_MESSAGE(bitArray.getSize() == bitArrayDeser.getSize(), m.str());

    for (size_t i = 0; i < bitArray.getSize(); ++i) {
        ostringstream m2;
        m2 << "mismatch index: " << i << endl;
        TEST_ASSERT_MESSAGE(bitArray[i] == bitArrayDeser[i], m.str() + m2.str());
    }
}

#endif	/* INTBITARRAYTEST_H */

