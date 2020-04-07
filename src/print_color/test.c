#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COLOR_BLACK "\033[30m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"
#define COLOR_NC "\033[0m"

#define debug(fmt, ...)                                                \
    fprintf(stderr, "%s:%d:%s()\t" fmt "\n", __FILE__, __LINE__,       \
    __FUNCTION__, ##__VA_ARGS__)

void __toUTCTime(struct timeval tv,char *buf){
	struct tm gmt;
	//char buf[128] = { 0 };
	if (gmtime_r(&tv.tv_sec, &gmt) == NULL)
		return;
	strftime(buf, 64, "%Z:%Y/%m/%d %H:%M:%S", &gmt);
	sprintf(buf, "%s.%d", buf, tv.tv_usec / 1000);
}
void __toLocalTime(struct timeval tv, char *buf){
	struct tm localt;
	char buf[128] = { 0 };
	if (gmtime_r(&tv.tv_sec, &localt) == NULL)
		return;
	strftime(buf, 64, "%Z:%Y/%m/%d %H:%M:%S", &localt);
	sprintf(buf, "%s.%d", buf, tv.tv_usec / 1000);
}
int main()
{
	puts(COLOR_YELLOW "*************************************" COLOR_NC);
	puts(COLOR_YELLOW "**  Listing OpenMAX IL Components  **" COLOR_NC);
	puts(COLOR_YELLOW "*************************************" COLOR_NC);

	fprintf(stderr, COLOR_RED "%s():color red test %s\n"COLOR_NC,__FUNCTION__ );

	debug("%s():color red test %s\n", __FUNCTION__);

	char *s = "abcdef";
	int len = strlen(s);
	int i;
	for (i = 0; i < len; i++){
		fprintf(stderr, "%#x  ", (unsigned char)s[i]);
	}

	struct timeval tv;
	gettimeofday(&tv, NULL);
	char buf[256];
	__toUTCTime(tv, buf);
	fprintf(stderr, COLOR_GREEN "%s():%s\n"COLOR_NC,__FUNCTION__,buf);

	__toLocalTime(tv, buf);
	fprintf(stderr, COLOR_YELLOW "%s():%s\n"COLOR_NC, __FUNCTION__, buf);
	return 0;
}