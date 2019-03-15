#ifndef ICOMPRESSOR_H
#define	ICOMPRESSOR_H

#include <string>

#include "util/WordFileReader.h"
#include "dictionary/util/WordList.h"
#include "dictionary/char_trie/Trie.h"
#include "util/etimer.h"

using namespace std;

/** Interface for a family of algorithms (strategy)
 * that compress NodeArray's. */
template <typename TNodeArray>
class ICompressor {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

public:

    //TODO pogledaj konstruktore kopije
    //ICompressor(const ICompressor& orig);
    virtual ~ICompressor() {};
    
    virtual void compressArray(TNodeArray& array) = 0;

    virtual TNodeArray* compressWords(string fileName);

    // short algorithm description
    virtual string description() = 0;

private:

};

/** Create NodeArray that stores the set of words in the
 * lexicographically sorted file. */
template <typename TNodeArray>
TNodeArray* ICompressor<TNodeArray>::compressWords(string fileName) {
        startEvent("reading_words_from_file");
        WordFileReader<TSymbol> reader(fileName);
        WordList<TSymbol>* words = reader.getWords();
        endEvent("reading_words_from_file");
        // construct trie
        startEvent("building_trie");
        Trie<TNodeArray>* trie = new Trie<TNodeArray>(0);
        for (size_t i = 0; i < words->numberOfWords(); ++i)
            trie->insertWord((*words)[i]);
        endEvent("building_trie");
        
        delete words;
        TNodeArray* nodes = &(trie->exportNodeArray());
        delete trie;
        // compress trie        
        this->compressArray(*nodes);

        return nodes;
}

#endif	/* ICOMPRESSOR_H */

