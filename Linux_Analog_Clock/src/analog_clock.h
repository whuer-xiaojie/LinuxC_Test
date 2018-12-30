/*
* ../xStudio/analog_clock.h
*/

#ifndef ANALOG_CLOCK_H
#define ANALOG_CLOCK_H

enum AnalogColockStyle{
	AGC_STYLE_DEFAULT,
	AGC_STYLE_FORMAT,
	AGC_STYLE_DATE,
	AGC_STYLE_WEEK
};

typedef struct AGC_Point_T{
	int x;
	int y;
}AGC_Point_T;

typedef struct AGC_Rect_T{
	int width;
	int height;
	AGC_Point_T start;
}AGC_Rect_T;

typedef struct AGC_Cricle_T{
	AGC_Point_T center;
	int radius;
} AGC_Cricle_T;

typedef struct AGC_Date_T{
	int year;
	int month;
	int day;
	int day_of_week;	/* Sunday = 0 */
}AGC_Date_T;

typedef struct AGC_Color_T{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
}AGC_Color_T;

typedef struct AGC_Pixel_T{
	uint8_t *pixle_data;
	int width;
	int height;
	int pixel_size;
}AGC_Pixel_T;

typedef struct AGC_Hand_T{
	AGC_Color_T color;
	AGC_Point_T start_point;
	AGC_Point_T end_point;
}AGC_Hand_T;

typedef struct AGC_Dial_T{
	AGC_Point_T center;
	AGC_Date_T  date;
	AGC_Pixel_T pixle_data;
	int style;
}AGC_Dial_T;

typedef struct AGC_Object_T {
	AGC_Dial_T dial;
	AGC_Hand_T hour_hand;
	AGC_Hand_T minute_hand;
	AGC_Hand_T second_hand;
}AGC_Object_T;


int8_t analog_clock_init();

#endif