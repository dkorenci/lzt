#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

using namespace std;

#include "util/WordFileReader.h"
#include "util/factory.h"
#include "util/regex.h"
#include "util/Timer.h"
#include "dictionary/util/WordList.h"
#include "node_array/vector_array/VectorArray.h"
#include "node_array/compact_array/CompactArray.h"
#include "node_array/compact_array/CompactArrayCreator.h"
#include "serialization/array/CompactArraySerializer.h"
#include "node_array/util/CmmExporter.h"
#include "compress/stats/StatCreator.hpp"
#include "libcsd_func.h"

map<string, string> params;

typedef long TIndex;
typedef unsigned char TSymbol;
//typedef short int TSymbol;
//typedef int TSymbol;
typedef VectorArray<TSymbol, TIndex> TNodeArray;
typedef CompactArray<TSymbol, TIndex> TCompactArray;

static void printUsage();
static void compress();
static void doCompress(string input_file, string output_file);
static void search();
static void test();
static void intersect();
static void batchRun();
static void createParameterMap(int argc, char** argv);
static TSymbol* stringToTSymbolString(string& str);
WindexTestData<TSymbol, TIndex>* testDataFromCompactArray(string fname, int n, bool check);

/** Check number of arguments, then invoke the main command
 * depending on the first argument. */
int main(int argc, char** argv) {        
    if (argc < 2) {
        printUsage();
    }
    else {
        string command = argv[1];        
        createParameterMap(argc-2, argv+2);
        if (command == "c") {
            compress();
        }
        else if (command == "s") {
            search();
        }
        else if (command == "i") {
            intersect();
        }
        else if (command == "b") {
            batchRun();
        }
        else if (command == "t") {
            test();
        }
        else {
            printUsage();
        }
    }

    return 0;
}

void createParameterMap(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        string arg = argv[i];
        // arguments for wich parameters are expected
        if (arg == "-i" || arg == "-d" || arg == "-s" || 
                arg == "-a" || arg == "-f" || arg == "-o" || arg == "-r" || arg == "-n") {
            if (i < argc - 1) {
                string val = argv[i+1];
                if (val.size() > 0 && val[0] != '-') params[arg] = val;
            }
        } else {
            // flag arguments
            if (arg == "-cmm") params[arg] = "rjecnik.cmm";
            else if (arg == "-st") params[arg] = "true";
            else if (arg == "-t") params[arg] = "true";
            else if (arg == "-e") params[arg] = "true";
            else if (arg == "-l") params[arg] = "true";
            else if (arg == "-c") params[arg] = "true";
            else if (arg == "-z") params[arg] = "true";
        }
    }
    // print parameters
//    for (map<string, string>::iterator it = params.begin(); it != params.end(); ++it) {
//        cout<<(*it).first<<" "<<(*it).second<<endl;
//    }
}

void printUsage() {
    cout<<endl<<"lzt lexicon utility"<<endl<<endl
        <<"compress: lzt c [-e] -i input_file -d dict_file [-cmm]"<<endl
        <<"   Use -cmm flag to store compressed trie in .cmm file"<<endl
        <<"   Use -e flag to create enumerated trie (that can map words to integer indices)"<<endl<<endl
        <<"search: lzt s -d dict_file [-s search_term]"<<endl
        <<"   If search term is ommited, read search queries from standard input."<<endl
        <<"   If the queries are read from stdin, '^' terminates the input."<<endl
        <<"   Search term is either a word or word*, * must be the last char and can be the only char. "<<endl
        <<"   Do not forget to escape * with a '\\' if necessary!"<<endl<<endl
        <<"intersect: lzt i -d dict_file -i input_file "<<endl
        <<"   For each word in the input file print the word on stdout if it is in the dictionary."<<endl<<endl
        <<"batch: lzt b -a algorithms [-d dict_folder] [-f dict_files] [-o out_folder] [-st][-t][-e]"<<endl
        <<"   Do a batch run, ie run each of the specified algorithms on each od the dict_files" << endl
        <<"   inside dict_folder, or all files in it if dict_files is ommited. " << endl
        <<"   For -st, write statistics, for -t, test, ie compare words in compressed dictionary" << endl
        <<"   with original  files, and for -e end run when first (comparison) error occurs. " << endl << endl
        <<"time: lzt t -d dict_file [-r num_runs] [-n num_queries] [-l] [-e] [-c]"<<endl
        <<"   time operations of locating words and/or extracting words from the compressed dict" << endl
        <<"   if -l do locate (find index for word), if -e do extract (find word for index) " << endl
        <<"   -c performs additional check that all the words match all the indices in dict" << endl
        <<"   -n specifies number of queries (words and respective indices) selected at random from dict (default 1000000) " << endl
        <<"   -r specifies number of runs the queries are run (default 10). " << endl << endl;        
}

