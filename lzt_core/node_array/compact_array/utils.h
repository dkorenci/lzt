#ifndef COMPACTARRAYUTILS_H
#define	COMPACTARRAYUTILS_H

/** Returns a code of node flags as a number in [0,3] ,
 * represented by (cow, eow) in binary. */
inline int nodeFlagsToInt(bool eow, bool cow) {
    int fl = 0;
    if (eow) fl += 1;
    if (cow) fl += 2;
    return fl;
}

/** Return eow bit from number representation of flags. */
inline bool intFlagsEow(int flags) {
    return (flags == 1 || flags == 3);
}

/** Return cow bit from number representation of flags. */
inline bool intFlagsCow(int flags) {
    return (flags == 2 || flags == 3);
}

#endif	/* COMPACTARRAYUTILS_H */

