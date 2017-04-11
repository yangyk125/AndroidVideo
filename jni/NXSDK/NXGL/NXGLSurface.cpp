/*
 * NXGLSurface.cpp
 *
 *  Created on: 2015-11-18
 *      Author: yangyk
 */

#include "NXSDK.h"
#include "NXGLSurface.h"

#include <android/log.h>

NXGLSurface::NXGLSurface(EGLint width, EGLint height, EGLConfig config, EGLint version)
{
	NXGL::NXGL_EGLInitialize();

	_nativeWindow  = NULL ;
	_surfaceWidth  = width ;
	_surfaceHeight = height ;
	_surfaceOpengl = NULL ;

	initSurface(config, version);
}

NXGLSurface::NXGLSurface(ANativeWindow* surface, EGLConfig config, EGLint version)
{
	NXGL::NXGL_EGLInitialize();

	_nativeWindow  = surface ;
	_surfaceWidth  = ANativeWindow_getWidth(_nativeWindow) ;
	_surfaceHeight = ANativeWindow_getHeight(_nativeWindow) ;
	_surfaceOpengl = NULL ;

	initSurface(config, version);
}

void NXGLSurface::initSurface(EGLConfig config, EGLint version)
{
	EGLBoolean result = EGL_FALSE ;

	if(_nativeWindow)
		_surfaceOpengl = eglCreateWindowSurface(NXGL::NXGL_EGLGetDisplay(), config, _nativeWindow, NULL);
	else {
		EGLint attris[] = {
			EGL_WIDTH, _surfaceWidth,
			EGL_HEIGHT, _surfaceHeight,
			EGL_NONE
		};
		_surfaceOpengl = eglCreatePbufferSurface(NXGL::NXGL_EGLGetDisplay(), config, attris);
	}

	if(!_surfaceOpengl)
	{
		uint32_t eglError = eglGetError();
		if(_nativeWindow)
			__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "EGL eglCreateWindowSurface() Error=0x%X", eglError);
		else
			__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "EGL eglCreatePbufferFromClientBuffer() Error=0x%X", eglError);
	}
}

NXGLSurface::~NXGLSurface()
{
	if(_surfaceOpengl)
	{
		eglDestroySurface(NXGL::NXGL_EGLGetDisplay(), _surfaceOpengl);
		_surfaceOpengl = 0 ;
	}

	_nativeWindow = NULL ;
	_surfaceWidth = 0 ;
	_surfaceHeight = 0 ;
}

bool NXGLSurface::isValid()
{
	return _surfaceOpengl ;
}

ANativeWindow* NXGLSurface::getNativeWindow()
{
	return _nativeWindow ;
}

EGLint NXGLSurface::getSurfaceWidth()
{
	return _surfaceWidth ;
}

EGLint NXGLSurface::getSurfaceHeight()
{
	return _surfaceHeight ;
}

EGLSurface NXGLSurface::getOpenGLSurface()
{
	return _surfaceOpengl ;
}
