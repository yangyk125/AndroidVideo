/*
 * NXSDK.cpp
 *
 *  Created on: 2015-9-17
 *      Author: yangyk
 */

#include "NXSDK.h"

#include "NXGL/NXGLDrawer.h"
#include "NXGL/NXGLTrimer.h"
#include "NXGL/NXGLCanvas.h"
#include "NXSL/NXSLSpeaker.h"
#include "NXSL/NXSLRecorder.h"

#include "NXPL/NXWBPlayer.h"

#include "NXXL/NXXLCapturer.h"

#include "NXFF/NXFFSWriter.h"
#include "NXFF/NXFFAWriter.h"
#include "NXFF/NXFFSReader.h"

#include "NXTM/NXTMAWriter.h"
#include "NXTM/NXTMAReader.h"
#include "NXTM/NXTMSWriter.h"
#include "NXTM/NXTMSReader.h"

#include "NXTM/NXTMSShoter.h"

#ifdef PRINT_NXSDK_OBJECT_COUNT
static volatile uint32_t	NXObjectCount = 0 ;
#endif

NXObject::NXObject()
{
#ifdef PRINT_NXSDK_OBJECT_COUNT
	__android_log_print(ANDROID_LOG_DEBUG, "NXSDK", "NXObject Create Count = %u", ++NXObjectCount);
#endif
}

NXObject::~NXObject()
{
#ifdef PRINT_NXSDK_OBJECT_COUNT
	__android_log_print(ANDROID_LOG_DEBUG, "NXSDK", "NXObject Destory Count = %u", --NXObjectCount);
#endif
}

NXCallback::NXCallback()
{
#ifdef PRINT_NXSDK_OBJECT_COUNT
	__android_log_print(ANDROID_LOG_DEBUG, "NXSDK", "NXObject Create Count = %u", ++NXObjectCount);
#endif
}

NXCallback::~NXCallback()
{
#ifdef PRINT_NXSDK_OBJECT_COUNT
	__android_log_print(ANDROID_LOG_DEBUG, "NXSDK", "NXObject Destory Count = %u", --NXObjectCount);
#endif
}

static bool NXSDK_Initialized = false ;
static bool NXSDK_Initialize() {
	if(!NXSDK_Initialized) {
		av_register_all();
		avcodec_register_all();
		NXSDK_Initialized = true ;
		return true ;
	}
	return false ;
}

NXPtr<NXSDKReader> NXSDK_CreateReader(const char* suffix) {
	NXSDK_Initialize() ;
	if(suffix && strncmp(suffix, "mp4s", 4) == 0)
		return NXPtr<NXSDKReader>(new NXFFSReader());
	else if(suffix && strncmp(suffix, "mpts", 4) == 0)
		return NXPtr<NXSDKReader>(new NXTMSReader());
	else if(suffix && strncmp(suffix, "mpta", 4) == 0)
		return NXPtr<NXSDKReader>(new NXTMAReader());
	return NULL ;
}

NXPtr<NXSDKWriter> NXSDK_CreateWriter(const char* suffix) {
	NXSDK_Initialize() ;
	if(suffix && strncmp(suffix, "mp4s", 4) == 0)
		return NXPtr<NXSDKWriter>(new NXFFSWriter());
	else if(suffix && strncmp(suffix, "mp4a", 4) == 0)
		return NXPtr<NXSDKWriter>(new NXFFAWriter());
	else if(suffix && strncmp(suffix, "mpts", 4) == 0)
		return NXPtr<NXSDKWriter>(new NXTMSWriter());
	else if(suffix && strncmp(suffix, "mpta", 4) == 0)
		return NXPtr<NXSDKWriter>(new NXTMAWriter());

	return NULL ;
}
NXPtr<NXSDKShoter> NXSDK_CreateShoter(const char* suffix) {
	NXSDK_Initialize() ;
	if(suffix && strncmp(suffix, "mpts", 4) == 0)
		return NXPtr<NXSDKShoter>(new NXTMSShoter());
	return NULL ;
}

NXPtr<NXSDKDrawer> NXSDK_CreateDrawer(const char* suffix) {
	NXSDK_Initialize() ;
	NXPtr<NXSDKDrawer> ptr = new NXGLDrawer();
	return ptr;
}

NXPtr<NXSDKTrimer> NXSDK_CreateTrimer(const char* suffix) {
	NXSDK_Initialize() ;
	NXPtr<NXSDKTrimer> ptr = new NXGLTrimer();
	return ptr;
}

NXPtr<NXSDKCanvas> NXSDK_CreateCanvas(const char* suffix) {
	NXSDK_Initialize() ;
	NXPtr<NXSDKCanvas> ptr = new NXGLCanvas();
	return ptr;
}

NXPtr<NXSDKPlayer> NXSDK_CreatePlayer(const char* suffix) {
	NXSDK_Initialize() ;
	if(suffix && strncmp(suffix, "wb", 2) == 0)
		return NXPtr<NXSDKPlayer>(new NXWBPlayer());
	return NULL ;
}

NXPtr<NXSDKSpeaker> NXSDK_CreateSpeaker(const char* suffix) {
	NXSDK_Initialize() ;
	NXPtr<NXSDKSpeaker> ptr = new NXSLSpeaker();
	return ptr;
}

NXPtr<NXSDKRecorder> NXSDK_CreateRecorder(const char* suffix) {
	NXSDK_Initialize() ;
	NXPtr<NXSDKRecorder> ptr = new NXSLRecorder();
	return ptr;
}

NXPtr<NXSDKCapturer> NXSDK_CreateCapturer(const char* suffix) {
	NXSDK_Initialize() ;
	NXPtr<NXSDKCapturer> ptr = new NXXLCapturer();
	return ptr;
}