void printWordList(string query, LzTrie<CompactArray<TSymbol, TIndex> >* lzTrie) {
    // convery string of chars to string of TSymbols
    TSymbol *queryTS = stringToTSymbolString(query);
    //cout<<"start printing"<<endl;
    if (query.find('*') != string::npos) { // list words
        TSymbol prefix[1000];      
        bool syntaxOk = getPrefixBeforeStar<TSymbol>(queryTS, prefix);
        //cout<<"prefix calculated"<<endl;
        if (syntaxOk) {            
            WordList<TSymbol>* list = lzTrie->getWordsByPrefix(prefix);
            wordListToStreamChars<TSymbol>(list, cout);
            //cout<<"number of words: "<<list->numberOfWords()<<endl;
            delete list;
        }
        else {
            cout<<"* symbol must be the last symbol of the input."<<endl;
        }
    }
    else { // print a single word
        bool contains = lzTrie->containsWord(queryTS);
        if (contains) cout<<"in the dictionary"<<endl;
        else cout<<"NOT in the dictionary"<<endl;
    }

    delete [] queryTS;
}

void test() {
    //cout<<"start testing\n";
    if (params.count("-d") == 0) {
        cout<<"Please specify the dictionary file." << endl;
        printUsage();
        return;
    }
    bool check = (params.count("-c") > 0);   
    bool locate = (params.count("-l") > 0);
    bool extract = (params.count("-e") > 0);
    int runs = 10; 
    if (params.count("-r") > 0) runs = std::atoi(params["-r"].c_str());
    int n = 1000000; 
    if (params.count("-n") > 0) n = std::atoi(params["-n"].c_str());
    WindexTestData<TSymbol, TIndex> *tdata = testDataFromCompactArray(params["-d"], n, check);
    if (locate) runLocateOrExtract(tdata, runs, true);            
    if (extract) runLocateOrExtract(tdata, runs, false);            
}

WindexTestData<TSymbol, TIndex>* testDataFromCompactArray(string fname, int n, bool check) {
    TCompactArray* carray = getCompactArrayFromCompressedFile<TSymbol, TIndex>(fname);    
    LzTrie<TCompactArray>* lzTrie = new LzTrie<TCompactArray>(*carray);
    string emptystr = "";
    TSymbol *queryTS = stringToTSymbolString(emptystr);    
    WordList<TSymbol>& list = *(lzTrie->getWordsByPrefix(queryTS));          
    WindexTestData<TSymbol, TIndex> *tdata = new WindexTestData<TSymbol, TIndex>();    
    WordIndexer<TCompactArray, LzTrieIterator<TCompactArray> >* windex = 
            new WordIndexer<TCompactArray, LzTrieIterator<TCompactArray> >(*carray);            
    tdata->windex = windex;
    int num = list.numberOfWords();
    if (n < num) { // choose n random indices and words   
        srand (time(NULL));        
        for (int i=0; i<n; i++) { 
            int wi = rand()%num;
            tdata->indices.push_back(wi);
            tdata->words.push_back(list[wi]);            
        }
    }
    else { // take all indices and words
    for (TIndex i = 0; i < list.numberOfWords(); ++i) {            
        tdata->indices.push_back(i);   
        tdata->words.push_back(list[i]);            
    }    
    }
    if (check) { // check that indices map to extracted words
        cout<<list.numberOfWords()<<endl;
        for (TIndex i = 0; i < list.numberOfWords(); ++i) {
            assert(wordsEqual(list[i], windex->getWord(i)));  
            //cout << "word:" << list[i] << endl;
            if (!wordsEqual(list[i], windex->getWord(i))) {
                cout<<"word index mismatch"<<i<<";"<<list[i]<<";"<<windex->getWord(i)<<endl;               
            }
        }
    }              
    return tdata;
}

void search() {
    if (params.count("-d") == 0) {
        cout<<"Please specify the dictionary file." << endl;
        printUsage();
        return;
    }
    bool stdInput = (params.count("-s") == 0);

    LzTrie<CompactArray<TSymbol, TIndex> >* lzTrie = 
            getLzTrieFromCompressedFile<TSymbol, TIndex>(params["-d"]);
    //cout<<"trie loaded"<<endl;

    if (stdInput) {
        string input;
        while (true) {
            cin >> input;
            if (input == "^") break;
            printWordList(input, lzTrie);
        }
    }
    else {
        // replace # by * in the query string
//        string query = params["-s"];
//        for (int i = 0; i < query.size(); ++i)
//            if (query[i] == '#') query[i] = '*';

        printWordList(params["-s"], lzTrie);
    }

    delete lzTrie;
}

