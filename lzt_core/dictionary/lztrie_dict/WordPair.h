#ifndef WORDPAIR_H
#define	WORDPAIR_H

#include "util/WordComparator.h"

template <typename TSymbol>
class WordPair {
public:

    TSymbol* first;
    TSymbol* second;

    virtual ~WordPair() {};
    
private:

};

/** Lexicographical comparison by first or second member of a pair. */
template <typename TSymbol>
class WordPairCompare {
public:
    WordPairCompare(bool f): first(f) {}

    inline bool operator()(WordPair<TSymbol> p1, WordPair<TSymbol> p2) {
        if (first) return WordComparator<TSymbol>::compare(p1.first, p2.first);
        else return WordComparator<TSymbol>::compare(p1.second, p2.second);
    }

private:
    // determines weather pair are compared by first or second member
    bool first;

};

/** Lexicographical comparison by both members, first than second. */
template <typename TSymbol>
class WordPairCompareBoth {
public:
    inline bool operator()(WordPair<TSymbol> p1, WordPair<TSymbol> p2) {
        if (WordComparator<TSymbol>::compare(p1.first, p2.first)) return true;
        if (WordComparator<TSymbol>::compare(p2.first, p1.first)) return false;
        
        // first members are equal, compare second members
        if (WordComparator<TSymbol>::compare(p1.second, p2.second)) return true;

        return false;
    }
};

/** Equality comparison functor. */
template <typename TSymbol>
class WordPairsEqual {
public:
    inline bool operator()(WordPair<TSymbol> p1, WordPair<TSymbol> p2) {
        return wordsEqual(p1.first, p2.first) && wordsEqual(p1.second, p2.second);
    }
};


#endif	/* WORDPAIR_H */

