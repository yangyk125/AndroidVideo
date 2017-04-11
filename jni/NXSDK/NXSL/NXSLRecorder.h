/*
 * NXSLRecorder.h
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

#include <vector>
#include <deque>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class NXSLRecorder: public NXSDKRecorder
{
private:
	SLObjectItf 	_engineObject ;
	SLEngineItf 	_engineEngine ;
	SLObjectItf 	_recordObject ;
	SLRecordItf 	_recordRecord ;
	SLAndroidSimpleBufferQueueItf _recordBufferQueue ;
private:
	NXSDKAudioFormat _audioFormat ;
	uint32_t 		 _smpsPerFrame ;
	uint32_t		 _cacheCounts ;

	uint32_t 				_frameLenth ;
	std::vector<uint8_t*>	_cacheArray ;
	std::deque<int32_t>		_fullCaches ;
	std::deque<int32_t>		_recdCaches ;
	NXLock					_fullLocks  ;
	NXLock					_recdLocks  ;
private:
	static void NXSLRecorderCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context) ;
	void recorder_callback(SLAndroidSimpleBufferQueueItf bufferQueue);
public:
	NXSLRecorder();
	virtual ~NXSLRecorder();
public:
	virtual bool open(NXINOUT NXSDKAudioFormat& audioFormat, uint32_t samplesPerPacket, uint32_t cachePackets)  ;
	virtual bool start() ;
	virtual bool pause(bool pause) ;
	virtual bool record_packet(NXOUT NXPtr<NXBuffer>& abuffer)  ;
	virtual bool stop()  ;
	virtual bool close()  ;
};

