#ifndef VISUALISATIONRUNNER_HPP
#define	VISUALISATIONRUNNER_HPP

#include <string>

#include "util/WordFileReader.h"
#include "util/factory.h"
#include "dictionary/char_trie/Trie.h"
#include "dictionary/util/WordList.h"
#include "node_array/vector_array/VectorArray.h"
#include "TrieToStringProjector.hpp"
#include "NodeArrayStringWrapper.hpp"
#include "compress/ICompressor.h"
#include "compress/CompressorFactory.hpp"

using namespace std;

/** Configures and runs compressor to visualise given input file.  */
template <typename TSymbol, typename TIndex>
class VisualisationRunner {

public:

    static void visualize(string dict, string algo);

};

/** Read input from dict file, prepare data, configure and run compressor
 * with visualisation of the compression.
 * algo is a string with compressor options, as described in CompressorFactory,
 * 'v' (visualization) flag should not be specified, it will be added automaticaly. */
template <typename TSymbol, typename TIndex>
void VisualisationRunner<TSymbol, TIndex>::visualize(string dict, string algo) {
    // read words
    WordFileReader<TSymbol> reader(dict);
    WordList<TSymbol>* words = reader.getWords();
    // create node array
    VectorArray<TSymbol, TIndex>* array = getArray<VectorArray<TSymbol, TIndex> >(*words);
    delete words;
    // create string from array
    TSymbol* str = TrieToStringProjector<VectorArray<TSymbol, TIndex> >::projectTrie(*array);
    delete array;

    // create wrapper
    typedef NodeArrayStringWrapper<TSymbol, TIndex> TWrapper;
    TWrapper wrapper(str);
    // create and run compressor
    ICompressor<TWrapper>* compr = CompressorFactory<TWrapper>::createFromDesc(algo+"v");
    compr->compressArray(wrapper);
}


#endif	/* VISUALISATIONRUNNER_HPP */

