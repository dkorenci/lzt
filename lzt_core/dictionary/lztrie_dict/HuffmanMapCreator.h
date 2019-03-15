#ifndef HUFFMANMAPCREATOR_H
#define	HUFFMANMAPCREATOR_H

#include <map>
#include <set>
#include <cstddef>
#include <cassert>
#include <iostream>

#include "serialization/serialization.h"
#include "serialization/BitVector.h"
#include "serialization/BitSequenceArray.h"
#include "serialization/BitSequence.h"

#include "huffman/HuffmanCodecCreator.h"
#include "huffman/HuffmanCoder.h"

#include "HuffmanIndexMap.h"

using namespace std;

/** Algorithm for creating HuffmanIndexMap. Its input is segmentSize plus the
 * mapping coded so that all the indexes are written in a flat array and a
 * BitVector denotes where sequences end. 
 * For a mapping:
 * 0: 1, 2
 * 1: 3
 * 2: 0, 4
 * Array is: 1 2 3 0 4, and bitVector: 0 1 1 0 1 
 * 
 * For a given mapping, HuffmanIndexMap that is created stores numbers in each
 * mapped sequence by preserving their order, in the above example 1 preceeds 2
 * and 0 preceeds 4.
 */
template <typename TIndex>
class HuffmanMapCreator {

public:
    HuffmanMapCreator(const TIndex* index, size_t numIndexes,
                        const BitVector& endings, size_t segSize);
    virtual ~HuffmanMapCreator();

    void createHuffmanMap(HuffmanIndexMap<TIndex>* map);

private:

    typedef typename HuffmanIndexMap<TIndex>::IndexDiff IndexDiff;
    typedef typename HuffmanIndexMap<TIndex>::SegmentStart SegmentStart;
    typedef typename map<size_t, size_t>::iterator TMapIter;

    // map being created
    HuffmanIndexMap<TIndex>* indexMap;

    /* All numbers being mapped to. Ending of a sequence mapped from one index
     * is determined by vector ending. */
    const TIndex* indexes;
    size_t numIndexes, segmentSize;
    /* Ending[i] is true if a series of numbers mapped to an index ends with index[i]. */
    const BitVector& ending;
    
    size_t numSegments;
    int diffBits, segStartBits;

    static const int INIT_DIFF_SIZE = 100;
    // array of index difference codes
    BitSequenceArray diff;
    // number of distinct difference code
    size_t diffCount;
    /* Mapping of differences (indexes of bit-coded differences
     * in BitSequenceArray) to their frequency. */
    map<size_t, size_t> diffMap;
    // frequencies of difference codes
    size_t *frequency;

    HuffmanCoder coder;

    void calcArrayParameters();
    void enumerateDiffCodes();
    void encodeIndexes();
    void computeHuffman();
    void calculateBits();

    //debug info output
    void printHuffman();

    class SegmentedArrayIterator;
    SegmentedArrayIterator getIterator();

};

/** Iterator for array of indexes that supports ++ operator, counts
 * sequences (consecutive indexes mapped to same index) and tracks beginning of
 * segments (segment is segmentSize of sequences) as it passes through the index array. */
template <typename TIndex>
class HuffmanMapCreator<TIndex>::SegmentedArrayIterator {

public:
  inline SegmentedArrayIterator(size_t n, const BitVector& e, size_t segSize)
    : N(n), ending(e), segmentSize(segSize), segStart(true), end(false), seqIndex(0), ind(0) {
        
    }

    inline size_t index() { return ind; }
    inline bool good() { return end == false; }
    inline bool segmentStart() { return segStart; }

    // Increase the array index and keep track of segment start.
    inline SegmentedArrayIterator& operator++() {
        if (end) return *this;

        if (ending[ind]) {
            // sequence of indexes mapped to one index ends
            seqIndex++;
            if (seqIndex % segmentSize == 0) segStart = true;
            else segStart = false;
        }
        else segStart = false;        

        ind++;
        
        if (ind == N) end = true;

        return *this;
    }


private:
    // position in the array
    size_t ind, seqIndex;
    // tracks if position is start of a segment
    bool segStart, end;

