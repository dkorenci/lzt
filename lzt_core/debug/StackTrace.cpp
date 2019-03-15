#include "StackTrace.h"

#include <sstream>

/** Creates a new object with a current stack trace. */
StackTrace::StackTrace() {
    recordStackTrace(2);
}

/** Creates a new object with a current stack trace.
 @param topPops
 Number of functions on the top of stack to be excluded.
 */
StackTrace::StackTrace(int topPops) {
    recordStackTrace(2 + topPops);
}

StackTrace::StackTrace(const StackTrace& orig) {
}

StackTrace::~StackTrace() {
}

/** Records the current stack trace.
 * @param topPops
 * Number of top stack locations to ignore. Callers should
 * normally remove itself and this method.
 */
void StackTrace::recordStackTrace(int topPops) {
    //TODO cwdebug is not longer used so if need arises again,
    // new way of extracting readable stack trace names must be devised.
/* Method body is empty if source is compiled for debug.
   In that case stack trace will be empty. */
#ifdef CWDEBUG
   void *pointers[maxStackSize];
   
   //TODO kad se kompajlira debug, nesmije biti inline funkcija

   size = backtrace(pointers, maxStackSize);

   for (size_t i = topPops; i < size; ++i) {
       // use libcwd to convert pointer to informative name
       libcwd::location_ct location(
                (char*)pointers[i] + libcwd::builtin_return_address_offset);
       string function_name;
       libcwd::demangle_symbol(location.mangled_function_name(), function_name);

       stringstream ss;
       ss << function_name << " (" << location << ")";
      
       stack.push_back(ss.str());
   }
#endif //CWDEBUG
}

/**  */
string StackTrace::printStackTrace() const {
    string trace;
    vector<string>::const_iterator it;
    for (it = stack.begin(); it != stack.end(); ++it) {
        trace += (*it); trace += "\n";
    }
    return trace;
}

