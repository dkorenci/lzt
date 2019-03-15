#include "HuffmanTest.h"
#include "../HuffmanCodecCreator.h"
#include "../HuffmanCoder.h"
#include "../HuffmanDecoder.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "debug/lzt_test.h"
#include "serialization/BitSequence.h"
#include "serialization/BitVector.h"
#include "serialization/IntBitArray.h"
#include "util/TempFile.h"

HuffmanTest::~HuffmanTest() {
}

/** Test calculation of cannonical huffman codes by reading a list of
 * symbol frequencies and comparing calculated codes with correct ones. */
void HuffmanTest::testCoderCreation() {
    fstream file("dictionary/lztrie_dict/huffman/utest/coder_test.txt", ios_base::in);
    TEST_ASSERT(file.good());
    int numTests;
    file >> numTests;
    for (int i = 0; i < numTests; ++i) {
        int numSymbols;

        // read symbol frequencies and calculate huff. codes
        file >> numSymbols;
        size_t freqs[numSymbols];
        for (int j = 0; j < numSymbols; ++j) file >> freqs[j];

        HuffmanCodecCreator creator(numSymbols, freqs);
        HuffmanCoder coder;
        creator.createCoder(&coder);

        ostringstream m;
        string code[numSymbols];
        m << "test case #: " << i+1 << endl;
        // read expected codes and output calculated codes
        for (int j = 0; j < numSymbols; ++j) {
            file >> code[j];
            HuffmanCode hcode = coder.getCode(j);
            //string c =  fromBitSequence<size_t>(hcode.code);
            m << "symbol: " << j 
              << " code: " << hcode.code.toString().substr(0, hcode.length) << endl;
        }

        // compare calculated to expected codes
        for (int j = 0; j < numSymbols; ++j) {
            ostringstream m2;
            HuffmanCode hcode = coder.getCode(j);
            string c = hcode.code.toString().substr(0, hcode.length);           
            m2 << "calculated codes: " << endl << m.str()
               << "error index: " << j << " exp. code: " << code[j] << " code: " << c << endl;
            TEST_ASSERT_MESSAGE(code[j] == c, m2.str());
        }        
    }
}

/** Test huffman codes to symbols mapping. Test case consist of:
 * number of symbols, symbol frequencies and values, one pair per line.
 * Than follows size of firstCode and lengthOffset arrays, and values that
 * must occur in firstCode, lengthOffset, and symbolTable arrays. */
void HuffmanTest::testDecoderCreation() {
    int numTests;
    fstream file("dictionary/lztrie_dict/huffman/utest/decoder_test.txt", ios_base::in);
    TEST_ASSERT(file.good());
    
    // numerical symbol codes must fit within these bits
    const int BITS_PER_SYMBOL = 30;
    file >> numTests;

    for (int i = 0; i < numTests; ++i) {
        int numSymbols;
        file >> numSymbols;
        // read frequencies and symbol codes
        size_t freqs[numSymbols]; size_t symbols[numSymbols];
        
        for (int j = 0; j < numSymbols; ++j)
            file >> freqs[j] >> symbols[j];

        // fill bit array with symbol codes
        BitSequenceArray symArray(numSymbols, BITS_PER_SYMBOL);
        for (int j = 0; j < numSymbols; ++j)
            symArray.setSequence(j, numberToBits<size_t>(symbols[j]));

        // create decoder
        HuffmanCodecCreator creator(numSymbols, freqs);
        HuffmanDecoder *decoder = new HuffmanDecoder;
        creator.createDecoder(decoder, symArray);

        ostringstream m;
        m << "test case #: " << i+1 << endl << "calculated decoder state:" << endl;

        // output the state of the decoder
        m << "first code, size: " << (decoder->firstCode).getSize() << endl;
        for (int j = 0; j < (decoder->firstCode).getSize(); ++j)
            m << (decoder->firstCode)[j] << " ";
        m << endl;

        m << "offset, size: " << (decoder->lengthOffset).getSize() << endl;
        for (int j = 0; j < (decoder->lengthOffset).getSize(); ++j)
            m << (decoder->lengthOffset)[j] << " ";
        m << endl;

        m << "symbols: " << endl;
        for (int j = 0; j < (decoder->symbolTable).getNumOfSequences(); ++j) {
            int numBits = (decoder->symbolTable).getSequenceSize();
            m << numberFromBits<size_t>((decoder->symbolTable)[j], numBits) << " ";
        }
        m << endl;

        //cout << m.str();

        // check the arrays
        size_t arraysSize; file >> arraysSize;
        size_t firstCode[arraysSize], offset[arraysSize];

        for (int j = 0; j < arraysSize; ++j) file >> firstCode[j];
        for (int j = 0; j < arraysSize; ++j) file >> offset[j];

        TEST_ASSERT_MESSAGE(arraysSize == (decoder->lengthOffset).getSize(), m.str());
        TEST_ASSERT_MESSAGE(arraysSize == (decoder->firstCode).getSize(), m.str());
         
        for (int j = 0; j < arraysSize; ++j) {
            ostringstream m2;
            m2 << m.str() << " mismatch index: " << j << endl;
            TEST_ASSERT_MESSAGE(firstCode[j] == (decoder->firstCode)[j], m2.str());
            TEST_ASSERT_MESSAGE(offset[j] == (decoder->lengthOffset)[j], m2.str());
        }

        // check the symbols
        size_t symTable[numSymbols];
        for (int j = 0; j < numSymbols; ++j) file >> symTable[j];

        for (int j = 0; j < numSymbols; ++j) {
            ostringstream m2;
            m2 << m.str() << " mismatch index: " << j << endl;
            int numBits = (decoder->symbolTable).getSequenceSize();
            TEST_ASSERT_MESSAGE (
            symTable[j] == numberFromBits<size_t>((decoder->symbolTable)[j], numBits) ,
                m2.str() );
        }

        delete decoder;
    }
}

