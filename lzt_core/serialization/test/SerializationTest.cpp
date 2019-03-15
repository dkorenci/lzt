#include <sstream>
#include <cassert>
#include <fstream>

#include "SerializationTest.h"
#include "../SerializationUtils.h"
#include "debug/lzt_test.h"
#include "util/utils.h"
#include "util/TempFile.h"
#include "util/constants.h"
#include "serialization/BitSequenceArray.h"
#include "../array/BitSequenceArraySer.h"

SerializationTest::SerializationTest() {
}

SerializationTest::SerializationTest(const SerializationTest& orig) {
}

SerializationTest::~SerializationTest() {
}

void SerializationTest::testIntegerSerialization() {
    serializeRandomIntegers<unsigned char>();
    serializeRandomIntegers<unsigned short>();
    serializeRandomIntegers<unsigned int>();
    serializeRandomIntegers<unsigned long>();
    serializeBooleans();
}

/** Generate an array of random integers of type T, serialze them to file,
 * read numbers from that file and check that they are equal to the array.  */
template <typename T>
void SerializationTest::serializeRandomIntegers() {
    T min = getMinUnsigned<T>();
    T max = getMaxUnsigned<T>();

    // generate random ints and assert they serialze correctly
    const int numberOfRandom = 500;
    T randomInts[numberOfRandom]; 
    // add zero, it must be tested
    randomInts[0] = 0;
    TempFile file;
    fstream stream(file.getName());

    for (int i = 0; i < numberOfRandom; ++i) {
        if (i > 0) randomInts[i] = getRandomNumber<T>(min, max);
        SerializationUtils::integerToStream<T>(randomInts[i], stream);
    }
    stream.close();

    stream.open(file.getName());
    for (int i = 0; i < numberOfRandom; ++i) {
        ostringstream m; m << "min: " << min << " max: " << max << endl;

        T deserialized = SerializationUtils::integerFromStream<T>(stream);

        m << "i: " << i << " randomInts[i]: " << randomInts[i]
          << " deserialzied: " << deserialized;
        TEST_ASSERT_MESSAGE(deserialized == randomInts[i], m.str());
    }
    stream.close();
}

/** Generate a sequence of booleans, serialize to file, deserialize and
 * check equality with initial sequence. */
void SerializationTest::serializeBooleans() {   
    const int N = 50;
    bool bools[N];

    TempFile file;
    fstream stream(file.getName());

    for (int i = 0; i < N; ++i) {
        bools[i] = ((i % 2) == 0);
        SerializationUtils::integerToStream<bool>(bools[i], stream);
    }
    stream.close();

    stream.open(file.getName());
    for (int i = 0; i < N; ++i) {        
        bool b = SerializationUtils::integerFromStream<bool>(stream);

        ostringstream m; m << "boolean serialization test " << endl;
        m << "i: " << i << " bools[i]: " << bools[i]
          << " deserialzied: " << b;
        TEST_ASSERT_MESSAGE(b == bools[i], m.str());
    }
    stream.close();
}

void SerializationTest::testBitBlocksArray() {
    const int numOfBits = BitSequence::getNumberOfBits();    
    for (int i = 0; i < numOfBits; ++i) {
        BitSequence bits; bits.setFalse();
        
        for (int j = 0; j <= i; ++j)
            bits.setBit (j, ((i - j) % 2 == 0) );
        
        BitBlocksArray blocks = SerializationUtils::getNonzeroBlocks(bits);

        ostringstream m; m << "i: " << i << endl << "bits: " << bits.toString() << endl;
        if (i < 8) TEST_ASSERT_MESSAGE(blocks.size == 1, m.str());

        BitSequence bits2 = SerializationUtils::fromNonzeroBlocks(blocks);

        m << "bits2: " << bits2.toString() << endl;
        TEST_ASSERT_MESSAGE(bits == bits2, m.str());
    }
}

void SerializationTest::testBitSequenceArray() {
    size_t numOfSeqs = 1000;
    serializeArrayOfRandomSeqs(numOfSeqs, 1);
    serializeArrayOfRandomSeqs(numOfSeqs, BITS_PER_CHAR - 1);
    serializeArrayOfRandomSeqs(numOfSeqs, BITS_PER_CHAR);
    serializeArrayOfRandomSeqs(numOfSeqs, 13);
    serializeArrayOfRandomSeqs(numOfSeqs, 79);
    serializeArrayOfRandomSeqs(numOfSeqs, 113);
}

/** Generate BitSequenceArray of random sequences, serialze to file and check
 * it is equal to the read array. */
void SerializationTest::serializeArrayOfRandomSeqs(size_t numOfSequences, int bitsPerSeq) {
    BitSequenceArray array(numOfSequences, bitsPerSeq);
    for (size_t i = 0; i < numOfSequences; ++i) {
        BitSequence bits;
        for (int j = 0; j < bitsPerSeq; ++j) {
            int rand = getRandomNumber<int>(0, 100);
            bits.setBit(j, rand % 2 == 0);
        }
        array.setSequence(i, bits);
    }

    TempFile file;
    fstream stream(file.getName());

    BitSequenceArraySer::arrayToStream(array, stream);
    stream.close();

    stream.open(file.getName());
    BitSequenceArray* arrayDeser = new BitSequenceArray;
    BitSequenceArraySer::arrayFromStream(*arrayDeser, stream);

    ostringstream ss;
    ss << "numOfSeq: " << array.getNumOfSequences()
       << " bitsPerSeq: " << array.getSequenceSize() << endl
       << "ser numOfSeq: " << arrayDeser->getNumOfSequences()
       << " ser bitsPerSeq: " << arrayDeser->getSequenceSize() << endl;
    // check size parameters equality
    TEST_ASSERT_MESSAGE(array.getNumOfSequences() == arrayDeser->getNumOfSequences(), ss.str());
    TEST_ASSERT_MESSAGE(array.getSequenceSize() == arrayDeser->getSequenceSize(), ss.str());

    for (size_t i = 0; i < array.getNumOfSequences(); ++i) {
        BitSequence bits = array[i];
        BitSequence bitsDeser = (*arrayDeser)[i];

        ostringstream ss;
        ss << "numOfSeq: " << array.getNumOfSequences()
           << " bitsPerSeq: " << array.getSequenceSize() << endl
           << " i: " << i << endl << "bits: " << bits.toString() << endl
           << "bits deser: " << bitsDeser.toString() << endl;

        bool bitsEqual = true;
        for (int j = 0; j < bitsPerSeq; ++j)
            if (bits[j] != bitsDeser[j]) {
                bitsEqual = false;
                break;
            }

        TEST_ASSERT_MESSAGE(bitsEqual, ss.str());
    }

    delete arrayDeser;
}

/** Return minimal unsigned integer of given type (all bits 0). */
template <typename T>
T SerializationTest::getMinUnsigned() {
    BitSequence bits;
    int typeBits = sizeof(T) * BITS_PER_CHAR;

    bits.setFalse();
    T min = fromBitSequence<T>(bits, typeBits);

    return min;
}

/** Return maximal unsigned integer of give type (all bits 1) */
template <typename T>
T SerializationTest::getMaxUnsigned() {
    BitSequence bits;
    int typeBits = sizeof(T) * BITS_PER_CHAR;

    for (int i = 0; i < typeBits; ++i) bits.setBit(i, true);
    T max = fromBitSequence<T>(bits, typeBits);

    return max;
}