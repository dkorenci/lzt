#ifndef REGEX_H
#define	REGEX_H

#include "node_array/types/symbol.h"

/** Fills the result with part of the word before '*', returns
 * true if word is of correct form which is string of symbols and than
 * optionally a '*' */
template <typename TSymbol>
bool getPrefixBeforeStar(TSymbol const * word, TSymbol *result) {
    size_t i;
    bool star = false;
    for (i = 0; word[i] != zeroSymbol<TSymbol>(); ++i) {
        if (word[i] == '*') break;
        result[i] = word[i];
    }
    result[i] = zeroSymbol<TSymbol>();

    if (word[i] == '*') {
        if (word[i+1] == zeroSymbol<TSymbol>()) return true;
        else return false;
    }
    else return true;
}

#endif	/* REGEX_H */