    // array size
    size_t N;
    // data about the index array
    size_t segmentSize;
    const BitVector& ending;

};

template <typename TIndex> HuffmanMapCreator<TIndex>::
HuffmanMapCreator(const TIndex* ind, size_t N, const BitVector& e, size_t segSize)
: indexes(ind), numIndexes(N), ending(e), segmentSize(segSize) {}

template <typename TIndex>
HuffmanMapCreator<TIndex>::~HuffmanMapCreator() {
    if (frequency != NULL) delete [] frequency;
}

template <typename TIndex>
void HuffmanMapCreator<TIndex>::createHuffmanMap(HuffmanIndexMap<TIndex>* m) {
    this->indexMap = m;
    calcArrayParameters();
    enumerateDiffCodes();
    if (diffBits > 0) computeHuffman();
    calculateBits();
    //printHuffman();
    encodeIndexes();
}


/* Utility method that creates the iterator for
 * the index array in the creator object. */
template <typename TIndex>
typename HuffmanMapCreator<TIndex>::SegmentedArrayIterator
HuffmanMapCreator<TIndex>::getIterator() {
    SegmentedArrayIterator sai(numIndexes, ending, segmentSize);
    return sai;
}

/** Calculate number of bits necessary for explicitly stored indexes
 * and for difference values, and number of segments. */
template <typename TIndex>
void HuffmanMapCreator<TIndex>::calcArrayParameters() {
    size_t maxIndex = 0; size_t maxAbsDiff = 0, numDiffs = 0;
    // current and previous index in the array iteration
    size_t i, pr = 0;
    SegmentedArrayIterator it = getIterator();
    numSegments = 0;

    do {
        i = it.index();
        
        if (it.segmentStart()) {
            numSegments++;
            if (indexes[i] > maxIndex) maxIndex = indexes[i];
        }
        else {
            numDiffs++;
            size_t absDiff =
            indexes[i] > indexes[pr] ? indexes[i] - indexes[pr] : indexes[pr] - indexes[i];
            if (absDiff > maxAbsDiff) maxAbsDiff = absDiff;
        }

        pr = it.index();
        ++it;
    } while (it.good());

    /* Bits to store all absolute differences plus one bit for signum
     * and one bit to designate end of sequence. */
    if (numDiffs > 0) diffBits = minBitsForNumber(maxAbsDiff) + 2;
    // diffBits == 0 will mean there are no difference codes
    else diffBits = 0;
    
    // bits to store max. index plus one bit for end-of-sequence flag
    segStartBits = minBitsForNumber(maxIndex) + 1;

    indexMap->diffBits = diffBits;
    indexMap->segStartBits = segStartBits;
    indexMap->segmentSize = segmentSize;    
};

/** Scan through the index array and generate and store all
 * distinct difference codes and their frequencies. */
template <typename TIndex>
void HuffmanMapCreator<TIndex>::enumerateDiffCodes() {
    // current and previous index
    size_t i, pr = 0;
    SegmentedArrayIterator iter = getIterator();
    diffMap.clear();
    // Encode the differences and build diffMap
    do {        
        if (iter.segmentStart() == false) {
            i = iter.index();
            // encode the difference info
            IndexDiff d; d.set(indexes[pr], indexes[i], ending[i]);            
            BitSequence bits = indexMap->diffToBits(d);

            // write the diff bits to integer
            size_t bitInt = numberFromBits<size_t>(bits, diffBits);

            // search for difference
            TMapIter it = diffMap.find(bitInt);

            if (it == diffMap.end()) { // not found, insert new
                map<size_t, size_t>::value_type ins(bitInt, 1);
                diffMap.insert(ins);
            }
            else { // found, increase frequency
                it->second++;
            }
        }

        pr = iter.index();
        ++iter;
    } while (iter.good());

    diffCount = diffMap.size();
    
    /* Code distinct differences from integers to bits of
     * BitSequenceArray (necessary for huffman coder creation) and
     * store their frequencies. */
    if (diffBits > 0) {
        diff.changeFormat(diffCount, diffBits);
        frequency = new size_t[diffCount];

        TMapIter it;
        /* Store frequencies and make map point to indexes of diffs
         * in frequency and diff array. */
        for (it = diffMap.begin(), i = 0; it != diffMap.end(); ++it, ++i) {
            frequency[i] = it->second;
            diff.setSequence(i, numberToBits(it->first));
            // now map bits to their index, necessary for HuffmanCoding
            it->second = i;
        }
    }
    else frequency = NULL;

}

