/*
* ../xStudio/analog_clock.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "analog_clock.h"
#include "freetype2/freetype/freetype.h"
#include "freetype2/freetype/ftglyph.h"

static int width = 800;
static int height = 600;
static int pixel_size = 3;

static void AGC_Draw_Point(AGC_Point_T *point,AGC_Color_T *color,AGC_Pixel_T *pixle)
{
	if (NULL == point || NULL == color || NULL == pixle ||NULL==pixle->pixle_data)
		return;
	if ((point->x >= 0 && point->y <= pixle->width) && (point->y >= 0 && point->y <= pixle->height)){
		uint8_t *p = pixle->pixle_data + (pixle->width*point->y + point->x)*pixle->pixel_size;
		*p++ = color->red;
		*p++ = color->green;
		*p++ = color->blue;
	}
}
static void AGC_Draw_Rect()
{

}
static void AGC_Draw_Cricle()
{

}
static void AGC_Draw_Line()
{

}
static void AGC_Fill_Rect()
{

}
static void AGC_Fill_Triangle()
{

}
static void AGC_Fill_Cricle()
{

}
