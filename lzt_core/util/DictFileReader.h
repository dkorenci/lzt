#ifndef DICTFILEREADER_H
#define	DICTFILEREADER_H

#include <vector>
#include <string>
#include <fstream>
#include <cstddef>
#include <iostream>

#include "dictionary/lztrie_dict/WordPair.h"
#include "util/WordFileReader.h"
#include "util/utils.h"
#include "node_array/types/symbol.h"

using namespace std;

/** Class for reading a list of WordPairs from a file. */
template <typename TSymbol>
class DictFileReader {
public:

    virtual ~DictFileReader();

    static vector<WordPair<TSymbol> >* readWords(string fname, TSymbol sep);
    static void freePairs(vector<WordPair<TSymbol> >* pairs);
    static WordPair<TSymbol> splitLine(const TSymbol* line, TSymbol sep);

};

/** Read words from a file, where one line should contain a pair of non-empty
 * strings separated by a sep symbol. If a line is not correctly formatted,
 * it is not included in the result.   */
template <typename TSymbol>
vector<WordPair<TSymbol> >* DictFileReader<TSymbol>::readWords(string fname, TSymbol sep) {
    fstream file(fname.c_str());
    
    vector<WordPair<TSymbol> >* pairs = new vector<WordPair<TSymbol> >();

    while(file.good()) {
        TSymbol* line;
        if (true) line = WordFileReader<TSymbol>::readLine(file);
        if (wordLength(line) > 0) {
            WordPair<TSymbol> pair = splitLine(line, sep);
            if (pair.first != 0 && pair.second != 0)
                pairs->push_back(pair);
        }
        delete [] line;
    }    

    return pairs;
}

/** Allocate memory for members of the pair and set first member
 * to substring of line from start to first sep symbol,
 * (not including), and set the second member to the rest of the string.
 * If there is no sep symbol, or its first occurence is the first or last symbol,
 * set both members of a pair to 0.  */
template <typename TSymbol>
WordPair<TSymbol> DictFileReader<TSymbol>::splitLine(const TSymbol* line, TSymbol sep) {
    WordPair<TSymbol> result;
    result.first = 0; result.second = 0;

    for (size_t i = 0; line[i] != zeroSymbol<TSymbol>(); ++i) {
        if (line[i] == sep) {
            // sep is first symbol
            if (i == 0) return result;
            // sep is last symbol
            if (line[i+1] == zeroSymbol<TSymbol>()) return result;

            // calculate lengths of the parts of result
            size_t lenFirst = i, lenSecond = 0, j;
            for (j = i + 2; line[j] != zeroSymbol<TSymbol>(); ++j);
            lenSecond = j - i - 1;

            // allocate memory and set end of strings
            result.first = new TSymbol[lenFirst+1];
            result.first[lenFirst] = zeroSymbol<TSymbol>();
            result.second = new TSymbol[lenSecond+1];
            result.second[lenSecond] = zeroSymbol<TSymbol>();
            // fill the parts of result
            for (size_t j = 0; j < lenFirst; ++j) result.first[j] = line[j];
            for (size_t j = 0; j < lenSecond; ++j) result.second[j] = line[i+1+j];

            return result;
        }
    }

    return result;
}

/** Free memory allocated for each pair in the vector, then delete vector. */
template <typename TSymbol>
void DictFileReader<TSymbol>::freePairs(vector<WordPair<TSymbol> >* pairs) {
    typename vector<WordPair<TSymbol> >::iterator it;
    for (it = pairs->begin(); it != pairs->end(); ++it) {
        delete [] it->first;
        delete [] it->second;
    }

    delete pairs;
}


#endif	/* DICTFILEREADER_H */

