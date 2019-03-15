#ifndef TEMPFILE_H
#define	TEMPFILE_H

#include <string>

using std::string;

/** Creates the file at construction and deletes it at destruction. */
class TempFile {
public:
    TempFile();    
    virtual ~TempFile();   

    const char * getName();

private:
    string name;
    FILE* file;

    TempFile(const TempFile& orig);

};

#endif	/* TEMPFILE_H */

