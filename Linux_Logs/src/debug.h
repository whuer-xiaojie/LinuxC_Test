/*
*debug.h
*/
#ifndef DEBUG_H
#define DEBUG_H

#define ZLOG_CONFIG "/etc/log.conf"
#define MAXSTRLEN 10240
#define DISPLAY_CAT "dis_cat"
#define NET_CAT "net_cat"
#define FPGA_CAT "fpga_cat"
#define RENDER_CAT "render_cat"

#define DEBUG 20
#define INFO 40
#define NOTICE 60
#define WARN 80
#define ERROR 100
#define FATAL 120
#define WRITE_FILE		(1 << 6)

#define LOG_WRITE(module,priority,format,argc...) log_write(__FILE__,__LINE__,module,priority,format,##argc)

enum Logmodule
{
	NORMAL_LOG = 0,
	DISPLAY_LOG,
	HTTP_LOG,
	FPGA_LOG,
};
void debug_read_config(void);
void debug_save_config(void);
int debug_init(void);
int debug_close(void);

void log_write(const char *file, int line, int module, int  priority, const char *format, ...);




#endif