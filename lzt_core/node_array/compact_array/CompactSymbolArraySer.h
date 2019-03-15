#ifndef COMPACTSYMBOLARRAYSER_H
#define	COMPACTSYMBOLARRAYSER_H

#include <iostream>
#include <cassert>

#include "CompactSymbolArray.h"
#include "serialization/SerializationUtils.h"
#include "serialization/BitSequenceArray.h"
#include "serialization/array/BitSequenceArraySer.h"

/** (De)Serializer for CompactSymbolArray. */
template <typename TSymbol>
class CompactSymbolArraySer {
public:

    static int const BITS_PER_SYMBOL = sizeof(TSymbol) * BITS_PER_CHAR;

    static void arrayToStream(CompactSymbolArray<TSymbol> const & array, ostream& stream);
    static void arrayFromStream(CompactSymbolArray<TSymbol>& array, istream& stream);

    virtual ~CompactSymbolArraySer() {};

private:

};

/** Serialize CompactSymbolArray to ostream. */
template <typename TSymbol>
void CompactSymbolArraySer<TSymbol>::
arrayToStream(CompactSymbolArray<TSymbol> const & array, ostream& stream) {
    // serialize integer members
    SerializationUtils::integerToStream(array.numOfSymbols, stream);
    SerializationUtils::integerToStream(array.numOfDistinct, stream);
    SerializationUtils::integerToStream(array.bitsPerIndex, stream);

    //TODO write generic method for serializing arrays of integer types
    // serialize symbolTable
    BitSequenceArray symbols(array.numOfDistinct, BITS_PER_SYMBOL );
    for (size_t i = 0; i < array.numOfDistinct; ++i)
        symbols.setSequence(i, toBitSequence(array.symbolTable[i], BITS_PER_SYMBOL));

    BitSequenceArraySer::arrayToStream(symbols, stream);

    // serialize indexes
    BitSequenceArraySer::arrayToStream(array.indexes, stream);
}

//    TSymbol* symbolTable;
//
//    BitSequenceArray indexes;

/** Deserialize CompactSymbolArray from istream. */
template <typename TSymbol>
void CompactSymbolArraySer<TSymbol>::
arrayFromStream(CompactSymbolArray<TSymbol>& array, istream& stream) {
    // deallocate array memory
    array.freeTable();

    // deserialize integer members
    array.numOfSymbols = SerializationUtils::integerFromStream<size_t>(stream);
    array.numOfDistinct = SerializationUtils::integerFromStream<size_t>(stream);
    array.bitsPerIndex = SerializationUtils::integerFromStream<int>(stream);

    // deserialize symbolTable
    BitSequenceArray* bitArray = new BitSequenceArray;
    BitSequenceArraySer::arrayFromStream(*bitArray, stream);
    array.symbolTable = new TSymbol[array.numOfDistinct];
    assert(array.numOfDistinct == bitArray->getNumOfSequences());
    for (size_t i = 0; i < array.numOfDistinct; ++i) {
        BitSequence bits = (*bitArray)[i];
        array.symbolTable[i] = fromBitSequence<TSymbol>(bits, BITS_PER_SYMBOL);
    }
    delete bitArray;

    // deserialize indexes
    BitSequenceArraySer::arrayFromStream(array.indexes, stream);
}



#endif	/* COMPACTSYMBOLARRAYSER_H */