void HuffmanTest::testCodingAndDecoding() {
    codeAndDecodeCases(false);
}

void HuffmanTest::testDecoderSerialization() {
    codeAndDecodeCases(true);
}

/** Test coding and decoding cycle on arrays of symbols generated
 * randomly form lists of symbol frequencies writen in a test case file.
 * If serialize is true, test by first serializing and then deserializing
 * the decoder to a temporary file. */
void HuffmanTest::codeAndDecodeCases(bool serialize) {
    int numTests;
    fstream file("dictionary/lztrie_dict/huffman/utest/codec_test.txt");
    TEST_ASSERT(file.good());

    file >> numTests;
    for (int i = 0; i < numTests; ++i) {
        size_t numSymbols;
        file >> numSymbols;
        
        size_t freqs[numSymbols];
        // size of set of symbols, where i-th symbol each occurs freqs[i] times
        size_t size = 0;
        for (int j = 0; j < numSymbols; ++j) {
            file >> freqs[j];
            size += freqs[j];
        }        

        // create array of indexes of symbols
        size_t array[size]; size_t cnt = 0;
        for (int j = 0; j < numSymbols; ++j)
            for (size_t k = 0; k < freqs[j]; ++k)
                array[cnt++] = j;
        TEST_ASSERT(cnt == size);
        random_shuffle(array, array + size);
        // output array
        ostringstream arr;
        arr<<"array: "<<endl;
        for (size_t j = 0; j < size; ++j) arr << array[j]<<" ";
        arr << endl;

        // start coding
        HuffmanCodecCreator hcreator(numSymbols, freqs);
        HuffmanCoder *coder = new HuffmanCoder;
        hcreator.createCoder(coder);
        // calculate number of all bits in the coded array
        size_t numBits = 0;
        for (int j = 0; j < numSymbols; ++j)
            numBits += (coder->getCode(j)).length * freqs[j];
        // code the array to the BitVector
        BitVector bits(numBits); size_t bitIndex = 0;
        for (size_t j = 0; j < size; ++j) {
            HuffmanCode hcode = coder->getCode(array[j]);            
            for (int k = 0; k < hcode.length; ++k)
                bits.setBit(bitIndex++, hcode.code[k]);
        }        
        TEST_ASSERT(bitIndex == numBits);
        delete coder;
        // write bits
        ostringstream bitOut;
        bitOut<<"coded array: "<<endl;
        for (size_t j = 0; j < numBits; ++j) bitOut << bits[j]?'1':'0';
        bitOut<<endl;

        // decode the array
        
        // create decoder
        size_t symbols[numSymbols];
        for (int j = 0; j < numSymbols; ++j) symbols[j] = j;
        IntBitArray<size_t> symBits(symbols, numSymbols);
        HuffmanDecoder* decoder = new HuffmanDecoder;
        hcreator.createDecoder(decoder, symBits.getBits());

        // write and read decoder from a file
        if (serialize) {
            TempFile file;
            // serialize
            fstream fstr(file.getName());
            decoder->writeToStream(fstr);
            fstr.close();
            // deserialize            
            fstr.open(file.getName());
            decoder->readFromStream(fstr);
            fstr.close();
        }

        // do decoding
        ostringstream decoding;
        size_t decodedIndex = 0;
        for (bitIndex = 0; bitIndex < numBits; ++bitIndex) {
            decoding<<bits[bitIndex]?'1':'0';
            decoder->processBit(bits[bitIndex]);
            if (decoder->isEndOfCode()) {
                size_t symbol = 
                numberFromBits<size_t>(decoder->getSymbol(), decoder->bitsPerSymbol());
                decoding << ": " << symbol << endl;
                
                ostringstream m2;
                m2 << "decoded symbol: " << symbol << " expected: " << array[decodedIndex] << endl;
                m2 << "serialization: " << serialize << endl;
                TEST_ASSERT_MESSAGE(symbol == array[decodedIndex], 
                        arr.str() + bitOut.str() + decoding.str() + m2.str());

                decodedIndex++;
            }
        }
        TEST_ASSERT(decodedIndex == size);

        delete decoder;
    }
}