#include "Timer.h"

Timer::Timer() {
}

Timer::Timer(const Timer& orig) {
}

Timer::~Timer() {
}

void Timer::start() {
    this->clock = std::clock();
}

double Timer::elSeconds() {
    return (std::clock() - this->clock) / (double) CLOCKS_PER_SEC;
}