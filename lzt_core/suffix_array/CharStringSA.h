#ifndef CHARSTRINGSA_H
#define	CHARSTRINGSA_H

#include <cstring>
#include <cctype>
#include <string>
#include <algorithm>
#include <stack>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace std;

class CharStringSA {
public:
    CharStringSA(const char* s);
    ~CharStringSA();

    void printSA(string fileName);
    void computeLCPInt(bool details);
    void computeLPF();

private:

    struct TLcpInt {
        static const int UNDEF = 1000000;

        int lcp, lb, rb;

        TLcpInt(int l, int left, int right):
            lcp(l), lb(left), rb(right) {}

        string toString() {
            ostringstream ss;
            ss <<lcp<<"-["<<lb<<",";             

            if  (rb == UNDEF) ss <<'*';
            else ss << rb;

            ss<<']';

            return ss.str();
        }
    };

    char* str;
    int L;
    // suffix array
    int* sarray;
    /* lcp[i] is the longest common prefix between suffixes
     * starting at sarray[i] and sarray[i+1] */
    int* lcp;

    void createSA();
    int calcLCP(const char *s1, const char *s2);
    void printStack(const stack<TLcpInt>& stck);

};

/** Comparator of suffix indexes, for sorting the suffix array. */
class IndexCompare {
public:
    IndexCompare(const char *s): str(s) {}

    // Compare suffixes starting at indexes i1 and i2.
    bool operator()(int i1, int i2) {
        while (true) {
            // suffix that is a prefix of another suffix is
            // considered larger because of '$' char larger than
            // all the others and (imaginary) appended at the end of the string
            if (str[i2] == 0) return true;
            if (str[i1] == 0) return false;

            if (str[i1] < str[i2]) return true;
            if (str[i1] > str[i2]) return false;

            i1++; i2++;
        }
    }

private:
    const char *str;

};

#endif	/* CHARSTRINGSA_H */

