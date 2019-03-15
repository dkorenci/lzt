#ifndef COUNTER_HPP
#define	COUNTER_HPP

#include <map>
#include <vector>
#include <iostream>

using namespace std;

/** Class that counts occurences of different values of type TVar.
 * TCount is used to store number of occurences. */
template <typename TVar, typename TCount = unsigned long>
class Counter {

public:

    typedef typename map<TVar, TCount>::iterator TIter;

    void inc(TVar value, TCount cnt = 1);
    vector< pair<TVar, TCount> >* toVector();
    void reset();

    void output(ostream& stream, bool indexes = true);

private:

    // map of occurences
    map<TVar, TCount> occ;

};

/** Increase by cnt the number assigned to value. */
template <typename TVar, typename TCount>
void Counter<TVar, TCount>::inc(TVar value, TCount cnt) {
    TIter it = occ.find(value);
    if (it != occ.end()) it->second += cnt;
    else occ[value] = cnt;
}

/** Set all counts to 0. */
template <typename TVar, typename TCount>
void Counter<TVar, TCount>::reset() {
    occ.clear();
}

/** Export pairs <value, count> as a STL vector. */
template <typename TVar, typename TCount>
vector< pair<TVar, TCount> >* Counter<TVar, TCount>::toVector() {
    vector< pair<TVar, TCount> >* vec =
            new vector< pair<TVar, TCount> >(occ.begin(), occ.end());

    return vec;
}

template <typename TVar, typename TCount>
void Counter<TVar, TCount>::output(ostream& stream, bool indexes) {
    long c = 1;
    for (TIter it = occ.begin(); it != occ.end(); ++it, ++c) {
        if (indexes) stream << c << " ";
        stream << it->first << " " << it->second << endl;
    }
}

#endif	/* COUNTER_HPP */

