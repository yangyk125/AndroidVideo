/*
 * NXGLTrimer.cpp
 *
 *  Created on: 2015-11-6
 *      Author: yangyk
 */

#include "NXGLTrimer.h"


static const char* TRIM_FRAG_SHADER_NV12 =
                "varying highp vec2 v_texCoord;\n"
                "uniform sampler2D  SamplerY;\n"
                "uniform sampler2D  SamplerUV;\n"
                "void main()\n"
                "{\n"
                    "highp vec3 yuv;\n"
                    "yuv.x = texture2D(SamplerY, v_texCoord).x;"
                    "highp vec4 uv = texture2D(SamplerUV, v_texCoord);\n"
                    "yuv.y = uv.r-0.5; \n"
                    "yuv.z = uv.a-0.5; \n"
                    "highp vec3 rgb = mat3(1,      1,     1,        \n"
                    "                      0,     -0.34,  1.77,     \n"
                    "                      1.42,  -0.71,  0) * yuv; \n"
                    "gl_FragColor = vec4(rgb, 1);\n"
                "}\n";

static const char* TRIM_FRAG_SHADER_NV21 =
                "varying highp vec2 v_texCoord;\n"
                "uniform sampler2D  SamplerY;\n"
                "uniform sampler2D  SamplerUV;\n"
                "void main()\n"
                "{\n"
                    "highp vec3 yuv;\n"
                    "yuv.x = texture2D(SamplerY, v_texCoord).x;"
                    "highp vec4 uv = texture2D(SamplerUV, v_texCoord);\n"
                    "yuv.z = uv.r-0.5; \n"
                    "yuv.y = uv.a-0.5; \n"
                    "highp vec3 rgb = mat3(1,      1,     1,        \n"
                    "                      0,     -0.34,  1.77,     \n"
                    "                      1.42,  -0.71,  0) * yuv; \n"
                    "gl_FragColor = vec4(rgb, 1);\n"
                "}\n";

static const char* TRIM_FRAG_SHADER_BGRA =
                "varying highp vec2 v_texCoord;\n"
                "uniform sampler2D  SamplerRGBA;\n"
                "void main()\n"
                "{\n"
                    "highp vec4 bgra = texture2D(SamplerRGBA, v_texCoord);"
                    "gl_FragColor = vec4(bgra.xyz, 1);\n"
                "}\n";

static const char* TRIM_VERTEX_SHADER =
                "uniform   mat4 u_Project ;   \n"
                "attribute vec4 a_Position;   \n"
                "attribute vec2 a_texCoord;   \n"
                "varying highp vec2 v_texCoord; \n"
                "void main()                  \n"
                "{                            \n"
                "   gl_Position = u_Project * a_Position;  \n"
                "   v_texCoord  = a_texCoord;   \n"
                "}                            \n";


NXGLTrimer::NXGLTrimer()
{
	_glContext 		= NULL;
	_imageWidth  	= 0;
	_imageHeight 	= 0;
	_canvasWidth    = 0;
	_canvasHeight   = 0;
	_glTextBGRAId   = 0;
	_glTextureYId   = 0;
	_glTextureUVId  = 0;
	_glImgProgramID = 0;

	_scaleCtxt 		= NULL ;
	_scaleBuff		= NULL ;
}

NXGLTrimer::~NXGLTrimer()
{
	_scaleCtxt 		= NULL ;
	_scaleBuff		= NULL ;
}

