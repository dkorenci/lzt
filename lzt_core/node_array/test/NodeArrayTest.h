#ifndef NODEARRAYTEST_H
#define	NODEARRAYTEST_H

#include <cstdlib>

#include "debug/lzt_test.h"
#include "util/WordFileReader.h"
#include "node_array/vector_array/VectorArray.h"
#include "node_array/na_utils.h"

class NodeArrayTest {
public:
    void testStringConversions();

};

template <typename TNodeArray>
class NodeArrayTestTemplate {
private:

    typedef typename TNodeArray::Index TIndex;
    typedef typename TNodeArray::Symbol TSymbol;

public:
    NodeArrayTestTemplate() {};
    ~NodeArrayTestTemplate() {};

    void testStringConversions();

};

/** Test converting the node array to and from string. */
template <typename TNodeArray>
void NodeArrayTestTemplate<TNodeArray>::testStringConversions() {
    WordFileReader<TSymbol> reader("../lzt_core/node_array/test/stringArrays.txt");
    reader.readWords();
    for(size_t i = 0; i < reader.getNumberOfWords(); ++i) {
        ostringstream message; message << "i: " << i << endl;
        string startArray = reader.getWord(i);
        TNodeArray* array = nodeArrayFromString<TNodeArray>(startArray);
        string endArray = nodeArrayToString(*array);

        message << "start array: " << startArray << endl;
        message << "end array: " << endArray << endl;
        TEST_ASSERT_MESSAGE(nodeArrayToString(*array) == startArray, message.str());

        delete array;
    }
}

#endif	/* NODEARRAYTEST_H */

