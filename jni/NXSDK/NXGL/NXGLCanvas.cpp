/*
 * NXGLCanvas.cpp
 *
 *  Created on: 2015-11-4
 *      Author: yangyk
 */

#include "NXGL.h"
#include "NXGLCanvas.h"

#include <android/log.h>

NXGLCanvas::NXGLCanvas()
{
	_frameBuffer  = 0 ;
	_rendrBuffer  = 0 ;
	_rendrTextur  = 0 ;
	_oldRndBuffer = 0 ;
	_oldFrmBuffer = 0 ;
}

NXGLCanvas::~NXGLCanvas()
{

}

bool NXGLCanvas::open(const NXSize& viewport)
{
	_targetSize = viewport ;

	GLint oldRBO, oldFBO;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);

	glGenTextures(1, &_rendrTextur) ;
	glBindTexture(GL_TEXTURE_2D, _rendrTextur);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    uint8_t* pdata = (uint8_t*)malloc(_targetSize.w*_targetSize.h*4) ;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)_targetSize.w, (GLsizei)_targetSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pdata);
    free(pdata) ;

	// generate FBO
	glGenFramebuffers(1, &_frameBuffer) ;
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer) ;

	glGenRenderbuffers(1, &_rendrBuffer) ;
	glBindRenderbuffer(GL_RENDERBUFFER, _rendrBuffer) ;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _rendrTextur, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "NXGLCanvas::open() Frame Buffer Invalid!!") ;

	glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);

	return true ;
}

bool NXGLCanvas::begin()
{
	glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint*)&_oldRndBuffer);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_oldFrmBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, _rendrBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);

	glViewport(0, 0, _targetSize.w, _targetSize.h) ;

	return true ;
}

NXPtr<NXBuffer> NXGLCanvas::end()
{
	NXPtr<NXBuffer> result = new NXBuffer(_targetSize.w, _targetSize.h, NXSDK_PIX_FMT_RGBA);
	result->lockBuffer() ;

	uint8_t* ptrCanvas = result->getPlanePtr(0) ;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4) ;
	glReadPixels(0, 0, (GLint)_targetSize.w, (GLint)_targetSize.h, GL_RGBA, GL_UNSIGNED_BYTE, ptrCanvas) ;

	result->unlockBuffer() ;

	glBindRenderbuffer(GL_RENDERBUFFER, _oldRndBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _oldFrmBuffer);

	return result ;
}

bool NXGLCanvas::close()
{
	glBindRenderbuffer(GL_RENDERBUFFER, _oldRndBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _oldFrmBuffer);

	glDeleteFramebuffers(1, &_frameBuffer) ;
	glDeleteRenderbuffers(1, &_rendrBuffer) ;
	glDeleteTextures(1,  &_rendrTextur) ;

	_frameBuffer = 0 ;
	_rendrBuffer = 0 ;
	_rendrTextur = 0 ;

	return true ;
}
