#ifndef TESTCASEREADER_H
#define	TESTCASEREADER_H

#include <fstream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstddef>
#include <climits>

#include "dictionary/util/WordList.h"

using namespace std;

/** Reads test cases from a text file.
 * Each case consist of N test sets, where N is a file parameter.
 * Each set start with a number M of words in the set followed by M
 * words, one word per line. Lines that start with // are ignored. */
template <typename TSymbol>
class TestCaseReader {

public:

    TestCaseReader(string fileName, size_t numSets)
    : stream(fileName.c_str()), NUM_SETS(numSets) {
        buff = new char [BUFF_LEN];
        setSizes.resize(NUM_SETS);
    }

    virtual ~TestCaseReader() {
        freeSets();
        delete [] buff;
    }

    size_t getNumSets() const { return NUM_SETS; }
    size_t getNumWords(size_t set) const { return setSizes[set]; }
    const TSymbol* getWord(size_t set, size_t word) const { return sets[set][word]; }
    WordList<TSymbol>* getWords(size_t set) const { return listFromVector(sets[set]); }

    /** Read one set of test data from the stream.
     * @return true if a complete data set was read without errors
     */
    bool readData() {
        freeSets();
        sets.resize(NUM_SETS);

        for (size_t i = 0; i < NUM_SETS; ++i) {
            size_t numWords;
            if ((numWords = readNumber()) == NULL_NUMBER) {
                freeSets();
                return false;
            }

            if (readWords(numWords, sets[i]) == false) {
                freeSets();
                return false;
            }

            setSizes[i] = numWords;
        }

        return true;
        
    }

private:
    static const int BUFF_LEN = 20000;
    static const size_t NULL_NUMBER = ULONG_MAX;
    static const char *commentPrefix;
    static const int cpLen = 2;
    char *buff;
    
    // number of sets per test case
    const size_t NUM_SETS;

    ifstream stream;
    
    // sets of the test case, each with a list of words
    vector<vector<TSymbol *> > sets;
    // vector with number of words per set
    vector<size_t> setSizes;

    // clear sets vectors
    void freeSets() {
        for (size_t i = 0; i < sets.size(); ++i) {
            // free words in the set
            for (size_t j = 0; j < sets[i].size(); ++j)
                delete [] sets[i][j];

            sets[i].clear();
        }
        sets.clear();
    }

    WordList<TSymbol>* listFromVector(const vector<TSymbol*>& v) const {
        WordList<TSymbol>* list = new WordList<TSymbol>();
        //TODO make this deep copying of words
        typename vector<TSymbol*>::const_iterator it;
        for (it = v.begin(); it != v.end(); ++it)
            list->addWord(*it);
        return list;
    }

    /** Read N words from stream and put them in words vector.
     * Return true if all the words are read correctly. */
    bool readWords(size_t N, vector<TSymbol *>& words) {
        for (int i = 0; i < N; ++i) {
            TSymbol* word = readLine();
            if (word == NULL) return false;
            words.push_back(word);
        }
        return true;
    }

    /** Return a number at the beginning of the first uncommented line, or
     * NULL_NUMBER if an error occured or nothing is read. */
    size_t readNumber() {
        char* line = readLineSkippingComments(stream, buff, BUFF_LEN);

        if (line == NULL) return NULL_NUMBER;

        /* no sstream because when buff id delete[]d and sstream connected
           to buff == line is freed after and that can cause mem corruption */
        size_t n = NULL_NUMBER;
        //TODO check if line is a positive integer
        sscanf(line, "%lu", &n);

        return n;
    }

    TSymbol* readLine() {
        char* line = readLineSkippingComments(stream, buff, BUFF_LEN);

        if (line == NULL) return NULL;
        //TODO this is not generic, partial specialization of readLine would be
        // convert chars to TSymbol's
        int lineLen = strlen(line);
        TSymbol *word = new TSymbol[lineLen+1];
        for (int j = 0; j < lineLen; ++j)
            word[j] = (TSymbol)line[j];
        word[lineLen] = zeroSymbol<TSymbol>();

        return word;
    }

    /** skip all lines that are comments, read and 
     * return first non-commented line. whitespaces are cleared from the
     * beginning of the line. return the line or NULL if error occurred or
     * nothing is read. */
    char* readLineSkippingComments(ifstream& istr, char* lineBuff, int buffLen) {
        while (true) {
            if (istr.good() == false) return NULL;

            istr >> ws;
            if (istr.good() == false) return NULL;

            istr.getline(lineBuff, buffLen);
            if (istr.good() == false && istr.eof() == false) return NULL;

            if (isComment(lineBuff) == false) break;
        }
        
        if (strlen(lineBuff) == 0) return NULL;

        return lineBuff;
    }

    bool isComment(const char* str) {
        int strLen = strlen(str);
        // check if the word just read is a comment, than don't save it
        if (strLen >= cpLen) {
            int j;
            for (j = 0; j < cpLen; ++j)
                if (commentPrefix[j] != str[j]) break;
            // match occured
            return (j == cpLen);
        }
        else return false;
    }

    void dealocateWords(vector<TSymbol *>& words) {
        typename vector<TSymbol *>::iterator it;
        for (it = words.begin(); it != words.end(); ++it)
            delete [] (*it);
    }

};

template <typename TSymbol> const char* TestCaseReader<TSymbol>::commentPrefix = "//";

#endif	/* TESTCASEREADER_H */

