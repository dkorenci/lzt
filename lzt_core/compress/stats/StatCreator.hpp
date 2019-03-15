#ifndef STATCREATOR_HPP
#define	STATCREATOR_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>

#include <ctime>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "util/WordFileReader.h"
#include "dictionary/util/WordList.h"
#include "util/factory.h"
#include "util/TempFile.h"
#include "compress/ICompressor.h"
#include "compress/NonCompressor.hpp"
#include "compress/CompressorFactory.hpp"
#include "compress/lz_compressor/LzCompressor.h"
#include "compress/sa_compressor/SaCompressor.hpp"
#include "node_array/compact_array/CompactArray.h"
#include "node_array/compact_array/CompactArrayCreator.h"
#include "serialization/array/CompactArraySerializer.h"
#include "node_array/na_utils.h"
#include "node_array/util/CmmExporter.h"
#include "util/etimer.h"

#include "TrieStats.hpp"

using namespace std;

/** Class that creates and outputs various compression statistics on
 * a number of trie dictionaries. Compression execution, compression testing
 * and stats creation should be decoupled at a later time. */
template <typename TNodeArray>
class StatCreator {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

public:

    StatCreator(bool stats, bool test, bool errThrow, bool cmm);

    void makeStats(string algos, string inFolder, string files, string outFolder);
    void makeDictStats(string dictFile, string outFolder);

    vector<ICompressor<TNodeArray>* > createAlgos(string desc);

private:

    /** Data collected from a run of an algorithm on a dictionary. */
    struct TRunData {
        // size of the final compressed file, in bytes
        size_t fileSize;
        // no. of nodes in compressed trie
        TIndex nodeSize;
        // running time, in seconds
        double runTime;
    };

    /** Action that will be taken upon running compression algorithms:
     * creating statistics, testing for correcnetss and throwing an 
     * TEST_EXCEPTION if test fails. */
    bool stats, test, errThrow, cmmExport;

    typedef typename vector<string>::iterator TStrIter;
    typedef typename vector<TIndex>::iterator TIndexIter;
    typedef typename vector<size_t>::iterator TSizeIter;
    typedef typename vector<ICompressor<TNodeArray>* >::iterator TAlgoIter;

    vector<string> extractFileNames(string inFolder, string files);
    void normFolder(string& folder);

    vector<ICompressor<TNodeArray>* > createAllCompressors();
    vector<ICompressor<TNodeArray>* > createAlgorithms();
    vector<ICompressor<TNodeArray>* > createMinsVersions();
    vector<ICompressor<TNodeArray>* > createLpfOnly();

    size_t getSerializedSize(TNodeArray* nodes);
    vector<string> tokenizeByWhitespace(string s);

public:

    static void testCompression(string algoDesc, const TNodeArray* trie,
                         const WordList<TSymbol>* words, string fname, bool errThrow);

};

template <typename TNodeArray>
StatCreator<TNodeArray>::StatCreator(bool s, bool t, bool ethrow, bool cmm = false):
    stats(s), test(t), errThrow(ethrow), cmmExport(cmm) {}

/** Test if all words-in-the-trie is equal to words and report error if not.
 * If errorStop == true, error throws TEST_EXCEPTION. */
template <typename TNodeArray>
void StatCreator<TNodeArray>::
testCompression(string algoDesc, const TNodeArray* trie,
                const WordList<TSymbol>* words, string fname, bool errThrow) {

    string message;
    message = "dictionary: " + fname + " compression method: " + algoDesc;

    // non-pointer variable's destructor would be called, deleting the trie
    // TODO fix, this is a memory leak
    LzTrie<TNodeArray>* lzTrie = new LzTrie<TNodeArray>(*trie);

    TSymbol emptyPrefix[1]; emptyPrefix[0] = zeroSymbol<TSymbol>();
    WordList<TSymbol>* result = lzTrie->getWordsByPrefix(emptyPrefix);

    bool wordsEq = (*result == *words);

    if (wordsEq == false) {
        if (errThrow == false) {
            cerr << "compression error with " + message << endl;
        }

        string bugFname = "bug_trie_words_" + fname;
        fstream bugWords(bugFname.c_str(), fstream::out);
        for (size_t i = 0; i < result->numberOfWords(); ++i)
            bugWords << (*result)[i] << endl;
        bugWords.close();

        if (errThrow) {
            delete result;
            TEST_ASSERT_MESSAGE(wordsEq, message);
        }
    }

    delete result;
}

