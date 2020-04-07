#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

int init_graphics(void);/*init the SDL and create the show window*/

int close_graphics(void);/*close the show window and quit SDL*/

void listbox_add(const char *s);

void x9_render_buffer_R8G8B8(const unsigned char *buf);/*draw the buffer to the window*/

int x9_create_texture(void);/*get the x9 thread context and texture from the x9 show window*/

int x9_close_texture(void);/*close the x9 thread context and texture*/

#ifdef __LCD__
void lcd_render_buffer_R8G8B8(const unsigned char *buf);/*draw the  buffer to the lcd show window*/

int lcd_create_texture(void);/*get the lcd thread context and texture from the lcd show window*/

int lcd_close_texture(void);/*close the lcd context and texture*/
#endif

#endif