/*
 * NXGL.cpp
 *
 *  Created on: 2015-9-8
 *      Author: yangyk
 */

#include "NXGL.h"

#include <android/log.h>
#include <string.h>

int NXGL::NXGL_MAJORVERSION = 0 ;
int NXGL::NXGL_MINORVERSION = 0 ;
EGLDisplay NXGL::NXGL_DISPLAY = 0 ;

bool NXGL::NXGL_EGLInitialize()
{
	if(NXGL::NXGL_DISPLAY)
		return true ;

	NXGL::NXGL_DISPLAY = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(!NXGL::NXGL_DISPLAY)
		return false ;

	EGLBoolean result = eglInitialize(NXGL::NXGL_DISPLAY, &NXGL::NXGL_MAJORVERSION, &NXGL::NXGL_MINORVERSION) ;
	if(!result)
		return false ;

#ifdef DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "EGLInitialize Major=%d, Minor=%d.", NXGL::NXGL_MAJORVERSION, NXGL::NXGL_MINORVERSION) ;
#endif

	return true ;
}
bool NXGL::NXGL_EGLUninitialize()
{
	eglTerminate(NXGL::NXGL_DISPLAY);
	NXGL::NXGL_MAJORVERSION = 0 ;
	NXGL::NXGL_MINORVERSION = 0 ;
	NXGL::NXGL_DISPLAY = 0 ;
	return true ;
}
int  NXGL::NXGL_EGLMajorVersion()
{
	return NXGL::NXGL_MAJORVERSION ;
}
int  NXGL::NXGL_EGLMinorVersion()
{
	return NXGL::NXGL_MINORVERSION ;
}
EGLDisplay NXGL::NXGL_EGLGetDisplay()
{
	return NXGL::NXGL_DISPLAY ;
}
