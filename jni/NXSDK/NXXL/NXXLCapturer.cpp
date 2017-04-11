/*
 * NXXLCapturer.cpp
 *
 *  Created on: 2015-10-20
 *      Author: yangyk
 */

#include "NXXLCapturer.h"


NXXLCapturer::NXXLCapturer()
{
	_capEngineObj = NULL ;
	_capEnginePtr = NULL ;
	_capCameraObj = NULL ;
	_capCameraPtr = NULL ;
	_capCameraCaps = NULL ;
}

NXXLCapturer::~NXXLCapturer()
{

}

bool NXXLCapturer::open()
{
	XAresult result = XA_BOOLEAN_FALSE ;
	result = xaCreateEngine(&_capEngineObj, 0, NULL, 0, NULL, NULL);
	if(result != XA_RESULT_SUCCESS) {
		__android_log_print(ANDROID_LOG_DEBUG, "MAXAL","xaCreateEngine() Failure = 0x%X!!", result);
		return false ;
	}

	result = (*_capEngineObj)->Realize(_capEngineObj, XA_BOOLEAN_FALSE);
	if(result != XA_RESULT_SUCCESS) {
		__android_log_print(ANDROID_LOG_DEBUG, "MAXAL","EngineObj->Realize() Failure = 0x%X!!", result);
		return false ;
	}

	result = (*_capEngineObj)->GetInterface(_capEngineObj, XA_IID_ENGINE, &_capEnginePtr);
	if (result != XA_RESULT_SUCCESS){
		__android_log_print(ANDROID_LOG_DEBUG, "MAXAL", "EngineObj->GetInterface() Failure = 0x%X!!", result);
		return false ;
	}

	XAInterfaceID iids[] = {XA_IID_CAMERA, XA_IID_CAMERACAPABILITIES};
	result = (*_capEnginePtr)->CreateCameraDevice(_capEnginePtr, &_capCameraObj, XA_DEFAULTDEVICEID_CAMERA, 2, iids, NULL);
	if (result != XA_RESULT_SUCCESS){
		__android_log_print(ANDROID_LOG_DEBUG, "MAXAL", "EnginePtr->CreateCameraDevice() Failure = 0x%X!!", result);
		return false ;
	}

	result = (*_capCameraObj)->Realize(_capCameraObj, XA_BOOLEAN_FALSE);
	if(result != XA_RESULT_SUCCESS) {
		__android_log_print(ANDROID_LOG_DEBUG, "MAXAL","CameraObj->Realize() Failure = 0x%X!!", result);
		return false ;
	}

	result = (*_capCameraObj)->GetInterface(_capCameraObj, XA_IID_CAMERA, &_capCameraPtr);
	if(result != XA_RESULT_SUCCESS) {
		__android_log_print(ANDROID_LOG_DEBUG, "MAXAL","CameraObj->GetInterface() Failure = 0x%X!!", result);
		return false ;
	}

	result = (*_capCameraObj)->GetInterface(_capCameraObj, XA_IID_CAMERACAPABILITIES, &_capCameraCaps);
	if(result != XA_RESULT_SUCCESS) {
		__android_log_print(ANDROID_LOG_DEBUG, "MAXAL","CameraObj->GetInterface() Failure = 0x%X!!", result);
		return false ;
	}

	__android_log_print(ANDROID_LOG_DEBUG, "MAXAL", "NXXLCapturer::open() success !!!");
	return true ;
}
NXSize NXXLCapturer::size()
{
	return NXSizeMake(0, 0) ;
}
bool NXXLCapturer::start()
{
	return false ;
}
bool NXXLCapturer::focus(NXPoint pt)
{
	return false ;
}
bool NXXLCapturer::pause(bool pause)
{
	return false ;
}
bool NXXLCapturer::stop()
{
	return false ;
}
bool NXXLCapturer::close()
{
	if(_capEngineObj && _capEnginePtr) {
		(*_capEngineObj)->Destroy(_capEngineObj);
		_capEngineObj = NULL ;
		_capEnginePtr = NULL ;
	}
	return true ;
}
NXSDKPixelFormat NXXLCapturer::format()
{
	return NXSDK_PIX_FMT_NONE ;
}
NXPtr<NXBuffer> NXXLCapturer::capture()
{
	return NXPtr<NXBuffer>();
}
