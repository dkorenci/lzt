#ifndef SAISCREATOR_HPP
#define	SAISCREATOR_HPP

#include <cstdlib>
#include <iostream>
#include <limits>

static unsigned char msk[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

#define tget(i) ( (t[(i)/8]&msk[(i)%8]) ? 1 : 0 )
#define tset(i, b) t[(i)/8]=(b) ? (msk[(i)%8]|t[(i)/8]) : ((~msk[(i)%8])&t[(i)/8])
//#define chr(i) (cs==sizeof(int)?((int*)s)[i]:((unsigned char *)s)[i])
#define isLMS(i) (i>0 && (i != EMPTY<TInt>()) && tget(i) && !tget(i-1))

using namespace std;

template <typename TInt>
inline TInt EMPTY() {
    return numeric_limits<TInt>::max();
}

// find the start or end of each bucket
// bucket corresponds to a range in SA with suffixes having the same character
template <typename TChar, typename TInt>
void getBuckets(TChar *s, TInt *bkt, TInt n, TInt K, bool end) {
    TInt i, sum = 0;
    for (i = 0; i <= K; i++) bkt[i] = 0; // clear all buckets
    for (i = 0; i < n; i++) bkt[s[i]]++; // compute the size of each bucket
    for (i = 0; i <= K; i++) {
        sum += bkt[i];
        bkt[i] = end ? sum : sum - bkt[i];
    }
}

// compute SAl
// put L chars in s in their positions in SA, put them in a bucket (range)
// corresponding to first char, starting from start of the bucket
template <typename TChar, typename TInt>
void induceSAl(unsigned char *t, TInt *SA, TChar *s, TInt *bkt,
        TInt n, TInt K, bool end) {
    TInt i, j;
    getBuckets(s, bkt, n, K, end); // find starts of buckets
    for (i = 0; i < n; i++) {
        if (SA[i] > 0 && SA[i] != EMPTY<TInt>()) { // added for the case when TInt is unsigned
            j = SA[i] - 1;
            if (j >= 0 && !tget(j)) SA[bkt[s[j]]++] = j;
        }
    }
}

// compute SAs
// put S chars in s in their positions in SA, put them in a bucket (range)
// corresponding to first char, starting from end of the bucket
template <typename TChar, typename TInt>
void induceSAs(unsigned char *t, TInt *SA, TChar *s, TInt *bkt,
        TInt n, TInt K, bool end) {
    TInt i, j;
    getBuckets(s, bkt, n, K, end); // find ends of buckets
    for (i = n - 1; i >= 0; i--) {
        if (SA[i] > 0 && SA[i] != EMPTY<TInt>()) { // added for the case when TInt is unsigned
            j = SA[i] - 1;
            if (j >= 0 && tget(j)) SA[--bkt[s[j]]] = j;
        }
        if (i == 0) break; // for the case when TInt is unsigned
    }
}

// find the suffix array SA of s[0..n-1] in {1..K}^n
// require s[n-1]=0 (the sentinel!), n>=2
// use a working space (excluding s and SA) of at most 2.25n+O(1) for a constant alphabet
// K is alphabet size, cs is character size (s can be a string of integers or characters)
// since SA_IS is recursive and on levels >= 1 , array of integers is sorted
template <typename TChar, typename TInt>
void SA_IS(TChar *s, TInt *SA, TInt n, TInt K) {
    TInt i, j;
    unsigned char *t = (unsigned char *) malloc(n / 8 + 1); // LS-type array in bits    
    // Classify the type of each character
    tset(n - 2, 0);
    tset(n - 1, 1); // the sentinel must be in s1, important!!!    
    for (i = n - 3; i >= 0; i--) {
        tset(i, (s[i] < s[i + 1] || (s[i] == s[i + 1] && tget(i + 1) == 1)) ? 1 : 0);
        if (i == 0) break; // for the case when TInt is unsigned integer type
    }
    // stage 1: reduce the problem by at least 1/2
    // sort all the S-substrings
    TInt *bkt = (TInt *) malloc(sizeof (TInt) *(K + 1)); // bucket array
    getBuckets(s, bkt, n, K, true); // find ends of buckets
    for (i = 0; i < n; i++) SA[i] = EMPTY<TInt>();
    for (i = 1; i < n; i++) // put LMS characters in corresponding buckets (at end)
        if (isLMS(i)) SA[--bkt[s[i]]] = i;
    // put L characters at starts of their buckets
    induceSAl(t, SA, s, bkt, n, K, false);
    // induce sort LMS prefixes
    induceSAs(t, SA, s, bkt, n, K, true);
    free(bkt);
    // according to the paper, Theorem 2.1: The above modified induced sorting
    // algorithm will correctly sort all the !non-size-one! LMS-prefixes and the sentinel.

    // but all the LMS chars should be in consecutive positions,
    // because the following code relies on this fact

    // compact all the sorted substrings into the first n1 items of SA
    // 2*n1 must be not larger than n (proveable)
    TInt n1 = 0;
    for (i = 0; i < n; i++)
        if (isLMS(SA[i])) SA[n1++] = SA[i];
    // now indexes of all the LMS chars are at first n1 positions

    // find the lexicographic names of all substrings
    for (i = n1; i < n; i++) SA[i] = EMPTY<TInt>(); // init the name array buffer
    TInt name = 0, prev = EMPTY<TInt>();
    for (i = 0; i < n1; i++) {
        TInt pos = SA[i];
        bool diff = false;
        // compare if prev and pos are equal LMS substrings
        for (TInt d = 0; d < n; d++) {
            if (prev == EMPTY<TInt>() || s[pos + d] != s[prev + d] || tget(pos + d) != tget(prev + d)) {
                diff = true;
                break;
            } else if (d > 0 && (isLMS(pos + d) || isLMS(prev + d))) break;
        }
    
        if (diff) {
            name++;
            prev = pos;
        }
        // calculate position for LMS substring so that order of these
        // positions corresponds with substrings order in the string
        pos = (pos % 2 == 0) ? pos / 2 : (pos - 1) / 2;
        SA[n1 + pos] = name - 1;
    }
    // shift LMS substrings to the end of SA array, without gaps
    for (i = n - 1, j = n - 1; i >= n1; i--)
        if (SA[i] != EMPTY<TInt>() && SA[i] >= 0) SA[j--] = SA[i];
    // stage 2: solve the reduced problem
    // recurse if names are not yet unique
    TInt *SA1 = SA, *s1 = SA + n - n1;
    if (name < n1)
        SA_IS(s1, SA1, n1, name - 1);
    else // generate the suffix array of s1 directly
        for (i = 0; i < n1; i++) SA1[s1[i]] = i;
    // stage 3: induce the result for the original problem
    bkt = (TInt *) malloc(sizeof (TInt) *(K + 1)); // bucket array
    // put all left-most S characters into their buckets
    getBuckets(s, bkt, n, K, true); // find ends of buckets
    for (i = 1, j = 0; i < n; i++)
        if (isLMS(i)) s1[j++] = i; // get p1
    for (i = 0; i < n1; i++) SA1[i] = s1[SA1[i]]; // get index in s
    for (i = n1; i < n; i++) SA[i] = EMPTY<TInt>(); // init SA[n1..n-1]
    for (i = n1 - 1; i >= 0; i--) {
        j = SA[i];
        SA[i] = EMPTY<TInt>();
        SA[--bkt[s[j]]] = j;
        if (i == 0) break; // for the case where TInt is unsigned integer type
    }
    induceSAl(t, SA, s, bkt, n, K, false);
    induceSAs(t, SA, s, bkt, n, K, true);
    free(bkt);
    free(t);
}



#endif	/* SAISCREATOR_HPP */

