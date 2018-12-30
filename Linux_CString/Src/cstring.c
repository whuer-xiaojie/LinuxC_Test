/*
* cstring.c
*
* Created On:2018/11/13
*
*/
#include "cstring.h"

#include <stdio.h>
#include <stdio.h>
#include <string.h>

bool StringStartWith(const char *pSrc, const char *pStr)
{
	if (pSrc == NULL || pStr == NULL)
		return False;

	int strLen = strlen(pStr);
	int srcLen = strlen(pSrc);

	if (srcLen < strLen){
		return False;
	}
	else if (srcLen == strLen){
		if (strcmp(pSrc, pStr) == 0)
			return True;
		else
			return False;
	}
	else{
		int i = 0;
		for (i = 0; i < strLen; i++){
			if (pSrc[i] != pStr[i])
				return False;
		}
		return True;
	}
	return True;
}

bool StringEndWith(const char *pSrc, const char *pStr)
{
	if (pSrc == NULL || pStr == NULL)
		return False;

	int strLen = strlen(pStr);
	int srcLen = strlen(pSrc);

	if (srcLen < strLen){
		return False;
	}
	else if (srcLen == strLen){
		if (strcmp(pSrc, pStr) == 0)
			return True;
		else
			return False;
	}
	else{
		int i = 0;
		char *temp = pSrc + (srcLen - strLen);
		for (i = 0; i < strLen; i++){
			if (temp[i] != pStr[i])
				return False;
		}
		return True;
	}
	return True;
}