/** Create huffman coder and decoder based on diffs and their frequencies. */
template <typename TIndex>
void HuffmanMapCreator<TIndex>::computeHuffman() {
    HuffmanCodecCreator creator(diffCount, frequency);
    creator.createCoder(&coder);
    creator.createDecoder(&(indexMap->decoder), diff);
}

/** Output huffman codes of diffs and their frequencies. */
template <typename TIndex>
void HuffmanMapCreator<TIndex>::printHuffman() {
    cout<< "huffman codes and frequencies: " << endl;
    for (size_t i = 0; i < diffCount; ++i) {
        HuffmanCode hcode = coder.getCode(i);
        cout << hcode.code.toString().substr(0, hcode.length) << " " << frequency[i] << endl;
    }
}

/** Calculate number of bits to hold segmentStarts and diffs and
 * allocate necessary space in the index map.  */
template <typename TIndex>
void HuffmanMapCreator<TIndex>::calculateBits() {
    size_t numBits = 0;
    for (size_t i = 0; i < diffCount; ++i)
        numBits += frequency[i] * coder.getLength(i);

    // segment start bits
    numBits += segStartBits * numSegments;

    indexMap->indexes.resize(numBits);
}

/** Write index array (in the form of huffman-coded differences and
 * segment start bits) to HuffmanIndexMap. */
template <typename TIndex>
void HuffmanMapCreator<TIndex>::encodeIndexes() {
    // Bit indexes of segment beginnings
    size_t *segmentStart = new size_t[numSegments];

    // counter of bits in the output bit vector
    size_t bitIndex = 0, segmentIndex = 0;
    // resize the output bit vector    
    size_t i, pr = 0;
    SegmentedArrayIterator it = getIterator();
    do {
        i = it.index();

        if (it.segmentStart()) {
            SegmentStart sstart;
            sstart.index = indexes[i]; sstart.end = ending[i];
            BitSequence bits = indexMap->segStartToBits(sstart);
            indexMap->indexes.setSequence(bitIndex, bits, segStartBits);                       
            segmentStart[segmentIndex++] = bitIndex;
            bitIndex += segStartBits;
        }
        else {            
            IndexDiff d; d.set(indexes[pr], indexes[i], ending[i]);            
            BitSequence bits = indexMap->diffToBits(d);
            // write the diff bits to integer
            size_t bitInt = numberFromBits<size_t>(bits, diffBits);
            // get mapped unique diff index
            TMapIter it = diffMap.find(bitInt);
            size_t diffIndex = it->second;
            HuffmanCode hcode = coder.getCode(diffIndex);           
            indexMap->indexes.setSequence(bitIndex, hcode.code, hcode.length);

            bitIndex += hcode.length;
        }

        pr = it.index();
        ++it;
    } while (it.good());

    assert(bitIndex == indexMap->indexes.getSize());
    assert(segmentIndex == numSegments);

    indexMap->segments.setArray(segmentStart, numSegments);

    delete [] segmentStart;
}

#endif	/* HUFFMANMAPCREATOR_H */

