#ifndef NODEARRAYSTATIC_H
#define	NODEARRAYSTATIC_H

#include "Node_constant.h"

/** Concept describing static array of nodes,
 *  static meaning array size and nodes cannot be changed. */
template <typename TSymbol, typename TIndex>
class NodeArrayStatic {

public:

    typedef TSymbol Symbol;
    typedef TIndex Index;

    typedef Node_constant<TSymbol, TIndex> Node_const;
   
    Node_const operator[](TIndex i) const;

    TIndex getSize() const;

};

#endif	/* NODEARRAYSTATIC_H */

