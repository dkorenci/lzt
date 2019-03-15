#include "TestException.h"

#include <cstring>

TestException::TestException(string m) throw()
: message(m) { }

TestException::TestException(string m1, string m2) throw()
: message(m1 + "\n" + m2) {
}

TestException::TestException(const TestException& orig) throw()
: message(orig.message) { }

TestException::~TestException() throw() { }

const char* TestException::what() const throw() {
    return message.c_str();
}