/** Create statistic for specified dictionary files.
 * Test the correctness of compressed tries if test == true. */
template <typename TNodeArray>
void StatCreator<TNodeArray>::makeStats(string algoDescs, string inFolder, string files, string outFolder) {
    vector<string> dictFiles = extractFileNames(inFolder, files);
    //vector<ICompressor<TNodeArray>* > algos = createMinsVersions();//  createAlgorithms();

    vector<ICompressor<TNodeArray>* > algos = createAlgos(algoDescs);

    normFolder(inFolder);
    normFolder(outFolder);

    // column width, in characters
    const int COL_WIDTH = 30;
    // write statistics header line
    fstream statsFile;
    if (stats) {
        statsFile.open((outFolder+"stats.txt").c_str(), fstream::out);
        statsFile << setw(COL_WIDTH) << "file_name";
        for (TAlgoIter it = algos.begin(); it != algos.end(); ++it) {
            statsFile << setw(COL_WIDTH) << (*it)->description();
        }
        statsFile << endl;
        statsFile.close();
    }

    // write stats
    for (TStrIter fit = dictFiles.begin(); fit != dictFiles.end(); ++fit) {
        string file = *fit;
        // read words (lines) from file
        WordFileReader<TSymbol> reader(inFolder + file);       
        WordList<TSymbol>* words = reader.getWords();
        // run algorithms and store run data
        vector<TRunData> runs;
        for (TAlgoIter ait = algos.begin(); ait != algos.end(); ++ait) {        
//            Trie<TNodeArray>* trie = getTrie<TNodeArray>(*words);
//            TNodeArray* nodes = &(trie->exportNodeArray());

            const clock_t before = clock();
            cout << "DICTIONARY: " << file << endl;
            startEvent("everything");
            
            TNodeArray* nodes = (*ait)->compressWords(inFolder + file);            
            startEvent("bit_packing");
            CompactArrayCreator<TNodeArray> compacter(*nodes);
            CompactArray<TSymbol, TIndex>* carray =  compacter.createCompactArray();       
            endEvent("bit_packing");            
            delete carray;
            
            endEvent("everything");
            
            //(*ait)->compressArray(*nodes);
            const clock_t after = clock();
            if (cmmExport) {
                CmmExporter<TNodeArray>::writeToFile(*nodes, file+(*ait)->description()+".cmm");
            }
            //TODO dodaj trie-catch-rethrow sa delete nodes
            if (test) {
                testCompression((*ait)->description(), nodes, words, file, errThrow);
            }
            // store run data
            TRunData run;
            run.nodeSize = nodes->getSize();
            run.fileSize = getSerializedSize(nodes);
            run.runTime =  ((double)(after - before)) / CLOCKS_PER_SEC;
            runs.push_back(run);
            // free trie
            delete nodes;
            
            cout << "TIME" << endl;
            printEvents();
            clearEvents();
            cout << endl;
        }


        // write stats
        if (stats) {
            statsFile.open((outFolder+"stats.txt").c_str(), fstream::app | fstream::out);
            statsFile << setw(COL_WIDTH) <<  file ;
            for (int i = 0; i < runs.size(); ++i) {
                TRunData run = runs[i];
                stringstream ss;
                ss << run.fileSize << ' ' << run.nodeSize << setprecision(6)
                        << ' ' << run.runTime;

                statsFile << setw(COL_WIDTH) << ss.str();
            }

            statsFile << endl;
            statsFile.close();
        }

        delete words;
    }
    statsFile.close();

    // free algorithm objects
    for (TAlgoIter it = algos.begin(); it != algos.end(); ++it) {
        delete (*it);
    }

    // creation of complex-per dictionary statistic
//    for (; it != dictFiles.end(); ++it) {
//        string fname = *it;
//        string output = outFolder + fname + "_stats/";
//
//        int result = mkdir(output.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
//
//        if (result != 0 && result != EEXIST)
//            cout << "error creating folder: " + output << endl;
//        else {
//            string input = inFolder + fname;
//            makeDictStats(input, output);
//        }
//    }

}


