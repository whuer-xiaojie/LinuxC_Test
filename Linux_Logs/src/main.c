#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "zlog.h"
static void display_test()
{
	LOG_WRITE(DISPLAY_LOG, ZLOG_LEVEL_DEBUG, "%s(): the display debug log", __FUNCTION__);
	LOG_WRITE(DISPLAY_LOG, ZLOG_LEVEL_ERROR, "%s(): the display error log", __FUNCTION__);
}
static void http_test()
{
	LOG_WRITE(HTTP_LOG, ZLOG_LEVEL_DEBUG, "%s(): the http debug log",__FUNCTION__);
	LOG_WRITE(HTTP_LOG, ZLOG_LEVEL_INFO, "%s(): the http info log", __FUNCTION__);
}
static void fpga_test()
{
	LOG_WRITE(FPGA_LOG, ZLOG_LEVEL_DEBUG, "%s(): the fpga debug log", __FUNCTION__);
	LOG_WRITE(FPGA_LOG, ZLOG_LEVEL_NOTICE, "%s():the fpga debug log", __FUNCTION__); 
}

int main()
{
	debug_init();
	display_test();
	http_test();
	fpga_test();
	/*log_write(DISPLAY_LOG, ZLOG_LEVEL_DEBUG, "[%s:%d] the display debug log", __FILE__, __LINE__);
	log_write(DISPLAY_LOG, ZLOG_LEVEL_ERROR, "[%s:%d] the display error log", __FILE__, __LINE__);

	log_write(HTTP_LOG, ZLOG_LEVEL_DEBUG, "[%s:%d] the http debug log", __FILE__, __LINE__);
	log_write(HTTP_LOG, ZLOG_LEVEL_INFO, "[%s:%d] the http info log", __FILE__, __LINE__);

	log_write(FPGA_LOG, ZLOG_LEVEL_DEBUG, "[%s:%d] the fpga debug log", __FILE__, __LINE__);
	log_write(FPGA_LOG, ZLOG_LEVEL_NOTICE, "[%s:%d] the fpga debug log", __FILE__, __LINE__);*/
	debug_close();
	return 0;
}