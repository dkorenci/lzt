#ifndef TRIETEST_H
#define	TRIETEST_H

#include <set>

#include "debug/lzt_test.h"
#include "dictionary/char_trie/Trie.h"
#include "node_array/vector_array/VectorArray.h"

class TrieTest {
private:
    typedef VectorArray<char, int> TNodeArray;
    
public:
    virtual ~TrieTest() {}

    //TODO add test create

    void testInsert();
    void testInsertBranch();
    void testInsertCow();
    void testInsertEow();
    void testInsertRandom();

};

template <typename TNodeArray>
class TrieTester {
public:

    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

    virtual ~TrieTester() {};

    //TODO pretvori TEST_ASSERT-e u TEST_ASSERT_MESSAGE-e

    void testInsertBranchGen(Trie<TNodeArray> *trie, const TSymbol CHAR, const TIndex MAX_SIZE);
    void testInsertCowGen(Trie<TNodeArray> *trie, const TSymbol CHAR, const TIndex MAX_SIZE);
    void testInsertEowGen(Trie<TNodeArray> *trie, const TSymbol CHAR, const TIndex MAX_SIZE);
    void testInsertRandomGen(Trie<TNodeArray> *trie, const TSymbol * ALPHA,
         const int ALPHA_SIZE, const TIndex MAX_SIZE, const TIndex NUM_STRINGS);

    //utility method
    static Trie<TNodeArray>* createTrie(TIndex size = 0);
    void deleteTrie(Trie<TNodeArray> * trie);
    void printWord(vector<TSymbol> word);
    TSymbol* vectorToPointer(vector<TSymbol> word);

private:

};

template<typename TNodeArray>
Trie<TNodeArray> * TrieTester<TNodeArray>::createTrie(TIndex size) {
    return new Trie<TNodeArray>(size);
}

template<typename TNodeArray>
void TrieTester<TNodeArray>::deleteTrie(Trie<TNodeArray> * trie) {
    delete trie;
}

template<typename TNodeArray>
void TrieTester<TNodeArray>::printWord(vector<TSymbol> word) {
    cout<<"|";
    typename vector<TSymbol>::iterator it;
    for (it = word.begin(); it != word.end(); ++it) {
        cout<<*it;
    }
    cout<<"\n";
}

// helper function for easy viewing vectors of symbols while debugging
template<typename TNodeArray>
typename TNodeArray::Symbol* TrieTester<TNodeArray>::vectorToPointer(vector<TSymbol> word) {
    TSymbol * s = new char [word.size()+1];
    int i;
    for (i = 0; i < word.size(); ++i) s[i] = word[i];
    s[i] = 0;
    return s;
}

/* insert many string that branchoff at each insert */
template<typename TNodeArray>
void TrieTester<TNodeArray>::testInsertBranchGen(Trie<TNodeArray> *trie, const TSymbol CHAR, const TIndex MAX_SIZE) {
    TSymbol * word = new TSymbol [MAX_SIZE + 1];
    //mark end of word
    word[MAX_SIZE] = zeroSymbol<TSymbol>();

    for (TIndex i = 0; i < MAX_SIZE; ++i) word[i] = CHAR;

    TEST_ASSERT( trie->insert(word, MAX_SIZE) );

    for (TIndex i = 0; i < MAX_SIZE; ++i) {
        word[i] = CHAR + 1;
        TEST_ASSERT( trie->insert(word, MAX_SIZE) );
    }

    for (TIndex i = 0; i < MAX_SIZE; ++i) word[i] = CHAR;

    TEST_ASSERT( trie->containsWord(word) );

    for (TIndex i = 0; i < MAX_SIZE; ++i) {
        word[i] = CHAR + 1;
        TEST_ASSERT( trie->containsWord(word) );
    }

    delete [] word;
}

