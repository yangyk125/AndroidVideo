/*
 * NXSLSpeaker.cpp
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#include "NXSLSpeaker.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


NXSLSpeaker::NXSLSpeaker()
{
	_engineObject = NULL ;
	_engineEngine = NULL ;
	_playerObject = NULL ;
	_playerPlayer = NULL ;
	_outputMixObj = NULL ;
	_outputVolume = NULL ;
	_playerBQueue = NULL ;
}

NXSLSpeaker::~NXSLSpeaker()
{
	if(_engineEngine)
	{
		this->stop();
		this->close();
	}
}

void NXSLSpeaker::NXSLPlayerCallback(SLBufferQueueItf bufferQueue, void* context)
{
	NXSLSpeaker* speaker = (NXSLSpeaker*)context ;
	speaker->player_callback(bufferQueue);
}

bool NXSLSpeaker::open(const NXSDKAudioFormat& audioFormat, uint32_t samplesPerCache, uint32_t cacheCount)
{
	SLresult result = SL_BOOLEAN_FALSE ;

	result = slCreateEngine(&_engineObject, 0, NULL, 0, NULL, NULL);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES slCreateEngine() Error=0x%X", result);
		return false ;
	}

	// realize the engine
	result = (*_engineObject)->Realize(_engineObject, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Engine Realize() Error=0x%X", result);
		return false ;
	}

	// get the engine interface, which is needed in order to create other objects
	result = (*_engineObject)->GetInterface(_engineObject, SL_IID_ENGINE, &_engineEngine);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Engine GetInterface() Error=0x%X", result);
		return false ;
	}

	const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
	const SLboolean req[1] = {SL_BOOLEAN_FALSE};
	result = (*_engineEngine)->CreateOutputMix(_engineEngine, &_outputMixObj, 1, ids, req);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Engine CreateOutputMix() Error=0x%X", result);
		return false ;
	}

	// realize the output mix
	result = (*_outputMixObj)->Realize(_outputMixObj, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES OutputMix Realize() Error=0x%X", result);
		return false ;
	}

	SLuint32 smprate = 0 ;
	if(audioFormat.sample_rate == 24000)
		smprate = SL_SAMPLINGRATE_24 ;
	else if(audioFormat.sample_rate == 44100)
		smprate = SL_SAMPLINGRATE_44_1 ;
	else if(audioFormat.sample_rate == 48000)
		smprate = SL_SAMPLINGRATE_48 ;
	else if(audioFormat.sample_rate == 64000)
		smprate = SL_SAMPLINGRATE_64 ;
	else
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Speaker Invalid Sample Rate = %u", (uint32_t)audioFormat.sample_rate) ;

	SLuint32 chanmask = 0 ;
	if(audioFormat.channels == 1)
		chanmask = SL_SPEAKER_FRONT_CENTER ;
	else if(audioFormat.channels == 2)
		chanmask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT ;
	else
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Speaker Invalid Channel Count = %u", (uint32_t)audioFormat.channels) ;

	SLuint32 smpbits = 0 ;
	if(audioFormat.sample_bits == 16)
		smpbits = SL_PCMSAMPLEFORMAT_FIXED_16 ;
	else if(audioFormat.sample_bits == 24)
		smpbits = SL_PCMSAMPLEFORMAT_FIXED_24 ;
	else if(audioFormat.sample_bits == 32)
		smpbits = SL_PCMSAMPLEFORMAT_FIXED_32 ;
	else
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Speaker Invalid Sample Bits Count = %u", (uint32_t)audioFormat.sample_bits) ;

	// configure audio sink
	SLDataLocator_BufferQueue loc_bq = {SL_DATALOCATOR_BUFFERQUEUE, cacheCount};
	SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, audioFormat.channels, smprate, smpbits, smpbits, chanmask, SL_BYTEORDER_LITTLEENDIAN};
	SLDataSource audioSrc = {&loc_bq, &format_pcm};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, _outputMixObj};
	SLDataSink audioSnk = {&loc_outmix, NULL};

	// create audio player
	const SLInterfaceID ids2[2] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
	const SLboolean req2[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

	result = (*_engineEngine)->CreateAudioPlayer(_engineEngine, &_playerObject, &audioSrc, &audioSnk, 2, ids2, req2);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES CreateAudioPlayer() Error=0x%X", result);
		return false ;
	}

	// realize the player
	result = (*_playerObject)->Realize(_playerObject, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES playerObject Realize() Error=0x%X", result);
		return false ;
	}

	// get the play interface
	result = (*_playerObject)->GetInterface(_playerObject, SL_IID_PLAY, &_playerPlayer);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES playerObject GetInterface() Error=0x%X", result);
		return false ;
	}

	// get the buffer queue interface
	result = (*_playerObject)->GetInterface(_playerObject, SL_IID_BUFFERQUEUE, &_playerBQueue);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES playerObject GetInterface() Error=0x%X", result);
		return false ;
	}

	// get volume interface
	result = (*_playerObject)->GetInterface(_playerObject, SL_IID_VOLUME, &_outputVolume);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES playerObject GetInterface() Error=0x%X", result);
		return false ;
	}

	// register callback on the buffer queue
	result = (*_playerBQueue)->RegisterCallback(_playerBQueue, NXSLPlayerCallback, this);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES playerQueue RegisterCallback() Error=0x%X", result);
		return false ;
	}

	_cacheLegth = samplesPerCache*audioFormat.channels*audioFormat.sample_bits/8 ;
	for(uint32_t idx=0; idx<cacheCount; idx++) {
		NXPtr<NXBuffer> ptr = new NXBuffer(_cacheLegth);
		memset(ptr->getPlanePtr(0), 0, _cacheLegth) ;
		_cacheArray.push_back(ptr) ;
	}

	return true ;
}

void NXSLSpeaker::player_callback(SLBufferQueueItf bufferQueue)
{
	if(_playrQueue.size() > 1)
	{
		_playrLockr.lock();
		uint32_t index = _playrQueue.front() ;
		_playrQueue.pop_front();
		_playrLockr.unlock();

		_emptyLockr.lock() ;
		_emptyQueue.push_back(index) ;
		_emptyLockr.unlock() ;
	}
	else if(_emptyQueue.size() > 0)
	{
		__android_log_print(ANDROID_LOG_WARN, "NXSL", "SLES player Enqueue empty buffer !!!");

		_emptyLockr.lock();
		uint32_t index = _emptyQueue.front();
		_emptyQueue.pop_front() ;
		_emptyLockr.unlock();

		NXPtr<NXBuffer>& cbuffer = _cacheArray[index] ;
		uint8_t* ptrCBuf = cbuffer->getPlanePtr(0) ;
		uint32_t lenIBuf = cbuffer->getPlaneLength(0) ;
		memset(ptrCBuf, 0, lenIBuf) ;

		SLresult result = (*_playerBQueue)->Enqueue(_playerBQueue, ptrCBuf, lenIBuf);
		if (SL_RESULT_SUCCESS != result)
			__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES player Enqueue Failure, result=0x%X", result);

		_playrLockr.lock();
		_playrQueue.push_back(index) ;
		_playrLockr.unlock();
	}
}

bool NXSLSpeaker::speak(NXPtr<NXBuffer>& buffer)
{
	uint32_t inLen = buffer->getPlaneLength(0) ;
	uint8_t* inBuf = buffer->getPlanePtr(0) ;

	if(inLen > _cacheLegth) {
		uint32_t reLen = inLen ;
		while(reLen > _cacheLegth) {
			NXPtr<NXBuffer> segmBuf = new NXBuffer(_cacheLegth);
			memcpy(segmBuf->getPlanePtr(0), inBuf+(inLen-reLen), _cacheLegth) ;
			_inputQueue.push_back(segmBuf) ;
			reLen -= _cacheLegth ;
		}
		if(reLen) {
			NXPtr<NXBuffer> segmBuf = new NXBuffer(reLen);
			memcpy(segmBuf->getPlanePtr(0), inBuf+(inLen-reLen), reLen) ;
			_inputQueue.push_back(segmBuf) ;
		}
	}
	else {
		_inputQueue.push_back(buffer) ;
	}

	while(_inputQueue.size() > 0) {

		NXPtr<NXBuffer> inBuffer = _inputQueue.front() ;
		_inputQueue.pop_front() ;

		if(_emptyQueue.size() == 0) {
			__android_log_print(ANDROID_LOG_WARN, "NXSL", "SLES player has no empty buffer !!");
			continue ;
		}

		_emptyLockr.lock();
		uint32_t index = _emptyQueue.front();
		_emptyQueue.pop_front() ;
		_emptyLockr.unlock();

		NXPtr<NXBuffer>& cbuffer = _cacheArray[index] ;
		uint8_t* ptrCBuf = cbuffer->getPlanePtr(0) ;
		uint8_t* ptrIBuf = inBuffer->getPlanePtr(0) ;
		uint32_t lenIBuf = inBuffer->getPlaneLength(0) ;
		memcpy(ptrCBuf, ptrIBuf, lenIBuf) ;

		SLresult result = (*_playerBQueue)->Enqueue(_playerBQueue, ptrCBuf, lenIBuf);
		if (SL_RESULT_SUCCESS != result)
			__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES player Enqueue Failure, result=0x%X", result);

		_playrLockr.lock();
		_playrQueue.push_back(index) ;
		_playrLockr.unlock();
	}

	return true ;
}

bool NXSLSpeaker::volume(float volume)
{
	return false ;
}

bool NXSLSpeaker::start()
{
	if(_playerPlayer == NULL)
		return false ;

	SLresult result = (*_playerPlayer)->SetPlayState(_playerPlayer, SL_PLAYSTATE_STOPPED);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES player set status() Error=0x%X", result);
		return false;
	}

	result = (*_playerBQueue)->Clear(_playerBQueue);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES playerQueue clear() Error=0x%X", result);
		return false;
	}

	_emptyLockr.lock();
	_emptyQueue.clear() ;
	_emptyLockr.unlock();

	_playrLockr.lock();
	_playrQueue.clear() ;
	_playrLockr.unlock();

	uint32_t cacheCount = _cacheArray.size() ;
	for(uint32_t idx=0; idx<cacheCount; idx++)
	{
		if(idx < cacheCount/2)
		{
			NXPtr<NXBuffer>& cbuffer = _cacheArray[idx] ;
			uint8_t* ptrCBuf = cbuffer->getPlanePtr(0) ;
			uint32_t lenCBuf = cbuffer->getPlaneLength(0) ;
			memset(ptrCBuf, 0, lenCBuf) ;

			SLresult result = (*_playerBQueue)->Enqueue(_playerBQueue, ptrCBuf, lenCBuf);
			if (SL_RESULT_SUCCESS != result) {
				__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES player Enqueue Failure, result=0x%X", result);
			}
			else {
				_playrLockr.lock();
				_playrQueue.push_back(idx) ;
				_playrLockr.unlock();
			}
		}
		else
		{
			_emptyLockr.lock();
			_emptyQueue.push_back(idx) ;
			_emptyLockr.unlock();
		}
	}

	result = (*_playerPlayer)->SetPlayState(_playerPlayer, SL_PLAYSTATE_PLAYING);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES player set status() Error=0x%X", result);
		return false;
	}

	return true ;
}

bool NXSLSpeaker::pause(bool pause)
{
	SLresult result = SL_BOOLEAN_FALSE ;
	SLuint32 status = 0 ;
	result = (*_playerPlayer)->GetPlayState(_playerPlayer, &status);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES player status() Error=0x%X", result);
		return false;
	}

	if(pause && status == SL_PLAYSTATE_PAUSED)
		return true ;

	if(!pause && status == SL_PLAYSTATE_PLAYING)
		return true ;

	result = (*_playerPlayer)->SetPlayState(_playerPlayer, pause ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES player set status() Error=0x%X", result);
		return false;
	}

	return true ;
}

bool NXSLSpeaker::stop()
{
	if(_playerPlayer == NULL)
		return false ;

	SLresult result = (*_playerPlayer)->SetPlayState(_playerPlayer, SL_PLAYSTATE_STOPPED);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES player set status() Error=0x%X", result);
		return false;
	}

	result = (*_playerBQueue)->Clear(_playerBQueue);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES playerQueue clear() Error=0x%X", result);
		return false;
	}

	_emptyLockr.lock();
	_emptyQueue.clear() ;
	_emptyLockr.unlock();

	_playrLockr.lock();
	_playrQueue.clear() ;
	_playrLockr.unlock();

	return true ;
}

bool NXSLSpeaker::close()
{
	if(_engineObject && _engineEngine) {
		(*_engineObject)->Destroy(_engineObject);
		_engineObject = NULL ;
		_engineEngine = NULL ;
		_playerObject = NULL ;
		_playerPlayer = NULL ;
		_outputMixObj = NULL ;
		_outputVolume = NULL ;
		_playerBQueue = NULL ;
	}

	_cacheArray.clear() ;
	_playrQueue.clear() ;
	_emptyQueue.clear() ;

	return true ;
}
