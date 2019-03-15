#include "char_symbol.h"

/** Decreased by one for compatibility with old C code, where
 * number stored in symbol field of pointers is full length of replaced
 * segment, not that length-1 (wich is enough because they must
 * match in first simbol). When converting to old format, these fields
 * must be increased by 1, so maxSymbols are decreased by 1. */

template <> char maxSymbol<char>() {
    return 125;
}

template <> char zeroSymbol<char>() {
    return (char)0;
}

template <> int symbolToInt<char>(char s) {
    return (int)s;
}



template <> unsigned char maxSymbol<unsigned char>() {
    return 253;
}

template <> unsigned char zeroSymbol<unsigned char>() {
    return (unsigned char)0;
}

template <> int symbolToInt<unsigned char>(unsigned char s) {
    return (int)s;
}
