#ifndef HUFFMANTEST_H
#define	HUFFMANTEST_H

/** Unit tests for Huffman (de)coding functionality. */
class HuffmanTest {
public:
    
    virtual ~HuffmanTest();

    void testCoderCreation();
    void testDecoderCreation();
    void testCodingAndDecoding();
    void testDecoderSerialization();

private:

    void codeAndDecodeCases(bool serialize);

};

#endif	/* HUFFMANTEST_H */

