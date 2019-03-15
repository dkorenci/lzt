#ifndef LZTUTILS_H
#define	LZTUTILS_H

#include <sstream>
#include <iomanip>
#include <vector>
#include <cassert>

#include "node_array/types/symbol.h"
#include "dictionary/lz_trie/LzTrieIterator.h"

using namespace std;

vector<string> * breakByDelimiter(string const & str, const char delimiter);

template <typename TSymbol>
string symbolToString(TSymbol s, bool pointer) {
    ostringstream ss;
    //TODO zahtjev na tip
    if (pointer) ss << symbolToInt(s);
    else ss << s;

    return ss.str();
}

template <typename TNode, typename TSymbol>
void symbolFromString(TNode n, string str) {
    istringstream ss(str);
    if (n.isPointer()) {
        int intSym;
        //TODO zahtjev na tip
        ss >> intSym;
        n.setSymbol((TSymbol)intSym);
    }
    else {
        assert(str.length() == 1);
        TSymbol s;
        ss >> s;
        n.setSymbol(s);
    }
}

template <typename TIndex>
string indexToString(TIndex i, bool pointer) {
    ostringstream ss;
    char nullPointer = '#';

    if (pointer == false && i == 0 )
        ss << nullPointer;
    else ss << i;

    return ss.str();
}

template <typename TNode, typename TIndex>
void indexFromString(TNode n, string str) {
    const string nullPointer = "#";
    if (str == nullPointer) {
        n.setSibling(0);
    }
    else {
        istringstream ss(str);
        TIndex ind; ss >> ind;
        n.setSibling(ind);
    }
}

template <typename TNode>
string nodeToString(TNode n) {
    ostringstream ss;
    char sep = '.';
    //write symbol
    ss << symbolToString(n.getSymbol(), n.isPointer()) << sep;
    //write pointer
    ss << indexToString(n.getSibling(), n.isPointer())<< sep;
    //write flags
    ss << n.getEow() << n.getCow();
    
    return ss.str();
}

template <typename TNode, typename TIndex, typename TSymbol>
void nodeFromString(TNode n, string strNode) {
    assert(strNode.length() >= 5);
    const char delimiter = '.';
    vector<string>* parts = breakByDelimiter(strNode, delimiter);
    assert(parts->size() == 3);
    flagsFromString(n, (*parts)[2]);
    indexFromString<TNode,TIndex>(n, (*parts)[1]);
    symbolFromString<TNode,TSymbol>(n, (*parts)[0]);

    delete parts;
}

template <typename TNode>
void flagsFromString(TNode n, string str) {
    assert(str.length() == 2);
    istringstream ss(str);
    
    if (str[0] == '1') n.setEow(true);
    else n.setEow(false);

    if (str[1] == '1') n.setCow(true);
    else n.setCow(false);
}

//TODO Change return type to TNodeArray *
template <typename TNodeArray>
string nodeArrayToString(const TNodeArray& na) {
    return subNodeArrayToString(na, (typename TNodeArray::Index)0, na.getSize());
}

template <typename TNodeArray>
string nodeArraySuffixToString(const TNodeArray& na, typename TNodeArray::Index si) {
    return subNodeArrayToString(na, si, na.getSize());
}

template <typename TNodeArray>
string nodeArraySuffShortToString(const TNodeArray& na, typename TNodeArray::Index si) {
    typename TNodeArray::Index end = si + maxSymbol<typename TNodeArray::Symbol>();
    if (end >= na.getSize()) end = na.getSize();
    return subNodeArrayToString(na, si, end);
}

/** Return string representation of a [b,e> segement of NodeArray */
template <typename TIndex, typename TNodeArray>
string subNodeArrayToString(const TNodeArray& na, TIndex b, TIndex e) {
    ostringstream ss;
    char nodeBorder = '|';
    ss << nodeBorder;
    //TODO operator[] na node array koji prima size_t ili long?
    for (TIndex i = b; i < e; i++)
        ss << nodeToString(na[i]) << nodeBorder;

    return ss.str();
}

/** Return string of symbols from [b,e> segement of NodeArray */
template <typename TIndex, typename TNodeArray>
string nodeArraySymbolsToString(const TNodeArray& na, TIndex b, TIndex e) {
    ostringstream ss;
    char symbolBorder = '.';

    for (TIndex i = b; i < e; i++) {
        if (b < i) ss << symbolBorder;
        ss << symbolToString(na[i].getSymbol(), na[i].isPointer());
    }

    return ss.str();
}

template <typename TNodeArray>
TNodeArray* nodeArrayFromString(string const & arrayStr) {
    const char nodeBorder = '|';
    vector<string>* strNodes = breakByDelimiter(arrayStr, nodeBorder);
    TNodeArray* array = new TNodeArray;
    array->createNodesAtEnd(strNodes->size());
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Node TNode;

    for (TIndex i = 0; i < array->getSize(); ++i) {
        TNode n = (*array)[i];
        nodeFromString<TNode,TIndex,TSymbol>(n, (*strNodes)[i]);
    }

    delete strNodes;
    
    return array;
}


/** Class that initializes an iterator type for iteration of a specific array type.
 * Behaviours for different iterator types is defined via class template partial
 * specialization. */
template <typename TIterator, typename TNodeArray>
class IterInit {
public:
    /** Initialize and return an iterator for the given node array.
     * Default functionality that works for integer iterators is just returning
     * 0 wich points to first node of the array. */
    static TIterator get(const TNodeArray &array) { return 0; }
};

/** Specialization of IterInit for LzTrieIterator. */
template <typename TNodeArray>
class IterInit<LzTrieIterator<TNodeArray>, TNodeArray> {
public:
    /** Return LzTrieIterator bound to array. */
    static LzTrieIterator<TNodeArray> get(const TNodeArray &array) {
        LzTrieIterator<TNodeArray> iter(array);
        return iter;
    }
};


#endif	/* LZTUTILS_H */

