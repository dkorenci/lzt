#ifndef LZTRIEDICTTEST_H
#define	LZTRIEDICTTEST_H

#include "../LzTrieDict.h"
#include "../LzTrieDictCreator.h"

#include <vector>
#include <map>
#include <set>
#include <cstddef>
#include <sstream>
#include <algorithm>

#include "util/DictFileReader.h"
#include "dictionary/lztrie_dict/WordPair.h"
#include "util/WordComparator.h"
#include "util/utils.h"
#include "debug/lzt_test.h"
#include "util/TestCaseReader.h"
#include "util/TempFile.h"

using namespace std;

class LzTrieDictTest {

public:
    virtual ~LzTrieDictTest();

    void testDictionary();
    void testSerialization();

};


template <typename TSymbol, typename TIndex>
class LzTrieDictTester {
public:

    LzTrieDictTester(bool ds): deser(ds) { }

    void test() {
        testDictionary("dictionary/lztrie_dict/utest/lzDictSmallCases.txt");
        testDictionary("dictionary/lztrie_dict/utest/lzDictLargeCases.txt");
    }

private:

    typedef LzTrieDict<TSymbol, TIndex> TDict;
    typedef map<TSymbol* , vector<TSymbol*>, WordComparator<TSymbol> > TRawDict;
    typedef vector<WordPair<TSymbol> > TPairs;

    typedef typename 
    map<TSymbol* , vector<TSymbol*>, WordComparator<TSymbol> >::iterator TMapIter;

    typedef typename vector<WordPair<TSymbol> >::iterator TVectorIter;    

    // weather or not dictionary will be serialized and deserialized upon creation
    bool deser;

    void testDictionary(string fileName);
    void testMapping(TPairs* pairs, TDict* dict, TRawDict* rawDict, string context);
    void testWordMiss(TDict* dict, WordList<TSymbol>* words, string context);
    void testPrefixFind(TDict* dict, TRawDict* rawDict, WordList<TSymbol>* prefixes, string context);

    LzTrieDict<TSymbol, TIndex>*
        createDictionary(vector<WordPair<TSymbol> >* pairs, size_t segSize);
    TRawDict* createRawDictionary(TPairs* pairs);
    TDict* reserialize(TDict* dict);
    vector<WordPair<TSymbol> > findByPrefixRaw(TRawDict* dict, const TSymbol* prefix);

    vector<WordPair<TSymbol> >* splitWords(WordList<TSymbol>* words, TSymbol sep);

};

/** For each test case in the file, read and preprocess test data and run tests. */
template <typename TSymbol, typename TIndex>
void LzTrieDictTester<TSymbol, TIndex>::testDictionary(string fileName) {
    TestCaseReader<TSymbol> reader(fileName, 5);
    bool testRead = false;
    int caseCounter = 1;
    while (reader.readData()) {
        testRead = true;
        // dictionary words
        WordList<TSymbol>* words = reader.getWords(0);
        // separator
        //TODO not generic, only for TSymbol == char
        TSymbol sep = reader.getWord(1, 0)[0];
        
        vector<WordPair<TSymbol> >* pairs = splitWords(words, sep);
        delete words;
        // raw dictionary for comparison
        TRawDict* rawDict = createRawDictionary(pairs);
        // list of segment sizes
        WordList<TSymbol>* segs = reader.getWords(2);

        WordList<TSymbol>* misses = reader.getWords(3);
        WordList<TSymbol>* prefixes = reader.getWords(4);

        for (int i = 0; i < segs->numberOfWords(); ++i) {
            // convert segment from string
            istringstream iss((*segs)[i]);
            size_t segSize; iss >> segSize;

            // tested dict description for assert error messages
            ostringstream context;
            context << "test file: " << fileName << endl
                    << "test case#: " << caseCounter << endl
                    << "segment size: " << segSize << endl;

            TDict* lzDict = createDictionary(pairs, segSize);

            testMapping(pairs, lzDict, rawDict, context.str());
            testWordMiss(lzDict, misses, context.str());
            testPrefixFind(lzDict, rawDict, prefixes, context.str());

            delete lzDict;
        }

        delete rawDict; delete segs; delete misses; delete prefixes;

        DictFileReader<TSymbol>::freePairs(pairs);

        caseCounter++;
    }
    TEST_ASSERT(testRead);
}