/** Executes compression of a list of words from input file
 * and writes compressed dictionary to the output file. */
void compress() {
    if (params.count("-i") == 0) {
        cout<<"Please specify the input file." << endl;
        printUsage();
        return;
    }
    if (params.count("-d") == 0) {
        params["-d"] = params["-i"]+".lzt";
    }

    doCompress(params["-i"], params["-d"]);
}

void writeWordsToFile(const WordList<TSymbol>* words, string fileName, bool linuxEow = false) {
    ofstream file; file.open(fileName.c_str(), ios::out);
    char buff[10000];

    for (size_t i = 0; i < words->numberOfWords(); ++i) {
        size_t len;
        const TSymbol * word = (*words)[i];
    
        for (len = 0; word[len] != zeroSymbol<TSymbol>(); ++len);
        for (size_t j = 0; j < len; ++j) buff[j] = (char)word[j];

        if (linuxEow) {
            buff[len] = 10;
            file.write(buff, len+1);
        }
        else {
            buff[len] = 13; buff[len+1] = 10;
            file.write(buff, len+2);
        }
    }

    file.close();
}

void doCompress(string inputFile, string outputFile) {    
    cout<<"compressing "<<inputFile<<" to "<<outputFile<<" ..."<<endl;
    bool zeroDelim = params.count("-z") > 0;        
    // create compressed array    
    WordFileReader<TSymbol> reader(inputFile, zeroDelim);
    WordList<TSymbol>* words = reader.getWords();
    cout << "files read" << endl << flush;
    words->sort();
    bool enumerate = params.count("-e") > 0;       
    Timer timer; 
    timer.start();
    //TNodeArray* array = getLzArray<TNodeArray>(*words, enumerate);
    TNodeArray* array = getLzArrayLCT<TNodeArray>(*words, enumerate);
    //printArray(*array);
    delete words;

    if (params.count("-cmm") > 0) {
        CmmExporter<TNodeArray>::writeToFile(*array, params["-cmm"]);
    }

    // create compact array
    CompactArrayCreator<TNodeArray> compacter(*array);
    CompactArray<TSymbol, TIndex>* carray =  compacter.createCompactArray();
    delete array;
    // serialize to file
    ofstream output(outputFile.c_str());
    CompactArraySerializer<TSymbol, TIndex> serializer(carray);
    serializer.arrayToStream(output);
    output.close();
    delete carray;

    cout<<"compression sucessful"<<endl;
    cout<<"  seconds elapsed: " << timer.elSeconds() << endl;
}

TSymbol* stringToTSymbolString(string& str) {
    TSymbol *tss = new TSymbol[str.size()+1];
    int i;
    for (i = 0; i < str.size(); ++i) tss[i] = (TSymbol)str[i];
    tss[i] = zeroSymbol<TSymbol>();
    return tss;
}

void intersect() {
    if (params.count("-i") == 0) {
        cout<<"Please specify the input file." << endl;
        printUsage();
        return;
    }
    if (params.count("-d") == 0) {
        cout<<"Please specify the dictionary file." << endl;
        printUsage();
        return;
    }
    LzTrie<CompactArray<TSymbol, TIndex> >* lzTrie =
            getLzTrieFromCompressedFile<TSymbol, TIndex>(params["-d"]);

    fstream file(params["-i"].c_str());
    while (file.good()) {
        string word;
        file >> ws;
        file >> word;
        TSymbol *tsWord = stringToTSymbolString(word);
        if (lzTrie->containsWord(tsWord)) cout<<word<<endl;
        delete [] tsWord;
    }

    delete lzTrie;
}

/** Run specified set of compression algorithms on a specified set of
 * dictionaries.  */
void batchRun() {
  //lzt b -a algorithms [-d dict_folder] [-f dict_files] [-o output_folder] [-st][-t][-e]

    if (params.count("-a") == 0) {
        cout << "Please specify algorithms." << endl;
        printUsage();
        return;
    }
    if (params.count("-d") == 0 && params.count("-f") == 0) {
        cout << "Please specifiy folder and/or files." << endl;
        printUsage();
        return;
    }

    bool stats = (params["-st"] == "true");
    bool test = (params["-t"] == "true");
    bool err = (params["-e"] == "true");

    StatCreator<TNodeArray> statCreator(stats, test, err);
    
    string dictFolder = (params.count("-d") == 0) ? "" : params["-d"];
    string dictFiles = (params.count("-f") == 0) ? "" : params["-f"];
    string outFolder = (params.count("-o") == 0) ? "" : params["-o"];
    string algos = params["-a"];

    statCreator.makeStats(algos, dictFolder, dictFiles, outFolder);
}