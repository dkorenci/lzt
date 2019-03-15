#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cctype>


#include "dictionary/lztrie_dict/LzTrieDict.h"
#include "dictionary/lztrie_dict/LzTrieDictCreator.h"
#include "dictionary/lztrie_dict/WordPair.h"
#include "util/DictFileReader.h"
#include "util/utils.h"
#include "dictionary/lztrie_dict/WordPair.h"

using namespace std;

typedef char TSymbol;
typedef unsigned int TIndex;

static void printUsage();
static map<string, string> createParameterMap(int argc, char** argv);
static void processSeparatorParam(map<string, string>& params);
static void processSegmentParam(map<string,string>& params);
static void compress(map<string, string> params);
static void doCompress(string inputFile, string dictFile, TSymbol sep, string seg, bool old);
static void doSearch(map<string, string> params);
static void searchTerm(string dictFile, string term, char sep);
static void searchFile(string file);
static void searchCommandLine();
static LzTrieDict<TSymbol, TIndex>* getDictFromFile(string file);

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage();
    }
    else {
        string command = argv[1];
        map<string, string> params = createParameterMap(argc-2, argv+2);

        if (command == "c") {
            compress(params);
        }
        else if (command == "s") {
            doSearch(params);
        }
        else {
            printUsage();
        }
    }

    return 0;
}

/** Check the correctness of -s parameter and set default if it
 * does not exits.*/
void processSeparatorParam(map<string,string>& params) {
    if (params.count("-s") == 0) {
        params["-s"] = "|";        
    }
    else {
        if (params["-s"].length() != 1) {
            cout << "Separator must be one character." << endl;
            printUsage();
            exit(0);
        }        
    }
}

/** Check the correctnes of -seg parameter and set default if it
 * does not exist. */
void processSegmentParam(map<string, string>& params) {
    // check if segment size parameter is a number
    if (params.count("-seg") != 0) {
        string seg = params["-seg"];
        bool number = true;
        if (isdigit(seg[0]) && seg[0] != '0') {
            for (int i = 1; i < seg.length(); ++i)
            if (isdigit(seg[i]) == false) {
                number = false;
                break;
            }
        }
        else number = false;

        if (number == false) {
            cout << "Segment size must be a number > 0." << endl;
            printUsage();
            exit(0);
        }
    }
    else {
        params["-seg"] = "20";
    }
}

/** Check parameters for input and output file, write error messages if 
 * necessary and and execute the compression if possible. */
void compress(map<string,string> params) {
    if (params.count("-i") == 0) {
        cout<<"Please specify the input file." << endl;
        printUsage();
        return;
    }
    // set default dictionary file, if none is specified
    if (params.count("-d") == 0) {
        params["-d"] = params["-i"] + ".lzd";
    }
    
    processSeparatorParam(params);
    processSegmentParam(params);

    bool oldCompress = (params.count("-old") > 0); // use old (slower) non LCPIntTree compression
    doCompress(params["-i"], params["-d"], params["-s"][0], params["-seg"], oldCompress);
}

void doCompress(string inputFile, string dictFile, TSymbol sep, string seg, bool old) {
    // read and parse input
    vector<WordPair<TSymbol> >* pairs =
            DictFileReader<TSymbol>::readWords(inputFile, sep);

    // no pairs of words were read from the file
    if (pairs->size() == 0) {
        cout << "Dictionary file must consist of correctly formated lines." << endl;
        printUsage();
        delete pairs;
        return;
    }

    // create dictionary
    LzTrieDictCreator<TSymbol, TIndex> dictCreator;
    LzTrieDict<TSymbol, TIndex> dict;

    size_t segSize = atol(seg.c_str());
    dictCreator.createDictionary(&dict, pairs, segSize, old);

    // write dictionary to file
    fstream output(dictFile.c_str(), ios_base::out);
    dict.writeToStream(output);
    output.close();
    
    DictFileReader<TSymbol>::freePairs(pairs);
}

