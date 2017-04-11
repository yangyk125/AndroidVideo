/*
 * NXSLSpeaker.h
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <vector>
#include <deque>

class NXSLSpeaker: public NXSDKSpeaker
{
private:
	SLObjectItf 					_engineObject ;
	SLEngineItf 					_engineEngine ;
	SLObjectItf 					_playerObject ;
	SLPlayItf 						_playerPlayer ;
	SLObjectItf 					_outputMixObj ;
	SLVolumeItf						_outputVolume ;
	SLBufferQueueItf				_playerBQueue ;

private:
	std::vector< NXPtr<NXBuffer> >	_cacheArray ;
	uint32_t 						_cacheLegth ;
	std::deque<uint32_t>			_playrQueue ;
	NXLock							_playrLockr ;
	std::deque<uint32_t>			_emptyQueue ;
	NXLock							_emptyLockr ;
	std::deque< NXPtr<NXBuffer> >	_inputQueue ;

private:
	static void NXSLPlayerCallback(SLBufferQueueItf bufferQueue, void* context) ;
	void player_callback(SLBufferQueueItf bufferQueue) ;
public:
	NXSLSpeaker();
	virtual ~NXSLSpeaker();
public:
	virtual bool open(const NXSDKAudioFormat& audioFormat, uint32_t samplesPerCache, uint32_t cacheCount) ;
	virtual bool volume(float volume) ;
	virtual bool start() ;
	virtual bool pause(bool pause) ;
	virtual bool speak(NXPtr<NXBuffer>& buffer) ;
	virtual bool stop()  ;
	virtual bool close()  ;
};

