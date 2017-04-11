/*
 * NXGLContext.h
 *
 *  Created on: 2015-9-1
 *      Author: yangyk
 */

#pragma once

#include "NXGL.h"
#include "NXGLSurface.h"

#include "../NXKT/NXPtr.hpp"

#include <android/window.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

class NXGLContext : public NXObject
{
private:
	EGLConfig			_curentConfig ;
	EGLContext			_eglesContext ;
	NXPtr<NXGLSurface>	_eglesSurface ;
public:
	//create context
	NXGLContext(bool onscreen, EGLint version = 2);
	//destory context
	virtual ~NXGLContext();
public:
	EGLConfig	getCurrentConfig() ;
public:
	bool 	isValid();
	bool 	setSurface(NXPtr<NXGLSurface>& surface) ;
	bool    makeCurrent();
	bool	releaseThread();
	bool    swapBuffers();
};

