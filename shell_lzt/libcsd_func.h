/*
 * Functionality from libCSD (https://github.com/migumar2/libCSD) 
 * for timing tests adpted to work with lztd classes. 
 */


#ifndef LIBCSD_FUNC_H
#define LIBCSD_FUNC_H

#define GET_TIME_DIVIDER ((double) 1.0 )	//getTime deals with "seconds" as time unit.
#define GET_TIME_UNIT "sec"
#define MSEC_TIME_DIVIDER  ((double) 1000.0)	//1 sec = 1000 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double) 1.0	)	//1 sec = 1 sec
#define SEC_TIME_UNIT 	  "sec"
#define MCSEC_TIME_DIVIDER  ((double)1000000)	//1 microsec = 0.000001 sec
#define MCSEC_TIME_UNIT 	  "microsec"

#include <vector>
#include <string>

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/stat.h> 
#include <sys/types.h> 

#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "node_array/compact_array/CompactArray.h"
#include "dictionary/char_trie/WordIndexer.h"

using namespace std; 

template <typename TSymbol, typename TIndex>
struct WindexTestData {
    vector<TSymbol const*> words;
    vector<TIndex> indices;
    WordIndexer<CompactArray<TSymbol, TIndex>, LzTrieIterator<CompactArray<TSymbol, TIndex> > >* windex;
};
    
inline double getTime(void)
{
	struct rusage buffer;
	struct timeval tp;
	struct timezone tzp;

	getrusage( RUSAGE_SELF, &buffer );
	gettimeofday( &tp, &tzp );

	return (double) buffer.ru_utime.tv_sec + 1.0e-6 * buffer.ru_utime.tv_usec;
}

template <typename TSymbol, typename TIndex>
void runLocateOrExtract(WindexTestData<TSymbol, TIndex>* tdata, int RUNS, bool locate)
{    
    uint numQueries = 0;
    if (locate) {
        numQueries = tdata->words.size();
        cout << "RUNNING WORD -> INDEX RESOLUTION TIMER" << endl;        
    }
    else {        
        numQueries = tdata->indices.size();
        cout << "RUNNING INDEX -> WORD RESOLUTION TIMER" << endl;
    }
    cout << numQueries << " queries, averaged over " << RUNS << " runs." << endl << flush;
   
    double t0, t1, total=0;
    WordIndexer<CompactArray<TSymbol, TIndex>, LzTrieIterator<CompactArray<TSymbol, TIndex> > >* windex = tdata->windex;
    vector<TSymbol const*> words = tdata->words;
    vector<TIndex> indices = tdata->indices;
    for (uint i=1; i<=RUNS; i++)
    {                
        t0 = getTime ();

        for (uint j=0; j<numQueries; j++)
            if (locate) windex->getIndex(words[j]);            
            else windex->getWord(indices[j]);

        t1 = (getTime () - t0);
        cout << "run " <<i<<" "<<(t1*SEC_TIME_DIVIDER) << " sec; "<< flush;
        total += t1;        
    }
    cout << endl;

    double avgrun = total/RUNS;
    double avgpattern = avgrun/numQueries;
    
    cout << "total seconds: " << (total*SEC_TIME_DIVIDER) << endl;
    cout << "avg. seconds per run: " << (avgrun*SEC_TIME_DIVIDER) << endl;
    cout << "avg. microseconds per pattern: " << (avgpattern*MCSEC_TIME_DIVIDER) << endl;    
    cout << flush;
}

#endif /* LIBCSD_FUNC_H */

