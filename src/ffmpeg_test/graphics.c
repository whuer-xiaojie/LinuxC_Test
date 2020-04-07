/*
*graphics.c
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __LCD

#include "lcd.h"

#endif

//#include "display.h"
//#include "graphics.h"

//x9 Show Window Size
#define X9_SCREEN_WIDTH	600
#define X9_SCREEN_HEIGHT	400

//SDL_OpenGL Global Variable
 SDL_Window *screen = NULL;

//x9_opengl context info define in the paint thread
extern SDL_GLContext gl_context;
extern GLuint program;/*the x9 shader program*/
extern GLint  positionLoc;/*the x9 shader variable entrance*/
extern GLint  texCoordLoc;
extern GLint  samplerLoc;
extern GLuint x9_textureId;/*the x9 paint texture name*/

//the x9 show region size
extern int screen_Width ;
extern int screen_Height;

#ifdef __LCD__
// LCD Show Window Info
#define LCD_SCREEN_WIDTH   300
#define LCD_SCREEN_HEIGHT  200

//SDL_OpenGL window Global Variable
 SDL_Window *lcd_screen = NULL;

 //lcd_opengl context  info define in the lcd_paint thread
 extern SDL_GLContext lcd_gl_context;

 extern GLuint lcd_program;/*the lcd shader program*/

 extern GLint  lcd_positionLoc;/*the lcd shader variable entrance*/
 extern GLint  lcd_texCoordLoc;
 extern GLint  lcd_samplerLoc;
 extern GLuint lcd_textureId;/*the lcd_paint texture name*/

 //the lcd show info size
 extern int lcd_Width;
 extern int lcd_Height;
#endif

/************************************************************************/
/*
*vertex Shader 源码
*attribute     变量用于获取定点属性信息(顶点坐标，纹理坐标，颜色等)
*a_position    存放顶点位置坐标信息（x,y）
*a_texCoord    存放纹理坐标信息(u,v)
*varying       变量用于和flagment Shader进行数据传递
*v_texCoord    传递纹理坐标信息给片元着色器  (经过GPU计算转化过的)
*/
/************************************************************************/

const char *vertex_shader_src =//"#version 200 core   \n"
"attribute vec4 a_position;    \n"
"attribute vec2 a_texCoord;    \n"
"varying vec2 v_texCoord;      \n"
"void main() {                 \n"
"    gl_Position = a_position; \n"
"    v_texCoord = a_texCoord;  \n"
"}                             \n";

/************************************************************************/
/*
*fragment Shader         源码
*precision mediump float 指定float类型变量的精度为中等精度
*v_texCoord              接受由vertex Shader传过来的纹理坐标
*s_texture               用于访问纹理坐标的接口（相当于全局变量）
*/
/************************************************************************/

const char *fragment_shader_src =//"#version 200 core   \n"
"precision mediump float;                             \n"
"varying vec2 v_texCoord;                             \n"
"uniform sampler2D s_texture;                         \n"
"void main() {                                        \n"
"    gl_FragColor = texture2D(s_texture, v_texCoord); \n"
"}                                                    \n";

/************************************************************************/
/*
*函数名：load_shader(const char *source, GLenum type)
*功能  ：创建着色器（Shader）并指定着色器的源码并进行编译
*参数  ：source 存放着色器（Shader）的源码的指针，type 需要创建的Shader的类型
*       （GL_VERTEX_SHADER和GL_FRAGMENT_SHADER）
*返回值：shader创建成功返回shader描述符，创建失败或者源码错误则返回0
*/
/************************************************************************/
static GLuint load_shader(const char *source, GLenum type)
{
	GLuint shader;
	const char *shaders[1] = { NULL };
	GLint status;
	//Create Shader
	if ((shader = glCreateShader(type)) == 0)
	{
		fprintf(stderr, "Create Shader failed\n");
		return 0;
	}

	//Get Shader Source Code
	shaders[0] = source;
	glShaderSource(shader, 1, shaders, NULL);
	shaders[0] = NULL;

	//Compile Shader Source Code
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		//Get Shader Compiled Error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			char * log = (char *)malloc(logLen);
			GLsizei written;
			glGetShaderInfoLog(shader, logLen, &written, log);
			fprintf(stderr, "Shaderlog:\n%s", log);
			free(log);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
/************************************************************************/
/*
*函数名：init_graphics(void)
*功能  ：初始化一个SDL窗口用于显示，并完成一个OpenGL的可执行程序对象（program）的
*        编译等，并获取该对象的定点着色器传递数据的“结点”
*参数  ：无
*返回值：初始化完成返回 0
*/
/************************************************************************/
int init_graphics(void)
{
	//Initialize SDL with Video Model
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "%s():Couldn't initialize SDL: %s\n", __FUNCTION__, SDL_GetError());
		exit(1);
		//atexit(SDL_Quit);
	}
	
	//Set RGB color Min_Size
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Set OpenGL Version 2.0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_ES);

	//Create the x9 Show Window
	if ((screen = SDL_CreateWindow("EZ_Display", 0, 0,
		X9_SCREEN_WIDTH, X9_SCREEN_HEIGHT, SDL_WINDOW_OPENGL)) == NULL)
	{
		fprintf(stderr, "%s():Couldn't set %dx%dx24 video mode: %s\n", __FUNCTION__,
			X9_SCREEN_WIDTH, X9_SCREEN_HEIGHT, SDL_GetError());
		exit(1);
		//atexit(SDL_Quit);
	}

