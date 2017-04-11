/*
 * NXGLDrawer.h
 *
 *  Created on: 2015-9-17
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"
#include "NXGLContext.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "NXGLMatrix.h"

class NXGLDrawer: public NXSDKDrawer
{
private:
	NXPtr<NXGLContext>	_glContext ;
	NXPtr<NXGLSurface>	_glSurface ;
	NXSDKPixelFormat	_pixFormat ;
	NXSDKDrawType		_drawrType ;

	NXRect				_drawRect  ;

	uint32_t 			_imageWidth ;
	uint32_t 			_imageHeight ;

	uint32_t 			_canvasWidth ;
	uint32_t 			_canvasHeight ;

	GLuint              _glTextBGRAId  ;
	GLuint              _glTextureYId  ;
	GLuint              _glTextureUVId ;

	GLuint              _glImgProgramID ;
	GLuint              _glBoxProgramID ;

	NXMatrix4 			_imageMVPMatrix ;
	NXMatrix4 			_rtboxMVPMatrix ;

	float				_imageRotation ;

	float 				_vertexArray[18] ;
	float 				_textureArray[12] ;

public:
	NXGLDrawer();
	virtual ~NXGLDrawer();
private:
	GLuint buildImageProgram() ;
	GLuint buildRectProgram() ;
	GLuint buildShader(const char* source, GLenum shaderType) ;
	void   bindTextureY(GLuint texture, uint8_t* buffer, GLuint width, GLuint height) ;
	void   bindTextureUV(GLuint texture, uint8_t* buffer, GLuint width, GLuint height) ;
	void   bindTextureRGBA(GLuint texture, uint8_t* buffer, GLuint width, GLuint height) ;
	void   display(uint32_t rectColor) ;
public:
	virtual bool open(uint32_t width, uint32_t height, NXSDKPixelFormat pixfmt, ANativeWindow* surface) ;
	virtual bool mode(NXSDKDrawType mode) ;
	virtual bool rotate(float radian) ;
	virtual bool draw(NXPtr<NXBuffer>& vbuffer) ;
	virtual bool draw(NXPtr<NXBuffer>& vbuffer, const LPNXRect rect, uint32_t color) ;
	virtual bool close() ;
};


