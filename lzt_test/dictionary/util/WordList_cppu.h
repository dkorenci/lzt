#ifndef WORDLIST_CPPU_H
#define	WORDLIST_CPPU_H

#include <cppunit/extensions/HelperMacros.h>

#include "dictionary/util/test/WordListTest.h"

class WordList_cppu : public CPPUNIT_NS::TestFixture, public WordListTest {
public:

    CPPUNIT_TEST_SUITE(WordList_cppu);

    CPPUNIT_TEST(testEquals);
    CPPUNIT_TEST(testSort);

    CPPUNIT_TEST_SUITE_END();

};

#endif	/* WORDLIST_CPPU_H */

