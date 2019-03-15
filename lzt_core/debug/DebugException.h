#ifndef DEBUGEXCEPTION_H
#define	DEBUGEXCEPTION_H

#include <string>

#include "StackTrace.h"

#include <exception>

using namespace std;

#ifdef LZT_DEBUG

/* Macros for working with DebugException that enable switching the
 * exception code on-off with LZT_DEBUG variable */

#define ASSERT_THROW(condition, message) \
        if ((condition) == false) { \
            std::string* m1 = new std::string("assertion failed: ");\
            (*m1) += #condition; \
            throw DebugException(m1, message); \
        }

//TODO sto ako se unutar prvog bloka pojavi , ?
#define IF_LZT_DEBUG(debug_code, nodebug_code) debug_code

#else

#define ASSERT_THROW(condition, message) {}

#define IF_LZT_DEBUG(debug_code, nodebug_code) nodebug_code

#endif // DE_DEBUG

/** Exception that carries debug info: stack trace
 * created at the point of throw and a list of debug messages. */
class DebugException {
public:

    DebugException(string* message);
    DebugException(string* message1, string* message2);
    DebugException(const DebugException& orig);
    virtual ~DebugException();

    string printStackTrace() const;
    vector<string*> const & getMessages() const;
    void addMessage(string* message);

private:

    StackTrace* stackTrace;
    vector<string *> messages;

};



#endif	/* DEBUGEXCEPTION_H */

