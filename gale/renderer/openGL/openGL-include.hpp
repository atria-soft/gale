/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

extern "C" {
	#if defined(__TARGET_OS__Linux)
		// TO ENABLE THE SHADER api ...
		#define GL_GLEXT_PROTOTYPES
		#include <GL/gl.h>
		// TODO : Check it it work
		// This is to prevent the use of these element that is not allowed in the openGL ES
		#undef glVertexPointer
		#undef glTexCoordPointer
		#undef glColorPointer
		#undef glPopMatrix
		#undef glPushMatrix
		#undef glMatrixMode
		#undef glLoadIdentity
		#undef glTranslatef
	#elif defined(__TARGET_OS__Android)
		// Include openGL ES 2
		#include <GLES3/gl3.h>
		#include <GLES2/gl2ext.h>
	#elif defined(__TARGET_OS__Windows)
		// TO ENABLE THE SHADER api ...
		//#define GL_GLEXT_PROTOTYPES
		#define GLEW_STATIC
		#include <GL/glew.h>
	#elif defined(__TARGET_OS__MacOs)
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
	#elif defined(__TARGET_OS__IOs)
		#include <OpenGLES/ES2/gl.h>
	#else
		#error you need to specify a __TAGET_OS__ ...
	#endif
}

