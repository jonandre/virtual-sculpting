#ifndef __LIBS
#define __LIBS
#include <GL/glew.h>
#include <GL/wglew.h>

#if defined(_M_IX86) && defined(WIN32)
	#pragma comment(lib, "glew32.lib")

#else
	#ifdef _M_X64
		#pragma comment(lib, "glew64.lib")
	#endif
#endif
#endif

