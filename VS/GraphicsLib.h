#ifndef __LIBS
#define __LIBS
#include "GL\glew.h"
#include "GL\wglew.h"

#ifdef _M_IX86
	#pragma comment(lib, "glew32.lib")

#else
	#ifdef _M_X64
		#pragma comment(lib, "glew64.lib")
	#endif
#endif
#endif __LIBS
