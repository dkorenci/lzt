#ifndef HUFFMANINDEXMAPTEST_H
#define	HUFFMANINDEXMAPTEST_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../HuffmanIndexMap.h"
#include "../HuffmanMapCreator.h"
#include "debug/lzt_debug.h"
#include "serialization/BitVector.h"
#include "util/utils.h"
#include "util/TempFile.h"

using namespace std;

class HuffmanIndexMapTest {
public:
    virtual ~HuffmanIndexMapTest();
    
    void testMap();
    void testSerialization();

private:


};

template <typename TIndex> class HuffIndexMapTester {
public:
    void testMap(string fname, bool deser);
    void generateFileRandom(string fname, TIndex N, TIndex M, int G);

private:
    HuffmanIndexMap<TIndex>* serializationCycle(HuffmanIndexMap<TIndex>* map);
};

/** Test by reading mappings from the file, creating HuffmanIndexMaps, optionaly
 * serializing and deserializing them, and asserting equality with mappings. */
template <typename TIndex>
void HuffIndexMapTester<TIndex>::testMap(string fname, bool deser) {
    string folder = "dictionary/lztrie_dict/utest/";
    fstream file((folder + fname).c_str());
    TEST_ASSERT(file.good());
    int numTests;
    file >> numTests;
    for (int i = 0; i < numTests; ++i) {
        // number of indexes being mapped from
        TIndex N; file >> N;
        // number of mapped to values in the file
        size_t M; file >> M;
        // description of the input mapping, indexes and endings of sequences
        BitVector endingsIn(M);
        TIndex *indexIn = new TIndex[M];
        size_t inCnt = 0;
        for (TIndex j = 0; j < N; ++j) {
            // number of indexes mapped to from j
            TIndex n; file >> n;
            for (TIndex k = 0; k < n; ++k) {
                TIndex index; file >> index;
                endingsIn.setBit(inCnt, false);
                indexIn[inCnt++] = index;
            }
            endingsIn.setBit(inCnt-1, true);
        }
        // test that M parameter is correct
        TEST_ASSERT(inCnt == M);
        // number of segment sizes
        int S; file >> S;
        // segment sizes
        int ssize[S]; for (int j = 0; j < S; ++j) file >> ssize[j];

        // test creation of HuffmanIndexMap for read mapping and all segment sizes
        for (int j = 0; j < S; ++j) {
            HuffmanMapCreator<TIndex> creator(indexIn, inCnt, endingsIn, ssize[j]);
            HuffmanIndexMap<TIndex>* map = new HuffmanIndexMap<TIndex>();
            creator.createHuffmanMap(map);
            if (deser) map = serializationCycle(map);
            // check equality of the created map and read map
            BitVector endingsOut(M);
            TIndex* indexOut = new TIndex[M];
            // count
            size_t outCnt = 0;
            ostringstream m;
            m << "file: " << fname << endl;
            m << "test case: " << i << " segment size: " << ssize[j] << endl;
            m << "map: " << endl;

            for (TIndex k = 0; k < N; ++k) {
                vector<TIndex> vals = map->getIndexes(k);
                m << k << " : ";
                for (TIndex l = 0; l < vals.size(); ++l) {
                    endingsOut.setBit(outCnt, false);
                    indexOut[outCnt++] = vals[l];
                    m << vals[l] << " ";
                }
                endingsOut.setBit(outCnt-1, true);
                m << endl;
            }

            TEST_ASSERT_MESSAGE(outCnt == M, m.str());

            for (size_t k = 0; k < M; ++k) {
                TEST_ASSERT_MESSAGE(endingsIn[k] == endingsOut[k], m.str());
                TEST_ASSERT_MESSAGE(indexIn[k] == indexOut[k], m.str());
            }

            delete [] indexOut;
            delete map;

            //cout << m.str();
        }        

        delete [] indexIn;
    }    
}

/** Serialize map to file, free it, return deserialized map. */
template <typename TIndex> HuffmanIndexMap<TIndex>*
HuffIndexMapTester<TIndex>::serializationCycle(HuffmanIndexMap<TIndex>* map) {
    TempFile file;
    // serialize
    fstream stream(file.getName());
    map->writeToStream(stream);
    stream.close();
    // deserialize
    delete map;
    map = new HuffmanIndexMap<TIndex>();
    stream.open(file.getName());
    map->readFromStream(stream);
    stream.close();

    return map;
}

/** Generates a random generated test file with mapping from [0,..,N-1]
 * to subset of [0,..,M-1], with max. G elemets per index. */
template <typename TIndex>
void HuffIndexMapTester<TIndex>::generateFileRandom(string fname, TIndex N, TIndex M, int G) {
    string folder = "dictionary/lztrie_dict/utest/";
    fstream file((folder + fname).c_str(), ios_base::out);
    randomSeed();

    vector<vector<TIndex> > map(N);

    size_t numInd = 0;
    for (TIndex i = 0; i < N; ++i) {
        int groupSize = getRandomNumber(1, G);
        numInd += groupSize;
        for (int j = 0; j < groupSize; ++j) map[i].push_back( getRandomNumber(0, M-1) );
    }
    // one test case per file
    file << "1" << endl << endl;
    file << N << " " << numInd << endl;
    for (TIndex i = 0; i < N; ++i) {
        file << map[i].size();
        for (int j = 0; j < map[i].size(); ++j)
            file << " " << map[i][j];
        file << endl;
    }

    file.close();
}

#endif	/* HUFFMANINDEXMAPTEST_H */

