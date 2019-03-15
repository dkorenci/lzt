#include "FileReaderTest.h"

FileReaderTest::~FileReaderTest() {
}

void FileReaderTest::testDictFileReader() {
    testDictReader<char>();
}

void FileReaderTest::testWordFileReader() {
    // this should be changed depending on the initial current folder settings
    WordFileReader<char> fileReader("util/utest/words.txt");
    fileReader.readWords();

    int noOfWords = 10;

 //   for (int i = 0; i < noOfWords; ++i) cout<<fileReader.getWord(i);

    TEST_ASSERT(fileReader.getNumberOfWords() == noOfWords);

    const char * words[] = { "ab", "cd ef", "gh ij", "klmnopqr",
                "stuvz", "abggh", "test jos jedan", "idemo dalje",
                "i tu je kraj", "kraj" };

    for (int i = 0; i < noOfWords; ++i)
        TEST_ASSERT_MESSAGE( strcmp(fileReader.getWord(i), words[i]) == 0, fileReader.getWord(i) );


}
