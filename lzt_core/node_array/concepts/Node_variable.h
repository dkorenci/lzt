#ifndef NODE_H
#define	NODE_H

/** Node concept, used for access and changing of data of a NodeArray. */
template <class TSymbol, class TIndex>  class Node_variable {

public:

    Node_variable(Node_variable const & node);

    void setSymbol(TSymbol s);
    void setSibling(TIndex s);
    void setEow(bool eow);
    void setCow(bool cow);
    void setPointer();

    TSymbol getSymbol() const;
    TIndex getSibling() const;
    bool getEow() const;
    bool getCow() const;
    bool isPointer() const;

    template <typename TNode_const>
    Node_variable& operator=(TNode_const rhs);

    template <typename TNode_const>
    bool operator<(TNode_const n2) const;

    template <typename TNode_const>
    bool operator==(TNode_const n2) const;

    template <typename TNode_const>
    bool operator!=(TNode_const n2) const;

};


#endif	/* NODE_H */

