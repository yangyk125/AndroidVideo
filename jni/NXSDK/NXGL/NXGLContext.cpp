/*
 * NXGLContext.cpp
 *
 *  Created on: 2015-9-1
 *      Author: yangyk
 */


#include "NXSDK.h"
#include "NXGLContext.h"

#include <android/log.h>


NXGLContext::NXGLContext(bool onscreen, EGLint version)
: _eglesContext(NULL)
, _curentConfig(NULL)
{
	NXGL::NXGL_EGLInitialize();

	EGLBoolean result = EGL_FALSE ;

	/*
	EGLint all_count = 0 ;
	EGLConfig all_configs[64] ;
	result = eglGetConfigs(NXGL::NXGL_EGLGetDisplay(), all_configs, 64, &all_count);
	if(!result)
	{
	 	 uint32_t eglError = eglGetError();
	 	 __android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglGetConfigs() Error=0x%X", eglError);
		return ;
	}
	*/

	EGLint surface_attris[] = {
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
		    EGL_DEPTH_SIZE, 16,
		    EGL_SAMPLE_BUFFERS, onscreen ? EGL_TRUE : EGL_FALSE,
		    EGL_SURFACE_TYPE, onscreen ? EGL_WINDOW_BIT : EGL_PBUFFER_BIT,
		    EGL_RENDERABLE_TYPE, version<=1 ? EGL_OPENGL_ES_BIT : EGL_OPENGL_ES2_BIT,
		    EGL_NONE
	};

	EGLConfig choseConfigs[16] ;
	EGLint choseCount = 0 ;
	result = eglChooseConfig(NXGL::NXGL_EGLGetDisplay(), surface_attris, choseConfigs, 16, &choseCount);
	if(!result)
	{
		uint32_t eglError = eglGetError();
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglChooseConfig() Error=0x%X", eglError);
		return ;
	}

	if(choseCount == 0)
	{
		uint32_t eglError = eglGetError();
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglChooseConfig() Result=0");
		return ;
	}

#ifdef DEBUG

	EGLint attrKeys[]   =   {EGL_BUFFER_SIZE,
							 EGL_RED_SIZE,
							 EGL_GREEN_SIZE,
							 EGL_BLUE_SIZE,
							 EGL_ALPHA_SIZE,
							 EGL_CONFIG_CAVEAT,
							 EGL_CONFIG_ID,
							 EGL_DEPTH_SIZE,
							 EGL_LEVEL,
							 EGL_MAX_PBUFFER_WIDTH,
							 EGL_MAX_PBUFFER_HEIGHT,
							 EGL_MAX_PBUFFER_PIXELS,
							 EGL_NATIVE_RENDERABLE,
							 EGL_NATIVE_VISUAL_ID,
							 EGL_NATIVE_VISUAL_TYPE,
							 EGL_SAMPLE_BUFFERS,
							 EGL_SAMPLES,
							 EGL_SURFACE_TYPE,
							 EGL_TRANSPARENT_TYPE,
							 EGL_TRANSPARENT_RED_VALUE,
							 EGL_TRANSPARENT_GREEN_VALUE,
							 EGL_TRANSPARENT_BLUE_VALUE,
							 EGL_RENDERABLE_TYPE,
							 EGL_MIN_SWAP_INTERVAL,
							 EGL_MAX_SWAP_INTERVAL};

	const char* nameKeys[] =    {"EGL_BUFFER_SIZE",
							 	 "EGL_RED_SIZE",
							 	 "EGL_GREEN_SIZE",
							 	 "EGL_BLUE_SIZE",
							 	 "EGL_ALPHA_SIZE",
							 	 "EGL_CONFIG_CAVEAT",
							 	 "EGL_CONFIG_ID",
							 	 "EGL_DEPTH_SIZE",
							 	 "EGL_LEVEL",
							 	 "EGL_MAX_PBUFFER_WIDTH",
							 	 "EGL_MAX_PBUFFER_HEIGHT",
							 	 "EGL_MAX_PBUFFER_PIXELS",
							 	 "EGL_NATIVE_RENDERABLE",
							 	 "EGL_NATIVE_VISUAL_ID",
							 	 "EGL_NATIVE_VISUAL_TYPE",
							 	 "EGL_SAMPLE_BUFFERS",
							 	 "EGL_SAMPLES",
							 	 "EGL_SURFACE_TYPE",
							 	 "EGL_TRANSPARENT_TYPE",
							 	 "EGL_TRANSPARENT_RED_VALUE",
							 	 "EGL_TRANSPARENT_GREEN_VALUE",
							 	 "EGL_TRANSPARENT_BLUE_VALUE",
							 	 "EGL_RENDERABLE_TYPE",
								 "EGL_MIN_SWAP_INTERVAL",
								 "EGL_MAX_SWAP_INTERVAL"};



	int attrCount = sizeof(attrKeys)/sizeof(EGLint) ;
	for(int jdx=0; jdx<attrCount; jdx++)
	{
		for(EGLint idx=0; idx<choseCount; idx++)
		{
			EGLint attrValue = 0 ;
			eglGetConfigAttrib(NXGL::NXGL_EGLGetDisplay(), choseConfigs[idx], attrKeys[jdx], &attrValue);
			__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "config=%d, keyname=%s, value10=%u, value16=0x%X.", idx, nameKeys[jdx], (uint32_t)attrValue,  (uint32_t)attrValue);
		}
	}

