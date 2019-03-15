/**
 * Data passed to dictionaries and lexicons.
 */

#ifndef WORD_H
#define	WORD_H

#include <cstddef>

#include "node_array/types/symbol.h"

template <typename TSymbol>
class Word {
public:
    
    size_t getLength() { return 0; }
    TSymbol operator[](size_t index) {
        //TODO potreba za null_symbol?
        return zeroSymbol<TSymbol>();
    }

    virtual ~Word() { }

};

template <typename TSymbol>
class Word<TSymbol *> {
public:

    size_t getLength() {
        if (lengthCalculated) {
            return length;
        }
        else {
            calculateLength();
            return length;
        }
    }

    TSymbol operator[](size_t index) {
        return word[index];
    }

    Word(const TSymbol *w) {
        word = w;
        lengthCalculated = false;
    }

    virtual ~Word() { }

private:
    const TSymbol *word;
    size_t length;
    bool lengthCalculated;

    void calculateLength() {
        size_t len = 0;
        for (len = 0; word[len] != zeroSymbol<TSymbol>(); len++);
        length = len;
        lengthCalculated = true;
    }

};

#endif	/* WORD_H */

