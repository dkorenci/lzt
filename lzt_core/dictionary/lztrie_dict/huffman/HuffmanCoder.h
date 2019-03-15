#ifndef HUFFMANCODER_H
#define	HUFFMANCODER_H

#include <cstddef>

#include "HuffmanCode.h"
#include "serialization/BitSequence.h"

class HuffmanCodecCreator;

/** Class that stores and returns huffman codes for an array of symbols.
 * It is optimized for speed, not size, so codes and their lengths are
 * stored as an array of integers. Symbols are fetched by index, not
 * symbol (sequence of bits) so client must maintain mapping from symbols
 * to indexes.
 * Instances of HuffmanCoder are created via HuffmanCodecCreator.
 */
class HuffmanCoder {
public:

    HuffmanCoder();
    virtual ~HuffmanCoder();

    HuffmanCode getCode(size_t symbolIndex);
    int getLength(size_t symbolIndex);

    friend class HuffmanCodecCreator;

private:
    size_t *codes;
    int *lengths;

};

#endif	/* HUFFMANCODER_H */