/** Process search parameters and invoke search if they are correct. */
void doSearch(map<string, string> params) {
    if (params.count("-d") == 0) {
        cout<<"Please specify the dictionary file." << endl;
        printUsage();
        return;
    }
    processSeparatorParam(params);

    if (params.count("-t") != 0) {
        searchTerm(params["-d"], params["-t"], params["-s"][0]);
    }
    else {
        if (params.count("-f") != 0)
            searchFile(params["-f"]);
        else
            searchCommandLine();
    }
}

LzTrieDict<TSymbol, TIndex>* getDictFromFile(string file) {
    ifstream stream(file.c_str());
    
    LzTrieDict<TSymbol, TIndex>* dict = new LzTrieDict<TSymbol, TIndex>;
    dict->readFromStream(stream);

    return dict;
}

void searchTerm(string dictFile, string term, char sep) {
    LzTrieDict<TSymbol, TIndex>* dict = 0;

    size_t star = term.find_first_of('*');

    // star not found
    if (star == string::npos) {
        dict = getDictFromFile(dictFile);

        vector<TSymbol*> result =
            dict->lookupWord(term.c_str());

        for (size_t i = 0; i < result.size(); ++i)
            cout << result[i] << endl;
        
        freeWords(result);
    }
    else {
        // star found, it must be the last character
        if (star != term.size() - 1) {
            cout << "Star symbol must be the last character of the search term." << endl;
            return;
        }
        // read dictionary
        dict = getDictFromFile(dictFile);
        // remove star from the end
        term = term.substr(0, term.size() - 1);
        vector<WordPair<TSymbol> > result =
            dict->findByPrefix(term.c_str());

        for (size_t i = 0; i < result.size(); ++i)
            cout << result[i].first << sep << result[i].second << endl;

        freePairs(result);
    }

    if (dict != 0) delete dict;
}

void searchFile(string file) {
    cout<<"File search not implemented yet."<<endl;
}

void searchCommandLine() {
    cout<<"Command line search not yet implemented."<<endl;
}

/** Scan command line arguments for occurence of parameters.
 * Parameter is sequence of two consecutive arguments where the
 * first one is of the form '-x' (parameter id) and second is optional, 
 * a value - string that  * does not start with '-'. 
 * Found parameters are returned as a map that maps ids to values or empty strings. */
map<string, string> createParameterMap(int argc, char** argv) {
    map<string, string> params;

    for (int i = 0; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-i" || arg == "-d" || arg == "-s" || arg == "-t" || arg == "-f" 
                || arg == "-old") {
            if (i < argc - 1) {
                string val = argv[i+1];
                if (val.size() > 0 && val[0] != '-') params[arg] = val;
                else params[arg] = "";
            }
        }
    }

    return params;
}

void printUsage() {
    cout<<endl<<"lztd dictionary compression utility"<<endl<<endl
        <<"compress: lztd c [-old] -i input_file [-d dict_file] [-s separator] [-seg segment_size]"<<endl
        <<"   Separator is a character thah separates domain words form codomain words"<<endl
        <<"    in the input_file. Default separator is '|'. Each line must be "<<endl
        <<"    of the form: dom_word[separator_char]cod_word."<<endl
        <<"   Segment size is a dictionary creation parameter that determines "<<endl
        <<"    how many consecutive indexes of words will be coded using "<<endl
        <<"    differences and not explicit indexes, default value is 20."<<endl
        <<"   Default dictionary file is input_file.lzd ."<<endl<<endl
        <<"search: lztd s -d dict_file [-t search_term | -f search_file] [-s separator]"<<endl
        <<"   If search term is given print all words in the dictionary associated with it."<<endl
        <<"   Search term is either a word or word*, * must be the last char and "<<endl
        <<"    can be the only char. If * occurs in the term, print results as"<<endl
        <<"    pairs of words separated with separator character."<<endl
        <<"   Do not forget to escape * with a '\\' if necessary!"<<endl
        <<"   If search file is given, interpret each line as a search term and print"<<endl
        <<"    results as words separated with separator character."<<endl
        <<"   If search term and file is ommited, read search terms from standard input."<<endl
        <<"   If the terms are read from stdin, '^' terminates the input."<<endl<<endl;
}