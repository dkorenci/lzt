#ifndef COMPACTARRAYSERIALIZER_H
#define	COMPACTARRAYSERIALIZER_H

#include <iostream>
#include <cassert>

#include "node_array/compact_array/CompactArray.h"
#include "serialization/SerializationUtils.h"
#include "serialization/array/BitSequenceArraySer.h"
#include "node_array/compact_array/CompactSymbolArray.h"
#include "node_array/compact_array/CompactSymbolArraySer.h"

//TODO improve interface, integrate it with CompactArray
template <typename TSymbol, typename TIndex>
class CompactArraySerializer {
public:

    CompactArraySerializer();
    CompactArraySerializer(CompactArray<TSymbol, TIndex> const* arr);

    void arrayToStream(ostream& stream);
    CompactArray<TSymbol, TIndex>* arrayFromStream(istream& stream);

private:
    CompactArray<TSymbol, TIndex> const *carray;
    CompactArray<TSymbol, TIndex>* array;
    // bits necessary to write symbol and index
    /* TODO ovo ce se morati racunati kad bude podrzana serijalizacija
       simbola i indexa koji su custom klase. */
    static const int SYMBOL_BITS = sizeof(TSymbol) * BITS_PER_CHAR;

    static const int NUM_OFFSETS = CompactArray<TSymbol, TIndex>::NUM_OFFSETS;

    void writeSymbols(ostream& stream);
    void readSymbols(istream& stream);

    void writeSiblings(ostream& stream);
    void readSiblings(istream& stream);

    void writeNumWords(ostream& stream);
    void readNumWords(istream& stream);

};

template <typename TSymbol, typename TIndex>
CompactArraySerializer<TSymbol, TIndex>
::CompactArraySerializer(const CompactArray<TSymbol,TIndex>* arr): carray(arr) {
    
}

template <typename TSymbol, typename TIndex>
CompactArraySerializer<TSymbol, TIndex>
::CompactArraySerializer(): carray(0), array(0) { }

/** Serialize to stream the array passed as the argument to the constructor.
 * If the object was constructed with default constructor, do nothing. */
template <typename TSymbol, typename TIndex>
void CompactArraySerializer<TSymbol, TIndex>::arrayToStream(ostream& stream) {
    if (carray == 0) return;
    //serialize variables
    SerializationUtils::integerToStream(carray->numOfDistinct, stream);
    SerializationUtils::integerToStream(carray->numOfNodes, stream);
    SerializationUtils::integerToStream(carray->bitsPerIndex, stream);
    SerializationUtils::integerToStream(carray->enumerated, stream);

    for (size_t i = 0; i < NUM_OFFSETS; ++i)
        SerializationUtils::integerToStream(carray->flagOffsets[i], stream);

    BitSequenceArraySer::arrayToStream(carray->array, stream);

    writeSymbols(stream);
    writeSiblings(stream);
    
    if (carray->enumerated) writeNumWords(stream);
}

template <typename TSymbol, typename TIndex>
void CompactArraySerializer<TSymbol, TIndex>::writeSiblings(ostream& stream) {
    BitSequenceArraySer::arrayToStream(carray->siblings, stream);
}

template <typename TSymbol, typename TIndex>
void CompactArraySerializer<TSymbol, TIndex>::writeNumWords(ostream& stream) {
    BitSequenceArraySer::arrayToStream(carray->numOfWords, stream);
}

template <typename TSymbol, typename TIndex>
void CompactArraySerializer<TSymbol, TIndex>::writeSymbols(ostream& stream) {
    CompactSymbolArraySer<TSymbol>::arrayToStream(carray->symbols, stream);
}

template <typename TSymbol, typename TIndex>
CompactArray<TSymbol, TIndex>* CompactArraySerializer<TSymbol, TIndex>
::arrayFromStream(istream& stream) {
    array = new CompactArray<TSymbol, TIndex>();

    // read integer variables
    array->numOfDistinct = SerializationUtils::integerFromStream<size_t>(stream);
    array->numOfNodes = SerializationUtils::integerFromStream<size_t>(stream);
    array->bitsPerIndex = SerializationUtils::integerFromStream<size_t>(stream);
    array->enumerated = SerializationUtils::integerFromStream<bool>(stream);

    // read flag offsets
    for (size_t i = 0; i < NUM_OFFSETS; ++i)
        array->flagOffsets[i] = SerializationUtils::integerFromStream<size_t>(stream);

    // read array (indexes)
    BitSequenceArraySer::arrayFromStream(array->array, stream);

    // read distinct symbols and siblings
    readSymbols(stream);
    readSiblings(stream);

    if (array->enumerated) readNumWords(stream);

    return array;
}

template <typename TSymbol, typename TIndex>
void CompactArraySerializer<TSymbol, TIndex>::readSymbols(istream& stream) {
    CompactSymbolArraySer<TSymbol>::arrayFromStream(array->symbols, stream);
}

template <typename TSymbol, typename TIndex>
void CompactArraySerializer<TSymbol, TIndex>::readSiblings(istream& stream) {
    BitSequenceArraySer::arrayFromStream(array->siblings, stream);
}

template <typename TSymbol, typename TIndex>
void CompactArraySerializer<TSymbol, TIndex>::readNumWords(istream& stream) {
    BitSequenceArraySer::arrayFromStream(array->numOfWords, stream);
}

#endif	/* COMPACTARRAYSERIALIZER_H */

