/*
 * NXSLRecorder.cpp
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#include "NXSLRecorder.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <android/log.h>
#include <assert.h>

NXSLRecorder::NXSLRecorder()
{
	_engineObject = NULL ;
	_engineEngine = NULL ;
}

NXSLRecorder::~NXSLRecorder()
{
	if(_recordRecord)
	{
		this->stop();
		this->close();
	}
}

// this callback handler is called every time a buffer finishes recording
void NXSLRecorder::NXSLRecorderCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void* context)
{
	NXSLRecorder* nxRecorder = (NXSLRecorder*)context ;
	nxRecorder->recorder_callback(bufferQueue) ;
}

bool NXSLRecorder::open(NXINOUT NXSDKAudioFormat& audioFormat, uint32_t samplesPerPacket, uint32_t cachePackets)
{
	_smpsPerFrame = samplesPerPacket ;
	_cacheCounts  = cachePackets ;
	_cacheArray.assign(_cacheCounts, NULL) ;

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

	// configure audio source
	SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
	SLDataSource audioSrc = {&loc_dev, NULL};

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
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder Invalid Sample Rate = %u", (uint32_t)audioFormat.sample_rate);

	SLuint32 chanmask = 0 ;
	if(audioFormat.channels == 1)
		chanmask = SL_SPEAKER_FRONT_CENTER ;
	else if(audioFormat.channels == 2)
		chanmask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT ;
	else
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder Invalid Channel Count = %u", (uint32_t)audioFormat.channels);

	SLuint32 smpbits = 0 ;
	if(audioFormat.sample_bits == 16)
		smpbits = SL_PCMSAMPLEFORMAT_FIXED_16 ;
	else if(audioFormat.sample_bits == 24)
			smpbits = SL_PCMSAMPLEFORMAT_FIXED_24 ;
	else if(audioFormat.sample_bits == 32)
			smpbits = SL_PCMSAMPLEFORMAT_FIXED_32 ;
	else
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder Invalid Sample Bits Count = %u", (uint32_t)audioFormat.sample_bits);

	// configure audio sink
	SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, _cacheCounts};
	SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, audioFormat.channels, smprate, smpbits, smpbits, chanmask, SL_BYTEORDER_LITTLEENDIAN};

	SLDataSink audioSnk = {&loc_bq, &format_pcm};

	// create audio recorder
	// (requires the RECORD_AUDIO permission)
	const SLInterfaceID id[2] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_RECORD};
	const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
	result = (*_engineEngine)->CreateAudioRecorder(_engineEngine, &_recordObject, &audioSrc, &audioSnk, 2, id, req);
	if (SL_RESULT_SUCCESS != result){
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES CreateAudioRecorder() Error=0x%X", result);
		return false ;
	}

	// realize the audio recorder
	result = (*_recordObject)->Realize(_recordObject, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder Engine() Error=0x%X", result);
		return false;
	}

	// get the record interface
	result = (*_recordObject)->GetInterface(_recordObject, SL_IID_RECORD, &_recordRecord);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder GetInterface() Error=0x%X", result);
		return false;
	}

	// get the buffer queue interface
	result = (*_recordObject)->GetInterface(_recordObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &_recordBufferQueue);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder GetInterface() Error=0x%X", result);
		return false;
	}

	// register callback on the buffer queue
	result = (*_recordBufferQueue)->RegisterCallback(_recordBufferQueue, NXSLRecorderCallback, this);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder RegisterCallback() Error=0x%X", result);
		return false;
	}

	_frameLenth = _smpsPerFrame*audioFormat.channels*audioFormat.sample_bits/8 ;
	for(int32_t idx=0; idx<_cacheArray.size(); idx++) {
		_cacheArray[idx] = (uint8_t*)malloc(_frameLenth);
		_recdCaches.push_back(idx) ;
	}

	_audioFormat = audioFormat ;
	return true ;
}


bool NXSLRecorder::start()
{
	SLresult result = (*_recordRecord)->SetRecordState(_recordRecord, SL_RECORDSTATE_STOPPED);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder stop() Error=0x%X", result);
		return false;
	}

	result = (*_recordBufferQueue)->Clear(_recordBufferQueue);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder Clear() Error=0x%X", result);
		return false;
	}

	_fullLocks.lock();
	_fullCaches.clear() ;
	_fullLocks.unlock();

	_recdLocks.lock();
	_recdCaches.clear() ;
	for(uint32_t idx=0; idx<_cacheCounts; idx++) {
		_recdCaches.push_back(idx) ;
		memset(_cacheArray[idx], 0, _frameLenth) ;
		result = (*_recordBufferQueue)->Enqueue(_recordBufferQueue, _cacheArray[idx], _frameLenth);
		if (SL_RESULT_SUCCESS != result)
			__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder Enqueue() Error=0x%X", result);
	}
	_recdLocks.unlock();

#ifdef DEBUG
	SLAndroidSimpleBufferQueueState recorderStatus = {0, 0};
	result = (*_recordBufferQueue)->GetState(_recordBufferQueue, &recorderStatus);
	if (SL_RESULT_SUCCESS != result)
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder GetState() Error=0x%X", result);
	else
		__android_log_print(ANDROID_LOG_DEBUG, "NXSL", "SLES Recorder Status Index=%d, Count=%d", (int)recorderStatus.index, (int)recorderStatus.count);
#endif

	// start recording
	result = (*_recordRecord)->SetRecordState(_recordRecord, SL_RECORDSTATE_RECORDING);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder SetRecordState() Error=0x%X", result);
		return false;
	}

	return true ;
}

bool NXSLRecorder::pause(bool pause)
{
	SLresult result = SL_BOOLEAN_FALSE ;
	SLuint32 status = 0 ;
	result = (*_recordRecord)->GetRecordState(_recordRecord, &status);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder status() Error=0x%X", result);
		return false;
	}

	if(pause && status == SL_RECORDSTATE_PAUSED)
		return true ;

	if(!pause && status == SL_RECORDSTATE_RECORDING)
		return true ;

	result = (*_recordRecord)->SetRecordState(_recordRecord, pause ? SL_RECORDSTATE_PAUSED : SL_RECORDSTATE_RECORDING);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder set status() Error=0x%X", result);
		return false;
	}

	return true ;
}

void NXSLRecorder::recorder_callback(SLAndroidSimpleBufferQueueItf bufferQueue)
{
	int32_t index = -1 ;

	if(_recdCaches.size() == 1) {
		NXPtr<NXBuffer> abuffer ;
		record_packet(abuffer) ;
	}

	if(_recdCaches.size() > 0)
	{
		_recdLocks.lock();
		index = _recdCaches.front();
		_recdCaches.pop_front();
		_recdLocks.unlock();
	}

	_fullLocks.lock();
	if(index >= 0)
		_fullCaches.push_back(index);
	else
		__android_log_print(ANDROID_LOG_WARN, "NXSL", "SLES Recorder No Empty Buffer......") ;
	_fullLocks.unlock();
}

bool NXSLRecorder::record_packet(NXOUT NXPtr<NXBuffer>& abuffer)
{
	if(!_recordRecord)
		return false ;

	if(_fullCaches.size() > 0)
	{
		abuffer = new NXBuffer(_frameLenth) ;
		uint8_t* audioBuffer = abuffer->getPlanePtr(0);

		_fullLocks.lock();
		int32_t index = _fullCaches.front();
		_fullCaches.pop_front();
		_fullLocks.unlock();

		uint8_t* buffer = _cacheArray[index];
		memcpy(audioBuffer, buffer, _frameLenth);

		_recdLocks.lock();
		_recdCaches.push_back(index);
		SLresult result = (*_recordBufferQueue)->Enqueue(_recordBufferQueue, buffer, _frameLenth);
		if (SL_RESULT_SUCCESS != result)
			__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder Enqueue() Error=0x%X", result);
		_recdLocks.unlock();

		return true ;
	}
	else {
		__android_log_print(ANDROID_LOG_WARN, "NXSL", "SLES Recorder No Full Buffer......");
		return false ;
	}
}

bool NXSLRecorder::stop()
{
	if(!_recordRecord)
		return false ;

	SLresult result = (*_recordRecord)->SetRecordState(_recordRecord, SL_RECORDSTATE_STOPPED);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder stop() Error=0x%X", result);
		return false;
	}

	result = (*_recordBufferQueue)->Clear(_recordBufferQueue);
	if (SL_RESULT_SUCCESS != result) {
		__android_log_print(ANDROID_LOG_ERROR, "NXSL", "SLES Recorder Clear() Error=0x%X", result);
		return false;
	}

	_recdLocks.lock();
	_recdCaches.clear() ;
	_recdLocks.unlock();

	_fullLocks.lock();
	_fullCaches.clear() ;
	_fullLocks.unlock();

	return true ;
}

bool NXSLRecorder::close()
{
	if(_engineObject && _engineEngine) {
		(*_engineObject)->Destroy(_engineObject);
		_engineObject = NULL ;
		_engineEngine = NULL ;
		_recordObject = NULL ;
		_recordRecord = NULL ;
		_recordBufferQueue = NULL ;
	}

	if (_cacheArray.size() > 0) {
		for(uint32_t idx=0; idx < _cacheArray.size(); idx++)
			free(_cacheArray[idx]);
		_cacheArray.clear();
		_recdCaches.clear() ;
		_fullCaches.clear() ;
		_smpsPerFrame = 0 ;
		_cacheCounts  = 0 ;
		_frameLenth   = 0 ;
	}

	return true ;
}
