#ifndef ENUMARRAYTEST_H
#define	ENUMARRAYTEST_H

#include <sstream>
#include <iostream>

#include "util/factory.h"
#include "dictionary/util/WordList.h"
#include "util/WordFileReader.h"
#include "debug/lzt_test.h"
#include "util/TempFile.h"
#include "serialization/array/CompactArraySerializer.h"
#include "node_array/compact_array/CompactArray.h"
#include "node_array/compact_array/CompactArrayCreator.h"

/** Test correct performance of algorithms with classes implementing
 * enumerated array concept. */
class EnumArrayTest {
public:
    virtual ~EnumArrayTest() {};

    void testCompression();
    void testSerialization();

private:

};

template <typename TNodeArray>
class EnumArrayTester {
public:
    
    EnumArrayTester(): numOfDicts(4), dictFolder("test_files/") {
        dicts = new const char *[numOfDicts];
        dicts[0] = "english.txt"; dicts[1] = "bugDict.txt";
        dicts[2] = "listTest.txt"; dicts[3] = "smallDict.txt";
    }

    virtual ~EnumArrayTester() {
        delete [] dicts;
    }

    void testCompression();
    void testSerialization();

private:
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Symbol TSymbol;

    const int numOfDicts;
    string dictFolder;
    const char** dicts;

    WordList<TSymbol>* getWords(int i) {
        WordFileReader<TSymbol> reader(dictFolder + dicts[i]);
        WordList<TSymbol>* words = reader.getWords();
        return words;
    }

    template <typename TNode>
    string enumNodeToString(TNode n);
};

template <typename TNodeArray>
void EnumArrayTester<TNodeArray>::testCompression() {
    for (int i = 0; i < numOfDicts; ++i) {
        WordList<TSymbol>* words = getWords(i);
        LzTrie<TNodeArray>* trie = getLzTrie<TNodeArray>(*words, true);
        WordList<TSymbol>* trieWords = trie->getWordsByPrefix("");

        stringstream m; m << "dict: " << dicts[i];        
        //TODO kada node postaje pointer, enumerated mora postati false
        TEST_ASSERT_MESSAGE(*words == *trieWords, m.str());

        delete words;
        delete trieWords;
    }
}

template <typename TNodeArray>
void EnumArrayTester<TNodeArray>::testSerialization() {
    for (int i = 0; i < numOfDicts; ++i) {
        WordList<TSymbol>* words = getWords(i);
        CompactArray<TSymbol, TIndex>* carray = getCompactedLzArray<TNodeArray>(*words, true);

        TempFile file;

        fstream stream(file.getName());
        CompactArraySerializer<TSymbol, TIndex> ser(carray);
        ser.arrayToStream(stream);
        stream.close();

        stream.open(file.getName());
        CompactArray<TSymbol, TIndex>* carrayDeser = ser.arrayFromStream(stream);
        stream.close();

        stringstream m; m << "array size: " << carray->getSize()
                            << " deser. array size: " << carrayDeser->getSize();
        TEST_ASSERT_MESSAGE(carray->getSize() == carrayDeser->getSize(), m.str());

        for (int j = 0; j < carray->getSize(); ++j) {
            stringstream m;
            m << "node index: " << j << endl
              << "node[j]: " << endl << enumNodeToString((*carray)[j]) << endl
              << "deser. node[j]: "<< endl << enumNodeToString((*carrayDeser)[j]) << endl;

            TEST_ASSERT_MESSAGE((*carray)[j] == (*carrayDeser)[j], m.str());
        }

        delete carray;
        delete carrayDeser;
    }
}

template <typename TNodeArray> template <typename TNode>
string EnumArrayTester<TNodeArray>::enumNodeToString(TNode n) {
    stringstream ss;
    ss << "symbol: " << n.getSymbol() << " cow: " << n.getCow() 
            << " eow: " << n.getEow() << endl << " sibling: " << n.getSibling()
            << " numOfWords: " << n.getNumWords();

    return ss.str();
}

#endif	/* ENUMARRAYTEST_H */

