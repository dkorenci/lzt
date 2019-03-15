#ifndef BITVECTORTEST_H
#define	BITVECTORTEST_H

#include <cstdlib>
#include "../BitVector.h"

class BitVectorTest {
public:

    virtual ~BitVectorTest();

    void testAccess();
    void testSequenceIO();
    void testResize();
    void testSerialize();
    
private:

    void testSameBit(size_t size, bool bit);
    void testReadWrite(size_t size, int mod, int limit);
    int fillVectorWithNumbers(BitVector* vector, size_t maxNumber);
    void testWithBinNumbers(size_t maxNumber);
    void testSerWithBinNumber(size_t maxNumber);

};

#endif	/* BITVECTORTEST_H */