/* insert sequence of string where previous is prefix of the next */
template<typename TNodeArray>
void TrieTester<TNodeArray>::testInsertCowGen(Trie<TNodeArray> *trie, const TSymbol CHAR, const TIndex MAX_SIZE) {
    TSymbol * word = new TSymbol [MAX_SIZE + 1];
    for (TIndex i = 0; i < MAX_SIZE; ++i) word[i] = CHAR;

    for (TIndex i = 0; i < MAX_SIZE; ++i) {
        TEST_ASSERT( trie->insert(word, i+1) );
    }

    for (TIndex i = 0; i < MAX_SIZE; ++i) {
        TSymbol s = word[i+1];
        word[i+1] = zeroSymbol<TSymbol>();
        TEST_ASSERT( trie->containsWord(word) );
        word[i+1] = s;
    }

    delete [] word;
}

/* Insert string that are prefixes of and existing strings, so only
 * eow flags have to be set. */
template<typename TNodeArray>
void TrieTester<TNodeArray>::testInsertEowGen(Trie<TNodeArray> *trie, const TSymbol CHAR, const TIndex MAX_SIZE) {
    TSymbol * word = new TSymbol [MAX_SIZE + 1];
    for (TIndex i = 0; i < MAX_SIZE; ++i) word[i] = CHAR;

    TEST_ASSERT( trie->insert(word, MAX_SIZE) );
    for (TIndex i = 0; i < MAX_SIZE-1; ++i) {
        TEST_ASSERT( trie->insert(word, i+1) );
    }

    word[MAX_SIZE] = zeroSymbol<TSymbol>();
    TEST_ASSERT( trie->containsWord(word) );

    for (TIndex i = 0; i < MAX_SIZE-1; ++i) {
        TSymbol s = word[i+1];
        word[i+1] = zeroSymbol<TSymbol>();
        TEST_ASSERT( trie->containsWord(word) );
        word[i+1] = s;
    }

    delete [] word;
}

/* Inserts number of randomly created string from the given alphabet. */
//TODO prije pocetka testa ispisi sve rijeci u file,
//i ako assert pukne, ispis rijec na kojoj je puknu
template<typename TNodeArray>
void TrieTester<TNodeArray>::testInsertRandomGen(Trie<TNodeArray> *trie, const TSymbol * ALPHA,
    const int ALPHA_SIZE, const TIndex MAX_SIZE, const TIndex NUM_STRINGS)
{
    typedef vector<TSymbol> Tword;
    set<Tword> word_set;

    srand(time(0));

    for (TIndex i = 0; i < NUM_STRINGS; ++i) {
        TIndex stringSize;
        do {
            stringSize =  ( ((double)rand()) / ((double)RAND_MAX) ) * (MAX_SIZE - 1);
        } while (stringSize == 0);

        Tword word;
        do {
            for (TIndex j = 0; j < stringSize; ++j) {
                int randSymbol =  ( ((double)rand()) / ((double)RAND_MAX) ) * (ALPHA_SIZE - 1);
                word.push_back( ALPHA[randSymbol] );
            }
        } while (word_set.find(word) != word_set.end());

        word_set.insert(word);
       // printWord(word);
    }

    TEST_ASSERT(word_set.size() == NUM_STRINGS);

    typename set<Tword>::iterator it;

//    for (it = word_set.begin(); it != word_set.end(); ++it)
//        printWord(*it);

    for (it = word_set.begin(); it != word_set.end(); ++it) {
        TEST_ASSERT( trie->insert((*it).begin(), (*it).size()) );
    }

    TSymbol w[MAX_SIZE + 1];
    for (it = word_set.begin(); it != word_set.end(); ++it) {
        //TODO promijeni da cijeli set bude sa char*
        TIndex i;
        for (i = 0; i < (*it).size(); ++i) w[i] = (*it)[i];
        w[i] = zeroSymbol<TSymbol>();
        TEST_ASSERT( trie->containsWord(w) );
    }
}



#endif	/* TRIETEST_H */

