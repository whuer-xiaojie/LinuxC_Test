/*
* cstring.h
*
* Created On:2018/11/13
*
*/

#ifndef CSTRING_H_
#define CSTRING_H_

#include <inttypes.h>

typedef uint8_t bool;
#define True 0
#define False -1

bool string_start_with(const char *pSrc, const char *pStr);
bool string_end_with(const char *pSrc, const char *pStr);

#endif  // CSTRING_H_
