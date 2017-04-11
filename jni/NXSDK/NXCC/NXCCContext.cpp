/*
 * NXCCContext.cpp
 *
 *  Created on: 2015-11-18
 *      Author: yangyk
 */

#include "NXCCContext.h"

NXCCContext::NXCCContext()
{
	_openglContext = new NXGLContext(true) ;
	_openglSurface = NULL ;
}

NXCCContext::~NXCCContext()
{
	_openglContext = NULL ;
	_openglSurface = NULL ;
}

NXPtr<NXGLContext>& NXCCContext::getDrawContext()
{
	return _openglContext ;
}

void NXCCContext::setDrawSurface(NXPtr<NXGLSurface>& suface)
{
	_openglSurface = suface ;
	_openglContext->setSurface(_openglSurface) ;
}

bool NXCCContext::hasDrawSurface()
{
	return _openglSurface != NULL ;
}

void NXCCContext::setCCCurrentContext()
{
	 _openglContext->makeCurrent() ;
}

void NXCCContext::setCCViewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h) ;
}

void NXCCContext::swapCCBuffers()
{
	if(_openglSurface)
		_openglContext->swapBuffers() ;
}

cocos2d::CCPoint NXCCContext::getCCWindowOrigin()
{
	return ccp(0, 0) ;
}

cocos2d::CCSize NXCCContext::getCCWindowSize()
{
	if(_openglSurface == NULL)
		return cocos2d::CCSize(0, 0) ;
	int32_t width = _openglSurface->getSurfaceWidth() ;
	int32_t height = _openglSurface->getSurfaceHeight() ;
	return cocos2d::CCSize(width, height) ;
}
