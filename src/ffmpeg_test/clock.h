/*
* clock.h
*/
#include <time.h>
#ifndef _CLOCK_H
#define _CLOCK_H

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW	4
#endif

void __sleep(unsigned long ms);
void __delay(unsigned long ms);
unsigned long __clock(void);
struct tm* getCurTime(void);


#endif	/* CLOCK_H */