#ifdef __LCD__
	//Create the lcd Show Window
	if ((lcd_screen = SDL_CreateWindow("LCD", X9_SCREEN_WIDTH, X9_SCREEN_HEIGHT,
		LCD_SCREEN_WIDTH, LCD_SCREEN_HEIGHT, SDL_WINDOW_OPENGL)) == NULL)
	{
		fprintf(stderr, "%s():Couldn't set %dx%dx24 video mode: %s\n", __FUNCTION__,
			LCD_SCREEN_WIDTH, LCD_SCREEN_HEIGHT, SDL_GetError());
		exit(1);
		//atexit(SDL_Quit);
	}
#endif // __LCD__

	return 0;
}

int x9_create_texture(void)
{
	GLint  status;
	GLuint vertex_shader;
	GLuint fragment_shader;
	int w, h;

	SDL_GetWindowSize(screen, &w, &h);

	//Create Opengl  Context
	gl_context = SDL_GL_CreateContext(screen);

	glEnable(GL_TEXTURE_2D);

	//Create Vertex Shader
	vertex_shader = load_shader(vertex_shader_src, GL_VERTEX_SHADER);
	if (vertex_shader == 0)
	{
		fprintf(stderr, "%s():Load vertex shader failed\n", __FUNCTION__);
		x9_close_texture();
		exit(1);
	}

	//Create Fragment Shader
	fragment_shader = load_shader(fragment_shader_src, GL_FRAGMENT_SHADER);
	if (fragment_shader == 0)
	{
		fprintf(stderr, "%s():Load fragment shader failed\n", __FUNCTION__);
		x9_close_texture();
		exit(1);
	}

	//Create a program
	if ((program = glCreateProgram()) == 0)
	{
		fprintf(stderr, "%s():glCreateProgram() failed\n", __FUNCTION__);
		x9_close_texture();
		exit(1);
	}

	//Attach Shader to Program
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	//Link program( A GPU executable program)
	glLinkProgram(program);

	//Get link Information
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		//Get link Error Info
		GLint infoLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char *infoLog = malloc(infoLen);
			glGetProgramInfoLog(program, infoLen, NULL, infoLog);
			fprintf(stderr, "%s():Link program: \"%s\"\n", __FUNCTION__, infoLog);
			free(infoLog);
		}
		glDeleteProgram(program);
		x9_close_texture();
		exit(1);
	}

	//Delete the shader
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	//Get Shader Start Point(position,texCoord,texture)
	positionLoc = glGetAttribLocation(program, "a_position");
	texCoordLoc = glGetAttribLocation(program, "a_texCoord");

	samplerLoc = glGetUniformLocation(program, "s_texture");

	//Create the View_port Start from Left_Top 
	//glViewport(0, h - screen_Height, screen_Width, screen_Height);

	//Init the Start Color
	glClearColor(0.0f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Show the Init_Window
	SDL_GL_SwapWindow(screen);
	SDL_ShowCursor(SDL_DISABLE);

	//get the x9 texture that used to update the show info
	glGenTextures(1, &x9_textureId);

	//Show the First Texture
	glBindTexture(GL_TEXTURE_2D, x9_textureId);

	//Set the Textrue's Attributes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
int x9_close_texture(void)
{
	glDeleteTextures(1, &x9_textureId);
	if (glIsProgram(program) == GL_TRUE)
	{
		glDeleteProgram(program);
	}
	SDL_GL_DeleteContext(gl_context);
	fprintf(stderr, "%s():x9_close_texture down\n", __FUNCTION__);
}

/************************************************************************/
/*
*函数名：int close_graphics(void)
*功能  ：释放初始化中创建的SDL窗口和Shader容器program以及Opengl的
*        上下文对象gl_context
*参数  ：无
*返回值：0表示释放完成
*/
/************************************************************************/
int close_graphics(void)
{
	SDL_DestroyWindow(screen);
#ifdef __LCD__
	SDL_DestroyWindow(lcd_screen);
#endif
	SDL_Quit();
      fprintf(stderr, "%s():lose_graphics down\n", __FUNCTION__);
	return 0;
}

/************************************************************************/
/*
*/
/************************************************************************/
void listbox_add(const char *s)
{
	fprintf(stderr, "%s\n", s);
}


/************************************************************************/
/*
*函数名：x9_render_buffer_R8G8B8(const unsigned char *buf)
*功能  ：利用Opengl的渲染管道把存放在buf中的图像显示到初始化的窗口上
*
*参数  ：输入：buf 存放需要显示的图像的位图信息（24位RGB格式存放）
*返回值：无
*/
/************************************************************************/
void x9_render_buffer_R8G8B8(const unsigned char *buf)
{
	//Init the Vertex Shader Info

	//Start From Left_Top 
	GLfloat vertices[] = { 
		-1.0f,  1.0f,  0.0f,	/* Position 0 */ 0.0f, 0.0f,		/* TexCooed 0 */
		-1.0f, -1.0f,  0.0f,	/* Position 1 */ 0.0f, 1.0f,		/* TexCooed 1 */
		 1.0f,  -1.0f,  0.0f,	/* Position 2 */ 1.0f, 1.0f,		/* TexCooed 2 */
		 1.0f,   1.0f,  0.0f,	/* Position 3 */ 1.0f, 0.0f		   /* TexCooed 3 */};

	//Include Position Color and texCoord 
	//GLfloat vertices[] = {
	//   -1.0f, 1.0f,0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f,
	//    0.0f,-1.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,
	//    0.0f, 1.0f,0.0f, 0.0f,0.0f,1.0f, 1.0f,1.0f,
	//   -1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f, 0.0f,1.0f };

	//Start from the Left_Bottom
	//GLfloat vertices[] = {
	//	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,		/* Bottom Left  */
	//	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,		/* Bottom Right */
	//	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,		/* Top Right */
	//	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f		/* Top Left */
	//};

	GLushort indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};


	if (buf == NULL)
	{
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(screen);
		SDL_ShowCursor(SDL_DISABLE);
		fprintf(stderr, "%s():no show data \n",__FUNCTION__);
		return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//Get the Show Data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_Width , screen_Height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, buf);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, 
	//	GL_RGB, GL_UNSIGNED_BYTE, buf);

	glUseProgram(program);

	//Transfer the Position and TexCoord Data to Vertex Shader
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE,
		5 * sizeof(GLfloat), vertices);
	glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE,
		5 * sizeof(GLfloat), &vertices[3]);
	glEnableVertexAttribArray(positionLoc);
	glEnableVertexAttribArray(texCoordLoc);

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, x9_textureId);

	glUniform1i(samplerLoc, 0);
	//Draw the Picture
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

	//Show picture to the Window
	SDL_GL_SwapWindow(screen);
	SDL_ShowCursor(SDL_DISABLE);
}

