#ifndef LZTRIEDICT_CPPU_H
#define	LZTRIEDICT_CPPU_H

#include <cppunit/extensions/HelperMacros.h>

#include "dictionary/lztrie_dict/utest/HuffmanIndexMapTest.h"
#include "dictionary/lztrie_dict/huffman/utest/HuffmanTest.h"
#include "dictionary/lztrie_dict/utest/LzTrieDictTest.h"


class LzTrieDict_cppu : public CPPUNIT_NS::TestFixture, 
        public HuffmanIndexMapTest, public HuffmanTest, public LzTrieDictTest {
public:

    CPPUNIT_TEST_SUITE(LzTrieDict_cppu);

    CPPUNIT_TEST(HuffmanTest::testCoderCreation);
    CPPUNIT_TEST(HuffmanTest::testDecoderCreation);
    CPPUNIT_TEST(HuffmanTest::testCodingAndDecoding);
    CPPUNIT_TEST(HuffmanTest::testDecoderSerialization);

    CPPUNIT_TEST(HuffmanIndexMapTest::testMap);
    CPPUNIT_TEST(HuffmanIndexMapTest::testSerialization);

    CPPUNIT_TEST(LzTrieDictTest::testDictionary);
    CPPUNIT_TEST(LzTrieDictTest::testSerialization);

    CPPUNIT_TEST_SUITE_END();

};

#endif	/* LZTRIEDICT_CPPU_H */

