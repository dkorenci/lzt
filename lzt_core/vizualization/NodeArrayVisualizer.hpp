#ifndef NODEARRAYVISUALIZER_HPP
#define	NODEARRAYVISUALIZER_HPP

#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

/** Class for creating comprehensive text based
 * visualizations of node arrays. Data for the array is written to
 * a 2d symbol grid, where each column represents a node, first a
 * pointer value, vertically, (if node is a pointer), then node char,
 * then the node's position, vertcally. */
template <typename TNodeArray>
class NodeArrayVisualizer {

private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

public:

    NodeArrayVisualizer(const TNodeArray& arr);
    ~NodeArrayVisualizer();

    void printArray(ostream& s);
    string intToString(long i);

private:

    void createGrid();
    void initGrid();
    void fillGrid();
    void calcNumDigits();
    void printStrToGrid(string str, int row, int col, bool above);

private:

    const TNodeArray& array;
    TSymbol **grid;
    int gridW, gridH;
    // number of decimal digits in the largest array index
    int numDigits;






};

template <typename TNodeArray>
NodeArrayVisualizer<TNodeArray>::NodeArrayVisualizer(const TNodeArray& arr): array(arr) {
      createGrid();
}

template <typename TNodeArray>
NodeArrayVisualizer<TNodeArray>::~NodeArrayVisualizer() {
    for (int i = 0; i < gridH; ++i) delete [] grid[i];
    
    delete [] grid;
}

template <typename TNodeArray>
void NodeArrayVisualizer<TNodeArray>::createGrid() {
    calcNumDigits();
    initGrid();
    fillGrid();
}

template <typename TNodeArray>
void NodeArrayVisualizer<TNodeArray>::printArray(ostream& s) {
    for (int i = 0; i < gridH; ++i) {
        for (int j = 0; j < gridW; ++j)
            s << grid[i][j];

        s << endl;
    }
}

/** Write the node array data to grid. */
template <typename TNodeArray>
void NodeArrayVisualizer<TNodeArray>::fillGrid() {
    // position of the last replaced character of the last pointer
    long replacedEnd = -1;
    for (int i = 0; i < gridW; ++i) {
        // print i to grid
        printStrToGrid(intToString(i), numDigits+1, i, false);
        // if pointer, print pointer to grid
        if (array[i].isPointer()) {
            printStrToGrid(intToString(array[i].getSibling()), numDigits - 1, i, true);
            // print pointer indicator char
            grid[numDigits][i] = '_';
            replacedEnd = i + array[i].getSymbol();
        }
        // print char to grid
        else { 
            TSymbol s;

            if (i <= replacedEnd) s = '_';
            else s = array[i].getSymbol();
            
            grid[numDigits][i] = s;
        }
    }
}

/** Allocate memory for the grid. */
template <typename TNodeArray>
void NodeArrayVisualizer<TNodeArray>::initGrid() {
    gridW = array.getSize();
    gridH = numDigits * 2 + 1;

    grid = new TSymbol* [gridH];
    for (int i = 0; i < gridH; ++i)
        grid[i] = new TSymbol[gridW];

    for (int i = 0; i < gridH; ++i)
        for (int j = 0; j < gridW; ++j)
            grid[i][j] = ' ';
}

/** Calculate number of decimal digits for the largest array index */
template <typename TNodeArray>
void NodeArrayVisualizer<TNodeArray>::calcNumDigits() {
    numDigits = 1; long exp10 = 10;
    while ( (array.getSize() - 1 < exp10) == false ) {
        exp10 *= 10; numDigits++;
    }    
}

/** Convert integer to c++ string, in base 10. */
template <typename TNodeArray>
string NodeArrayVisualizer<TNodeArray>::intToString(long num) {
    char s[100];
    sprintf(s, "%ld", num);
    return s;
}

/** Print string to a column of the grid at specified position.
 * above determines if the end or start of the string will be positiond at row,
 * i.e. if the string will be above or below the specified position. */
template <typename TNodeArray>
void NodeArrayVisualizer<TNodeArray>::printStrToGrid(string s, int row, int col, bool above) {
    int startRow;
    if (above) startRow = row - s.size() + 1;
    else startRow = row;

    for (int i = 0; i < s.size(); ++i)
        grid[startRow+i][col] = s[i];
}

#endif	/* NODEARRAYVISUALIZER_HPP */