/** Create a list of all compression algorithms, for testing. */
template <typename TNodeArray>
vector<ICompressor<TNodeArray>* > StatCreator<TNodeArray>::createAllCompressors() {
    vector<ICompressor<TNodeArray>* > algos;
    {        
        LzCompressor<TNodeArray>* lzCompr = new LzCompressor<TNodeArray>();
        algos.push_back(lzCompr);

        LzCompressorConfig conf; conf.pointerBreak = true;
        lzCompr = new LzCompressor<TNodeArray>(conf);
        algos.push_back(lzCompr);
    }

    {
        // plan sequential pairwise replacement, with recursion
        SaCompressorConfig conf;
        conf.algorithm = SEQ_PAIRWISE;
        conf.recReplace = true;
        SaCompressor<TNodeArray>* saCompr = new SaCompressor<TNodeArray>(conf);
        algos.push_back(saCompr);
    }

    {
        // simple LCP intervals with and without recursion
        SaCompressorConfig conf;
        conf.algorithm = LCP_INT;
        conf.recReplace = false;
        SaCompressor<TNodeArray>* saCompr = new SaCompressor<TNodeArray>(conf);
        algos.push_back(saCompr);

        conf.recReplace = true;
        saCompr = new SaCompressor<TNodeArray>(conf);
        algos.push_back(saCompr);
    }

    {
        SaCompressorConfig conf;
        conf.algorithm = LPF;
        SaCompressor<TNodeArray>* saCompr = new SaCompressor<TNodeArray>(conf);
        algos.push_back(saCompr);
    }


    return algos;
}

/** Construct and return an array of objects each representing different
 * compression algorithm (strategy pattern). */
template <typename TNodeArray>
vector<ICompressor<TNodeArray>* > StatCreator<TNodeArray>::createAlgorithms() {
    vector<ICompressor<TNodeArray>* > algos;

    {
        NonCompressor<TNodeArray>* noc = new NonCompressor<TNodeArray>;
        algos.push_back(noc);
    }

    {
        LzCompressor<TNodeArray>* lzCompr = new LzCompressor<TNodeArray>();
        algos.push_back(lzCompr);
    }

    {
        // plan sequential pairwise replacement, with recursion
        SaCompressorConfig conf;
        conf.algorithm = SEQ_PAIRWISE;
        conf.recReplace = true;
        SaCompressor<TNodeArray>* saCompr = new SaCompressor<TNodeArray>(conf);
        algos.push_back(saCompr);
    }

    {
        // simple LCP intervals with and without recursion
        SaCompressorConfig conf;
        conf.algorithm = LCP_INT;
        conf.recReplace = false;
        SaCompressor<TNodeArray>* saCompr = new SaCompressor<TNodeArray>(conf);
        algos.push_back(saCompr);

        conf.recReplace = true;
        saCompr = new SaCompressor<TNodeArray>(conf);
        algos.push_back(saCompr);
    }

    return algos;
}

/** Construct and return an array of objects each representing different
 * compression algorithm (strategy pattern). */
template <typename TNodeArray>
vector<ICompressor<TNodeArray>* > StatCreator<TNodeArray>::createMinsVersions() {
    vector<ICompressor<TNodeArray>* > algos;

    // add non-compressor
    {
        NonCompressor<TNodeArray>* noc = new NonCompressor<TNodeArray>;
        algos.push_back(noc);
    }
    // add lz-compressor
    {
        LzCompressor<TNodeArray>* lzCompr = new LzCompressor<TNodeArray>();
        algos.push_back(lzCompr);
    }

    return algos;
}

template <typename TNodeArray>
vector<ICompressor<TNodeArray>* > StatCreator<TNodeArray>::createLpfOnly() {
    vector<ICompressor<TNodeArray>* > algos;
    
    {
        SaCompressorConfig conf;
        conf.algorithm = LPF;
        SaCompressor<TNodeArray>* saCompr = new SaCompressor<TNodeArray>(conf);
        algos.push_back(saCompr);
    }

    return algos;
}


