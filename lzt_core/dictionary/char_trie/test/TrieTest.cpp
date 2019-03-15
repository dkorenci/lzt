#include "debug/lzt_test.h"

#include "TrieTest.h"

void TrieTest::testInsert() {
    Trie<TNodeArray> trie;

    TEST_ASSERT( trie.isEmpty() );
    const int NWORDS = 11;
    const char *words[NWORDS] = 
    {"a", "ab", "abc", "aba", "abad", "aca", "ba", "baa", "bab", "bb", "c"};
    for (int i = 0; i < NWORDS; ++i) {
        TEST_ASSERT( trie.insert(words[i], sstringLength<char, int>(words[i])) );
        TEST_ASSERT( trie.containsWord(words[i] ));
    }
}

void TrieTest::testInsertBranch() {
    TrieTester<TNodeArray> tester;
    Trie<TNodeArray> trie;
    tester.testInsertBranchGen(&trie, 'a', 1000);
}

void TrieTest::testInsertCow() {
    TrieTester<TNodeArray> tester;
    Trie<TNodeArray> trie;
    tester.testInsertCowGen(&trie, 'a', 1000);
}

void TrieTest::testInsertEow() {
    TrieTester<TNodeArray> tester;
    Trie<TNodeArray> trie;
    tester.testInsertEowGen(&trie, 'a', 1000);
}

void TrieTest::testInsertRandom() {
    TrieTester<TNodeArray> tester;
    Trie<TNodeArray> trie;
    tester.testInsertRandomGen(&trie, "abcde", 5, 10, 20000);
}
