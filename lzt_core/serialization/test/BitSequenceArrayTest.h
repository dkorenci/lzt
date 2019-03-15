#ifndef BITSEQUENCEARRAYTEST_H
#define	BITSEQUENCEARRAYTEST_H

#include "debug/lzt_test.h"
#include "../BitSequenceArray.h"

class BitSequenceArrayTest {
public:
    BitSequenceArrayTest() {};
    BitSequenceArrayTest(const BitSequenceArrayTest& orig) {};
    virtual ~BitSequenceArrayTest() {};

    void testAccess();
    void testResize();
    void testChangeFormat();

private:

    static size_t const ONE = 1;

    void readWriteNumberResize(long initSize, int B, long resizeStep, int numSteps);
    void readWriteNumbers(BitSequenceArray* array, size_t arraySize, int numberSize);

};

#endif	/* BITSEQUENCEARRAYTEST_H */