bool NXGLTrimer::open(const NXSize& sourceSize, const NXSize& targetSize, NXSDKPixelFormat infmt, NXSDKPixelFormat outfmt)
{
	_pixFormat = infmt ;
	_outFormat = outfmt ;

	_imageWidth = sourceSize.w ;
	_imageHeight = sourceSize.h ;

	_canvasWidth = targetSize.w ;
	_canvasHeight = targetSize.h ;

#ifdef DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() target_width=%d, target_height=%d", _canvasWidth, _canvasHeight);
#endif

	_glContext = new NXGLContext(false, NXGL_VERSION_OPENGLES2) ;
	if(!_glContext->isValid())
	{
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() context is invalid.");
		return false ;
	}

	_glSurface = new NXGLSurface(targetSize.w, targetSize.h, _glContext->getCurrentConfig(), NXGL_VERSION_OPENGLES2) ;
	if(!_glSurface->isValid())
	{
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() surface is invalid.");
		return false ;
	}

	if(!_glContext->setSurface(_glSurface))
	{
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() bind context and surface failure.");
		return false ;
	}

	if(!_glContext->makeCurrent())
	{
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() make context current failure.");
		return false ;
	}

	_glImgProgramID = buildImageProgram();
	if(_glImgProgramID == 0)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() image program is invalid.");
		return false ;
	}

	if (_pixFormat == NXSDK_PIX_FMT_NV12 || _pixFormat == NXSDK_PIX_FMT_NV21) {
		glGenTextures(1, &_glTextureYId);
		glGenTextures(1, &_glTextureUVId);
	}
	else if (_pixFormat == NXSDK_PIX_FMT_BGRA) {
		glGenTextures(1, &_glTextBGRAId);
	}

	if(_glTextureYId == 0 && _glTextureUVId == 0 && _glTextBGRAId == 0)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() texture id is invalid.");
		return false ;
	}

	if(_outFormat != NXSDK_PIX_FMT_RGBA) {
		_scaleCtxt = sws_getContext(_canvasWidth, _canvasHeight, (AVPixelFormat)NXKit::pixel_format_convert_nx(NXSDK_PIX_FMT_RGBA),
									_canvasWidth, _canvasHeight, (AVPixelFormat)NXKit::pixel_format_convert_nx(_outFormat), SWS_BICUBIC, NULL, NULL, NULL);
		_scaleBuff = new NXBuffer(_canvasWidth, _canvasHeight, NXSDK_PIX_FMT_RGBA) ;
	}

	__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() success.");

	return true ;
}


GLuint NXGLTrimer::buildImageProgram()
{
    GLuint vertexShader = this->buildShader(TRIM_VERTEX_SHADER, GL_VERTEX_SHADER);
    GLuint fragmentShader = 0 ;

    if(_pixFormat == NXSDK_PIX_FMT_NV12)
        fragmentShader = this->buildShader(TRIM_FRAG_SHADER_NV12, GL_FRAGMENT_SHADER) ;
    else if(_pixFormat == NXSDK_PIX_FMT_NV21)
    	fragmentShader = this->buildShader(TRIM_FRAG_SHADER_NV21, GL_FRAGMENT_SHADER) ;
    else if(_pixFormat == NXSDK_PIX_FMT_BGRA)
        fragmentShader = this->buildShader(TRIM_FRAG_SHADER_BGRA, GL_FRAGMENT_SHADER) ;

    GLuint programHandle = glCreateProgram();
    if (programHandle)
    {
        glAttachShader(programHandle, vertexShader);
        glAttachShader(programHandle, fragmentShader);
        glLinkProgram(programHandle);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* bufLog = (char*) malloc(bufLength+1);
                bufLog[bufLength] = 0 ;
                if (bufLog) {
                    glGetProgramInfoLog(programHandle, bufLength, NULL, bufLog);
                    printf("program log = %s", bufLog);
                    free(bufLog);
                }
            }
            glDeleteProgram(programHandle);
            programHandle = 0;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    return programHandle;
}


GLuint NXGLTrimer::buildShader(const char* source, GLenum shaderType)
{
    GLuint shaderHandle = glCreateShader(shaderType);

    if (shaderHandle)
    {
        glShaderSource(shaderHandle, 1, &source, 0);
        glCompileShader(shaderHandle);

        GLint compiled = 0;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)  {
                char* bufLog = (char*) malloc(infoLen+1);
                bufLog[infoLen] = 0 ;
                if (bufLog) {
                    glGetShaderInfoLog(shaderHandle, infoLen, NULL, bufLog);
                    printf("shader log = %s", bufLog);
                    free(bufLog);
                }
                glDeleteShader(shaderHandle);
                shaderHandle = 0;
            }
        }
    }
    return shaderHandle;
}

void NXGLTrimer::bindTextureY(GLuint texture, uint8_t* buffer, GLuint width, GLuint height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1) ;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void NXGLTrimer::bindTextureUV(GLuint texture, uint8_t* buffer, GLuint width, GLuint height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2) ;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void NXGLTrimer::bindTextureRGBA(GLuint texture, uint8_t* buffer, GLuint width, GLuint height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4) ;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


