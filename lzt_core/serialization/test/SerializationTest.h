#ifndef SERIALIZATIONTEST_H
#define	SERIALIZATIONTEST_H

class SerializationTest {
public:
    SerializationTest();
    SerializationTest(const SerializationTest& orig);
    virtual ~SerializationTest();

    void testIntegerSerialization();
    void testPositiveNumSerialization();
    void testBitBlocksArray();
    void testBitSequenceArray();

private:

    template <typename T>
    void serializeRandomIntegers();

    template <typename T>
    void serializePozitiveNumbers();

    void serializeBooleans();

    void serializeArrayOfRandomSeqs(size_t numOfSeqs, int numOfBits);

    template <typename T>
    static T getMinUnsigned();

    template <typename T>
    static T getMaxUnsigned();

};

#endif	/* SERIALIZATIONTEST_H */

