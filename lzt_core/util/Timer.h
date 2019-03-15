#ifndef TIMER_H
#define	TIMER_H

#include <ctime>

class Timer {

public:
    Timer();
    Timer(const Timer& orig);
    virtual ~Timer();

    // reset time to 0
    void start();
    // returns time elapsed since last start, in seconds
    double elSeconds();

private:
    std::clock_t clock;

};

#endif	/* TIMER_H */

