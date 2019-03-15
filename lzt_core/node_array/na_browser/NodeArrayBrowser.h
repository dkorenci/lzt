#ifndef NODEARRAYBROWSER_H
#define	NODEARRAYBROWSER_H

#include <fstream>
#include <string>
#include <vector>
#include <cstddef>

using namespace std;

/** Utility for browsing string representation of a node array. */
class NodeArrayBrowser {

public:

    NodeArrayBrowser(const char* fileName);
    NodeArrayBrowser(const NodeArrayBrowser& orig);
    virtual ~NodeArrayBrowser();

    string nodeAt(size_t i);

private:

    static const char delimiter = '|';
    string array;
    // maps node indexes to string positions
    vector<long> nodePos;

    void analyzeArray();
    size_t findNextChar(size_t p, char c);

};

#endif	/* NODEARRAYBROWSER_H */

