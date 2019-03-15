#include <assert.h>

#include "NodeArrayBrowser.h"

NodeArrayBrowser::NodeArrayBrowser(const char* filename) {
    ifstream file(filename);
    while (true) {
        char ch;
        file>>ch;
        if (file.good()) array.push_back(ch);
        else break;
    }
    analyzeArray();
}

NodeArrayBrowser::NodeArrayBrowser(const NodeArrayBrowser& orig) {
}

NodeArrayBrowser::~NodeArrayBrowser() {
}

string NodeArrayBrowser::nodeAt(size_t i) {
    string n;
    size_t begin = nodePos[i]; n.push_back(array[begin]);
    /* start from second char cause the first char
     * (symbol part of the node) could be a delimiter */
    for (size_t j = begin + 1; array[j] != delimiter; ++j)        
        n.push_back(array[j]);
    return n;
}

void NodeArrayBrowser::analyzeArray() {
    size_t i = 0;
    while (i < array.length()) {
        assert(array[i] == delimiter);
        
        if (i == array.length() - 1) break;
        else nodePos.push_back(i+1);       

        // find next delimiter
        // TODO rjesi opcenito, ovo radi samo ako je symbol u node-u dug 1 char
        i = findNextChar(i+1, delimiter);
    }
}

/** Find next occurence of character c after position p. */
size_t NodeArrayBrowser::findNextChar(size_t p, char c) {
    for (size_t i = p + 1; i < array.length(); ++i)
        if (array[i] == c) return i;

    return 0;
}

