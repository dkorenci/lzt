#ifndef BITSEQUENCETEST_H
#define	BITSEQUENCETEST_H

#include "debug/lzt_test.h"

class BitSequenceTest {
public:
    BitSequenceTest() {};
    BitSequenceTest(const BitSequenceTest& orig) {};
    virtual ~BitSequenceTest() {};

    void testAccess();
    void testMaxNonzero();
    void testTransform();
    void testShift();
    void testPositiveNums();
    void testEquals();

private:

};

#endif	/* BITSEQUENCETEST_H */

