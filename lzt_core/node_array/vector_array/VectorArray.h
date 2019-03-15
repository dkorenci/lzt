#ifndef VECTORARRAY_H
#define	VECTORARRAY_H

#include <vector>
#include <string>
#include <iostream>

#include "VectorArrayNode.h"

using namespace std;

/**
 * Implements node array with stl vectors.
 */
template <class TSymbol, class TIndex>
class VectorArray
{

public:

    typedef VectorArrayNodeVar<TSymbol, TIndex, VectorArray<TSymbol, TIndex>* > Node;
    typedef VectorArrayNode<TSymbol, TIndex, const VectorArray<TSymbol, TIndex>* > NodeConst;

    typedef TSymbol Symbol;
    typedef TIndex Index;

    VectorArray(bool en = false);
    VectorArray(TIndex size, bool en = false);
    virtual ~VectorArray();

    Node operator[](TIndex i);
    NodeConst operator[](TIndex i) const;

    void createNodesAtEnd(TIndex num);

    bool isEnumerated() const;
    TIndex getSize() const;
    TIndex getCapacity() const;
    void resize(TIndex capacity);
    long getAllocatedMemory() const;

    template <class symbol, class index, class arraypointer> friend class VectorArrayNode;
    template <class symbol, class index, class arraypointer> friend class VectorArrayNodeVar;


private:

    // number of nodes the array can hold
    TIndex capacity;

    // number of nodes in the array
    TIndex size;

    // node symbols
    vector<TSymbol> symbols;
    // node sibling offsets
    vector<TIndex> siblings;
    // end of word flags
    vector<bool> eow;
    // continuation of word flags
    vector<bool> cow;

    // are the array nodes is enumerated or not
    bool enumerated;

    // number of words containing a node
    vector<TIndex> numOfWords;
    
};

// ********************* IMPLEMENTATION ********************* //

template <class Tsymbol, class Tindex>
VectorArray<Tsymbol, Tindex>::VectorArray(bool en): size(0), capacity(0), enumerated(en) {
}

/**
 * Create empty array with given capacity.
 */
template <class Tsymbol, class Tindex>
VectorArray<Tsymbol, Tindex>::VectorArray(Tindex cap, bool en):
    symbols(cap), siblings(cap), cow(cap), eow(cap), capacity(cap), size(0), enumerated(en)
{
    if (enumerated) numOfWords.resize(cap);
}

template <class TSymbol, class TIndex>
VectorArray<TSymbol, TIndex>::~VectorArray() {
}

template <class Tsymbol, class Tindex>
void VectorArray<Tsymbol, Tindex>::resize(Tindex cap) {
    symbols.resize(cap);
    siblings.resize(cap);
    eow.resize(cap);
    cow.resize(cap);

    if (enumerated) numOfWords.resize(cap);

    capacity = cap;
    if (capacity < size) size = capacity;
}

template <class Tsymbol, class Tindex>
inline typename VectorArray<Tsymbol, Tindex>::Node
VectorArray<Tsymbol, Tindex>::operator[](Tindex i) {
    Node n(this, i);
    return n;
}

template <class Tsymbol, class Tindex>
inline typename VectorArray<Tsymbol, Tindex>::NodeConst
VectorArray<Tsymbol, Tindex>::operator[](Tindex i) const {
    //TODO dali se ovo moze pretvoriti u const?
    NodeConst n(this, i);
    return n;
}

//TODO maknuti i prijeci na resizeArray;
template <class Tsymbol, class Tindex>
inline void VectorArray<Tsymbol, Tindex>::createNodesAtEnd(Tindex num) {
    // TODO koji je bolji nacin povecavanja
    if (getSize() + num > getCapacity()) resize((getCapacity() + num) * 2);
    size += num;
}

/** return the number of nodes array can store without resizing */
template <class Tsymbol, class Tindex>
Tindex VectorArray<Tsymbol, Tindex>::getCapacity() const {
    return capacity;
}

template <class Tsymbol, class Tindex>
bool VectorArray<Tsymbol, Tindex>::isEnumerated() const {
    return enumerated;
}

// Get size of the trie.
template <class Tsymbol, class Tindex>
Tindex VectorArray<Tsymbol, Tindex>::getSize() const {
    return size;
}

template <class Tsymbol, class Tindex>
long VectorArray<Tsymbol, Tindex>::getAllocatedMemory() const {
    long mem = 0;

    //TODO vidi dali je ovo ispravno
    mem += sizeof(this);
    mem += symbols.capacity() * sizeof(typename vector<Tsymbol>::value_type);
    mem += siblings.capacity() * sizeof(typename vector<Tindex>::value_type);
    mem += cow.capacity() * sizeof(typename vector<bool>::value_type);
    mem += eow.capacity() * sizeof(typename vector<bool>::value_type);

    return mem;
}

#endif	/* VECTORARRAY_H */

