#ifndef NONCOMPRESSOR_HPP
#define	NONCOMPRESSOR_HPP

#include "ICompressor.h"

/** Compressor that does nothing, enables to handle the case of
 * the non-compressed array, in context of compression algorithm
 * without special-case handling.  */
template <typename TNodeArray>
class NonCompressor : public ICompressor<TNodeArray> {
    public:

    void compressArray(TNodeArray& array) {}

    string description() { return "no.compr"; }

};


#endif	/* NONCOMPRESSOR_HPP */

