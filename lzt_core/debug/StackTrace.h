#ifndef STACKTRACE_H
#define	STACKTRACE_H

#include <execinfo.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

/** Get the stacktrace of the current execution point in the program.
 * This class is system dependent as it uses libc and libcwd framework.
 * Program must be compiled with -g and -DCWDEBUG and linked with -lcwd.
 */
class StackTrace {
public:

    StackTrace();
    StackTrace(int topPops);
    StackTrace(const StackTrace& orig);
    virtual ~StackTrace();

    string printStackTrace() const;

private:

    static const size_t maxStackSize = 1000;
    size_t size;

    vector<string> stack;

    void recordStackTrace(int pop);

};

#endif	/* STACKTRACE_H */

