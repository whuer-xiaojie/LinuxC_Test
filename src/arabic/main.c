/*
* main.c
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "convert.h"

static void clear_line_enter(char *str)
{
	int str_len = strlen(str);
	int i;
	for (i = str_len - 1; i >= 0; i--)
	{
		if (str[i] == '\n' || str[i] == '\r')
			str[i] = '\0';
	}
}
static int  fread_line(FILE *fp, char *dest)
{
	if ((fgets(dest, 10240, fp)) == NULL)
		return -1;
	//clear_string_enter(dest);
	clear_line_enter(dest);
	return 0;
}

int main()
{
	char *file = "test.txt"; char *utf8_str[10240];
	char *unicode_str = NULL; int len = 0;
	FILE *fd = fopen(file, "r");
	char *src_srt = "مرحبًا بالعربية";
	if (fd == NULL){
		fprintf(stderr, "%s():open file error\n", __FUNCTION__);
		return 0;
	}
	if (fread_line(fd, utf8_str) == -1){
		fprintf(stderr, "%s():read file line failed\n", __FUNCTION__);
		return 0;
	}
	//unicode_str = string_utf8_to_unicode(utf8_str, &len);
	unicode_str = string_utf8_to_unicode(src_srt, &len);
	if (unicode_str == NULL || len <= 0){
		fprintf(stderr, "%s():convert to unicode failed\n", __FUNCTION__);
		return 0;
	}
	int i;
	for (i = 0; i < len; i+=2){
		unsigned long charcode = (unicode_str[i + 1] << 8) | unicode_str[i];
		if (charcode >= 0x0600 && charcode <= 0x06FF){
			fprintf(stderr, "arabic:%#X\n", unicode_str[i]);
		}
		else if (charcode>=0x4E00 && charcode <= 0x9FBF){
			fprintf(stderr, "chinese:%#X\n", charcode);
		}
		
	}
	return 0;
}