void x9_render_buffer_R8G8B8A8(const unsigned char *buf)
{

}

#ifdef __LCD__
int lcd_create_texture(void)
{
	GLint  lcd_status;
	GLuint lcd_vertex_shader;
	GLuint lcd_fragment_shader;
	
	//Create Opengl  Context
	lcd_gl_context = SDL_GL_CreateContext(lcd_screen);

	glEnable(GL_TEXTURE_2D);

	//Create Vertex Shader
	lcd_vertex_shader = load_shader(vertex_shader_src, GL_VERTEX_SHADER);
	if (lcd_vertex_shader == 0)
	{
		fprintf(stderr, "%s():Load vertex shader failed\n", __FUNCTION__);
		lcd_close_texture();
		exit(1);
	}

	//Create Fragment Shader
	lcd_fragment_shader = load_shader(fragment_shader_src, GL_FRAGMENT_SHADER);
	if (lcd_fragment_shader == 0)
	{
		fprintf(stderr, "%s():Load fragment shader failed\n", __FUNCTION__);
		lcd_close_texture();
		exit(1);
	}
	//Create a program
	if ((lcd_program = glCreateProgram()) == 0)
	{
		fprintf(stderr, "%s():glCreateProgram() failed\n", __FUNCTION__);
		lcd_close_texture();
		exit(1);
	}

	//Attach Shader to Program
	glAttachShader(lcd_program, lcd_vertex_shader);
	glAttachShader(lcd_program, lcd_fragment_shader);

	//Link program( A GPU executable program)
	glLinkProgram(lcd_program);
	//Get link Information
	glGetProgramiv(lcd_program, GL_LINK_STATUS, &lcd_status);
	if (!lcd_status) 
	{
		//Get link Error Info
		GLint lcd_infoLen;
		glGetProgramiv(lcd_program, GL_INFO_LOG_LENGTH, &lcd_infoLen);
		if (lcd_infoLen > 1)
		{
			char *lcd_infoLog = malloc(lcd_infoLen);
			glGetProgramInfoLog(lcd_program, lcd_infoLen, NULL, lcd_infoLog);
			fprintf(stderr, "%s():Link program: \"%s\"\n", __FUNCTION__, lcd_infoLog);
			free(lcd_infoLog);
		}
		glDeleteProgram(lcd_program);
		lcd_close_texture();
		exit(1);
	}
	//Delete the shader
	glDeleteShader(lcd_vertex_shader);
	glDeleteShader(lcd_fragment_shader);

	//Get Shader Start Point(position,texCoord,texture)
	lcd_positionLoc = glGetAttribLocation(lcd_program, "a_position");
	lcd_texCoordLoc = glGetAttribLocation(lcd_program, "a_texCoord");
	lcd_samplerLoc = glGetUniformLocation(lcd_program, "s_texture");

	//Create the View_port Start from Left_Top 
	//glViewport(0, 640, 500, 500);
	//Init the Start Color
	glClearColor(1.0f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Show the Init_Window
	SDL_GL_SwapWindow(lcd_screen);
	SDL_ShowCursor(SDL_DISABLE);

	//get the lcd texture that define in the lcd thread
	glGenTextures(1, &lcd_textureId);
	//Bind the  lcd_Texture
	glBindTexture(GL_TEXTURE_2D, lcd_textureId);

	//Set the Textrue's Attributes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

int lcd_close_texture(void)
{
	glDeleteTextures(1, &lcd_textureId);
	if (glIsProgram(lcd_program) == GL_TRUE)
	{
		glDeleteProgram(lcd_program);
	}
	SDL_GL_DeleteContext(lcd_gl_context);
	
	fprintf(stderr, "%s():lcd_close_texture down\n",__FUNCTION__);
}
void lcd_render_buffer_R8G8B8(const unsigned char *buf)
{
	//Init the Vertex Shader Info

	//Start From Left_Top 
	//GLfloat vertices[] = { 
	//	-1.0f,  1.0f,  0.0f,	/* Position 0 */ 0.0f, 0.0f,		/* TexCooed 0 */
	//	-1.0f, -1.0f,  0.0f,	/* Position 1 */ 0.0f, 1.0f,		/* TexCooed 1 */
	//	 1.0f,  -1.0f,  0.0f,	/* Position 2 */ 1.0f, 1.0f,		/* TexCooed 2 */
	//	 1.0f,   1.0f,  0.0f,	/* Position 3 */ 1.0f, 0.0f		   /* TexCooed 3 */};

	//Include Position Color and texCoord 
	//GLfloat vertices[] = {
	//   -1.0f, 1.0f,0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f,
	//    0.0f,-1.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,
	//    0.0f, 1.0f,0.0f, 0.0f,0.0f,1.0f, 1.0f,1.0f,
	//   -1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f, 0.0f,1.0f };

	//Start from the Left_Bottom
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,		/* Bottom Left  */
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,		/* Bottom Right */
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,		/* Top Right */
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f		/* Top Left */
	};

	GLushort indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	if (buf == NULL)
	{
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(lcd_screen);
		SDL_ShowCursor(SDL_DISABLE);
		fprintf(stderr, "%s():no show data \n",__FUNCTION__);
		return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Get the Show Data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, lcd_Width, lcd_Height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, buf);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imageWidth, imageHeight, 
	//	GL_RGB, GL_UNSIGNED_BYTE, buf);

	glUseProgram(lcd_program);

	//Transfer the Position and TexCoord Data to Vertex Shader
	glVertexAttribPointer(lcd_positionLoc, 3, GL_FLOAT, GL_FALSE,
		5 * sizeof(GLfloat), vertices);
	glVertexAttribPointer(lcd_texCoordLoc, 2, GL_FLOAT, GL_FALSE,
		5 * sizeof(GLfloat), &vertices[3]);
	glEnableVertexAttribArray(lcd_positionLoc);
	glEnableVertexAttribArray(lcd_texCoordLoc);

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, lcd_textureId);

	glUniform1i(lcd_samplerLoc, 0);

	//Draw the Picture
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

	//Show picture to the Window
	SDL_GL_SwapWindow(lcd_screen);
	SDL_ShowCursor(SDL_DISABLE);
}
void lcd_render_buffer_R8G8B8A8(const unsigned char *buf)
{

}
#endif // __LCD__
