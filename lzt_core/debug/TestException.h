#ifndef TESTEXCEPTION_H
#define	TESTEXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class TestException : public exception {
public:

    TestException(string message) throw();
    TestException(string m1, string m2) throw();
    TestException(const TestException& orig) throw();
    virtual ~TestException() throw();
    //TODO question: dupli virtual?
    virtual const char* what() const throw();

private:
    string message;

};

#endif	/* TESTEXCEPTION_H */