/** Get size of the node array serialized to file. */
template <typename TNodeArray>
size_t StatCreator<TNodeArray>::getSerializedSize(TNodeArray* nodes) {
    CompactArrayCreator<TNodeArray> compactCreator(*nodes);
    CompactArray<TSymbol, TIndex>* compactArray = compactCreator.createCompactArray();

    TempFile file;

    fstream stream(file.getName());
    CompactArraySerializer<TSymbol, TIndex> ser(compactArray);
    ser.arrayToStream(stream);
    stream.close();
    delete compactArray;

    struct stat buf;
    stat(file.getName(), &buf);
    size_t size = buf.st_size;

    return size;
}

/** Create vector of compression algorithms from string description. */
template <typename TNodeArray>
vector<ICompressor<TNodeArray>* > StatCreator<TNodeArray>::createAlgos(string desc) {
    vector<ICompressor<TNodeArray>* > algos;

    vector<string> descs = tokenizeByWhitespace(desc);

    if (descs.empty()) algos = createAllCompressors();
    else {        
        for (TStrIter it = descs.begin(); it != descs.end(); ++it) {
            algos.push_back( CompressorFactory<TNodeArray>::createFromDesc(*it) );
        }  
    }

    return algos;
}

template <typename TNodeArray>
vector<string> StatCreator<TNodeArray>::tokenizeByWhitespace(const string s) {
    //whitespaces
    string ws = " \n\t";
    vector<string> tokens;
    /* Tokenize if string is not empty. */
    if (s.find_first_not_of(ws) != string::npos) {
        size_t start = 0, end;
        while ((start = s.find_first_not_of(ws, start)) != string::npos) {
            end = s.find_first_of(ws, start);
            if (end == string::npos) end = s.size();
            string t = s.substr(start, end - start);
            tokens.push_back(t);
            start = end;
        }
    }

    return tokens;
}

/** Create list of file names from input. If files is empty, return all the
 * files in the folder. If not, get names by tokenizing files by witespace. */
template <typename TNodeArray> vector<string>
StatCreator<TNodeArray>::extractFileNames(string inFolder, string files) {
    // list of dictionary file names
    vector<string> fileNames = tokenizeByWhitespace(files);

    if (fileNames.empty()) {
        /** No files given, make file list from all files in inFolder. */
        normFolder(inFolder);
        struct dirent *ent;
        DIR *dir = opendir (inFolder.c_str());
        if (dir != NULL) {
          // list ordinary files in the directory
          while ((ent = readdir (dir)) != NULL)
              if (ent->d_type == DT_REG) {
                  string fname(ent->d_name);                  
                  fileNames.push_back(fname);
              }
         
          closedir (dir);
        }
        else cout << "error opening folder" << endl;
    }

    return fileNames;
}

/** 'Normalize' folder name by adding '/' at the end, if it does not
 * exist and if the name is not empty. */
template <typename TNodeArray>
void StatCreator<TNodeArray>::normFolder(string& folder) {
    //whitespaces
    string ws = " \n\t";
    if (folder.find_first_not_of(ws) == string::npos) folder = "";
    else {
        size_t s = folder.size();
        if (folder[s-1] != '/') folder = folder + "/";
    }
}

template <typename TNodeArray>
void StatCreator<TNodeArray>::makeDictStats(string file, string outFolder) {
    WordFileReader<TSymbol> reader(file);
    WordList<TSymbol>* words = reader.getWords();

    Trie<TNodeArray>* trie = getTrie<TNodeArray>(*words);

    TrieStats<TNodeArray> stats;
    stats.computeStats(&(trie->getNodeArray()));    

    int N = 3;
    STAT_TYPE type[3] = {NODE_CNT, SYMBOL_CNT, OFFSET_CNT};
    string fname[3] = {"nodes.txt", "symbols.txt", "offsets.txt"};

    for (int i = 0; i < N; ++i) {
        fstream file1((outFolder+fname[i]).c_str(), fstream::out);
        stats.outputStats(file1, type[i]);
        file1.close();
    }



    delete trie;
    delete words;
}

#endif	/* STATCREATOR_HPP */