/** Assert equality of two dictionaries. For each left word, resulting
 * sequence of words for both dictionaries has to be the same. This means
 * words have to be equal and in the same order. */
template <typename TSymbol, typename TIndex>
void LzTrieDictTester<TSymbol, TIndex>::
testMapping(TPairs* pairs, TDict* dict, TRawDict* rawDict, string context) {
    // set used to assert only once for each word
    set<TSymbol*, WordComparator<TSymbol> > checkedList;
    for (TVectorIter it = pairs->begin(); it != pairs->end(); ++it)
    if (checkedList.find(it->first) == checkedList.end()) {
        const vector<TSymbol*>& rawResult = (*rawDict)[it->first];
        vector<TSymbol*> lzResult = dict->lookupWord(it->first);

        // print results
        ostringstream ms; ms << context << endl;
        ms << "first word: " << it->first << endl;
        ms << "raw mapping: ";
        typename vector<TSymbol*>::const_iterator jt;
        for (jt = rawResult.begin(); jt != rawResult.end(); ++jt) ms << *jt << ", ";
        ms << endl << "lz mapping: ";
        for (jt = lzResult.begin(); jt != lzResult.end(); ++jt) ms << *jt << ", ";
        ms << endl;

        // assert equality
        TEST_ASSERT_MESSAGE(rawResult.size() == lzResult.size(), ms.str());
        for (size_t i = 0; i < rawResult.size(); ++i) {
            TEST_ASSERT_MESSAGE(wordsEqual(rawResult[i], lzResult[i]), ms.str());
        }

        // first word is checked, add it to list
        checkedList.insert(it->first);
    }
}

/** Assert that none of the words in the misses list is in the dictionary. */
template <typename TSymbol, typename TIndex>
void LzTrieDictTester<TSymbol, TIndex>::
testWordMiss(TDict* dict, WordList<TSymbol>* misses, string context) {
    for (size_t i = 0; i < misses->numberOfWords(); ++i) {
        const TSymbol* word = (*misses)[i];
        vector<TSymbol*> result = dict->lookupWord(word);

        ostringstream m;
        m << "missed word: " << word << endl << "result: " << endl;
        for (int i = 0; i < result.size(); ++i) m << result[i] << endl;

        TEST_ASSERT_MESSAGE(result.size() == 0, m.str());
    }
}

template <typename TSymbol, typename TIndex>
void LzTrieDictTester<TSymbol, TIndex>::
testPrefixFind(TDict* dict, TRawDict* rawDict, WordList<TSymbol>* prefixes, string context) {
    for (int i = 0; i < prefixes->numberOfWords(); ++i) {
        const TSymbol* prefix = (*prefixes)[i];
        // get lists of pairs whose first member is prefixed by prefix
        vector<WordPair<TSymbol> > dictPref = dict->findByPrefix(prefix);
        vector<WordPair<TSymbol> > rawPref = findByPrefixRaw(rawDict, prefix);

        // sort vectors
        WordPairCompareBoth<TSymbol> comp;
        sort(dictPref.begin(), dictPref.end(), comp);
        sort(rawPref.begin(), rawPref.end(), comp);

        // write results to message
        ostringstream m;
        m << "prefix: " << prefix << endl;
        m << "lzDict result: " << endl;
        for (size_t i = 0; i < dictPref.size(); ++i)
            m << dictPref[i].first << "," << dictPref[i].second << endl;
        m << "rawDict result: " << endl;
        for (size_t i = 0; i < rawPref.size(); ++i)
            m << rawPref[i].first << "," << rawPref[i].second << endl;

        TEST_ASSERT_MESSAGE(dictPref.size() == rawPref.size(), context + m.str());

        // assert pair by pair equality
        WordPairsEqual<TSymbol> eqComp;
        for (size_t i = 0; i < dictPref.size(); ++i)
        if (eqComp(dictPref[i], rawPref[i]) == false) {
            m << "unequality index: " << i << endl
              << dictPref[i].first << "," << dictPref[i].second << endl
              << rawPref[i].first << "," << rawPref[i].second << endl;

            TEST_ASSERT_MESSAGE(false, context + m.str());
        }               
    }
}

