#include "WordIndexerTest.h"

void WordIndexerTest::testWordCounting() {
    EnumTrieTester<TNodeArray, TIndex> tester;
    tester.testWordCounting();
}

void WordIndexerTest::testGetWord() {
    EnumTrieTester<TNodeArray, TIndex> tester;
    tester.testGetWord();

    EnumTrieTester<TNodeArray, LzTrieIterator<TNodeArray> > lztester;
    lztester.testGetWord();
}

void WordIndexerTest::testGetIndex() {
    EnumTrieTester<TNodeArray, TIndex> tester;
    tester.testGetIndex();

    EnumTrieTester<TNodeArray, LzTrieIterator<TNodeArray> > lztester;
    lztester.testGetIndex();
}

void WordIndexerTest::testWithDictionary() {
    EnumTrieTester<TNodeArray, TIndex> tester;
    tester.testWithDictionary();

    EnumTrieTester<TNodeArray, LzTrieIterator<TNodeArray> > lztester;
    lztester.testWithDictionary();
}