/*
 * NXGLDrawer.cpp
 *
 *  Created on: 2015-9-17
 *      Author: yangyk
 */

#include "NXSDK.h"
#include "NXGLDrawer.h"

#include <stdio.h>


static const char* DRAW_FRAG_SHADER_NV12 =
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

static const char* DRAW_FRAG_SHADER_NV21 =
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

static const char* DRAW_FRAG_SHADER_BGRA =
                "varying highp vec2 v_texCoord;\n"
                "uniform sampler2D  SamplerRGBA;\n"
                "void main()\n"
                "{\n"
                    "highp vec4 bgra = texture2D(SamplerRGBA, v_texCoord);"
                    "gl_FragColor = vec4(bgra.xyz, 1);\n"
                "}\n";

static const char* DRAW_VERTEX_SHADER =
                "uniform   mat4 u_Project ;   \n"
                "attribute vec4 a_Position;   \n"
                "attribute vec2 a_texCoord;   \n"
                "varying highp vec2 v_texCoord; \n"
                "void main()                  \n"
                "{                            \n"
                "   gl_Position = u_Project * a_Position;  \n"
                "   v_texCoord  = a_texCoord;   \n"
                "}                            \n";


static const char* RECT_FRAG_SHADER =
        "uniform highp vec4 u_LineColor ; \n"
        "void main()\n"
        "{\n"
            "gl_FragColor = u_LineColor ;\n"
        "}\n";

static const char* RECT_DRAW_VERTEX_SHADER =
        "uniform   mat4 u_Project ;   \n"
        "attribute vec4 a_Position;   \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = u_Project * a_Position;  \n"
        "}                            \n";


NXGLDrawer::NXGLDrawer()
{
	_imageRotation = 0 ;
}

NXGLDrawer::~NXGLDrawer()
{
	this->close();
}

bool NXGLDrawer::open(uint32_t width, uint32_t height, NXSDKPixelFormat pixfmt, ANativeWindow* surface)
{
	_pixFormat = pixfmt ;

	_imageWidth = width ;
	_imageHeight = height ;

	_canvasWidth = ANativeWindow_getWidth(surface);
	_canvasHeight = ANativeWindow_getHeight(surface);

#ifdef DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLDrawer::open() surface_width=%d, surface_height=%d", _canvasWidth, _canvasHeight);
#endif

	_glContext = new NXGLContext(true, NXGL_VERSION_OPENGLES2) ;
	if(!_glContext->isValid())
	{
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLTrimer::open() context is invalid.");
		return false ;
	}

	_glSurface = new NXGLSurface(surface, _glContext->getCurrentConfig(), NXGL_VERSION_OPENGLES2) ;
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
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLDrawer::open() image program is invalid.");
		return false ;
	}

	_glBoxProgramID = buildRectProgram();
	if(_glBoxProgramID == 0)
	{
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLDrawer::open() rect program is invalid.");
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
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLDrawer::open() texture id is invalid.");
		return false ;
	}

	this->mode(NXSDKDrawTwist) ;
	this->rotate(0) ;

	__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLDrawer::open() success.");

	return true ;
}


