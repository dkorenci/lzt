#ifndef SEARCHCOMPRESSEDTEST_H
#define	SEARCHCOMPRESSEDTEST_H

#include <cppunit/extensions/HelperMacros.h>

class SearchCompressedTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(SearchCompressedTest);

    CPPUNIT_TEST(testContains);

    CPPUNIT_TEST_SUITE_END();

public:
    SearchCompressedTest();
    virtual ~SearchCompressedTest();
    void setUp();
    void tearDown();

    void testContains();

private:
    char *mutation;
    static const int MSIZE = 20000;

    void createMutation(const char *word, const char *alphabet);

};

#endif	/* SEARCHCOMPRESSEDTEST_H */

