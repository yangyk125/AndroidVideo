/*
 * NXGLSurface.h
 *
 *  Created on: 2015-11-18
 *      Author: yangyk
 */

#pragma once

#include "NXGL.h"

#include "../NXKT/NXDef.h"


class NXGLSurface: public NXObject
{
private:
	ANativeWindow*	_nativeWindow ;
	EGLint			_surfaceWidth ;
	EGLint			_surfaceHeight ;
	EGLSurface		_surfaceOpengl ;
public:
	//create surface off-screen
	NXGLSurface(EGLint width, EGLint height, EGLConfig config, EGLint version = 2);
	//create surface on-surface with ui surface.
	NXGLSurface(ANativeWindow* surface, EGLConfig config, EGLint version = 2) ;
	//init
	void initSurface(EGLConfig config, EGLint version);
	//delete opengl surface
	virtual ~NXGLSurface();
public:
	bool 			isValid();
	ANativeWindow*	getNativeWindow() ;
	EGLint			getSurfaceWidth() ;
	EGLint			getSurfaceHeight() ;
	EGLSurface		getOpenGLSurface() ;
};

