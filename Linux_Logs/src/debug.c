/*
* debug.c
*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

#include "debug.h"
#include "zlog.h"
//#include "main.h"
//#include "../xStudio/display.h"
static int create_logs=1;/* 0-means don't creat logs 1-means create logs */

static zlog_category_t *display_zc = NULL;
static zlog_category_t *http_zc = NULL;
static zlog_category_t *fpga_zc = NULL;

void debug_read_config(void)
{

}
void debug_save_config(void)
{

}
int debug_init(void)
{
	int rc;
	if (create_logs)
	{
		if ((rc = zlog_init(ZLOG_CONFIG)) != 0)
		{
			fprintf(stderr, "%s():init zlog failed\n", __FUNCTION__);
			return;
		}
		if ((display_zc = zlog_get_category(DISPLAY_CAT)) == NULL)
			fprintf(stderr, "%s():get %s failed\n", __FUNCTION__, DISPLAY_CAT);
		if ((http_zc = zlog_get_category(NET_CAT)) == NULL)
			fprintf(stderr, "%s():get %s failed\n", __FUNCTION__, NET_CAT);
		if ((fpga_zc = zlog_get_category(FPGA_CAT)) == NULL)
			fprintf(stderr, "%s():get %s failed\n", __FUNCTION__, FPGA_CAT);
	}
}
int debug_close(void)
{
	zlog_fini();
}

static void write_all_type_log(zlog_category_t *zc, int priority, const char *str)
{
	switch (priority)
	{
	default:
	case ZLOG_LEVEL_DEBUG :
		zlog_debug(zc, str);
		break;
	case ZLOG_LEVEL_INFO:
		zlog_info(zc, str);
		break;
	case ZLOG_LEVEL_NOTICE:
		zlog_notice(zc, str);
		break;
	case ZLOG_LEVEL_WARN:
		zlog_warn(zc, str);
		break;
	case ZLOG_LEVEL_ERROR:
		zlog_error(zc, str);
		break;
	case ZLOG_LEVEL_FATAL:
		zlog_fatal(zc, str);
		break;
	}
}
static void write_display_log(int  priority,const char *str)
{
	int rc; zlog_category_t *zc;
	
	/*if ((rc = zlog_init(ZLOG_CONFIG)) != 0)
	{
		fprintf(stderr, "%s():init zlog failed\n", __FUNCTION__);
		return;
	}
	if((zc = zlog_get_category(DISPLAY_CAT))==0)
	{
		fprintf(stderr,"%s():get %s failed\n",__FUNCTION__,DISPLAY_CAT);
		zlog_fini();
		return;
	}*/
	if (display_zc)
	{
		write_all_type_log(display_zc, priority, str);
		//zlog_fini();
	}
	return;
}
static void write_http_log(int  priority, const char *str)
{
	int rc; zlog_category_t *zc;
	/*if ((rc = zlog_init(ZLOG_CONFIG)) != 0)
	{
		fprintf(stderr, "%s():init zlog failed\n", __FUNCTION__);
		return;
	}
	if ((zc = zlog_get_category(NET_CAT)) == 0)
	{
		fprintf(stderr, "%s():get %s failed\n", __FUNCTION__, NET_CAT);
		zlog_fini();
		return;
	}*/
	if (http_zc)
	{
		write_all_type_log(http_zc, priority, str);
		//zlog_fini();
	}
	return;
}
static void write_fpga_log(int  priority, const char *str)
{
	int rc; zlog_category_t *zc;
	/*if ((rc = zlog_init(ZLOG_CONFIG)))
	{
		fprintf(stderr, "%s():init zlog failed\n", __FUNCTION__);
		return;
	}
	if (!(zc = zlog_get_category(FPGA_CAT)) )
	{
		fprintf(stderr, "%s():get %s failed\n", __FUNCTION__, FPGA_CAT);
		zlog_fini();
		return;
	}*/
	if (fpga_zc)
	{
		write_all_type_log(fpga_zc, priority, str);
		//zlog_fini();
	}
	return;
}
void log_write(const char *file, int line, int module, int  priority, const char *format, ...)
{
	va_list ap;
	char s[MAXSTRLEN]; char logs[MAXSTRLEN];
	va_start(ap, format);
	vsprintf(logs, format, ap);
	va_end(ap);
	sprintf(s, "[%s:%d]  %s", file,line, logs);
	switch (module)
	{
	case DISPLAY_LOG:
	{
		write_display_log(priority,s);
		break;
	}
	case HTTP_LOG:
	{
		write_http_log(priority, s);
		break;
	}
	case FPGA_LOG:
	{
		write_fpga_log(priority, s);
		break;
	}
	case NORMAL_LOG:
	default:
		fprintf(stderr, "%s\n", s);
		break;
	}
}
