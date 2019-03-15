#ifndef COMPACTARRAYNODE_H
#define	COMPACTARRAYNODE_H

template <typename TSymbol, typename TIndex>
class CompactArrayNode {

public:

    CompactArrayNode() {};
    CompactArrayNode(const CompactArrayNode& orig);
    virtual ~CompactArrayNode() {};

    TSymbol getSymbol() const { return symbol; }
    TIndex getSibling() const { return sibling; }
    TIndex getNumWords() const { return numOfWords; }
    bool getEow() const { return eow; }
    bool getCow() const { return cow; }

    bool isPointer() const { return (eow == false) && (cow == false); };

    bool operator<(CompactArrayNode n2) const;
    bool operator==(CompactArrayNode n2) const;
    bool operator!=(CompactArrayNode n2) const;

    template <typename T1, typename T2> friend class CompactArray;

private:

    TSymbol symbol;
    TIndex sibling;
    TIndex numOfWords;
    bool eow;
    bool cow;
    // true if numOfWords field is considered, ie. node is form enumerated array
    bool enumerated;

};

template <typename TSymbol, typename TIndex>
CompactArrayNode<TSymbol, TIndex>::CompactArrayNode(const CompactArrayNode& src) {
    symbol = src.symbol;
    sibling = src.sibling;
    numOfWords = src.numOfWords;
    eow = src.eow;
    cow = src.cow;
}

template <typename TSymbol, typename TIndex>
bool CompactArrayNode<TSymbol, TIndex>::operator <(CompactArrayNode n2) const {
    if (symbol < n2.symbol) return true;
    else if (n2.symbol < symbol) return false;

    if (eow < n2.eow) return true;
    else if (n2.eow < eow) return false;

    if (cow < n2.cow) return true;
    else if (n2.cow < cow) return false;

    //TODO na koju vrijednost postaviti sibling node nema pointera
    if (sibling < n2.sibling) return true;
    else if (n2.sibling < sibling) return false;

    if (enumerated) {
        if (numOfWords < n2.numOfWords) return true;
        if (numOfWords > n2.numOfWords) return false;
    }

    return false;
}

template <typename TSymbol, typename TIndex>
bool CompactArrayNode<TSymbol, TIndex>::operator ==(CompactArrayNode n2) const {
    bool equal;
    
    equal = (symbol == n2.symbol && sibling == n2.sibling &&
             eow == n2.eow && cow == n2.cow);

    if (enumerated) equal = equal && (numOfWords == n2.numOfWords);

    return equal;
}

template <typename TSymbol, typename TIndex>
bool CompactArrayNode<TSymbol, TIndex>::operator !=(CompactArrayNode n2) const {
    return !( (*this) == n2 );
}

#endif	/* COMPACTARRAYNODE_H */

