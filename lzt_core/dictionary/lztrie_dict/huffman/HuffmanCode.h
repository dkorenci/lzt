#ifndef HUFFMANCODE_H
#define	HUFFMANCODE_H

#include "serialization/BitSequence.h"

/** Simple structure representing huffman code of a symbol.
 * Bits of a code are stored at positions 0,..,length-1 , value
 * of other bits can be undefined. */
struct HuffmanCode {

    BitSequence code;
    int length;

};

#endif	/* HUFFMANCODE_H */

