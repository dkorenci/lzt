#ifndef BITSEQUENCEARRAYSER_H
#define	BITSEQUENCEARRAYSER_H

#include <iostream>

#include "serialization/BitSequenceArray.h"
#include "serialization/BitSequence.h"

/** Serializes BitSequenceArray to and deserializes from stream objects. */
class BitSequenceArraySer {
public:
    virtual ~BitSequenceArraySer() {};
    //TODO add serialization test
    static void arrayToStream(BitSequenceArray const & array, ostream & stream);
    static void arrayFromStream(BitSequenceArray &array, istream& stream);

private:

};

#endif	/* BITSEQUENCEARRAYSER_H */

