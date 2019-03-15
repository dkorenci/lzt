#include <cassert>
#include <iostream>

#include "HuffmanCodecCreator.h"
#include "HuffmanCoder.h"

HuffmanCodecCreator::~HuffmanCodecCreator() {
    delete [] lengths;
    delete [] firstCode;
    delete [] numLengths;
}

/** Create decoder for given set of symbols and their frequencies. */
HuffmanCodecCreator::HuffmanCodecCreator(size_t numSymbols, const size_t* freq):
    freqs(freq), N(numSymbols)
{
    //TODO special cases
    calcLengths();
    calcStartCodes();
}

/** Assuming the children subtrees of node i are heaps, create heap
 * from subtree with root i by pushing A[i] down the tree. */
void HuffmanCodecCreator::heapSift(size_t i) {
    size_t left, right, smallest;
    while(true) {
        left = 2 * i; right = 2 * i + 1; smallest = i;
        if (left <= H && A[A[left]] < A[A[smallest]]) smallest = left;
        if (right <= H && A[A[right]] < A[A[smallest]]) smallest = right;
        
        if (smallest != i) {
            size_t temp = A[smallest];
            A[smallest] = A[i]; A[i] = temp;
            i = smallest;
        }
        else break;
    } 
}

/** Calculate lengths of Huffman codes for the symbols. */
void HuffmanCodecCreator::calcLengths() {
    
    // case with only one symbol
    if (N == 1) {
        lengths = new size_t[1];
        lengths[0] = 1;
        return;
    }

    A = new size_t[2 * N + 1];

    /** Init array, heap of pointers is stored at positions [1,..,N], and
     * frequencies at positions [N+1, 2N]. */
    for (size_t i = 1; i <= N; ++i) {
        A[N + i] = freqs[i-1]; A[i] = N + i;
    }

    // build min heap from indexes in A at positions 1,..,N
    H = N;
    for (size_t i = H/2; i >= 1; --i) heapSift(i);

    // main phase, building the tree of frequencies
    while (H > 1) {
        // take indexes of nodes with two smallest frequencies
        size_t m1, m2;
        m1 = A[1]; A[1] = A[H]; H--;
        heapSift(1);
        m2 = A[1];

        // combine frequency nodes into a node with frequency that is their sum
        A[H+1] = A[m1] + A[m2];
        // set old nodes to point to new node
        A[m1] = A[m2] = H + 1;
        // insert new node in the heap
        A[1] = H + 1;
        heapSift(1);
    }

    // calculate the code lengths wich are depths of node in the frequency tree
    A[2] = 0;
    for (size_t i = 3; i <= 2 * N; ++i) A[i] = A[A[i]] + 1;

    // copy calculated lengths and free the working array
    lengths = new size_t[N];
    for (size_t i = 0; i < N; ++i) lengths[i] = A[N+1+i];

    delete [] A;
}

/** Given the huff. code lengths and frequencies of symbols, for each length
 * calculate starting cannonical Huffman codes . */
void HuffmanCodecCreator::calcStartCodes() {
    maxLen = 0;
    // find length of longest huff code
    for (size_t i = 0; i < N; ++i)
        if (lengths[i] > maxLen) maxLen = lengths[i];

    // calculate number of codes per length
    numLengths = new size_t[maxLen+1];
    for (int i = 1; i <= maxLen; ++i) numLengths[i] = 0;
    for (size_t i = 0; i < N; ++i) numLengths[lengths[i]]++;

    // calculate first codes
    firstCode = new size_t[maxLen+1];
    firstCode[maxLen] = 0;
    for (int l = maxLen - 1; l >= 1; --l)
        firstCode[l] = (firstCode[l+1] + numLengths[l+1]) / 2;
}

/** Create a decoder of cannonical huff. codes for
 * given symbols and frequencies */
void HuffmanCodecCreator::
createDecoder(HuffmanDecoder* dec, const BitSequenceArray& symbols) {
    /* Copy first codes to decoder, temp array is used for copying because
     * array in the decoder is 0-based and array in the creator is 1-based. */
    size_t fc[maxLen];
    for (int i = 0; i < maxLen; ++i) fc[i] = firstCode[i+1];
    (dec->firstCode).setArray(fc, maxLen);

    /* Calculate 0-based indexes in the array of symbols where symbols
     * with codes of some length will start. */
    size_t lengthOffset[maxLen+1];
    lengthOffset[maxLen] = 0;
    for (int i = maxLen-1; i >= 1; --i) 
        lengthOffset[i] = lengthOffset[i+1] + numLengths[i+1];

    // counter of huffman codes for symbols with codes of some length
    size_t nextCode[maxLen+1];
    // init counters
    for (int i = 1; i <= maxLen; ++i) nextCode[i] = firstCode[i];

    // copy symbols to decoder
    (dec->symbolTable).changeFormat(symbols.getNumOfSequences(), symbols.getSequenceSize());
    for (size_t i = 0; i < N; ++i) {
        size_t len = lengths[i];
        size_t symbolIndex = lengthOffset[len] + (nextCode[len] - firstCode[len]);

        (dec->symbolTable).setSequence(symbolIndex, symbols[i]);

        // check that codes for given length remain in the correct range        
        assert(len == 1 || (nextCode[len]/2) < (nextCode[len-1]));

        nextCode[lengths[i]]++;        
    }

    /* Copy length offsets to decoder, temp array is
     * for copying 1-based to 0-based array. */
    size_t lo[maxLen];
    for (int i = 0; i < maxLen; ++i) lo[i] = lengthOffset[i+1];
    (dec->lengthOffset).setArray(lo, maxLen);

    // set initial decoding parameters
    dec->reset();
}

void HuffmanCodecCreator::createCoder(HuffmanCoder* coder) {    
    // counter of huffman codes for symbols with codes of some length
    size_t nextCode[maxLen+1];
    // init counters
    for (int i = 1; i <= maxLen; ++i) nextCode[i] = firstCode[i];    

    coder->codes = new size_t[N];
    coder->lengths = new int[N];

    // copy symbols to decoder
    for (size_t i = 0; i < N; ++i) {
        (coder->codes)[i] = nextCode[lengths[i]];
        (coder->lengths)[i] = lengths[i];
        nextCode[lengths[i]]++;
    }    
}