bool NXGLTrimer::param(const NXSDKTrimParam& param)
{
	_trimParams = param ;

    float vertexArray[] = {
        -1,  1,  0,
        -1, -1,  0,
         1, -1,  0,

        -1,  1,  0,
         1, -1,  0,
         1,  1,  0,
    };

    float left = _trimParams.cutrange.x ;
    float top  = _trimParams.cutrange.y ;
    float rigt = _trimParams.cutrange.x + _trimParams.cutrange.w ;
    float botm = _trimParams.cutrange.y + _trimParams.cutrange.h ;

    float textureX[6] ;
    if(!_trimParams.hreverse) //opengl default is updown !
    {
        textureX[0] = rigt ;
        textureX[1] = rigt ;
        textureX[2] = left ;
        textureX[3] = rigt ;
        textureX[4] = left ;
        textureX[5] = left ;
    }
    else
    {
        textureX[0] = left ;
        textureX[1] = left ;
        textureX[2] = rigt ;
        textureX[3] = left ;
        textureX[4] = rigt ;
        textureX[5] = rigt ;
    }

    float textureY[6] ;
    if(!_trimParams.vreverse)
    {
        textureY[0] = top ;
        textureY[1] = botm ;
        textureY[2] = botm ;
        textureY[3] = top ;
        textureY[4] = botm ;
        textureY[5] = top ;
    }
    else
    {
        textureY[0] = botm ;
        textureY[1] = top ;
        textureY[2] = top ;
        textureY[3] = botm ;
        textureY[4] = top ;
        textureY[5] = botm ;
    }

    float textureArray[] = {
        textureX[0],    textureY[0],
        textureX[1],    textureY[1],
        textureX[2],    textureY[2],

        textureX[3],    textureY[3],
        textureX[4],    textureY[4],
        textureX[5],    textureY[5]
    };

    memcpy(_vertexArray, vertexArray, sizeof(vertexArray));
    memcpy(_textureArray, textureArray, sizeof(textureArray));

	NXMatrix4 projectMatrix = NXMatrix4MakeOrtho(-1, 1, -1, 1, 1, -1);
	NXMatrix4 rotateMatrix =  NXMatrix4RotationZ(-_trimParams.rotate);
	NXMatrix4 lookAtMatrix = NXMatrix4MakeLookAt(0, 0, 1, 0, 0, 0, 0, -1, 0);

	_imageMVPMatrix = NXMatrix4Multiply(projectMatrix, NXMatrix4Multiply(lookAtMatrix, rotateMatrix));

	return true ;
}

