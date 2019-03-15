#ifndef FILEREADER_H
#define	FILEREADER_H

#include <fstream>
#include <vector>
#include <cstring>
#include <cstddef>
#include <iostream>

#include "node_array/types/symbol.h"
#include "dictionary/util/WordList.h"
#include "util/utils.h"

#include "IteratorDictString.h"

using namespace std;

//TODO Make WFR return char strings and let client convert them to TSymbols

/** Reads words from the file, one per line,
 * and stores them in memory for indexed access. */
template <typename TSymbol> class WordFileReader {
public:

    WordFileReader();
    WordFileReader(string fileName);
    WordFileReader(string fileName, bool zero);
    virtual ~WordFileReader();

    void setFileName(string name);
    // read words (separated by whitespace) from file
    void readWords();

    //TODO remove functionality of word storage and retrieval, that
    // is the role of WordList
    size_t getNumberOfWords();
    TSymbol const * getWord(size_t i);

    WordList<TSymbol>* getWords();

    static TSymbol* readLine(istream& stream);

private:

    string fileName;
    bool zeroDelim;
    FILE * file;
    vector<TSymbol *> words;

    void clearWords();
    WordList<TSymbol>* getWordsCSD();    
    WordList<TSymbol>* getWordsNative();

};

template <typename TSymbol>
WordFileReader<TSymbol>::WordFileReader(): fileName(""), zeroDelim(false) { }

template <typename TSymbol>
WordFileReader<TSymbol>::WordFileReader(string name) : fileName(name), zeroDelim(false) { }

template <typename TSymbol>
WordFileReader<TSymbol>::WordFileReader(string name, bool zero) : fileName(name), zeroDelim(zero) { }

template <typename TSymbol>
WordFileReader<TSymbol>::~WordFileReader() {
    clearWords();
}

/** Set a new name for the word file and clear word that were read
 * from the previous file. */
template <typename TSymbol>
void WordFileReader<TSymbol>::setFileName(string name) {
    fileName = name;
    clearWords();
}

template <typename TSymbol>
size_t WordFileReader<TSymbol>::getNumberOfWords() {
    return words.size();
}

template <typename TSymbol>
TSymbol const * WordFileReader<TSymbol>::getWord(size_t i) {
    if (i >= words.size()) return NULL;
    return words[i];
}

/** Return string that correspond to lines in the file. Empty lines
 * are ignored. */
template <typename TSymbol>
WordList<TSymbol>* WordFileReader<TSymbol>::getWords() {
    if (zeroDelim) return this->getWordsCSD();
    else return this->getWordsNative();
}

/** Return string that correspond to lines in the file. Empty lines
 * are ignored. */
template <typename TSymbol>
WordList<TSymbol>* WordFileReader<TSymbol>::getWordsNative() {
    ifstream stream(fileName.c_str());
    WordList<TSymbol>* words = new WordList<TSymbol>();

    while(stream.good()) {
        TSymbol* line = readLine(stream);
        if (wordLength(line) > 0)
            words->addWord(line);

        delete [] line;
    }

    return words;
}

template <typename T> T * loadValue(ifstream & in, const size_t len) {
    assert(in.good());
    T * ret = new T[len];
    in.read((char*)ret,len*sizeof(T));
    return ret;
}

/* Return words that correspond to non-zero char sequences in the file  
 * Empty sequences are ignored. */
template <typename TSymbol>
WordList<TSymbol>* WordFileReader<TSymbol>::getWordsCSD() {    
    ifstream in(fileName.c_str());
    int numRead = 0;
    if (in.good())
    {
        //cout << "starting read" << endl << flush;
        in.seekg(0,ios_base::end);
        uint lenStr = in.tellg()/sizeof(uchar);
        in.seekg(0,ios_base::beg);            
        uchar *str = loadValue<uchar>(in, lenStr);
        //cout << "file loaded" << endl << flush;
        in.close();
        IteratorDictString *it = new IteratorDictStringPlain(str, lenStr);
        uint lenCurrent=0, elements=0, maxlen=0;
        WordList<TSymbol>* words = new WordList<TSymbol>();
        while (it->hasNext())
        {
            TSymbol* line = (TSymbol*)it->next(&lenCurrent);             
            //cout << line << ' ' << lenCurrent << endl << flush;
            if (lenCurrent > 0) {
                if (lenCurrent > maxlen) {
                    maxlen = lenCurrent;
                    cout << "longest word "<<maxlen<<endl<<flush;
                }
                words->addWord(line);            
                numRead += 1;
                //if (numRead % 1000 == 0) cout << numRead << endl << flush;
            }
        }
        cout << "READ FILE" << endl;        
        return words;
    }       
    else return NULL;
}

/** Read a line of characters from stream and convert to string of TSymbols.
 * Both CR+LF and LF are considered end of line delimiters. */
template <typename TSymbol>
TSymbol* WordFileReader<TSymbol>::readLine(istream& stream) {
    const char CR = 13, LF = 10;
    bool carriageReturn = false;
    vector<char> buffer;

    while (stream.good()) {
        char ch = (char)stream.get();
        if (stream.gcount() == 0) break;
        
        if (ch == LF) { // end of line
            // remove carriage return if it preceedes line feed
            if (carriageReturn) buffer.pop_back();
            break;
        }
        else {
            if (ch == CR) carriageReturn = true;
            else carriageReturn = false;

            buffer.push_back(ch);
        }
    }

    TSymbol* line = new TSymbol[buffer.size()+1];
    size_t i;
    for (i = 0; i < buffer.size(); ++i) line[i] = (TSymbol)buffer[i];
    line[i] = zeroSymbol<TSymbol>();

    return line;
}

template <typename TSymbol>
void WordFileReader<TSymbol>::readWords() {
    ifstream stream(fileName.c_str());
    clearWords();
    const int buffLen = 10000;
    char buffer[buffLen];

    while(stream.good()) {
        stream >> ws;
        if (stream.good() == false) break;
        stream.getline(buffer, buffLen);

        const int size = strlen(buffer);
        TSymbol* word = new TSymbol[size+1];

        for (int i = 0; i < size; ++i)
            word[i] = (TSymbol)buffer[i];

        word[size] = zeroSymbol<TSymbol>();

        words.push_back(word);
    }
}

template <typename TSymbol>
void WordFileReader<TSymbol>::clearWords() {
    typename vector<TSymbol *>::iterator it;
    for (it = words.begin(); it != words.end(); ++it )
        delete [] (*it);
    
    words.clear();
}

#endif	/* FILEREADER_H */

