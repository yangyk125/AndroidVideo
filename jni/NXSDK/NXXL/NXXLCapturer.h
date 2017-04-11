/*
 * NXMXLCapturer.h
 *
 *  Created on: 2015-10-20
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

#include <OMXAL/OpenMAXAL.h>
#include <OMXAL/OpenMAXAL_Android.h>
#include <OMXAL/OpenMAXAL_Platform.h>

class NXXLCapturer: public NXSDKCapturer
{
private:
	XAObjectItf		_capEngineObj ;
	XAEngineItf		_capEnginePtr ;
	XAObjectItf		_capCameraObj ;
	XACameraItf		_capCameraPtr ;
	XACameraCapabilitiesItf	_capCameraCaps ;
public:
	NXXLCapturer();
	virtual ~NXXLCapturer();
public:
	virtual bool 	open() ;
	virtual NXSize 	size() ;
	virtual bool    start() ;
	virtual bool 	focus(NXPoint pt) ;
	virtual bool    pause(bool pause);
	virtual bool 	stop() ;
	virtual bool 	close() ;
	virtual NXSDKPixelFormat format() ;
	virtual NXPtr<NXBuffer> capture() ;
};