GLuint NXGLDrawer::buildImageProgram()
{
    GLuint vertexShader = this->buildShader(DRAW_VERTEX_SHADER, GL_VERTEX_SHADER);
    GLuint fragmentShader = 0 ;

    if(_pixFormat == NXSDK_PIX_FMT_NV12)
        fragmentShader = this->buildShader(DRAW_FRAG_SHADER_NV12, GL_FRAGMENT_SHADER) ;
    else if(_pixFormat == NXSDK_PIX_FMT_NV21)
    	fragmentShader = this->buildShader(DRAW_FRAG_SHADER_NV21, GL_FRAGMENT_SHADER) ;
    else if(_pixFormat == NXSDK_PIX_FMT_BGRA)
        fragmentShader = this->buildShader(DRAW_FRAG_SHADER_BGRA, GL_FRAGMENT_SHADER) ;

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

GLuint NXGLDrawer::buildRectProgram()
{
    GLuint vertexShader = this->buildShader(RECT_DRAW_VERTEX_SHADER, GL_VERTEX_SHADER);
    GLuint fragmentShader = this->buildShader(RECT_FRAG_SHADER, GL_FRAGMENT_SHADER);

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

GLuint NXGLDrawer::buildShader(const char* source, GLenum shaderType)
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
            if (infoLen) {
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

void NXGLDrawer::bindTextureY(GLuint texture, uint8_t* buffer, GLuint width, GLuint height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1) ;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void NXGLDrawer::bindTextureUV(GLuint texture, uint8_t* buffer, GLuint width, GLuint height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2) ;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void NXGLDrawer::bindTextureRGBA(GLuint texture, uint8_t* buffer, GLuint width, GLuint height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4) ;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


bool NXGLDrawer::mode(NXSDKDrawType mode)
{
	_drawrType = mode ;

	float vertexArray[18] =  {
			-1,  1, 0,
			-1, -1, 0,
			 1, -1, 0,
			-1,  1, 0,
			 1, -1, 0,
			 1,  1, 0, };

	float textureArray[12] =  { 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0 };

	if (_drawrType == NXSDKDrawTwist) {
			//原始大小。
	}
	else if (_drawrType == NXSDKDrawCenter)
	{
		float maxsize = MAX(_imageWidth, _imageHeight);
		float swidth = _imageWidth / maxsize;
		float sheight = _imageHeight / maxsize;

		float x = (1.0f - swidth) / 2;
		float y = (1.0f - sheight) / 2;
		float w = swidth;
		float h = sheight;

		vertexArray[0] = -1.0 + 2 * x;
		vertexArray[1] = 1.0 - 2 * y;
		vertexArray[2] = 0;
		vertexArray[3] = -1.0 + 2 * x;
		vertexArray[4] = 1.0 - 2 * (y + h);
		vertexArray[5] = 0;
		vertexArray[6] = -1.0 + 2 * (x + w);
		vertexArray[7] = 1.0 - 2 * (y + h);
		vertexArray[8] = 0;

		vertexArray[9] = -1.0 + 2 * x;
		vertexArray[10] = 1.0 - 2 * y;
		vertexArray[11] = 0;
		vertexArray[12] = -1.0 + 2 * (x + w);
		vertexArray[13] = 1.0 - 2 * (y + h);
		vertexArray[14] = 0;
		vertexArray[15] = -1.0 + 2 * (x + w);
		vertexArray[16] = 1.0 - 2 * y;
		vertexArray[17] = 0;
	}
	else if (_drawrType == NXSDKDrawCutout)
	{

		float minsize = MIN(_imageWidth, _imageHeight);
		float dwidth = _imageWidth - minsize;
		float dheight = _imageHeight - minsize;

		float x = dwidth / 2 / _imageWidth;
		float y = dheight / 2 / _imageHeight;
		float w = 1 - 2 * x;
		float h = 1 - 2 * y;

		textureArray[0] = x;
		textureArray[1] = y;
		textureArray[2] = x;
		textureArray[3] = y + h;
		textureArray[4] = x + w;
		textureArray[5] = y + h;
		textureArray[6] = x;
		textureArray[7] = y;
		textureArray[8] = x + w;
		textureArray[9] = y + h;
		textureArray[10] = x + w;
		textureArray[11] = y;
	}

	memcpy(_vertexArray, vertexArray, sizeof(vertexArray));
	memcpy(_textureArray, textureArray, sizeof(textureArray));

	return true ;
}

bool NXGLDrawer::rotate(float radian)
{
	_imageRotation = radian ;

	NXMatrix4 projectMatrix = NXMatrix4MakeOrtho(-1, 1, -1, 1, 1, -1);
	NXMatrix4 rotateMatrix =  NXMatrix4RotationZ(-_imageRotation);
	NXMatrix4 lookAtMatrix = NXMatrix4MakeLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

	_imageMVPMatrix = NXMatrix4Multiply(projectMatrix, NXMatrix4Multiply(lookAtMatrix, rotateMatrix));
	_rtboxMVPMatrix = NXMatrix4Multiply(projectMatrix, lookAtMatrix);

	return true ;
}

bool NXGLDrawer::draw(NXPtr<NXBuffer>& vbuffer)
{
	return this->draw(vbuffer, NULL, 0x0) ;
}

bool NXGLDrawer::draw(NXPtr<NXBuffer>& vbuffer, const LPNXRect rect, uint32_t color)
{
	if (!_glContext->isValid()) {
		__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "NXGLDrawer::draw() context is invalid.");
		return false;
	}

	if (rect)
		_drawRect = *rect;
	else
		memset(&_drawRect, 0, sizeof(_drawRect));

	if(!_glContext->makeCurrent())
		return false ;

	vbuffer->lockBuffer();
	if (_pixFormat == NXSDK_PIX_FMT_NV12 || _pixFormat == NXSDK_PIX_FMT_NV21) {
		uint8_t* bufferY = vbuffer->getPlanePtr(0);
		uint8_t* bufferUV = vbuffer->getPlanePtr(1);
		this->bindTextureY(_glTextureYId, bufferY, _imageWidth, _imageHeight);
		this->bindTextureUV(_glTextureUVId, bufferUV, _imageWidth >> 1,_imageHeight >> 1);
	} else if (_imageWidth == NXSDK_PIX_FMT_BGRA) {
		uint8_t* bufferBGRA = vbuffer->getPlanePtr(0);
		this->bindTextureRGBA(_imageWidth, bufferBGRA, _imageWidth, _imageHeight);
	}
	vbuffer->unlockBuffer();

	display(color);

	if(!_glContext->swapBuffers())
		return false ;

	return true ;
}

void NXGLDrawer::display(uint32_t rectColor)
{
	float drawBoxArray[] = {
	    -1.0+2*_drawRect.x, 1.0-2*_drawRect.y, 0.5,
	    -1.0+2*_drawRect.x, 1.0-2*(_drawRect.y+_drawRect.h), 0.5,
	    -1.0+2*(_drawRect.x+_drawRect.w), 1.0-2*(_drawRect.y+_drawRect.h), 0.5,
	    -1.0+2*(_drawRect.x+_drawRect.w), 1.0-2*_drawRect.y, 0.5,
	} ;

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

	glUseProgram(_glBoxProgramID);
	GLint boxProject = glGetUniformLocation(_glBoxProgramID, "u_Project");
	glUniformMatrix4fv(boxProject, 1, GL_FALSE, _rtboxMVPMatrix.m);

	GLint boxLineColor = glGetUniformLocation(_glBoxProgramID, "u_LineColor");

	float alpha = 1.0f * ((rectColor & 0xFF000000) >> 24) / 255;
	float red   = 1.0f * ((rectColor & 0x00FF0000) >> 16) / 255;
	float green = 1.0f * ((rectColor & 0x0000FF00) >> 8) / 255;
	float blue  = 1.0f * ((rectColor & 0x000000FF) >> 0) / 255;

	glUniform4f(boxLineColor, red, green, blue, alpha);

	GLint posAttrib2 = glGetAttribLocation(_glBoxProgramID, "a_Position") ;
	glEnableVertexAttribArray(posAttrib2);
	glVertexAttribPointer(posAttrib2, 3, GL_FLOAT, GL_FALSE, 0, drawBoxArray);

	glLineWidth(2);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glFlush();
}


bool NXGLDrawer::close()
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

	if (_glBoxProgramID)
	{
		glDeleteProgram(_glBoxProgramID);
		_glBoxProgramID = 0;
	}

	_glContext = NULL ;
	_glSurface = NULL ;

	_imageWidth = 0 ;
	_imageHeight = 0 ;
	_canvasWidth = 0 ;
	_canvasHeight = 0 ;

	memset(&_drawRect, 0, sizeof(_drawRect)) ;

	return true ;
}
