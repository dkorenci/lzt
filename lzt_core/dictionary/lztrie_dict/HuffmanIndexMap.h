#ifndef HUFFMANINDEXMAP_H
#define	HUFFMANINDEXMAP_H

#include <cassert>
#include <cstddef>
#include <vector>
#include <iostream>

#include "huffman/HuffmanDecoder.h"
#include "serialization/IntBitArray.h"
#include "serialization/BitVector.h"
#include "serialization/BitSequence.h"
#include "huffman/HuffmanCode.h"
#include "serialization/SerializationUtils.h"

using namespace std;

template <typename TIndex> class HuffmanMapCreator;

/** Data structure that maps each integer (index) from a set {0,..,N} to
 * a sequence of integers from {0,..,M}.
 * Mapping is stored compactly. For segmentSize parameter only values at the
 * beginning of a sequence of each segmentSize-th element are stored explicitly,
 * other values are coded as a difference between consecutive indexes plus
 * information about sequence ending. Huffman coding is used to store
 * this difference info, also reffered to as diff or difference code.
 */
template <typename TIndex>
class HuffmanIndexMap {
public:
    
    virtual ~HuffmanIndexMap() {};

    vector<TIndex> getIndexes(TIndex i);

    void readFromStream(istream& stream);
    void writeToStream(ostream& stream) const;

    template <typename TInd> friend class HuffmanMapCreator;

private:

    /* Difference between two consecutive indexes with
     * information weather the sequence ends with the second index. */
    struct IndexDiff {
        long delta;
        bool end;

        /** Set this to difference num2 - num1, with ending e. */
        template <typename TInt>
        void set(TInt num1, TInt num2, bool e) {
            end = e;
            delta = (long)num2 - (long)num1;
        }
    };

    /** At the start of a segment explicit index and sequence ending info is stored.*/
    struct SegmentStart {
        size_t index;
        bool end;
    };

    // First value mapped to each segmentSize-th index is stored explicitly.
    size_t segmentSize;

    // number of bits necessary to store all the diffs
    int diffBits;
    // number of bits necessary to store indexes at segment beginnings
    int segStartBits;

    /* Bit array with explicitly coded indexes and huffman codes
     * of differences between consecutive indexes. */
    BitVector indexes;
    
    // Indexes of bits at wich segments begin.
    IntBitArray<size_t> segments;

    HuffmanDecoder decoder;

    IndexDiff diffFromBits(BitSequence bits) const;
    BitSequence diffToBits(IndexDiff diff) const;

    SegmentStart segStartFromBits(BitSequence bits) const;
    BitSequence segStartToBits(SegmentStart start) const;

};

template <typename TIndex> 
vector<TIndex> HuffmanIndexMap<TIndex>::getIndexes(TIndex i) {
    size_t segmentIndex = i / segmentSize;
    size_t segmentOffset = i % segmentSize;

    size_t bitIndex = segments[segmentIndex];

    BitSequence bits = indexes.getSequence(bitIndex, segStartBits);
    bitIndex += segStartBits;

    SegmentStart sstart = segStartFromBits(bits);
    // current index value
    TIndex val = sstart.index;
    // current sequence end
    bool end = sstart.end;
    vector<TIndex> result;

    if (segmentOffset == 0) result.push_back(val);
    // run the loop if first code is not the end of sequence
    if (segmentOffset > 0 || end == false) {
        if (end) segmentOffset--;
        
        decoder.reset();
        // TODO nakon debuggiranja zamjeniti uvjet sa: bitIndex < indexes.getSize()
        while (true) {
            // read and process the next bit;
            decoder.processBit(indexes[bitIndex++]);
            if (decoder.isEndOfCode()) {
                bits = decoder.getSymbol();
                IndexDiff diff = diffFromBits(bits);
                val = val + diff.delta;
                end = diff.end;

                if (segmentOffset == 0) result.push_back(val);

                if (end) {
                    if (segmentOffset > 0) segmentOffset--;
                    else break;
                }
            }
        }
    }
    
    return result;
}

/** Extracts IndexDiff information from first nbits of a BitSequence. */
template <typename TIndex> inline typename HuffmanIndexMap<TIndex>::IndexDiff
HuffmanIndexMap<TIndex>::diffFromBits(BitSequence bits) const {
    IndexDiff diff;
    // read absolute value of delta
    diff.delta = numberFromBits<long>(bits, diffBits-2);
    // apply signum
    if (bits[diffBits-2]) diff.delta = -diff.delta;
    // read end
    diff.end = bits[diffBits-1];

    return diff;
}

/** Writes IndexDiff information to first nbits of a BitSequence. */
template <typename TIndex>
inline BitSequence HuffmanIndexMap<TIndex>::diffToBits(IndexDiff diff) const {
    // write abs. value of delta
    BitSequence bits = numberToBits(diff.delta);
    // number bits must not span to signum bits
    assert(bits[diffBits-2] == false);
    // write signum
    if (diff.delta < 0) bits.setBit(diffBits-2, true);
    else bits.setBit(diffBits-2, false);
    // write ending
    bits.setBit(diffBits-1, diff.end);

    return bits;
}


/** Extracts SegmentStart information from first nbits of a BitSequence. */
template <typename TIndex> typename HuffmanIndexMap<TIndex>::SegmentStart
HuffmanIndexMap<TIndex>::segStartFromBits(BitSequence bits) const {
    SegmentStart start;
    // read value of index
    start.index = numberFromBits<size_t>(bits, segStartBits-1);
    // read end
    start.end = bits[segStartBits-1];

    return start;
}

/** Writes IndexDiff information to first nbits of a BitSequence. */
template <typename TIndex>
BitSequence HuffmanIndexMap<TIndex>::segStartToBits(SegmentStart start) const {
    // write index value
    BitSequence bits = numberToBits(start.index);
    // number bits must not span to end bit
    assert(bits[segStartBits-1] == false);
    // write ending
    bits.setBit(segStartBits-1, start.end);

    return bits;
}

template <typename TIndex>
void HuffmanIndexMap<TIndex>::readFromStream(istream& stream) {
    segmentSize = SerializationUtils::integerFromStream<size_t>(stream);
    diffBits = SerializationUtils::integerFromStream<int>(stream);
    segStartBits = SerializationUtils::integerFromStream<int>(stream);
    indexes.readFromStream(stream);
    segments.readFromStream(stream);
    decoder.readFromStream(stream);
}

template <typename TIndex>
void HuffmanIndexMap<TIndex>::writeToStream(ostream& stream) const {
    SerializationUtils::integerToStream(segmentSize, stream);
    SerializationUtils::integerToStream(diffBits, stream);
    SerializationUtils::integerToStream(segStartBits, stream);
    indexes.writeToStream(stream);
    segments.writeToStream(stream);
    decoder.writeToStream(stream);
}

#endif	/* HUFFMANINDEXMAP_H */

