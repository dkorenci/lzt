#ifndef FSACONVERTER_HPP
#define	FSACONVERTER_HPP

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <locale>

#include "s_fsa_subset/nnode.h"
#include "s_fsa_subset/build_fsa.h"
#include "s_fsa_subset/fsa.h"
#include "s_fsa_subset/common.h"
#include "s_fsa_subset/prefix.h"
#include "s_fsa_subset/fsa.h"

#include "compress/ICompressor.h"
#include "compress/sa_compressor/LCPTreeCompressor.hpp"

using namespace std;

/* Class for testing compression method that combines finite state automaton constructed 
 * with Jan Daciuk's s_fsa package and Lz Trie compression. 
 * This is done in method compressWords() - first an automaton is created, 
 * then converted to LzTrie which is then compressed.
 */
template <typename TNodeArray>
class FsaConverter : public ICompressor<TNodeArray> {
private:
    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;

public:

    void compressArray(TNodeArray& array);
    string description();

    TNodeArray* compressWords(string fileName);
    FsaConverter(bool compr = false);

private:

    // comparator comparing fsa arcs by letter
    struct FsaArcCompare {

        FsaArcCompare(): loc("C") {}

        inline bool operator()(fsa_arc_ptr a1, fsa_arc_ptr a2) const {
            // compare characters using "C" locale with the same collation
            // as in all the input files, to keep the trie sorted
            string c1(1,a1.get_letter()), c2(1,a2.get_letter());
            return loc(c1, c2);
        }

        private:
        locale loc;
    };

    void constructFsa(string fileName);
    void compressAfterConvert();
    void initFsaData();
    void initTrieData();
    void encodeAutomaton(fsa_arc_ptr state);
    void writeArcToArray(fsa_arc_ptr arc, TIndex ind);
    void writePointerToArray(TIndex ptrInd, TIndex ind);
    void cleanup();

    // flag to compress trie after conversion from FSA
    bool compress;
    // name of the file for storing the constructed FSA
    const char *fsaFile;    
    prefix_fsa* fsa; // object for reading the constructed FSA
    fsa_arc_ptr start; // first arc of the fsa    
    TNodeArray* array; // the trie being constructed
    static const int INIT_TRIE_CAP = 1000; // initial trie capacity    
    TIndex pos, depth; // first unfilled position in the trie
    // mapping of fsa states to positions in the node array where
    // they are stored after they are traversed
    map<arc_pointer, TIndex> stateMap;
    // arc comparator
    FsaArcCompare comp;
    
};

template <typename TNodeArray>
FsaConverter<TNodeArray>::FsaConverter(bool compr):
    fsaFile("automaton.fsa"), compress(compr)  {}

/** Construct FSA from file containing list of words and write it to file. */
template <typename TNodeArray>
void FsaConverter<TNodeArray>::constructFsa(string fileName) {
    char  FILLER = '_';
    automaton autom;
    autom.FILLER = FILLER;
    ifstream inpf(fileName.c_str());    
    autom.build_fsa(inpf);
    
    ofstream outf(fsaFile, ios::binary);
    autom.write_fsa(outf);
}

/** Read the automaton from .fsa file and init data for its traversal. */
template <typename TNodeArray>
void FsaConverter<TNodeArray>::initFsaData() {
    const char	*lang_file = NULL;
    const int	MAX_LINE_LEN = 512;
    // create automaton reading object from data in fsaFile
    word_list fname;
    fname.insert(fsaFile);
    fsa = new prefix_fsa(&fname, lang_file);
    dict_list* dict = &(fsa->dictionary);
    fsa->set_dictionary(dict->item());
    fsa_arc_ptr	*dummy = NULL;
    // calculate the addres of the first node in the fsa pointed to by current_dict
    fsa_arc_ptr auxnode = dummy->first_node(fsa->current_dict);
    // calculate the node that the firstnode points to, this is the start node
    start = auxnode.set_next_node(fsa->current_dict);
}

template <typename TNodeArray>
void FsaConverter<TNodeArray>::initTrieData() {
    array = new TNodeArray();
    array->resize(INIT_TRIE_CAP);
    pos = 0;
    stateMap.clear();
}

/** Write FSA arc to trie node at position ind */
template <typename TNodeArray>
void FsaConverter<TNodeArray>::writeArcToArray(fsa_arc_ptr arc, TIndex ind) {
    typename TNodeArray::Node n = (*array)[ind];
    n.setSymbol(arc.get_letter());
    n.setEow(arc.is_final());
    n.setCow(arc.get_goto() != 0);
}

/** Set trie node at position to a closed pointer pointing to position ptrInd. */
template <typename TNodeArray>
void FsaConverter<TNodeArray>::writePointerToArray(TIndex ptrInd, TIndex ind) {
    typename TNodeArray::Node n = (*array)[ind];
    typename TNodeArray::Node target = (*array)[ptrInd];

    // if target state consists of a single edge
    if (target.getCow() == false && target.getSibling() == 0) {
        n.set(target); // copy the edge
    }
    else { // point (*array)[ind] to ptrInd
        n.setPointer();
        n.setSibling(ptrInd);
        n.setSymbol(0);
    }
}

// Write state and all the states reachable from it to node array
template <typename TNodeArray>
void FsaConverter<TNodeArray>::encodeAutomaton(fsa_arc_ptr state) {
    depth++;
//    cout << "depth: " << depth << endl;
    stateMap[state.arc] = pos; //
    // get all the arcs in state and sort them by symbol
    vector<fsa_arc_ptr> arcs;
    fsa_arc_ptr arc = state;
    while (true) {
        arcs.push_back(arc);
        if (arc.is_last()) break;
        else ++arc;
    }
    
    sort(arcs.begin(), arcs.end(), comp);
    // copy to trie arcs and states these arcs lead to
    TIndex previousArc = 0; // array index of the previous arc
    for (int i = 0; i < arcs.size(); ++i) {
        arc = arcs[i];        
        array->createNodesAtEnd(1);
        writeArcToArray(arc, pos);
        // create sibling link between edges on the same level
        if (i > 0) (*array)[previousArc].setSibling(pos-previousArc);
        previousArc = pos; pos++;        
        if (arc.get_goto() != 0) { // copy the state the arc leads to
            fsa_arc_ptr next = arc.set_next_node(fsa->current_dict);
            // next state has not been visited
            if (stateMap.count(next.arc) == 0) encodeAutomaton(next);
            else {
                array->createNodesAtEnd(1);
                writePointerToArray(stateMap[next.arc], pos++);
            }
        }
    }
    depth--;
}

/** Dummy method, for compatibility with ICompressor */
template <typename TNodeArray>
void FsaConverter<TNodeArray>::compressArray(TNodeArray& array) { }

template <typename TNodeArray>
string FsaConverter<TNodeArray>::description() {
    string result = "fsa";
    return result+(compress?"+lct":"");
}

template <typename TNodeArray>
TNodeArray* FsaConverter<TNodeArray>::compressWords(string fileName) {
    constructFsa(fileName);
    initFsaData();    
    initTrieData();
    depth = 0;
    encodeAutomaton(start);
    array->resize(array->getSize());
    cleanup();
    if (compress) compressAfterConvert();    
    return array;  
}

/** Compress array using LCPTreeCompressor. */
template <typename TNodeArray>
void FsaConverter<TNodeArray>::compressAfterConvert() {
    LCPTreeCompressor<TNodeArray> compressor;
    compressor.compressArray(*array);
}

template <typename TNodeArray>
void FsaConverter<TNodeArray>::cleanup() {
    delete fsa;
    stateMap.clear();
}

#endif	/* FSACONVERTER_HPP */

