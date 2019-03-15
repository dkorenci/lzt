#ifndef NODEARRAYTEST_H
#define	NODEARRAYTEST_H

#include "NodeArrayStaticTest.h"

template <typename TNodeArray>
class NodeArrayTest : public NodeArrayStaticTest<TNodeArray> {

public:

    typename TNodeArray::Node TNode;

    static void testArray();
    static void testNode(TNode node);


};

template <typename TNodeArray>
void NodeArrayTest<TNodeArray>::testArray() {
    // test existence of default constructor
    TNodeArray array;

    // TODO zahtjev na tip
    TIndex index = 0; index++;

    // test resize
    array.resize(index);
    // test getSize
    index = array.getSize();

    // test operator[]
    TNode n = array[0];
    for (index = 0; i < array.getSize(); ++i) {
        //TODO strict type checking
        // test operator =
        n = array[index];
        testNode(array[index]);
        NodeArrayStaticTest::testNode_const(array[index]);
    }

    // test static part of the array
    NodeArrayStaticTest::testArray(array);
}

/** Test variable part of variable node. */
template <typename TNodeArray>
void NodeArrayTest<TNodeArray>::testNode(TNode node) {
    // test copy contructor existence when passing the function parameter
    TSymbol s; TIndex i;
    bool b;
    node.setSymbol(s);
    node.setSibling(i);
    node.setEow(b);
    node.setCow(b);
    node.setPointer();
}

#endif	/* NODEARRAYTEST_H */

