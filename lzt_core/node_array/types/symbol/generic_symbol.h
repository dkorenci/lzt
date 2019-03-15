#ifndef GENERIC_SYMBOL_H
#define	GENERIC_SYMBOL_H

#include <limits>

//TODO umjesto definicija samo deklaracije, da nebi doslo do
// nehoticne uporabe generickih funkcija koje ne funkcioniraju tocno

/** @return largest number that can fit in given TSymbol type */
template <typename TSymbol> TSymbol maxSymbol() {
    return std::numeric_limits<TSymbol>::max()-2;
}

/** @return zero symbol used for example for marking end of string */
template <typename TSymbol>
TSymbol zeroSymbol() {
    return 0;
}

template <typename TSymbol>
int symbolToInt(TSymbol s) {
    return (int)s;
}

#endif	/* GENERIC_SYMBOL_H */

