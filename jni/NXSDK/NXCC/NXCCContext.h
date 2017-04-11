/*
 * NXCCContext.h
 *
 *  Created on: 2015-11-18
 *      Author: yangyk
 */

#pragma once

#include "cocos2d.h"

#include "../NXGL/NXGLContext.h"
#include "../NXGL/NXGLSurface.h"

class NXCCContext: public cocos2d::CCContext
{
private:
	NXPtr<NXGLContext>	_openglContext ;
	NXPtr<NXGLSurface>	_openglSurface ;
public:
	NXCCContext();
	virtual ~NXCCContext();
public:
	NXPtr<NXGLContext>& getDrawContext();
	void  setDrawSurface(NXPtr<NXGLSurface>& suface) ;
	bool  hasDrawSurface() ;
public:
	virtual void setCCCurrentContext() ;
	virtual void setCCViewport(int x, int y, int w, int h) ;
	virtual void swapCCBuffers() ;
	virtual cocos2d::CCPoint getCCWindowOrigin() ;
	virtual cocos2d::CCSize getCCWindowSize()  ;
};

