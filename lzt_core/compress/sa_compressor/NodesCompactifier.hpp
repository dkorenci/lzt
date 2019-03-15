#ifndef NODESCOMPACTIFIER_HPP
#define	NODESCOMPACTIFIER_HPP

#include <cassert>

#include "serialization/BitVector.h"
#include "node_array/na_utils.h"

/** Class that works on node array after the compression phase, when
 * replace pointers are set but nodes are not removed from array.
 * It removes nodes and corrects replace pointers and sibling pointers. */
template <typename TNodeArray>
class NodesCompactifier {
public:
    
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

    NodesCompactifier(TNodeArray* n, BitVector& inPointer, bool sptr = true);

    void correctPointersAndSiblings();
    void compactArray();
    void correctPointerLengths();

private:

    TNodeArray* nodes;
    BitVector& inPointer;
    TIndex N;
    // if true, pointer ranges will be corrected for replaced positions
    bool shortenPtrRange;

};

/** Arguments are the array to compactify and BitVector that
 * marks weather an array position is contained in a pointer. */
template <typename TNodeArray>
NodesCompactifier<TNodeArray>::NodesCompactifier(TNodeArray* n, BitVector& ip, bool sptr):
    nodes(n), inPointer(ip), shortenPtrRange(sptr) {
    N = nodes->getSize();
}

/** Correct sibling offsets and compression pointers. Indexes of nodes that
 * are pointed to change when subsequences are replaced with single pointer
 * nodes. */
template <typename TNodeArray>
void NodesCompactifier<TNodeArray>::correctPointersAndSiblings() {
    // numOfReplaced[i] == no. of replaced nodes up to and including index i
    TIndex* numOfReplaced = (TIndex *)malloc(N * sizeof(TIndex));

    TIndex r = 0;
    for (TIndex i = 0; i < N; ++i) {
        if (inPointer[i]) r++;
        numOfReplaced[i] = r;
    }

    for (TIndex i = 0; i < N; ++i) {
            if (inPointer[i]) continue;
            TNode n = (*nodes)[i];

            bool check = ( n.getSibling() != 0 );

            if (n.isPointer()) {
                // assertion with data output in case assertion is broken
                if (inPointer[n.getSibling()]) {
                    cout << i << " " << n.getSibling() << endl;
                    cout << nodeArraySuffShortToString(*nodes, i) << endl;
                    cout << nodeArraySuffShortToString(*nodes, n.getSibling()) << endl;
                    assert(inPointer[n.getSibling()] == false);
                }
                // store old sibling
                TIndex target = n.getSibling();                
                // correct sibling pointer by removing replaced chars from the count
                n.setSibling(n.getSibling() - numOfReplaced[n.getSibling()]);
                // correct the length of the pointer sequence
                TSymbol plen = n.getSymbol(); 
                if (shortenPtrRange && plen > 0) {
                    assert((numOfReplaced[target+plen] - numOfReplaced[target]) <= plen);
                    n.setSymbol(plen - (numOfReplaced[target+plen] - numOfReplaced[target]));
                    // all the symbols pointed to are replaced
                    if (n.getSymbol() == 0) {
                        TNode n2 = (*nodes)[target];
                        // so the target must be a pointer
                        if (n2.isPointer() == false) {
                            cout << i << " " << target << " " << (int) plen << endl;
                            cout << nodeArraySuffShortToString(*nodes, i) << endl;
                            cout << nodeArraySuffShortToString(*nodes, target) << endl;
                            for (int i = -10; i < 30; ++i) cout <<target+i<<","<<(int)((inPointer)[target+i])<<"|";
                            cout << endl;
                            assert(n2.isPointer());
                        }
                        //cout << "hit me" << endl;
                        n.setSibling(n2.getSibling());
                        n.setSymbol(n2.getSymbol());
                    }
                }
            }
            else if (n.getSibling() != 0) {
                // assertion with data output in case assertion is broken
                if (inPointer[i+n.getSibling()]) {
                    cout << i << " " << i + n.getSibling() << endl;
                    cout << nodeArraySuffShortToString(*nodes, i) << endl;
                    cout << nodeArraySuffShortToString(*nodes, i + n.getSibling()) << endl;
                    cout << nodeArraySuffShortToString(*nodes, i + n.getSibling()-30) << endl;
                    assert(inPointer[i+n.getSibling()] == false);
                }
                n.setSibling(n.getSibling() -
                    (numOfReplaced[i+n.getSibling()] - numOfReplaced[i]));
            }

            if (check) {
                assert(0 <= n.getSibling());
                assert(n.getSibling() < N);
            }

        }

    free((void *)numOfReplaced);
    //delete [] numOfReplaced;
}

/**
 * Remove all the replaced elements from the array.
 */
template <typename TNodeArray>
void NodesCompactifier<TNodeArray>::compactArray() {
    TIndex newI = 0;
    for (TIndex oldI = 0; oldI < N; oldI++) {
        if (inPointer[oldI]) continue;

        if (newI != oldI) {
            (*nodes)[newI].set((*nodes)[oldI]);
        }
        newI++;
    }
    (*nodes).resize(newI);
    N = newI;
}

/**
 * When a replacement occurs in the middle of a replacing sequence, corresponding
 * replaced sequence pointers have wrong size field. These sizes have to be
 * corrected.
 */
template <typename TNodeArray>
void NodesCompactifier<TNodeArray>::correctPointerLengths() {
    // length of pointers when fully expanded to nodes
    TIndex* expandedLength = (TIndex *)malloc(N * sizeof(TIndex));
    for (TIndex i = 0; i < N; ++i)
        if ((*nodes)[i].isPointer())
            expandedLength[i] = (*nodes)[i].getSymbol() + 1;

    for (TIndex i = 0; i < N; i++) {
        TNode n1 = (*nodes)[i];
        if (n1.isPointer()) {
            TIndex j = n1.getSibling();
            // cumulative expanded length of inspected nodes
            TIndex length = 0;
            // corrected length of the pointer
            TSymbol clength = 0;

            while (length < expandedLength[i]) {
                assert(clength <= maxSymbol<TSymbol>());
                // length not increased in the first iteraton of the loop
                // because first symbols are not counted in pointer length
                if (length > 0) clength++;

                TNode n2 = (*nodes)[j];
                if (n2.isPointer()) {
                    length += expandedLength[j];
                }
                else {
                    length++;
                }
                j++;
            }

            if (length != expandedLength[i]) {

            }
            assert( length == expandedLength[i] );
            // it could happen that clenghth == 0, when pointer points to another pointer
//            if (!clength) {
//                cout << "length: " << length << " exp.length: "
//                     << expandedLength[i] << " clength: " << (int)clength << endl;
//                cout << nodeArraySuffShortToString(*nodes, i) << endl;
//                cout << nodeArraySuffShortToString(*nodes, n1.getSibling()) << endl;
//            }
            //assert( clength );
            assert( clength <= n1.getSymbol() );

            if (clength < n1.getSymbol())
                n1.setSymbol(clength);
        }
    }

    free((void*)expandedLength);
}

#endif	/* NODESCOMPACTIFIER_HPP */

