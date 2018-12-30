/*
* convert.h
*/

#ifndef CONVERT_H
#define CONVERT_H

#include "iconv.h"

int convert_to_unicode(char *out, size_t outbytes, const char *in, size_t inbytes);

char* xStudio_string_utf8_to_unicode(char * src, int *eff_len/*Effective length*/);
#endif