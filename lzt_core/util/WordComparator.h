#ifndef WORDCOMPARATOR_H
#define	WORDCOMPARATOR_H

#include "node_array/types/symbol.h"
#include "util/utils.h"

/** Generic word comparator, lexicographically compares two strings of symbols. */
template <typename TSymbol>
class WordComparator {
public:
    virtual ~WordComparator() {}

    inline bool operator()(TSymbol const *w1, TSymbol const *w2) const {
        return compare(w1, w2);
    }

    /** Comparison that can be used without object instantiation.
     * Compare two words lexicographicaly, return true if w1 is less than w2. */
    static inline bool compare(TSymbol const *w1, TSymbol const *w2) {
        size_t i;
        for (i = 0; w1[i] != zeroSymbol<TSymbol>(); ++i) {
            if (w2[i] == zeroSymbol<TSymbol>()) return false;
            if (w1[i] < w2[i]) return true;
            if (w1[i] > w2[i]) return false;
        }
        // w2 ends at the same positions, so w1 == w2
        if (w2[i] == zeroSymbol<TSymbol>()) return false;
        // w1 is prefix of w2
        else return true;
    }

private:

};

#endif	/* WORDCOMPARATOR_H */

