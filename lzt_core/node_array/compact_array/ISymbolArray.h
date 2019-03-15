#ifndef ISYMBOLARRAY_H
#define	ISYMBOLARRAY_H

/** Common interface for all implementations of a TSymbol array
 * needed in a CompactArray. */
template <typename TSymbol>
class ISymbolArray {
public:

    virtual TSymbol operator[](size_t i) = 0;

    virtual ~ISymbolArray() {};

};

#endif	/* ISYMBOLARRAY_H */

