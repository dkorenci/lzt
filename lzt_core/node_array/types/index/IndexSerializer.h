#ifndef INDEXSERIALIZER_H
#define	INDEXSERIALIZER_H

#include "serialization/BitSequence.h"

class IndexSerializer {
public:
    template <typename TIndex>
    static BitSequence indexToBits(TIndex i);
private:

};

#endif	/* INDEXSERIALIZER_H */

