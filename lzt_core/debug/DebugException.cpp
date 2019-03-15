#include "DebugException.h"

DebugException::DebugException(string* m)
: stackTrace( new StackTrace(1) )
{
    messages.push_back(m);
}

/** Utility constructor for easier and safer ASSERT_THROW macro declaration. */
DebugException::DebugException(string* m1, string *m2)
: stackTrace( new StackTrace(1) )
{
    messages.push_back(m1);
    messages.push_back(m2);
}

DebugException::~DebugException() {
    delete stackTrace;
    for (size_t i = 0; i < messages.size(); ++i)
        delete messages[i];
}

string DebugException::printStackTrace() const {
    return stackTrace->printStackTrace();
}

vector<string*> const & DebugException::getMessages() const {
    return messages;
}

/** If exception is caught and rethrown messages can be added in between. */
void DebugException::addMessage(string *m) {
    messages.push_back(m);
}


