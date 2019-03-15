#ifndef ETIMER_H
#define	ETIMER_H

#include <map>
#include <string>
#include <ctime>
#include <iostream>

using namespace std;

struct TRange {
    TRange(): start(0), end(0) {}

    clock_t start;
    clock_t end;
};

static map<string, TRange> events;

void startEvent(string eventName);
void endEvent(string eventName);

void printEvents();
void clearEvents();

#endif	/* ETIMER_H */

