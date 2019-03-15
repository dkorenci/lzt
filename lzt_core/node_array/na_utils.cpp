#include "na_utils.h"

/** Break a string to substrings separated with delimiter. */
vector<string> * breakByDelimiter(string const & str, const char delimiter) {
    vector<string>* tokens = new vector<string>;

    size_t i = 0; if (str[i] == delimiter) i++;
    string piece; piece.clear();

    for (; i < str.size(); ++i) {
        if (str[i] != delimiter) piece.push_back(str[i]);
        
        if (str[i] == delimiter || i == str.size() - 1) {
            tokens->push_back(piece);
            piece.clear();
        }
    }

    return tokens;
}
