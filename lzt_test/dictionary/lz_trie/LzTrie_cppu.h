#ifndef LZTRIE_CPPU_H
#define	LZTRIE_CPPU_H

#include <cppunit/extensions/HelperMacros.h>

#include "dictionary/lz_trie/test/LzTrieTest.h"
#include "dictionary/lz_trie/test/LzTrieIteratorTest.h"

class LzTrie_cppu : public CPPUNIT_NS::TestFixture, public LzTrieTest,
public LzTrieIteratorTest {
public:
    
    CPPUNIT_TEST_SUITE(LzTrie_cppu);

    CPPUNIT_TEST(LzTrieTest::testListCases);
    CPPUNIT_TEST(LzTrieTest::testListLarge);

    CPPUNIT_TEST(LzTrieIteratorTest::test);

    CPPUNIT_TEST_SUITE_END();

private:

};

#endif	/* LZTRIE_CPPU_H */

