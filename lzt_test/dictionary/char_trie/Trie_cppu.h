#ifndef TRIE_CPPU_H
#define	TRIE_CPPU_H

#include <cppunit/extensions/HelperMacros.h>

#include "dictionary/char_trie/test/TrieTest.h"
#include "dictionary/char_trie/test/WordIndexerTest.h"

class Trie_cppu : public CPPUNIT_NS::TestFixture, public TrieTest, public WordIndexerTest {
public:
    CPPUNIT_TEST_SUITE(Trie_cppu);

    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testInsertBranch);
    CPPUNIT_TEST(testInsertCow);
    CPPUNIT_TEST(testInsertEow);
    CPPUNIT_TEST(testInsertRandom);
    
    //EnumTrie tests
    CPPUNIT_TEST(WordIndexerTest::testWordCounting);
    CPPUNIT_TEST(WordIndexerTest::testGetWord);
    CPPUNIT_TEST(WordIndexerTest::testGetIndex);
    CPPUNIT_TEST(WordIndexerTest::testWithDictionary);

    CPPUNIT_TEST_SUITE_END();

private:

};

#endif	/* TRIE_CPPU_H */

