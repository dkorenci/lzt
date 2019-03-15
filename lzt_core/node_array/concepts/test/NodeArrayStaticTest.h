#ifndef NODEARRAYSTATICTEST_H
#define	NODEARRAYSTATICTEST_H

template <typename TNodeArray>
class NodeArrayStaticTest {
public:

    // test types that have to be defined
    typename TNodeArray::Index TIndex;
    typename TNodeArray::Symbol TSymbol;
    
    typename TNodeArray::Node_const TNode_const;

    virtual ~NodeArrayConceptsTest() {};

    static void testArray(TNodeArray array);
    static void testNode_const(TNode_const n);

private:

};

template <typename TNodeArray>
void NodeArrayStaticTest<TNodeArray>::testArray(TNodeArray const & array) {
    // test getSize and test operator []
    for (TIndex i = 0; i < array.getSize(); ++i)
        testNode_const( array[i] );
}

template <typename TNodeArray>
void NodeArrayStaticTest<TNodeArray>::testNode_const(TNode_const const & n) {
    //TODO strict type checking, not just conversion
    TSymbol s = n.getSymbol();
    TIndex i = n.getIndex();
    bool eow = n.getEow();
    bool cow = n.getCow();
    bool p = n.isPointer();
    //TODO create test with dummy node type that implements getter methods
    bool l = (n < n);
    bool e = (n == n);
    bool ne = (n != n);
}

#endif	/* NODEARRAYSTATICTEST_H */