#endif //DEBUG

	_curentConfig = choseConfigs[0] ;

	result = eglBindAPI(EGL_OPENGL_ES_API);
	if (!result) {
		uint32_t eglError = eglGetError();
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglBindAPI() Error=0x%X", eglError);
		return;
	}

	EGLint contextattr[] = { EGL_CONTEXT_CLIENT_VERSION, version, EGL_NONE };
	_eglesContext = eglCreateContext(NXGL::NXGL_EGLGetDisplay(), _curentConfig, EGL_NO_CONTEXT, contextattr);
	if (!_eglesContext) {
		uint32_t eglError = eglGetError();
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglCreateContext(version=%d) Error=0x%X", version, eglError);
		return;
	}

#ifdef DEBUG
	/*
	const char* strExtensions = (const char*)glGetString(GL_EXTENSIONS) ;
	if(strExtensions)
	{
		bool suptNPOT = strstr(strExtensions, "texture_npot") ;
		bool suptBGRA = strstr(strExtensions, "GL_IMG_texture_format_BGRA8888") ;

		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "Current GPU Support NPOT = %d", (int)suptNPOT);
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "Current GPU Support Read BGRA = %d", (int)suptBGRA);
	}
	if(strExtensions)
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "OpenGLES Extensions = %s.", strExtensions) ;
	*/
	__android_log_print(ANDROID_LOG_DEBUG, "NXGL", "Create NXGLContext Done.");

#endif
}

NXGLContext::~NXGLContext()
{
	if(_eglesContext)
	{
		eglMakeCurrent(NXGL::NXGL_EGLGetDisplay(), NULL, NULL, NULL);
		eglDestroyContext(NXGL::NXGL_EGLGetDisplay(), _eglesContext);
		_eglesContext = 0 ;
	}

	_eglesSurface = NULL ;
	_curentConfig = NULL ;
}

EGLConfig NXGLContext::getCurrentConfig()
{
	return _curentConfig ;
}

bool NXGLContext::isValid()
{
	return _eglesContext ;
}

bool NXGLContext::setSurface(NXPtr<NXGLSurface>& surface)
{
	_eglesSurface = surface ;
	if (_eglesSurface != NULL && _eglesSurface->getNativeWindow())
	{
		EGLSurface glSurface = _eglesSurface->getOpenGLSurface() ;
		//if context is bind to surface, set current and set the swap interval.
		EGLBoolean result = eglMakeCurrent(NXGL::NXGL_EGLGetDisplay(), glSurface, glSurface, _eglesContext);
		if (!result) {
			uint32_t eglError = eglGetError();
			__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglMakeCurrent() Error=0x%X", eglError);
		}

		result = eglSwapInterval(NXGL::NXGL_EGLGetDisplay(), 0);
		if (!result) {
			uint32_t eglError = eglGetError();
			__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglSwapInterval() Error=0x%X", eglError);
		}
	}

	return true ;
}

bool NXGLContext::makeCurrent()
{
	if(!this->isValid())
		return false ;

	EGLBoolean result = EGL_TRUE ;
	if(_eglesSurface != NULL && _eglesSurface->isValid())
	{
		EGLSurface glSurface = _eglesSurface->getOpenGLSurface() ;
		result = eglMakeCurrent(NXGL::NXGL_EGLGetDisplay(), glSurface, glSurface, _eglesContext);
		if(!result) {
			uint32_t eglError = eglGetError();
			__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglMakeCurrent() Error=0x%X", eglError);
			return false ;
		}
	}

	return true ;
}

bool NXGLContext::releaseThread()
{
	if(!this->isValid())
		return false ;

	if(!_eglesSurface->isValid())
		return false ;

	EGLBoolean result =  eglReleaseThread();
	if(!result)
	{
		uint32_t eglError = eglGetError();
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglReleaseThread() Error=0x%X", eglError);
		return false ;
	}

	return true ;
}

bool NXGLContext::swapBuffers()
{
	if(!this->isValid())
		return false ;

	if(!_eglesSurface)
		return false ;

	if(!_eglesSurface->isValid())
		return false ;

	EGLSurface glSurface = _eglesSurface->getOpenGLSurface() ;

	EGLBoolean result = eglSwapBuffers(NXGL::NXGL_EGLGetDisplay(), glSurface);
	if(!result)
	{
		uint32_t eglError = eglGetError();
		__android_log_print(ANDROID_LOG_ERROR, "NXGL", "EGL eglSwapBuffers() Error=0x%X", eglError);
		return false ;
	}

	return true ;
}
