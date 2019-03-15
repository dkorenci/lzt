#ifndef WORDLIST_H
#define	WORDLIST_H

#include <cstddef>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>

#include "util/utils.h"
#include "util/WordComparator.h"
#include "util/WordEquality.h"

using namespace std;
using namespace __gnu_cxx;

template <typename TSymbol>
class WordList {
public:

    WordList() {};
    WordList(const WordList& orig);
    virtual ~WordList();

    void addWord(TSymbol const * word);
    size_t numberOfWords() const;
    void sort();
    TSymbol const* operator[](size_t i) const;
    bool operator==(WordList<TSymbol> const & list) const;

private:

    typedef typename vector<TSymbol*>::iterator TIter;

    vector<TSymbol*> words;

    WordComparator<TSymbol> wordCompare;

};

template <typename TSymbol>
WordList<TSymbol>::~WordList() {
    for (TIter it = words.begin(); it != words.end(); ++it) {
        delete [] *it;
    }
}

template <typename TSymbol>
size_t WordList<TSymbol>::numberOfWords() const {
    return words.size();
}

/** Clone word and add it to list. */
template <typename TSymbol>
void WordList<TSymbol>::addWord(TSymbol const * word) {
    size_t len = wordLength(word);
    TSymbol* w = new TSymbol[len+1];
    wordCopy(w, word);
    words.push_back(w);
}

template <typename TSymbol>
void WordList<TSymbol>::sort() {
    WordComparator<TSymbol> lessThan;
    bool sorted = true;
    for (size_t i = 0; i < words.size()-1; ++i)
        if (lessThan(words[i+1], words[i])) {
            sorted = false;
            break;
        }

    std::sort(words.begin(), words.end(), lessThan);
}

template <typename TSymbol>
TSymbol const * WordList<TSymbol>::operator [](size_t i) const {
    return words[i];
}

template <typename TSymbol>
bool WordList<TSymbol>::operator ==(WordList<TSymbol> const & list) const {
    multiset<TSymbol const *, WordComparator<TSymbol> > ms1, ms2;
    ms1.insert(words.begin(), words.end());
    ms2.insert(list.words.begin(), list.words.end());

    if (ms1.size() != ms2.size()) return false;

    typedef typename multiset<TSymbol const *, WordComparator<TSymbol> >::iterator Tit;

    for (Tit it = ms1.begin(); it != ms1.end(); ++it) {
        if (ms2.count(*it) != ms1.count(*it))
            return false;
    }

    return true;
}

template <typename TSymbol>
void wordListToStream(WordList<TSymbol> const * wordList, ostream& str) {
    for (size_t i = 0; i < wordList->numberOfWords(); ++i)
        str<<(*wordList)[i]<<endl;
}

template <typename TSymbol>
void wordListToStreamChars(WordList<TSymbol> const * wordList, ostream& str) {
    for (size_t i = 0; i < wordList->numberOfWords(); ++i) {
        TSymbol const * s = (*wordList)[i];
        for (size_t j = 0; s[j] != 0; ++j)
            str<<(unsigned char)(s[j]);
        str<<endl;
    }
}

#endif	/* WORDLIST_H */

