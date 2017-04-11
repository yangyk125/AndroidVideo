/*
 * NXGL.h
 *
 *  Created on: 2015-9-8
 *      Author: yangyk
 */

#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class NXGL {
private:
	static EGLDisplay NXGL_DISPLAY ;
	static int   NXGL_MAJORVERSION ;
	static int   NXGL_MINORVERSION ;
public:
	static bool NXGL_EGLInitialize() ;
	static bool NXGL_EGLUninitialize() ;
	static int  NXGL_EGLMajorVersion() ;
	static int  NXGL_EGLMinorVersion() ;
	static EGLDisplay NXGL_EGLGetDisplay();
};



#define NXGL_VERSION_OPENGLES1 	1
#define NXGL_VERSION_OPENGLES2 	2
#define NXGL_VERSION_OPENGLES3 	3
