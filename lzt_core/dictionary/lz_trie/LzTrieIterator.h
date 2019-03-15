#ifndef LZTRIEITERATOR_H
#define	LZTRIEITERATOR_H

#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace std;

/** Iterator for traversing lz-compressed trie in the same way
 * uncompressed trie can be travered using integer indexes. Only prefix ++ and +=
 * operations are needed to traverse the trie. These operations for
 * moving to child and sibling nodes are supported, and they have to be
 * used correctly, ie += only for nodes with siblings with a sibling offeset
 * and ++ only if cow flag is set on a node. For algorithms on tries that
 * use only ++ and +=, LzTrieIterator can replace integer indexes via
 * template parameters. */
template <typename TNodeArray>
class LzTrieIterator {

private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::NodeConst TNodeConst;

public:
    //TODO implement assignment operator
    LzTrieIterator(const TNodeArray& nodes, bool verbose=false);
    LzTrieIterator(const LzTrieIterator& src);
    virtual ~LzTrieIterator();

    void operator=(TIndex i);
    void operator+=(TIndex s);
    void operator++();
    operator TIndex() const;

    TIndex node() const;
    bool end() const;
    void start();

private:

    /** For recursive trie searching, data about a location where
     * search moved when a pointer was followed */
    struct ArraySegment {
        // current position in the segment
        TIndex pos;
        // end of segment, this position is included in the search
        TIndex end;

        ArraySegment(TIndex p, TIndex e): pos(p), end(e) {}
        inline void set(TIndex p, TIndex e) { pos = p; end = e; }
    };

    const TNodeArray& nodes;

    // initial stack capacity
    static const int STACK_INIT_CAP = 25;
    // stack of followed trie pointers
    ArraySegment* stack;
    int stackSize;
    int stackCapacity;

    void popFinishedSegments();
    void followPointers();
    
    bool verbose;
    void printSegment(ArraySegment s);

};

template <typename TNodeArray>
LzTrieIterator<TNodeArray>::~LzTrieIterator() {
    free(stack);
}

template <typename TNodeArray>
LzTrieIterator<TNodeArray>::LzTrieIterator(const TNodeArray& n, bool v): nodes(n), verbose(v) {
    stack = (ArraySegment *)malloc(STACK_INIT_CAP * sizeof(ArraySegment));
    stackCapacity = STACK_INIT_CAP;
    start();
};

template <typename TNodeArray>
LzTrieIterator<TNodeArray>::LzTrieIterator(const LzTrieIterator& src)
: nodes(src.nodes) {
    // copy stack
    stackSize = src.stackSize;
    stackCapacity = src.stackCapacity;
    stack = (ArraySegment *)malloc(stackCapacity * sizeof(ArraySegment));
    for (int i = 0; i < stackSize; ++i) stack[i] = src.stack[i];
}

/** Operator that enables following syntax: Titer it; it = 0;
 * If i != 0, operator has no effect.   */
template <typename TNodeArray>
inline void LzTrieIterator<TNodeArray>::operator =(TIndex i) {
    if (i == 0) start();
}

/** Operator for moving to a sibling node. Is s is not the sibling offset
 * value of the node currently pointed to by the iterator, behaviour
 * is undefinded. */
template <typename TNodeArray>
inline void LzTrieIterator<TNodeArray>::operator +=(TIndex offset) {
    if (verbose) cout<<"+="<<endl;
    stack[stackSize-1].pos += offset;
    popFinishedSegments();
    followPointers();
}

/** Operator for moving to a child node, ie node that comes after
 * the current node in the array. */
template <typename TNodeArray>
inline void LzTrieIterator<TNodeArray>::operator ++() {
    if (verbose) cout<<"++"<<endl;
    stack[stackSize-1].pos++;
    popFinishedSegments();
    followPointers();
    //return *this;
}

/** Conversion that enables the iterator to be used as an operator[] argument
 * for node arrays. */
template <typename TNodeArray>
inline LzTrieIterator<TNodeArray>::operator  TIndex() const {
    return stack[stackSize-1].pos;
}

/** Point iterator to the first node. */
template <typename TNodeArray>
inline void LzTrieIterator<TNodeArray>::start() {
    // put the segment representing the whole array on the stack
    stack[0].set(0, nodes.getSize() - 1);
    stackSize = 1;
}

/** Remove from stack the ArraySegments whose end is reached. */
template <typename TNodeArray>
void LzTrieIterator<TNodeArray>::popFinishedSegments() {
   while (stackSize > 0) {
        ArraySegment s = stack[stackSize-1];
        // assert the segment is within bounds
        if (!(s.pos <= s.end + 1)) {cout << stackSize-1 << endl;}
        assert(s.pos <= s.end + 1);

        if (s.pos == s.end + 1) { // we're outside of the top segment
            // pop
            if (verbose) {
                cout<<"pop"<<endl;
                this->printSegment(stack[stackSize-1]);
            }
            stackSize--;
            if (stackSize > 0) {
                // assert that position from wich popped segment was followed is a pointer
                assert( nodes[stack[stackSize-1].pos].isPointer() );
                // increase the position at (new) top level
                (stack[stackSize-1].pos)++;
            }
        }
        // there are nodes left in the top segment
        else break;
    }
}

template <typename TNodeArray>
void LzTrieIterator<TNodeArray>::printSegment(ArraySegment s) {
    cout <<"seg["<<s.pos<<","<<s.end<<"]"<<endl;
}

/** While current position of the topmost segment is a pointer, follow it
 * by putting segment pointed to on the stack and go to it's first position.  */
template <typename TNodeArray>
void LzTrieIterator<TNodeArray>::followPointers() {
    if (stackSize == 0) return;

    while (true) {
        // current node in the topmost segment
        TNodeConst topNode = nodes[stack[stackSize-1].pos];
        if (topNode.isPointer() == false) break;
        
        // increase stack capacity if needed
        if (stackSize == stackCapacity) {
            stackCapacity = stackCapacity * 2;
            stack = (ArraySegment *)realloc(stack, stackCapacity * sizeof(ArraySegment));
        }

        // push segment pointed to
        TIndex top = topNode.getSibling();
        stack[stackSize].set( top, top + topNode.getSymbol() );
        stackSize++;   
        if (verbose) {
            cout<<"push"<<endl;
            this->printSegment(stack[stackSize-1]);
        }
    }
}

#endif	/* LZTRIEITERATOR_H */

