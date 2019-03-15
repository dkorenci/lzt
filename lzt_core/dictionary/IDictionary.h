#ifndef IDICTIONARY_H
#define	IDICTIONARY_H

#include "ILexicon.h"

/** Lexicon that maps words of TSymbol letters to TData type. */
template <typename TSymbol, typename TData>
class IDictionary : public ILexicon<TSymbol> {

public:

    virtual TData& lookupWord(const TSymbol* word) const = 0;

    virtual ~IDictionary();

private:

};

#endif	/* IDICTIONARY_H */

