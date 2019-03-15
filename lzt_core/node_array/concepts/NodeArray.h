#ifndef NODEARRAY_H
#define	NODEARRAY_H

#include "Node_variable.h"
#include "Node_constant.h"

/** Concept describing an resizable and changable array of Nodes. */
template <typename TSymbol, typename TIndex>
class NodeArray {
public:

    NodeArray();

    typedef TIndex Index;
    typedef TSymbol Symbol;

    typedef Node_variable<TSymbol, TIndex> Node;
    typedef Node_constant<TSymbol, TIndex> Node_const;

    Node operator[](TIndex i);
    Node_const operator[](TIndex i) const;

    TIndex getSize() const;    
    void resize(TIndex newSize);
};

#endif	/* NODEARRAY_H */

