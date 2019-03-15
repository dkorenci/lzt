#include "TempFile.h"
#include "utils.h"
#include <cstdio>

TempFile::TempFile(): name("file_") {
    name += getRandomString();
    name += ".tmp";
    file = fopen(name.c_str(), "w+");
    fclose(file);
    //printf("%s\n", name.c_str());
}

TempFile::~TempFile() {
    //fclose(file);
    remove(name.c_str());
}

const char * TempFile::getName() {
    return name.c_str();
}