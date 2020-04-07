/* 
* convert.c
*/

#include "convert.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int convert_to_unicode(char *out, size_t outbytes, const char *in, size_t inbytes) {
    int ret = -1;

    char *lang = getenv("LANG");

    if (lang == NULL) {
        fprintf(stderr, "Environment \" LANG= \" must be setted.\n");
        return ret;
    }

    char *env = memchr(lang, '.', strlen(lang));

    if (env)
        lang = env + 1;

    iconv_t *icnv = iconv_open("UCS-2", lang);

    //fprintf(stderr, "%s():%s\n", __FUNCTION__,lang);

    if (icnv == NULL) {
        fprintf(stderr, "Open codec failed!\n");
        return ret;
    }

    if ((int)iconv(icnv, (char **)&in, &inbytes, &out, &outbytes) < 0) {
        //fprintf(stderr, "%s():Convert characters failed! %s inbytes=%d outbytes=%d in=%s\n", __FUNCTION__, lang,inbytes,outbytes,in);
        iconv_close(icnv);
        return ret;
    }

    iconv_close(icnv);

    return outbytes;
}

char *string_utf8_to_unicode(char *src, int *eff_len /*Effective length*/) {
    char *dest = NULL;
    int src_len;
    int dest_len;
    int real_len;

    src_len = strlen(src);
    dest_len = src_len * 3;
    if (src == NULL || src_len <= 0)
        return NULL;
    if ((dest = (char *)malloc(dest_len)) == NULL) {
        fprintf(stderr, "%():malloc for dest failed\n", __FUNCTION__);
        return NULL;
    }
    //memset(dest, 0, dest_len);

    if ((real_len = convert_to_unicode(dest, (size_t)dest_len, src, (size_t)src_len)) < 0) {
        fprintf(stderr, "%s():convert the string to unicode failed\n", __FUNCTION__);
        if (dest) {
            free(dest);
            dest = NULL;
        }
        return NULL;
    }
    //fprintf(stderr, "%s():real_len =%d  src_len =%d  dest_len=%d  sizeof dest=%d \n", __FUNCTION__, real_len, src_len, dest_len,strlen(dest));
    real_len = (src_len * 3) - real_len;
    *eff_len = real_len;
    //fprintf(stderr, "%s():real_len =%d  src_len =%d  dest_len=%d \n", __FUNCTION__, real_len,src_len,dest_len );
    return dest;
}
