#ifndef CMMEXPORTER_H
#define	CMMEXPORTER_H

#include <string>
#include <cstdio>

using namespace std;

/** Class for writing a node array in a .cmm format used by Ristov's implementation. */
template <typename TNodeArray>
class CmmExporter {

private:
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::NodeConst TNodeConst;

    typedef int TCmmOffset;

    struct CmmNode {
        unsigned char code;
        unsigned char down;
        unsigned char eow;
        TCmmOffset sideoff;
    };

public:
    
    static void writeToFile(const TNodeArray& nodes, string file);
    static void writeNode(CmmNode n, FILE* file);

private:
    
    static CmmNode nodeToCmm(TNodeConst n);

};

template <typename TNodeArray>
void CmmExporter<TNodeArray>::writeToFile(const TNodeArray& nodes, string fileName) {
    FILE* file = fopen(fileName.c_str(), "w+b");

    for (TIndex i = 0; i < nodes.getSize(); ++i) {
        CmmNode n = nodeToCmm(nodes[i]);
        writeNode(n, file);
    }

    fclose(file);
}

template <typename TNodeArray> typename CmmExporter<TNodeArray>::CmmNode
CmmExporter<TNodeArray>::nodeToCmm(typename CmmExporter<TNodeArray>::TNodeConst n) {
    CmmNode cn;

    if (n.isPointer()) {
        if (n.getSymbol() != 0) cn.code = (unsigned char)n.getSymbol() + 1;
        else cn.code = 0;
    }
    else cn.code = (unsigned char)n.getSymbol();
    
    cn.sideoff = (TCmmOffset)n.getSibling();
    cn.eow = n.getEow() ? 1 : 0;
    cn.down = n.getCow() ? 1 : 0;
    
    return cn;
}

template <typename TNodeArray>
void CmmExporter<TNodeArray>::writeNode(CmmNode n, FILE* file) {
    fwrite(&(n.code), sizeof(char), 1, file);
    fwrite(&(n.down), sizeof(char), 1, file);
    fwrite(&(n.eow), sizeof(char), 1, file);
    fwrite(&(n.sideoff), sizeof(TCmmOffset), 1, file);
}

#endif	/* CMMEXPORTER_H */

