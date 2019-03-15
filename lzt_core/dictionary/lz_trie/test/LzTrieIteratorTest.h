#ifndef LZTRIEITERATORTEST_H
#define	LZTRIEITERATORTEST_H

#include <string>
#include <sstream>

#include "../LzTrieIterator.h"

#include "util/factory.h"
#include "util/TestCaseReader.h"
#include "dictionary/util/WordList.h"
#include "debug/lzt_test.h"
#include "node_array/vector_array/VectorArray.h"
#include "node_array/na_utils.h"

using namespace std;

class LzTrieIteratorTest {
public:

    virtual ~LzTrieIteratorTest();
    void test();

private:

};

template <typename TNodeArray>
class LzTrieIterTester {

private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::NodeConst TNodeConst;

    typedef LzTrieIterator<TNodeArray> TIter;
    // separator of array node string representations, when building string from array
    static const char separator = '|';

public:

    void test();
    
private:

    void testDfs(string fname);

    string nodeToString(TNodeConst node);

    template <typename TIterator>
    string getDfsString(const TNodeArray& array);

    template <typename TIterator>
    string dfs(TIterator node, const TNodeArray& array);

};

template <typename TNodeArray>
void LzTrieIterTester<TNodeArray>::test() {
    testDfs("dictionary/lz_trie/test/iterSmallCases.txt");
    testDfs("dictionary/lz_trie/test/iterLargeCases.txt");
}

/** Test by creating lz-compressed node array from words in the file,
 * and asserting that node sequence obtained by dfs on lz-array using lz-iterator
 * is equal to the sequence from dfs on non-compressed array.  */
template <typename TNodeArray>
void LzTrieIterTester<TNodeArray>::testDfs(string fname) {
    TestCaseReader<TSymbol> reader(fname, 1);
    bool testRead = false;
    int caseCounter = 1;
    while(reader.readData()) {
        testRead = true;
        WordList<TSymbol>* words = reader.getWords(0);
        TNodeArray* array = getArray<TNodeArray>(*words);
        TNodeArray* lzArray = getLzArray<TNodeArray>(*words);

        delete words;

        string arrayStr = getDfsString<TIndex>(*array);
        string arrayStrLz = getDfsString< LzTrieIterator<TNodeArray> >(*lzArray);

        ostringstream ss; 
        ss << fname << endl << "test case: " << caseCounter
           << endl << arrayStr << endl << arrayStrLz;

        TEST_ASSERT_MESSAGE(arrayStr == arrayStrLz, ss.str());

        caseCounter++;
    }
    // at least one test must be processed per file
    TEST_ASSERT(testRead);
}

/** Return string representation of an node that is equal for corresponding
 * nodes in lz-compressed and uncompressed arrays. */
template <typename TNodeArray>
string LzTrieIterTester<TNodeArray>::nodeToString(TNodeConst node) {
    ostringstream ss;
    ss << node.getSymbol() << node.getEow() << node.getCow();
    return ss.str();
}

/** Create string representation of a trie that contains string representations
 * of nodes from dfs traversal that is done using node iterators. */
template <typename TNodeArray> template <typename TIterator>
string LzTrieIterTester<TNodeArray>::getDfsString(const TNodeArray& array) {
    string result; result += separator;
    TIterator node = IterInit<TIterator, TNodeArray>::get(array);
    // do dfs on all first level nodes
    while (true) {
        result += dfs(node, array);
        if (array[node].getSibling() != 0) node += array[node].getSibling();
        else break;
    }

    return result;
}

/** Do dfs on a trie creating a string from traversed nodes. */
template <typename TNodeArray> template <typename TIterator>
string LzTrieIterTester<TNodeArray>::dfs(TIterator node, const TNodeArray& array) {
    string result = nodeToString(array[node]) + separator;
    // visit children, if any
    if (array[node].getCow()) {
        // move to first child
        ++node;
        while (true) {
            result += dfs(node, array);
            if (array[node].getSibling() != 0) node += array[node].getSibling();
            else break;
        }
    }
    
    return result;
}

#endif	/* LZTRIEITERATORTEST_H */