bool NXGLTrimer::trim(NXPtr<NXBuffer>& source, NXOUT NXPtr<NXBuffer>& result)
{
	if (!_glContext->isValid()) {
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::draw() context is invalid.");
		return false;
	}

	if(!_glContext->makeCurrent())
		return false ;

	source->lockBuffer();
	if (_pixFormat == NXSDK_PIX_FMT_NV12 || _pixFormat == NXSDK_PIX_FMT_NV21) {
		uint8_t* bufferY = source->getPlanePtr(0);
		uint8_t* bufferUV = source->getPlanePtr(1);
		this->bindTextureY(_glTextureYId, bufferY, _imageWidth, _imageHeight);
		this->bindTextureUV(_glTextureUVId, bufferUV, _imageWidth>>1, _imageHeight>>1);
	} else if (_imageWidth == NXSDK_PIX_FMT_BGRA) {
		uint8_t* bufferBGRA = source->getPlanePtr(0);
		this->bindTextureRGBA(_imageWidth, bufferBGRA, _imageWidth, _imageHeight);
	}
	source->unlockBuffer();

	glViewport(0, 0, _canvasWidth, _canvasHeight);

	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(_glImgProgramID);
	GLint uProject = glGetUniformLocation(_glImgProgramID, "u_Project");
	glUniformMatrix4fv(uProject, 1, GL_FALSE, _imageMVPMatrix.m);

	GLint posAttrib = glGetAttribLocation(_glImgProgramID, "a_Position") ;
	GLint txtAttrib = glGetAttribLocation(_glImgProgramID, "a_texCoord") ;

	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(txtAttrib);

	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, _vertexArray);
	glVertexAttribPointer(txtAttrib, 2, GL_FLOAT, GL_FALSE, 0, _textureArray);

	if (_pixFormat == NXSDK_PIX_FMT_NV12 || _pixFormat == NXSDK_PIX_FMT_NV21) {
		GLint texYLocation = glGetUniformLocation(_glImgProgramID, "SamplerY");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _glTextureYId);
		glUniform1i(texYLocation, 0); //0 means GL_TEXTURE0

		GLint texUVLocation = glGetUniformLocation(_glImgProgramID,"SamplerUV");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _glTextureUVId);
		glUniform1i(texUVLocation, 1); //1 means GL_TEXTURE1
	}
	else if (_pixFormat == NXSDK_PIX_FMT_BGRA) {
		GLint texRGBALocation = glGetUniformLocation(_glImgProgramID,"SamplerRGBA");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _glTextBGRAId);
		glUniform1i(texRGBALocation, 0);
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	glFinish() ;

	if(!_glContext->swapBuffers())
		return false ;

	//shot the render result
	result = new NXBuffer(_canvasWidth, _canvasHeight, _outFormat);
	result->lockBuffer() ;

	if(_outFormat == NXSDK_PIX_FMT_RGBA) {
		uint8_t* ptrCanvas = result->getPlanePtr(0) ;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4) ;
		glReadPixels(0, 0, _canvasWidth, _canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, ptrCanvas) ;
	}
	else {

		uint8_t* ptrBuffer = _scaleBuff->getPlanePtr(0) ;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4) ;
		glReadPixels(0, 0, _canvasWidth, _canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, ptrBuffer) ;

		switch(_outFormat) {
			case NXSDK_PIX_FMT_YUV420P:
			{
				uint8_t* insbox[4] = {ptrBuffer, NULL} ;
				int 	 insize[4] = {_canvasWidth*4, 0} ;
				uint8_t* ousbox[4] = {result->getPlanePtr(0), result->getPlanePtr(1), result->getPlanePtr(2), NULL} ;
				int 	 ousize[4] = {_canvasWidth, _canvasWidth/2, _canvasWidth/2, 0} ;
				int ret = sws_scale(_scaleCtxt, insbox, insize, 0, _canvasHeight, ousbox, ousize);
				assert(ret == _canvasHeight) ;
				break ;
			}
			case NXSDK_PIX_FMT_NV12:
			case NXSDK_PIX_FMT_NV21:
			{
				uint8_t* insbox[4] = {ptrBuffer, NULL} ;
				int 	 insize[4] = {_canvasWidth*4, 0} ;
				uint8_t* ousbox[4] = {result->getPlanePtr(0), result->getPlanePtr(1), NULL} ;
				int 	 ousize[4] = {_canvasWidth, _canvasWidth, 0} ;
				int ret = sws_scale(_scaleCtxt, insbox, insize, 0, _canvasHeight, ousbox, ousize);
				assert(ret == _canvasHeight) ;
				break ;
			}
			case NXSDK_PIX_FMT_ARGB:
			case NXSDK_PIX_FMT_RGBA:
			case NXSDK_PIX_FMT_ABGR:
			case NXSDK_PIX_FMT_BGRA:
			{
				uint8_t* insbox[4] = {ptrBuffer, NULL} ;
				int 	 insize[4] = {_canvasWidth*4, 0} ;
				uint8_t* ousbox[4] = {result->getPlanePtr(0), NULL} ;
				int 	 ousize[4] = {_canvasWidth*4, 0} ;
				int ret = sws_scale(_scaleCtxt, insbox, insize, 0, _canvasHeight, ousbox, ousize);
				assert(ret == _canvasHeight) ;
				break ;
			}
		}
	}

	result->unlockBuffer() ;

	return true ;
}

bool NXGLTrimer::close()
{
	if(_glContext)
		_glContext->makeCurrent() ;

	if (_glImgProgramID)
	{
		glDeleteProgram(_glImgProgramID);
		_glImgProgramID = 0;

		if (_pixFormat == NXSDK_PIX_FMT_NV12 || _pixFormat == NXSDK_PIX_FMT_NV21)
		{
			glDeleteTextures(1, &_glTextureYId);
			glDeleteTextures(1, &_glTextureUVId);
			_glTextureYId = 0;
			_glTextureUVId = 0;
		}
		if (_pixFormat == NXSDK_PIX_FMT_BGRA)
		{
			glDeleteTextures(1, &_glTextBGRAId);
			_glTextBGRAId = 0;
		}
	}

	if(_scaleCtxt) {
		sws_freeContext(_scaleCtxt) ;
		_scaleCtxt = NULL ;
		_scaleBuff = NULL ;
	}

	_glContext = NULL ;
	_glSurface = NULL ;

	_imageWidth = 0 ;
	_imageHeight = 0 ;
	_canvasWidth = 0 ;
	_canvasHeight = 0 ;

	return true ;
}
