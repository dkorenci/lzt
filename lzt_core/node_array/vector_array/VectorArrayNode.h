#ifndef TRIENODE_H
#define	TRIENODE_H

#include "VectorArray.h"

// forward declaration
template <class Tsymbol, class Tindex> class VectorArray;

/** Basic vector array node that implements most of the operations,
 * all of wich are const (they do not change the vector array). */
template <class Tsymbol, class Tindex, class TArrayPointer>
class VectorArrayNode
{

public:

    VectorArrayNode(TArrayPointer nodeArray, Tindex nodeIndex);

    virtual ~VectorArrayNode() {};

    template <class symbol, class index> friend class VectorArray;

    Tsymbol getSymbol() const;

    Tindex getSibling() const;
    
    bool getEow() const;

    bool getCow() const;

    Tindex getNumWords() const;

    bool isPointer() const;

    bool operator<(VectorArrayNode n2) const;
    bool operator==(VectorArrayNode n2) const;
    bool operator!=(VectorArrayNode n2) const;

protected:
    Tindex nodeIndex;
    TArrayPointer nodeArray;

};

template <class Tsymbol, class Tindex, class TArrayPointer>
VectorArrayNode<Tsymbol, Tindex, TArrayPointer>::
VectorArrayNode(TArrayPointer nodeArray, Tindex nodeIndex) {
    this->nodeArray = nodeArray;
    this->nodeIndex = nodeIndex;
}

template <class Tsymbol, class Tindex, class TArrayPointer>
inline Tsymbol VectorArrayNode<Tsymbol, Tindex, TArrayPointer>::
getSymbol() const {
    return nodeArray->symbols[nodeIndex];
}

template <class Tsymbol, class Tindex, class TArrayPointer>
inline Tindex VectorArrayNode<Tsymbol, Tindex, TArrayPointer>::getSibling() const {
    return nodeArray->siblings[nodeIndex];
}

template <class Tsymbol, class Tindex, class TArrayPointer>
inline bool VectorArrayNode<Tsymbol, Tindex, TArrayPointer>::getEow() const {
    return nodeArray->eow[nodeIndex];
}

template <class Tsymbol, class Tindex, class TArrayPointer>
inline bool VectorArrayNode<Tsymbol, Tindex, TArrayPointer>::getCow() const {
    return nodeArray->cow[nodeIndex];
}

template <class Tsymbol, class Tindex, class TArrayPointer>
inline Tindex VectorArrayNode<Tsymbol, Tindex, TArrayPointer>::getNumWords() const {
    return nodeArray->numOfWords[nodeIndex];
}

template <class Tsymbol, class Tindex, class TArrayPointer>
inline bool VectorArrayNode<Tsymbol, Tindex, TArrayPointer>::isPointer() const {
    return !(nodeArray->cow[nodeIndex] || nodeArray->eow[nodeIndex]);
}

template <typename TSymbol, typename TIndex, typename TArrayPointer>
inline bool VectorArrayNode<TSymbol, TIndex, TArrayPointer>::operator<(VectorArrayNode<TSymbol, TIndex, TArrayPointer> n2) const {
    if (getSymbol() < n2.getSymbol()) return true;
    else if (n2.getSymbol() < getSymbol()) return false;

    if (getEow() < n2.getEow()) return true;
    else if (n2.getEow() < getEow()) return false;

    if (getCow() < n2.getCow()) return true;
    else if (n2.getCow() < getCow()) return false;

    //TODO na koju vrijednost postaviti sibling node nema pointera
    if (getSibling() < n2.getSibling()) return true;
    else if (n2.getSibling() < getSibling()) return false;

    if (nodeArray->enumerated) {
        if (getNumWords() < n2.getNumWords()) return true;
        else if (n2.getNumWords() < getNumWords()) return false;
    }

    return false;
}

//TODO modificirati operatore da primaju i const i ne-const nodeove
template <typename TSymbol, typename TIndex, typename TArrayPointer>
inline bool VectorArrayNode<TSymbol, TIndex, TArrayPointer>::operator==(VectorArrayNode<TSymbol, TIndex, TArrayPointer> n2) const {
    if (!(getSymbol() == n2.getSymbol())) return false;
    if (getEow() != n2.getEow()) return false;
    if (getCow() != n2.getCow()) return false;
    if (!(getSibling() == n2.getSibling())) return false;
    //TODO copy enumerated data to the node to avoid access
    if (nodeArray->enumerated)
        if (!(getNumWords() == n2.getNumWords())) return false;

    return true;
}

template <typename TSymbol, typename TIndex, typename TArrayPointer>
inline bool VectorArrayNode<TSymbol, TIndex, TArrayPointer>::operator!=(VectorArrayNode<TSymbol, TIndex, TArrayPointer> n2) const {
    return !(this->operator==(n2));
}


template <class Tsymbol, class Tindex, class TArrayPointer>
class VectorArrayNodeVar : public VectorArrayNode<Tsymbol, Tindex, TArrayPointer> {

public:

    VectorArrayNodeVar(TArrayPointer p, Tindex ni)
    : VectorArrayNode<Tsymbol, Tindex, TArrayPointer>(p, ni) {}

    template <typename TNode>
    void set(TNode node);

    inline void setSymbol(Tsymbol s) {
        this->nodeArray->symbols[this->nodeIndex] = s;
    }

    inline void setSibling(Tindex s) {
        this->nodeArray->siblings[this->nodeIndex] = s;
    }

    inline void setEow(bool eow) {
        this->nodeArray->eow[this->nodeIndex] = eow;
    }

    inline void setCow(bool cow) {
        this->nodeArray->cow[this->nodeIndex] = cow;
    }   

    inline void setNumWords(Tindex numW) {
        this->nodeArray->numOfWords[this->nodeIndex] = numW;
    }

    inline void setPointer() {
        this->nodeArray->cow[this->nodeIndex] = false;
        this->nodeArray->eow[this->nodeIndex] = false;
        // numOfWords is irrelevant for pointer nodes
        if (this->nodeArray->enumerated)
            this->nodeArray->numOfWords[this->nodeIndex] = 0;
    }

};

/** Copy data from node to position in the array pointed to by this. */
template <typename Tsymbol, typename Tindex, typename TArrayPointer>
template <typename TNode> void VectorArrayNodeVar<Tsymbol, Tindex, TArrayPointer>::
set(TNode node) {
    this->nodeArray->siblings[this->nodeIndex] = node.getSibling();
    this->nodeArray->symbols[this->nodeIndex] = node.getSymbol();
    this->nodeArray->cow[this->nodeIndex] = node.getCow();
    this->nodeArray->eow[this->nodeIndex] = node.getEow();

    if (this->nodeArray->enumerated)
        this->nodeArray->numOfWords[this->nodeIndex] = node.getNumWords();
}

#endif	/* TRIENODE_H */

