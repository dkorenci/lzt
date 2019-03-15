#ifndef NODECONST_H
#define	NODECONST_H

/** Node_const concept, for access to the data of a NodeArray(Static) */
template <typename TSymbol, typename TIndex>
class Node_constant {
public:

    Node_constant(Node_constant const & node);

    TSymbol getSymbol() const;
    TIndex getSibling() const;
    bool getEow() const;
    bool getCow() const;
    bool isPointer() const;

    template <typename TNode_const>
    bool operator<(TNode_const n2) const;

    template <typename TNode_const>
    bool operator==(TNode_const n2) const;

    template <typename TNode_const>
    bool operator!=(TNode_const n2) const;


};

#endif	/* NODECONST_H */

