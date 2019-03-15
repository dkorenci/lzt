#ifndef NODEARRAYTESTER_H
#define	NODEARRAYTESTER_H

#include "NodeArrayTest.h"

template <typename TSymbol, typename TIndex, typename TNodeArray>
class NodeArrayTester {

public:

    void runTests();

    virtual ~NodeArrayTester() {}

private:

    typedef typename TNodeArray::Node TNode;

    TIndex getSizedIndex(int size);
    void testConstructors();
    void testNodeAccess();

};

template <typename TSymbol, typename TIndex, typename TNodeArray>
void NodeArrayTester<TSymbol, TIndex, TNodeArray>::runTests() {
    testConstructors();
    testNodeAccess();
}

    /// create variable of TIndex type with given value

template <typename TSymbol, typename TIndex, typename TNodeArray>
TIndex NodeArrayTester<TSymbol, TIndex, TNodeArray>::getSizedIndex(int size) {
    CPPUNIT_ASSERT(size >= 0);
    TIndex index = 0;
    for (int i = 0; i < size; ++i) index++;
    return index;
}

template <typename TSymbol, typename TIndex, typename TNodeArray>
void NodeArrayTester<TSymbol, TIndex, TNodeArray>::testConstructors() {
    // no-arg contructor must exist
    TNodeArray na;

    // constructor that accepts capacity must exist
    TIndex size = getSizedIndex(100), tsize = getSizedIndex(100);
    TNodeArray na2(size);
    CPPUNIT_ASSERT(na2.getCapacity() == tsize);
}

    /// test the [] operator
template <typename TSymbol, typename TIndex, typename TNodeArray>
void NodeArrayTester<TSymbol, TIndex, TNodeArray>::testNodeAccess() {
    TNodeArray na;
    //TODO da se testu preda parametar max index size?
    const int S = 500;
    TIndex size = getSizedIndex(S);
    na.createNodesAtEnd(size);

    /* TODO dodaj randomizaciju i spremi generirane
     * vrijednosti u polje TNode-ova i tako testiraj, dodaj
     * i simbole pomocu alfabeta s kojim se inicijalizira klasa */
    TIndex it;
    for (int i = 0, it = 0; i < S; ++i, it++) {
        na[it].setSibling(it);
        na[it].setEow(true);
        na[it].setCow(false);
    }

    for (int i = 0, it = 0; i < S; ++i, it++) {
        CPPUNIT_ASSERT(na[it].getSibling() == it);
        CPPUNIT_ASSERT(na[it].getEow() == true);
        CPPUNIT_ASSERT(na[it].getCow() == false);
    }
}

#endif	/* NODEARRAYTESTER_H */

