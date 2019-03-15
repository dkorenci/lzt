//STANDALONE UTILITY FUNCTIONS

#include "utils.h"

#include <ctime>

void randomSeed() {
    srand(time(0));
}

/** Generate a random string that is very likely to be universally unique. */
string getRandomString() {
    typedef unsigned long TRange;
    TRange min = 1000000ul, max = 1000000000ul;
    ostringstream ss;

    ss << "rand_" << time(NULL) << getRandomNumber(min, max);

    return ss.str();
}
