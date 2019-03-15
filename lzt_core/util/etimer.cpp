#include <iomanip>

#include "etimer.h"

void startEvent(string eventName) {    
     clock_t t = clock();
     events[eventName].start = t;
     events[eventName].end = t - 1;
}

void endEvent(string eventName) {
     clock_t t = clock();
     events[eventName].end = t;
}

void printEvents() {
    map<string, TRange>::iterator it;
    for (it = events.begin(); it != events.end(); ++it) {
        TRange rng = it->second;
        double time =  ((double)(rng.end - rng.start)) / CLOCKS_PER_SEC;
        cout << it->first << ": " << setprecision(7) << time << endl;
    }
}

void clearEvents() {
    events.clear();
}
