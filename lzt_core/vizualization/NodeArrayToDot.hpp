#ifndef NODEARRAYTODOT_HPP
#define	NODEARRAYTODOT_HPP

#include <string>
#include <fstream>
#include <cstdio>
#include <cctype>
#include <map>

using namespace std;

/** Class for writing a node array to GraphViz's dot file, for visualization. */
template <typename TNodeArray>
class NodeArrayToDot {

private:

    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

public:

    NodeArrayToDot(TNodeArray* arr): array(arr) {};
    void write(string file);


private:

    void writeBeginning();
    void writeEnd();
    void writeTree(TIndex i);
    TIndex resolvePointer(TIndex i);
    
    // filter out all non-aphanumeric symbols
    char processSymbol(char c) {
        if (isalpha(c)) return c;
        else return 'x';
    }

    string intToStr(int);

    const TNodeArray* array;
    fstream file;
    long nodeCounter;
    map<TIndex, string> posToNode;

};

template <typename TNodeArray>
void NodeArrayToDot<TNodeArray>::write(string fname) {
    file.open(fname.c_str(), ios_base::out);
    writeBeginning();

    posToNode.clear();
    nodeCounter = 0;
    writeTree(0);

    writeEnd();
    file.close();
}

/** Write a subtree with a first edge at index i in the array. */
template <typename TNodeArray>
void NodeArrayToDot<TNodeArray>::writeTree(TIndex i) {
    long nodeIndex = nodeCounter;
    // current node label
    string node = "N"; node += intToStr(nodeCounter);    
    while (true) {
        posToNode[i] = node;
        // next node label
        string next = "N"; next += intToStr(nodeCounter+1);
        // 
        TIndex res = resolvePointer(i);
        TIndex sib = (*array)[res].getSibling();
        bool pointer = (*array)[i].isPointer();
        TSymbol symbol;
        if (pointer) symbol = '_';
        else symbol = processSymbol((*array)[i].getSymbol());

        // draw edge to next node
        file << node << " -> " << next << " [label=" << symbol << "];" << endl;
        // draw pointer edge
        if (pointer) {
            string targetNode;
            if (posToNode.count((*array)[i].getSibling()) == 0) targetNode = node;
            else targetNode = posToNode[(*array)[i].getSibling()];
                //cout << (*array)[i].getSibling() << endl;
            file << node << " -> " << targetNode
                 << "[constraint=false,color=red]"<< endl;
        }
        // 
        nodeCounter++;
        // continue recursively if there are edges (nodes) at deeper levels
        if (pointer) {
            TIndex offset = (*array)[i].getSymbol();
            TIndex next = (i + offset + 1);

            if ((*array)[i+offset].getCow()) writeTree(next);

//            if (sib) {
//                if (next < i + sib) writeTree(next);
//            }
//            else {
//
//            }
        }
        else {
            if ((*array)[i].getCow()) writeTree(i+1);
        }
        
        
        if (sib != 0) i += sib;
        else break;
    }
}

/** Recursively resolve pointers. */
template <typename TNodeArray>
typename TNodeArray::Index NodeArrayToDot<TNodeArray>::resolvePointer(TIndex i) {
    while((*array)[i].isPointer()) i = (*array)[i].getSibling();
    return i;
}

template <typename TNodeArray>
string NodeArrayToDot<TNodeArray>::intToStr(int i) {
    char number[20];
    sprintf(number,"%d",i);
    return number;
}

template <typename TNodeArray>
void NodeArrayToDot<TNodeArray>::writeBeginning() {
    file << "digraph DFA { " << endl
         << "splines=false;" << endl
         << "bgcolor=\"white\";" << endl
	 << "rankdir=LR;" << endl
         << "node [shape=circle, width=0.4, label=\"\", style=filled, color=\"black\"];" << endl
	 << "edge [arrowsize = 0.2, penwidth=3];" << endl;
}


template <typename TNodeArray>
void NodeArrayToDot<TNodeArray>::writeEnd() {
    file << "}" << endl;
}

#endif	/* NODEARRAYTODOT_HPP */

