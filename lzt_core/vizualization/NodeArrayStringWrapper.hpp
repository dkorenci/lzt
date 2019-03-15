#ifndef NODEARRAYSTRINGWRAPPER_HPP
#define	NODEARRAYSTRINGWRAPPER_HPP

#include <exception>
#include <cassert>

using namespace std;

template <typename TSymbol, typename TIndex>
class StringWrapperNode;

template <typename TSymbol, typename TIndex>
class StringWrapperNodeC;

/** Class that wraps a string and presents it through
 * TNodeArray interface so that algorithms that compress TNodeArray
 * can work with strings of symbols.
 * For non symbol node data null is returned, but pointers are set because
 * it is neccessary for compression. */
template <typename TSymbol, typename TIndex>
class NodeArrayStringWrapper {

public:

    typedef TSymbol Symbol;
    typedef TIndex Index;
    typedef StringWrapperNode<TSymbol, TIndex> Node;

    template <typename Tsymbol, typename Tindex> friend class StringWrapperNode;
    template <typename Tsymbol, typename Tindex> friend class StringWrapperNodeC;

    NodeArrayStringWrapper(TSymbol* s);
    NodeArrayStringWrapper(TIndex size, bool e);
    NodeArrayStringWrapper();
    NodeArrayStringWrapper(const NodeArrayStringWrapper& orig);
    ~NodeArrayStringWrapper();
    
    TIndex getSize() const {
        return strLen;
    }

    // stub method, for compatibility with TNodeArray interface
    void resize(TIndex s) {}

    void createNodesAtEnd(TIndex num);

    StringWrapperNode<TSymbol, TIndex> operator[](TIndex i) {
        return StringWrapperNode<TSymbol, TIndex>(this, i);
    }

    StringWrapperNodeC<TSymbol, TIndex> operator[](TIndex i) const {
        return StringWrapperNodeC<TSymbol, TIndex>(this, i);
    }

private:
    static const int NO_POINTER = -1;

    TSymbol* str;
    TIndex strLen;
    long *pointers;

};

template <typename TSymbol, typename TIndex>
NodeArrayStringWrapper<TSymbol, TIndex>::NodeArrayStringWrapper(TSymbol* s): str(s) {
    strLen = 0;
    while(str[strLen] != 0) strLen++;
    // create and init pointer data array
    pointers = new long[strLen];
    for (TIndex i = 0; i < strLen; ++i)
        pointers[i] = NO_POINTER;
}

/** Dummy constructor, for compatibility with TNodeArray concept. */
template <typename TSymbol, typename TIndex>
NodeArrayStringWrapper<TSymbol, TIndex>::NodeArrayStringWrapper(TIndex size, bool e) {
    assert(false);
}

/** Dummy constructor, for compatibility with TNodeArray concept. */
template <typename TSymbol, typename TIndex>
NodeArrayStringWrapper<TSymbol, TIndex>::NodeArrayStringWrapper() {
    assert(false);
}

/* Copy constructor */
template <typename TSymbol, typename TIndex>
NodeArrayStringWrapper<TSymbol, TIndex>::
NodeArrayStringWrapper(const NodeArrayStringWrapper& orig) {
    strLen = orig.strLen;
    str = new TSymbol[strLen];
    pointers = new long[strLen];
        
    for (TIndex i = 0; i < strLen; ++i) {
        pointers[i] = orig.pointers[i];
        str[i] = orig.str[i];
    }
}

/** Dummy function, for compatibility with TNodeArray concept. */
template <typename TSymbol, typename TIndex>
void NodeArrayStringWrapper<TSymbol, TIndex>::createNodesAtEnd(TIndex num) {
    assert(false);
}

template <typename TSymbol, typename TIndex>
NodeArrayStringWrapper<TSymbol, TIndex>::~NodeArrayStringWrapper() {
    delete [] pointers;
    delete [] str;
}

template <typename TSymbol, typename TIndex>
class StringWrapperNode {
private:
    typedef NodeArrayStringWrapper<TSymbol, TIndex> TArray;

public:
    StringWrapperNode(TArray* a, TIndex i): array(a), index(i) {}
    
    TIndex getSibling() {
        if (isPointer() == false) return 0;
        else return array->pointers[index];
    }

    TSymbol getSymbol() {
        return array->str[index];
    }

    bool getCow() { return false; }
    bool getEow() { return false; }

    bool isPointer() {
        return array->pointers[index] != array->NO_POINTER ;
    }

    /** Does nothing because flags are not set to indicate that node is
     * a pointer, sibling has to be set to some value to make WrapperNode
     * a pointer, and correct code will do that immediately after calling
     * setPointer. */
    void setPointer() {
        assert(array->pointers[index] == array->NO_POINTER);
    }

    void setSibling(TIndex sib) {
        array->pointers[index] = sib;
    }
  
    void setSymbol(TSymbol s) {
        array->str[index] = s;
    }

    /** These methods should not be called on a stubby WrapperNode. */
    void setEow(bool eow) { throwException(); }
    void setCow(bool cow) { throwException(); }

    template <typename TNode>
    bool operator==(TNode n2) {
        if (this->isPointer()) {
            if (n2.isPointer() == false) return false;
            else {
                return getSibling() == n2.getSibling() &&
                        getSymbol() == n2.getSymbol();
            }
        }
        else {
            if (n2.isPointer() == true) return false;
            else {
                return getSymbol() == n2.getSymbol();
            }
        }
    }

    void set(StringWrapperNode n2) {
        array->pointers[index] = n2.array->pointers[n2.index];
        array->str[index] = n2.array->str[n2.index];
    }

    template <typename TNode>
    bool operator!=(TNode n2) {
        return  !(this->operator==(n2));
    }

    template <typename TNode>
    bool operator<(TNode n2) {
        if (isPointer()) {
            if (n2.isPointer() == false) return false;
            else {
                if (getSibling() < n2.getSibling()) return true;
                if (getSibling() > n2.getSibling()) return false;
                if (getSymbol() < n2.getSymbol()) return true;
                return false;
            }
        }
        else {
            if (n2.isPointer()) return true;
            else {
                return getSymbol() < n2.getSymbol();
            }
        }
    }

private:

    void throwException() {
        throw exception();
    }

    TIndex index;
    TArray* array;
    
};

// const version of the node, for code compatibility
template <typename TSymbol, typename TIndex>
class StringWrapperNodeC {
private:
    typedef NodeArrayStringWrapper<TSymbol, TIndex> TArray;

public:
    StringWrapperNodeC(const TArray* a, TIndex i): array(a), index(i) {}

    TIndex getSibling() {
        if (isPointer() == false) return 0;
        else return array->pointers[index];
    }

    TSymbol getSymbol() {
        return array->str[index];
    }

    bool getCow() { return false; }
    bool getEow() { return false; }

    bool isPointer() {
        return array->pointers[index] != array->NO_POINTER ;
    }

    template <typename TNode>
    bool operator==(TNode n2) {
        if (this->isPointer()) {
            if (n2.isPointer() == false) return false;
            else {
                return getSibling() = n2.getSibling() &&
                        getSymbol() == n2.getSymbol();
            }
        }
        else {
            if (n2.isPointer() == true) return false;
            else {
                return getSymbol() == n2.getSymbol();
            }
        }
    }

    template <typename TNode>
    bool operator!=(TNode n2) {
        return  !(this->operator==(n2));
    }

private:

    void throwException() {
        throw exception();
    }

    TIndex index;
    const TArray* array;

};


#endif	/* NODEARRAYSTRINGWRAPPER_HPP */

