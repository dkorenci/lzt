#ifndef UTILS_H
#define	UTILS_H

#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>
#include <ctime>
#include <cstdlib>
#include <cstddef>

#include "node_array/types/symbol.h"
#include "dictionary/util/WordList.h"
#include "dictionary/lztrie_dict/WordPair.h"

using namespace std;

// number to string
template <typename T>
string nts ( T Number )
{
	stringstream ss;
	ss << Number;
	return ss.str();
}

template <typename TSymbol, typename TIndex>
TIndex sstringLength(const TSymbol* word) {
    TIndex l;
    //TODO zahtjev na tip
    for (l = 0; word[l] != zeroSymbol<TSymbol>(); ++l);
    return l;
}

template <typename A>
string subArrayToString(A const & array, size_t b, size_t e) {
    ostringstream ss;
    char border = '.';

    for (size_t i = b; i < e; i++) {
        if (b < i) ss << border;
        ss << array[i];
    }

    return ss.str();
}

/** Get a random number from [min,max] */
template <typename T>
T getRandomNumber(T min, T max) {
    T range = max - min;
    return min + (T)( ((double)rand() / (double)RAND_MAX) * (double)range);
}

void randomSeed();

string getRandomString();

template <typename TSymbol>
inline size_t wordLength(TSymbol const * word) {
    size_t size;
    for (size = 0; word[size] != zeroSymbol<TSymbol>(); ++size);
    return size;
}

template <typename TSymbol>
inline void wordCopy(TSymbol *dest, TSymbol const *src) {
    size_t i;
    for (i = 0; src[i] != zeroSymbol<TSymbol>(); ++i)
        dest[i] = src[i];
    dest[i] = src[i];
}

template <typename TSymbol>
inline TSymbol* wordClone(const TSymbol* word) {
    size_t l = wordLength(word);
    TSymbol* clone = new TSymbol[l+1];
    
    for (size_t i = 0; i < l; ++i) clone[i] = word[i];
    clone[l] = zeroSymbol<TSymbol>();

    return clone;
}

/** Return true if prefix is a prefix of word. */
template <typename TSymbol>
inline bool isPrefix(const TSymbol* prefix, const TSymbol* word) {
    if (prefix[0] == zeroSymbol<TSymbol>()) return true;

    for (size_t i = 0; prefix[i] != zeroSymbol<TSymbol>(); ++i) {
        if (word[i] == zeroSymbol<TSymbol>()) return false;
        if (prefix[i] != word[i]) return false;
    }

    return true;
}

/** Free blocks of symbols pointed to by elements of the vector. */
template <typename TSymbol>
void freeWords(vector<TSymbol*>& words) {
    for (size_t i = 0; i < words.size(); ++i)
        delete [] words[i];
}

/** Free blocks of symbols pointed to by pairs in the vector. */
template <typename TSymbol>
void freePairs(vector<WordPair<TSymbol> >& pairs) {
    for (size_t i = 0; i < pairs.size(); ++i) {
        delete [] pairs[i].first;
        delete [] pairs[i].second;
    }
}

/** Combine partial hash value into a cumulative hash value, for calculating
 * hashes of composite types. */
inline size_t hashCombine(size_t hash_cum, size_t hash_part) {
    hash_cum ^= hash_part + 0x9e3779b9 + hash_cum << 6 + hash_cum >> 2;
    return hash_cum;
}

template <typename TSymbol>
inline bool wordsEqual(TSymbol const *w1, TSymbol const *w2) {
    if (wordLength(w1) != wordLength(w2)) return false;
    for (size_t i = 0; w1[i] != zeroSymbol<TSymbol>(); ++i)
        if (w1[i] != w2[i]) return false;
    return true;
}

template <typename TSymbol>
inline string symbolsToString(TSymbol const *symb) {    
    ostringstream ss;
    for (size_t i = 0; symb[i] != zeroSymbol<TSymbol>(); ++i)
        ss <<  symb[i];          
    return ss.str();
}

#endif	/* UTILS_H */

