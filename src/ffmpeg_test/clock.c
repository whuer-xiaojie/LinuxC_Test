/*
* clock.c
*/

#include <stdio.h>
#include <sys/select.h>

#include "clock.h"

/************************************************************************/
/*  
*函数：void __sleep(unsigned long ms)
*功能：系统调用，实现休眠
*参数：输入:unsigned long ms 需要休眠的时间单位毫秒
*/
/************************************************************************/
void __sleep(unsigned long ms)
{
	struct timeval tv;

	tv.tv_sec = ms / 1000;
	tv.tv_usec = ms % 1000 * 1000;
	select(0, NULL, NULL, NULL, &tv);
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
void __delay(unsigned long ms)
{
	unsigned long i, j;

	for (i = 0; i < ms; i++)
		for (j = 0; j < 60000; j++)
			__asm__("");
}
/************************************************************************/
/*    
*函数：unsigned long __clock(void)
*功能：获取系统当前的时间
*参数：
*输出：系统当前的时间（UTC）单位为毫秒
*/
/************************************************************************/
unsigned long __clock(void)
{
	struct timespec ts;
	unsigned long n_ticks;

	if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0)
		return 0;
	n_ticks = ts.tv_sec;
	n_ticks *= 1000;
	n_ticks += ((unsigned long)ts.tv_nsec + 500000L) / 1000000L;
	return n_ticks;
}
/************************************************************************/
/*  
*函数：struct tm* getCurTime(void)
*功能：获取系统当前的时间（当地时间）
*参数：
*输出：系统时间转换后当地时间结构体（sturct tm*）
*/
/************************************************************************/
struct tm* getCurTime(void)
{
	time_t timer;//time_t就是long int 类型
	struct tm *t;
	timer = time(NULL);
	t = localtime(&timer);
	return t;
}
