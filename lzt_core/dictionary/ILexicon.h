#ifndef ILEXICON_H
#define	ILEXICON_H

#include "Word.h"

/** Set of words of TSymbol letters */
template <typename TSymbol>
class ILexicon {

public:

    virtual bool containsWord(const TSymbol* word) = 0;

    virtual ~ILexicon() {}
    
};


#endif	/* ILEXICON_H */

