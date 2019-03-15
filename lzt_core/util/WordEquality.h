#ifndef WORDEQUALITY_H
#define	WORDEQUALITY_H

#include "utils.h"

template <typename TSymbol>
class WordEquality {
public:

    inline bool operator()(TSymbol const *w1, TSymbol const *w2) const {
        return wordsEqual(w1, w2);
    }

};

#endif	/* WORDEQUALITY_H */

