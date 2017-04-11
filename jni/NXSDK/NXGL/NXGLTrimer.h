/*
 * NXGLTrimer.h
 *
 *  Created on: 2015-11-6
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

#include "NXGLContext.h"
#include "NXGLSurface.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

#include "NXGLMatrix.h"

class NXGLTrimer: public NXSDKTrimer
{
private:
	NXPtr<NXGLContext>	_glContext ;
	NXPtr<NXGLSurface>	_glSurface ;

	NXSDKPixelFormat	_pixFormat ;
	NXSDKPixelFormat	_outFormat ;

	uint32_t 			_imageWidth ;
	uint32_t 			_imageHeight ;

	uint32_t 			_canvasWidth ;
	uint32_t 			_canvasHeight ;

	GLuint              _glTextBGRAId  ;
	GLuint              _glTextureYId  ;
	GLuint              _glTextureUVId ;

	GLuint              _glImgProgramID ;

	NXMatrix4 			_imageMVPMatrix ;

	NXSDKTrimParam		_trimParams ;

	float 				_vertexArray[18] ;
	float 				_textureArray[12] ;

	SwsContext*			_scaleCtxt ;
	NXPtr<NXBuffer>		_scaleBuff ;

public:
	NXGLTrimer();
	virtual ~NXGLTrimer();
private:
private:
	GLuint buildImageProgram() ;
	GLuint buildShader(const char* source, GLenum shaderType) ;
	void   bindTextureY(GLuint texture, uint8_t* buffer, GLuint width, GLuint height) ;
	void   bindTextureUV(GLuint texture, uint8_t* buffer, GLuint width, GLuint height) ;
	void   bindTextureRGBA(GLuint texture, uint8_t* buffer, GLuint width, GLuint height) ;
public:
	virtual bool open(const NXSize& sourceSize, const NXSize& targetSize, NXSDKPixelFormat infmt, NXSDKPixelFormat outfmt) ;
	virtual bool param(const NXSDKTrimParam& param) ;
	virtual bool trim(NXPtr<NXBuffer>& source, NXOUT NXPtr<NXBuffer>& result) ;
	virtual bool close() ;
};