/** Find all pairs of words in the raw dict for wich first word has given prefix. */
template <typename TSymbol, typename TIndex>
vector<WordPair<TSymbol> > LzTrieDictTester<TSymbol, TIndex>::
findByPrefixRaw(TRawDict* dict, const TSymbol* prefix) {
    vector<WordPair<TSymbol> > result;
    
    for (TMapIter it = dict->begin(); it != dict->end(); ++it)
        // first word matches prefix
        if (isPrefix(prefix, it->first)) {
            // add to result the pairs: first word x associated words
            // get associated words
            vector<TSymbol*>& words = it->second;
            // add pairs
            for (int i = 0; i < words.size(); ++i) {
                WordPair<TSymbol> pair;
                pair.first = it->first; pair.second = words[i];
                result.push_back(pair);
            }        
        }

    return result;
}

/** Create LzTrieDict from pairs of words, reserialize if flag is set and return. */
template <typename TSymbol, typename TIndex>
typename LzTrieDictTester<TSymbol, TIndex>::TDict* LzTrieDictTester<TSymbol, TIndex>::
createDictionary(vector<WordPair<TSymbol> >* pairs, size_t segSize) {
    LzTrieDict<TSymbol, TIndex>* lztrieDict = new LzTrieDict<TSymbol, TIndex>;

    LzTrieDictCreator<TSymbol, TIndex> creator;
    creator.createDictionary(lztrieDict, pairs, segSize, false);
    if (deser) lztrieDict = reserialize(lztrieDict);

    return lztrieDict;
}

/** Create raw dictionary (a mapping of words to vectors of words) from pairs
 * of words. Pointers are shallow copied from pairs to the map. */
template <typename TSymbol, typename TIndex>
typename LzTrieDictTester<TSymbol, TIndex>::TRawDict* LzTrieDictTester<TSymbol, TIndex>::
createRawDictionary(vector<WordPair<TSymbol> >* pairs) {
    TRawDict* rawDict = new TRawDict;

    /* Fill raw dict with first words in pairs associated to all
     * second words that occur in a same pair with it. */
    for (TVectorIter it = pairs->begin(); it != pairs->end(); ++it) {
        TMapIter mit = rawDict->find(it->first);
        // word not yet in the dict, insert new list of associated words
        if (mit == rawDict->end()) {
            vector<TSymbol*> assoc;
            assoc.push_back(it->second);
            (*rawDict)[it->first] = assoc;
        }
        else {
            // add second word to the list of associated words
            (*rawDict)[it->first].push_back(it->second);
        }
    }

    return rawDict;
}

/* Split each word by sep symbol and create array of pairs. Memory for words of
 * pairs is allocated. Each word must contain a sep surrounded by other symbols. */
template <typename TSymbol, typename TIndex>
vector<WordPair<TSymbol> >* LzTrieDictTester<TSymbol, TIndex>::
splitWords(WordList<TSymbol>* words, TSymbol sep) {
    vector<WordPair<TSymbol> >* pairs = new vector<WordPair<TSymbol> >;
    for (size_t i = 0; i < words->numberOfWords(); ++i) {
        const TSymbol *word = (*words)[i];
        pairs->push_back( DictFileReader<TSymbol>::splitLine(word, sep) );
    }
    return pairs;
}

/** Serialize dict to file, free memory, deserialize and return. */
template <typename TSymbol, typename TIndex>
typename LzTrieDictTester<TSymbol, TIndex>::TDict*
LzTrieDictTester<TSymbol, TIndex>::
reserialize(LzTrieDictTester<TSymbol, TIndex>::TDict* dict) {
    TempFile file;
    // serialize
    fstream stream(file.getName());
    dict->writeToStream(stream);
    stream.close();
    // deserialize
    delete dict;
    dict = new LzTrieDict<TSymbol, TIndex>();
    stream.open(file.getName());
    dict->readFromStream(stream);
    stream.close();

    return dict;
}

#endif	/* LZTRIEDICTTEST_H */

