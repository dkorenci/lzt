#ifndef TRIETOSTRINGPROJECTOR_HPP
#define	TRIETOSTRINGPROJECTOR_HPP

/** Create a string of readable characters that resembles the
 * structure of a trie where each node is interpreted as a character. */
template <typename TNodeArray>
class TrieToStringProjector {

private:

    typedef typename TNodeArray::Symbol TSymbol;
    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Node TNode;

public:

    static TSymbol* projectTrie(TNodeArray& trie) {
        TSymbol* str = new TSymbol[trie.getSize()+1];
        for (TIndex i = 0; i < trie.getSize(); ++i) {
            str[i] = projectNode(trie[i]);
        }
        str[trie.getSize()] = 0;
        return str;
    }

private:

    /* All the symbols should be lowercase english letters,
     * sibling nodes are mapped (hashed) to other readable ascii chars. */
    static TSymbol projectNode(TNode n) {
        // mapping of sibling nodes to symbols
        const int sibToSymL = 61;
        const char *sibToSym =
            "1234567890!@#$%^&*()-+=<>,./?;:\\|'{}[]\" EFGHIJKLMNOPQRSTUVXYZ";

        // table of symbols to wich symbols nodes will be hashed
        const int symbolsL = 29;
        const char* symbols = "abcdefghijklmnopqrstuvxyzABCD";

        int eow = (int)n.getEow(), cow = (int)n.getCow();
        int sib = (int)n.getSibling(), sym = (int)n.getSymbol();

        if (n.getSibling() == 0) {
            return n.getSymbol();
            //return (n.getSymbol() * (eow + 1) * (cow + 1)  );
        }
        else {


            return sibToSym[((sym + eow + cow) * sib) % sibToSymL];
        }
    }

};

#endif	/* TRIETOSTRINGPROJECTOR_HPP */

