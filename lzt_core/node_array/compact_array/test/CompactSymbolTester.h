#ifndef COMPACTSYMBOLTESTER_H
#define	COMPACTSYMBOLTESTER_H

#include <cstddef>
#include <string>
#include <sstream>
#include <iostream>

#include "debug/lzt_test.h"
#include "util/utils.h"
#include "util/TempFile.h"

#include "../CompactSymbolArray.h"
#include "../CompactSymbolArraySer.h"

/** Tester for CompactSymbolArray. */
template <typename TSymbol>
class CompactSymbolTester {
public:

    CompactSymbolTester(TSymbol min, TSymbol max, size_t arrayLength, bool random);

    void testCreate();
    void testSerialize();

    virtual ~CompactSymbolTester();

private:
    const TSymbol MIN, MAX;
    const size_t arrayLength;
    bool random;
    TSymbol* array;

    void fillArray();

};

template <typename TSymbol>
CompactSymbolTester<TSymbol>::CompactSymbolTester(TSymbol min, TSymbol max, size_t len, bool rand)
: MIN(min), MAX(max), arrayLength(len), random(rand) {
    array = new TSymbol[arrayLength];
    fillArray();
}

template <typename TSymbol>
CompactSymbolTester<TSymbol>::~CompactSymbolTester() {
    delete [] array;
}

/** Fill the normal array with data. If random is true, fill with
 * random symbols from range [MIN, MAX]. Else fill with symbols starting
 * from MIN and increased by one for each array element. When MAX is reached
 * this way, go back to MIN. */
template <typename TSymbol>
void CompactSymbolTester<TSymbol>::fillArray() {
    if (random) randomSeed();
    TSymbol s = MIN;
    for (size_t i = 0; i < arrayLength; i++) {
        if (random) {
            array[i] = getRandomNumber<TSymbol>(MIN, MAX);
        }
        else {
            array[i] = s;
            s++;
            if (s == MAX) s = MIN;
        }
    }
}

template <typename TSymbol>
void CompactSymbolTester<TSymbol>::testCreate() {
    CompactSymbolArray<TSymbol> carray(array, arrayLength);
    
    ostringstream bm;
    bm << "min: " << MIN << " max: " << MAX << " arrayLength: "
        << arrayLength << " random: " << random << endl;

    for (size_t i = 0; i < arrayLength; ++i) {
        TSymbol expected = array[i];
        TSymbol symbol = carray[i];


        if (symbol != expected) {
            typedef TSymbol TCast;
            ostringstream m;
            m << bm.str() << "expected: " << expected << " symbol: " << symbol << endl;
            m << " carray length: " << carray.size() << " i: " << i << endl;
            m << "array: " << endl;
            for (size_t j = 0; j < arrayLength; ++j)
                m << (TCast)array[j] << " " << (TCast)carray[j] << endl;
            
            TEST_ASSERT_MESSAGE(symbol == expected, m.str());
        }
    }
}

/** Test (De)Serialization of CompactSymbolArray to a stream. */
template <typename TSymbol>
void CompactSymbolTester<TSymbol>::testSerialize() {
    CompactSymbolArray<TSymbol> carray(array, arrayLength);
    // serialize
    TempFile file;
    fstream stream(file.getName());
    CompactSymbolArraySer<TSymbol>::arrayToStream(carray, stream);
    stream.close();

    // deserialize
    CompactSymbolArray<TSymbol> deserArray;
    stream.open(file.getName());
    CompactSymbolArraySer<TSymbol>::arrayFromStream(deserArray, stream);
    stream.close();

    // check equality
    TEST_ASSERT(carray.size() == deserArray.size());

    for (size_t i = 0; i < carray.size(); ++i)
        TEST_ASSERT(carray[i] == deserArray[i]);
}


#endif	/* COMPACTSYMBOLTESTER_H */

