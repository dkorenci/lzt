#ifndef CHAR_SYMBOL_H
#define	CHAR_SYMBOL_H

#include "node_array/types/symbol/generic_symbol.h"

template <> char maxSymbol<char>();
template <> char zeroSymbol<char>();
template <> int symbolToInt<char>(char s);

template <> unsigned char maxSymbol<unsigned char>();
template <> unsigned char zeroSymbol<unsigned char>();
template <> int symbolToInt<unsigned char>(unsigned char s);

#endif	/* CHAR_SYMBOL_H */

