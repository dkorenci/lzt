#ifndef INTBITARRAY_H
#define	INTBITARRAY_H

#include <cstddef>
#include <iostream>
#include <string>
#include <sstream>

#include "serialization/BitSequence.h"
#include "serialization/BitSequenceArray.h"
#include "serialization/serialization.h"
#include "serialization/array/BitSequenceArraySer.h"
#include "serialization/SerializationUtils.h"

using namespace std;

/** Static array for compact storage and serialization of an
 * array of integers. Number of bits per integer is minimum
 * number that can store the integer with the largest modulo,
 * plus the bit for signum if there are negative integers. */
template <typename T>
class IntBitArray {
public:

    IntBitArray();
    IntBitArray(T* ints, size_t len);
    
    void setArray(T* ints, size_t len);
    size_t getSize() const;
    T operator[](size_t i) const;
    const BitSequenceArray& getBits() const;

    string toString();

    void writeToStream(ostream& stream) const;
    void readFromStream(istream& stream);

private:
    // True if integers are signed.
    bool signum;
    BitSequenceArray numbers;

    int calculateMinBits(T* ints, size_t len);
    void calculateSignum(T* ints, size_t len);
    void copyNumbersToBits(T* ints, size_t len, int bitsPerInt);
    inline T abs(T n);

};

template <typename T>
IntBitArray<T>::IntBitArray() {}

template <typename T>
IntBitArray<T>::IntBitArray(T* ints, size_t len) {
    setArray(ints, len);
}

template <typename T>
void IntBitArray<T>::setArray(T* ints, size_t len) {
    int bits = calculateMinBits(ints, len);
    calculateSignum(ints, len);
    // add one bit per number if numbers are signed
    if (signum) bits++;
    copyNumbersToBits(ints, len, bits);
}

/** Calculate min. number of bits neccesary to store numbers between 0
 * and largest absolute value in the array.*/
template <typename T>
int IntBitArray<T>::calculateMinBits(T* ints, size_t len) {
    int max = -1;
    for (size_t i = 0; i < len; ++i) {
        BitSequence bits = numberToBits(ints[i]);
        int b = bits.maxNonzeroBit();
        if (b == -1) b = 1;
        else b++;

        if (b > max) max = b;
    }
    return max;
}

/** Determine if there are negative integers in the array. */
template <typename T>
void IntBitArray<T>::calculateSignum(T* ints, size_t len) {
    signum = false;
    for (size_t i = 0; i < len; ++i) {
        if (ints[i] < 0) {
            signum = true;
            return;
        }
    }    
}

template <typename T>
size_t IntBitArray<T>::getSize() const {
    return numbers.getNumOfSequences();
}

template <typename T>
const BitSequenceArray& IntBitArray<T>::getBits() const {
    return numbers;
}


/** Return absolute value of a number. */
template <typename T>
T IntBitArray<T>::abs(T n) {
    return n >= 0 ? n : -n;
}

template <typename T>
void IntBitArray<T>::copyNumbersToBits(T* ints, size_t len, int bitsPerInt) {
    /* Allocate space for bit representations, bitsPerInt and len
     * will be stored in the numbers object. */
    numbers.changeFormat(len, bitsPerInt);
    for (size_t i = 0; i < len; ++i) {
        BitSequence bits;
        bits = numberToBits(ints[i]);
        if (signum) {
            // set negative number indicatior bit to highest index
            if (ints[i] < 0) bits.setBit(bitsPerInt-1, true);
        }
        numbers.setSequence(i, bits);
    }
}

/** Get i-th number in the array. */
template <typename T>
inline T IntBitArray<T>::operator [](size_t i) const {
    T result;
    BitSequence bits = numbers[i];

    if (signum) {
        // max index of non-signum bits
        int maxBitIndex = numbers.getSequenceSize()-1;
        // get absoulte value of result ignoring sign bit
        result = numberFromBits<T>(bits, maxBitIndex);
        // check sign and apply if neccesary
        if (bits[maxBitIndex]) result = -result;
    }
    else {
        // get value using all bits
        result = numberFromBits<T>(bits, numbers.getSequenceSize());
    }

    return result;
}

template <typename T>
string IntBitArray<T>::toString() {
    ostringstream ss;
    ss << "length: " << getSize() << endl;
    for (size_t i = 0; i < getSize(); ++i) ss << (*this)[i] << " ";
    ss << endl;

    return ss.str();
}

template <typename T>
void IntBitArray<T>::writeToStream(ostream& stream) const {
    SerializationUtils::integerToStream(signum, stream);
    BitSequenceArraySer::arrayToStream(numbers, stream);
}

template <typename T>
void IntBitArray<T>::readFromStream(istream& stream) {
    signum = SerializationUtils::integerFromStream<bool>(stream);
    BitSequenceArraySer::arrayFromStream(numbers, stream);
}

#endif	/* INTBITARRAY_H */

