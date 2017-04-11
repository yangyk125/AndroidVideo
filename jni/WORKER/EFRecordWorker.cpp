/*
 * EFRecordWorker.cpp
 *
 *  Created on: 2015-11-20
 *      Author: yangyk
 */


#include "EFRecordWorker.h"
#include "NXWorker.h"


JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_createRecordWorker(JNIEnv* env, jobject obj, jobject surface, jint cameraWidth, jint cameraHeight, jstring folder)
{
	EFRecordWorker* recordWorker = new EFRecordWorker();
	setJavaNativeObject(env, obj, recordWorker);

	ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);
	const char* ptrRecFolder = env->GetStringUTFChars(folder, NULL) ;
	std::string strRecFolder = ptrRecFolder ;

	env->ReleaseStringUTFChars(folder, ptrRecFolder);
	return recordWorker->createRecordWorker(nativeWindow, cameraWidth, cameraHeight, strRecFolder.c_str()) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_updateCameraImage(JNIEnv* env, jobject obj, jint width, jint height, jbyteArray data)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;

	jbyte* pdata = env->GetByteArrayElements(data, 0);
	jboolean result = recordWorker->updateCameraImage(width, height, (const uint8_t*)pdata) ;
	env->ReleaseByteArrayElements(data, pdata, 0);
	return result ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_destoryRecordWorker(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	delJavaNativeObject(env, obj);
	bool result = recordWorker->destoryRecordWorker() ;
	delete recordWorker ;
	return result ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_pausedRecordWorker(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->pausedRecordWorker() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_resumeRecordWorker(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->resumeRecordWorker() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_setRecordProgress(JNIEnv* env, jobject obj, jobject progress)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->setRecordProgress(env, progress) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_pauseLoopAnimation(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->pauseLoopAnimation() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_resumeLoopAnimation(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->resumeLoopAnimation() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_startRecordVideo(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->startRecordVideo() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_stopRecordVideo(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->stopRecordVideo() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_deleteRecordVideo(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->deleteRecordVideo() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_showFilterPanel(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->showFilterPanel() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_hideFilterPanel(JNIEnv* env, jobject obj, jint index)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->hideFilterPanel(index) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_buildRecordVideos(JNIEnv* env, jobject obj, jobject progress)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->buildRecordVideos(env, progress) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFRecordWorker_buildFinishVideos(JNIEnv* env, jobject obj)
{
	EFRecordWorker* recordWorker = (EFRecordWorker*)getJavaNativeObject(env, obj) ;
	if(recordWorker == NULL)
		return false ;
	return recordWorker->buildFinishVideos() ;
}


////////////////////////////////////////////////////////////////////

#define FILTERS_HIDE_CONTEXT 1454001

EFRecordWorker::EFRecordWorker()
{
	_nineptParent = NULL ;
	_recordCanvas = NULL ;
	memset(_nineptArrays, 0, sizeof(_nineptArrays)) ;
	memset(_nineptFilter, 0, sizeof(_nineptFilter)) ;
}

EFRecordWorker::~EFRecordWorker()
{
	_recordProgs = NULL ;
	_buildProgrs = NULL ;
	_imageCamera = NULL ;
}

bool EFRecordWorker::createRecordWorker(ANativeWindow* surface, int cameraWidth, int cameraHeight, const char* rootFolder)
{
	_rootFolder = rootFolder ;

	NXKit::format_string(_recdFolder, "%s/record", _rootFolder.c_str()) ;
	NXFile::mkdir(_recdFolder.c_str()) ;

	NXKit::format_string(_vidoFolder, "%s/videos", _rootFolder.c_str()) ;
	NXFile::mkdir(_vidoFolder.c_str()) ;

	_cameraSize.w = cameraWidth ;
	_cameraSize.h = cameraHeight ;

	_mediaFormat.track_mask = NXSDKTrackVideo | NXSDKTrackAudio1 ;
	_mediaFormat.media_duration = 0 ;
	_mediaFormat.video_format.width = MIN(_cameraSize.w, _cameraSize.h) ;
	_mediaFormat.video_format.height = MIN(_cameraSize.w, _cameraSize.h) ;
	_mediaFormat.video_format.frame_rate = 25 ;
	_mediaFormat.video_pixfmt = NXSDK_PIX_FMT_NV21 ;
	_mediaFormat.video_bit_rate = 0 ;
	_mediaFormat.audio_format.channels = 1 ;
	_mediaFormat.audio_format.sample_bits = 16 ;
	_mediaFormat.audio_format.sample_rate = 48000 ;
	_mediaFormat.audio_bit_rate = 0 ;

	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_CREATEWORKER, static_cast<WBHandleMessage>(&EFRecordWorker::on_create_worker)) ;
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_DESTORYWORKER, static_cast<WBHandleMessage>(&EFRecordWorker::on_destory_worker)) ;
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_UPDATECAMERA, static_cast<WBHandleMessage>(&EFRecordWorker::on_update_camera)) ;

	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_PAUSEDWORKER, static_cast<WBHandleMessage>(&EFRecordWorker::on_paused_worker)) ;
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_RESUMEWORKER, static_cast<WBHandleMessage>(&EFRecordWorker::on_resume_worker)) ;

	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_SETRECDPROGRS, static_cast<WBHandleMessage>(&EFRecordWorker::on_record_progs)) ;

	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_PAUSEDANIM, static_cast<WBHandleMessage>(&EFRecordWorker::on_pause_animing)) ;
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_RESUMEANIM, static_cast<WBHandleMessage>(&EFRecordWorker::on_resume_animing)) ;

	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_STARTRECORD, static_cast<WBHandleMessage>(&EFRecordWorker::on_start_record)) ;
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_STOPRECORD, static_cast<WBHandleMessage>(&EFRecordWorker::on_stop_record)) ;
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_DELETEVIDEO, static_cast<WBHandleMessage>(&EFRecordWorker::on_delete_record)) ;

	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_SHOWFILTERS, static_cast<WBHandleMessage>(&EFRecordWorker::on_show_filters)) ;
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_HIDEFILTERS, static_cast<WBHandleMessage>(&EFRecordWorker::on_hide_filters)) ;

	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_BUILDVIDEOS, static_cast<WBHandleMessage>(&EFRecordWorker::on_build_videos)) ;
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_BUILDFINISH, static_cast<WBHandleMessage>(&EFRecordWorker::on_build_finish)) ;

	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_CREATEWORKER, this, NULL, NULL, surface) ;

	return true ;
}

bool EFRecordWorker::updateCameraImage(int width, int height, const uint8_t* data)
{
	if(is_building())
		return false ;

#ifdef DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, "CAMERA", "EFRecordWorker::updateCameraImage() Camera update new image !!") ;
#endif

	NXPtr<NXBuffer> buffer = new NXBuffer(width, height, NXSDK_PIX_FMT_NV21);
	buffer->lockBuffer() ;
	uint8_t* ybuf = buffer->getPlanePtr(0);
	uint8_t* uvbuf = buffer->getPlanePtr(1);
	memcpy(ybuf, data, width*height) ;
	memcpy(uvbuf, data+width*height, width*height/2) ;
	buffer->unlockBuffer() ;

	theWorkbeach.postHighMessasge(WORKBEACH_RECORDWORK_MESSAGE_UPDATECAMERA, this, (NXPtr<NXObject>*)&buffer) ;
	return true ;
}

bool EFRecordWorker::destoryRecordWorker()
{
	theWorkbeach.sendNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_DESTORYWORKER, this) ;
	return true ;
}

bool EFRecordWorker::pausedRecordWorker()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_PAUSEDWORKER, this) ;
	return true ;
}

bool EFRecordWorker::resumeRecordWorker()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_RESUMEWORKER, this) ;
	return true ;
}

bool EFRecordWorker::setRecordProgress(JNIEnv* env, jobject progress)
{
	jobject globalRef = env->NewGlobalRef(progress) ;
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_SETRECDPROGRS, this, NULL, NULL, globalRef) ;
	return true ;
}

bool EFRecordWorker::pauseLoopAnimation()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_PAUSEDANIM, this) ;
	return true ;
}

bool EFRecordWorker::resumeLoopAnimation()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_RESUMEANIM, this) ;
	return true ;
}

bool EFRecordWorker::startRecordVideo()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_STARTRECORD, this) ;
	return true ;
}

bool EFRecordWorker::stopRecordVideo()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_STOPRECORD, this) ;
	return true ;
}

bool EFRecordWorker::deleteRecordVideo()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_DELETEVIDEO, this) ;
	return true ;
}

bool EFRecordWorker::showFilterPanel()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_SHOWFILTERS, this) ;
	return true ;
}

bool EFRecordWorker::hideFilterPanel(int index)
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_HIDEFILTERS, this, NULL, NULL, NULL, (uint32_t)index) ;
	return true ;
}

bool EFRecordWorker::buildRecordVideos(JNIEnv* env, jobject progress)
{
	jobject globalRef = env->NewGlobalRef(progress) ;
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_BUILDVIDEOS, this, NULL, NULL, globalRef) ;
	return true ;
}

bool EFRecordWorker::buildFinishVideos()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_RECORDWORK_MESSAGE_BUILDFINISH, this) ;
	return true ;
}


/// - - - /// - - - /// - - - /// - - - /// - - -

void EFRecordWorker::on_create_worker(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	ANativeWindow* nativeSurface = (ANativeWindow*) param3;

	_recordAudio = NXSDK_CreateRecorder(NULL) ;
	_recordAudio->open(_mediaFormat.audio_format, _mediaFormat.audio_format.sample_rate/_mediaFormat.video_format.frame_rate, 8) ;
	_recordAudio->start() ;

	EGLConfig config = NXCCDirector::getSharedDirector().getAnimationConfig() ;
	_drawSurface = new NXGLSurface(nativeSurface, config);

	NXCCDirector::getSharedDirector().setAnimationInterval(1.0f/30) ;
	NXCCDirector::getSharedDirector().setAnimationSurface(_drawSurface) ;
	NXCCDirector::getSharedDirector().drawSurfaceChanged() ;
	NXCCDirector::getSharedDirector().makeRunningScene() ;
	NXCCDirector::getSharedDirector().startAnimation() ;

	create_recd_element() ;
}

void EFRecordWorker::on_paused_worker(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_recordAudio->pause(true) ;

	_nineptParent->removeFromParent() ;
}

void EFRecordWorker::on_resume_worker(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXCCDirector::getSharedDirector().setAnimationSurface(_drawSurface) ;
	NXCCDirector::getSharedDirector().drawSurfaceChanged() ;
	NXCCDirector::getSharedDirector().getRunningScene()->addChild(_nineptParent, 0) ;

	_recordAudio->pause(false) ;
}

void EFRecordWorker::on_destory_worker(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXCCDirector::getSharedDirector().stopAnimation() ;
	NXCCDirector::getSharedDirector().getRunningScene()->removeAllChildren() ;
	NXCCDirector::getSharedDirector().delAnimationSurface() ;

	destory_recd_element() ;

	_recordAudio->stop() ;
	_recordAudio->close() ;
	_recordAudio = NULL ;

	_imageCamera = NULL ;
	_drawSurface = NULL ;

	_recordProgs = NULL ;
	_buildProgrs = NULL ;
}

void EFRecordWorker::on_update_camera(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_imageCamera = *(NXPtr<NXBuffer>*)&param1 ;

	if(_recordCanvas && _imageCamera)
		_recordCanvas->updateImateData(_imageCamera) ;
}

void EFRecordWorker::on_record_progs(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_recordProgs = new EFRecordProgress((jobject)param3);
}

void EFRecordWorker::on_pause_animing(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXCCDirector::getSharedDirector().pauseAnimation(true) ;
}

void EFRecordWorker::on_resume_animing(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXCCDirector::getSharedDirector().pauseAnimation(false) ;
}


void EFRecordWorker::on_start_record(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_recordTimer)
		return ;

	std::string segmentFile ;
	NXKit::format_string(segmentFile, "%s/segment_%02d.mpt", _recdFolder.c_str(), (int)_recordSgmts.size()) ;

	_recordSgmts.push_back(segmentFile) ;
	_recordFrams.push_back(0) ;

	NXSDKMediaFormat segmFmt = _mediaFormat ;
	segmFmt.video_format.width = _cameraSize.w ;
	segmFmt.video_format.height = _cameraSize.h ;

	_recordWrite = NXSDK_CreateWriter("mpta") ;
	_recordWrite->open(segmentFile.c_str(), "mpt", segmFmt) ;
	_recordWrite->start() ;

	_recordTimer = new NXTimer();
	_recordTimer->open(this, NXTimeMake(1, _mediaFormat.video_format.frame_rate), true) ;
	_recordTimer->start() ;
}
void EFRecordWorker::on_stop_record(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_recordTimer == NULL)
		return ;

	_recordTimer->stop() ;
	_recordTimer->stoped() ;
	_recordTimer->close() ;
	_recordTimer = NULL ;

	__android_log_print(ANDROID_LOG_ERROR, "Recorder", "close record file !!");

	_recordWrite->stop() ;
	_recordWrite->stoped() ;
	_recordWrite->close() ;
	_recordWrite = NULL ;
}
void EFRecordWorker::on_delete_record(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(this->is_recording())
		return ;

	if(_recordSgmts.size() > 0) {
		_recordSgmts.pop_back() ;

		uint32_t frames = _recordFrams.back() ;
		_recordFrams.pop_back() ;

		_recordProgs->onDeleteLastSegment(1.0f*frames/_mediaFormat.video_format.frame_rate) ;

		uint32_t allFrames = 0 ;
		for(int idx=0; idx<_recordFrams.size(); idx++)
			allFrames += _recordFrams[idx] ;
		_recordProgs->onRecordSegmentsDuration(1.0f*allFrames/_mediaFormat.video_format.frame_rate) ;
	}

	if(_recordSgmts.size() == 0)
		_recordProgs->onDeleteAllTheSegments() ;
}
void EFRecordWorker::on_show_filters(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	for(int idx=0; idx<9; idx++)
		_nineptArrays[idx]->setVisible(true) ;

	CCSize screenSize = NXCCDirector::getSharedDirector().getRunningScene()->getContentSize() ;

	CCScaleTo* animation1 = CCScaleTo::create(0.5, 1.0f/3) ;
	CCMoveTo*  animation2 = CCMoveTo::create(0.5, ccp(0.5*screenSize.width, 0.5*screenSize.height)) ;

	animation1->setActionListener(this) ;
	animation2->setActionListener(this) ;

	_nineptParent->runAction(animation1) ;
	_nineptParent->runAction(animation2) ;
}
void EFRecordWorker::on_hide_filters(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_ninpetIndex = param4 ;

	CCSize screenSize = NXCCDirector::getSharedDirector().getRunningScene()->getContentSize() ;

	float offsetx = 1.5f-(_ninpetIndex%3) ;
	float offsety = -0.5f+(_ninpetIndex/3);

	/*
	switch(param4)
	{
		case 0:offsetx= 1.5f; offsety=-0.5f; break ;
		case 1:offsetx= 0.5f; offsety=-0.5f; break ;
		case 2:offsetx=-0.5f; offsety=-0.5f; break ;
		case 3:offsetx= 1.5f; offsety= 0.5f; break ;
		case 4:offsetx= 0.5f; offsety= 0.5f; break ;
		case 5:offsetx=-0.5f; offsety= 0.5f; break ;
		case 6:offsetx= 1.5f; offsety= 1.5f; break ;
		case 7:offsetx= 0.5f; offsety= 1.5f; break ;
		case 8:offsetx=-0.5f; offsety= 1.5f; break ;
	}
	*/

	CCScaleTo* animation1 = CCScaleTo::create(0.5, 1.0f) ;
	CCMoveTo*  animation2 = CCMoveTo::create(0.5, ccp(offsetx*screenSize.width, offsety*screenSize.width)) ;

	animation1->setContext1(FILTERS_HIDE_CONTEXT) ;

	animation1->setActionListener(this) ;
	animation2->setActionListener(this) ;

	_nineptParent->runAction(animation1) ;
	_nineptParent->runAction(animation2) ;
}
void EFRecordWorker::on_build_videos(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(this->is_recording())
		return ;

	if(_buildMisson == NULL)
	{
		NXCCDirector::getSharedDirector().pauseAnimation(true) ;

		NXSDKMediaFormat mediaFormat = _mediaFormat ;
		mediaFormat.video_format.width = _cameraSize.w ;
		mediaFormat.video_format.height = _cameraSize.h ;

		NXSDKMediaFormat buildFormat = mediaFormat ;

		buildFormat.video_format.width = buildFormat.video_format.height = MIN(mediaFormat.video_format.width, mediaFormat.video_format.height) ;
		buildFormat.video_pixfmt = NXSDK_PIX_FMT_RGBA ;

		_buildProgrs = new EFBuildProgress((jobject)param3);
		_buildMisson = new EFBuilderMission(_buildProgrs, _recdFolder, buildFormat, mediaFormat, _recordSgmts, _recordCanvas) ;

		theWorkbeach.addWorkerMission((NXPtr<NXWorkitem>&)_buildMisson) ;
	}
}
void EFRecordWorker::on_build_finish(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_buildMisson)
	{
		_buildMisson = NULL ;
		_buildProgrs = NULL ;

		NXCCDirector::getSharedDirector().pauseAnimation(false) ;
	}
}
void EFRecordWorker::on_timer_calling(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_recordWrite && _recordTimer && _imageCamera)
	{
		NXPtr<NXBuffer> abuffer = NULL ;
		if(_recordAudio->record_packet(abuffer) && abuffer != NULL){

			_recordWrite->append_buffer(abuffer, NXSDKTrackAudio1) ;
			_recordWrite->append_buffer(_imageCamera, NXSDKTrackVideo) ;

			uint32_t& frameCount =_recordFrams[_recordFrams.size()-1] ;
			frameCount ++ ;

			if(_recordProgs && frameCount>0 && frameCount%_mediaFormat.video_format.frame_rate == 0)
			{
				uint32_t allFrames = 0 ;
				for(int idx=0; idx<_recordFrams.size(); idx++)
					allFrames += _recordFrams[idx] ;

				_recordProgs->onRecordSegmentsDuration(1.0f*allFrames/_mediaFormat.video_format.frame_rate) ;
			}
		}
	}
}


void EFRecordWorker::create_recd_element()
{
	CCSize screenSize = NXCCDirector::getSharedDirector().getRunningScene()->getContentSize() ;

	float imageWidth  = _cameraSize.w ;
	float imageHeight = _cameraSize.h ;

	float cutx = (imageWidth-imageHeight)/2/imageWidth ;
	float cuty = 0 ;
	float cutw = imageHeight/imageWidth ;
	float cuth = 1 ;

	_recordCanvas  = new NXCCSpriteNV12();
	_recordCanvas->init(false, ccp(screenSize.width, screenSize.height), ccsz(imageWidth, imageHeight), CCRectMake(cutx, cuty, cutw, cuth)) ;
	_recordCanvas->setPosition(ccp(screenSize.width/2, screenSize.height/2)) ;
	_recordCanvas->setRotation(90) ;

	_nineptParent = new NXCCNode();
	_nineptParent->setContentSize(ccp(screenSize.width*3, screenSize.height*3)) ;
	_nineptParent->setPosition(ccp(screenSize.width/2, screenSize.height/2)) ;
	_nineptParent->setScale(1.0f) ;

	_ninpetIndex = 4 ;

	float itemWidth  = screenSize.width ;
	float itemHeight = screenSize.height ;

	std::string filterFile ; //"/storage/emulated/0/e4fun/filters/%d.png" ;
	NXKit::format_string(filterFile, "%s/filters/%%d.png", _rootFolder.c_str()) ;

	for(int h=0; h<3; h++)
	{
		for(int w=0; w<3; w++)
		{
			int idx = h*3+w ;

			NXCCShareNode* pareSprite = new NXCCShareNode();
			pareSprite->setContentSize(ccsz(itemWidth, itemHeight)) ;
			pareSprite->setPosition(ccp(itemWidth*w+itemWidth/2, 3*itemHeight-itemHeight*h-itemHeight/2)) ;

			_nineptParent->addChild(pareSprite, 1) ;

			NXCCSprite* filtSprite = new NXCCSprite();
			if(idx == _ninpetIndex)
			{
				filtSprite->init() ;
				filtSprite->setOpacity(0) ;
			}
			else
			{
				std::string pngFile ;
				NXKit::format_string(pngFile, filterFile.c_str(), idx+1) ;
				filtSprite->initWithFile(pngFile.c_str()) ;
			}

			pareSprite->setVisible(idx==_ninpetIndex) ;

			filtSprite->setTextureRect(CCRectMake(0, 0, itemWidth, itemHeight)) ;
			filtSprite->setContentSize(ccsz(itemWidth, itemHeight)) ;
			filtSprite->setPosition(ccp(itemWidth/2, itemHeight/2)) ;

			pareSprite->addChild(_recordCanvas, 2) ;
			pareSprite->addChild(filtSprite, 3) ;

			_nineptArrays[idx] = pareSprite ;
			_nineptFilter[idx] = filtSprite ;
		}
	}

	NXCCDirector::getSharedDirector().getRunningScene()->addChild(_nineptParent, 0) ;
}


void EFRecordWorker::destory_recd_element()
{
	NXCCDirector::getSharedDirector().setCurrentContext() ;

	if(_nineptParent)
	{
		for(int idx=0; idx<9; idx++)
		{
			_nineptArrays[idx]->removeFromParent() ;
			_nineptArrays[idx]->removeAllChildren() ;
			_nineptArrays[idx]->release() ;
			_nineptArrays[idx] = NULL ;

			_nineptFilter[idx]->release() ;
			_nineptFilter[idx] = NULL ;
		}

		_nineptParent->removeFromParent() ;
		_nineptParent->removeAllChildren() ;
		_nineptParent->release() ;
		_nineptParent = NULL ;

		_ninpetIndex = 0 ;
	}

	if(_recordCanvas)
	{
		_recordCanvas->removeFromParent() ;
		_recordCanvas->release() ;
		_recordCanvas = NULL ;
	}
}

bool EFRecordWorker::is_recording()
{
	return _recordTimer != NULL ;
}

bool EFRecordWorker::is_building()
{
	return _buildMisson != NULL ;
}

void EFRecordWorker::on_timer_start(NXTimer* timer)
{
	theWorkbeach.registerMessage(WORKBEACH_RECORDWORK_MESSAGE_RECORDFRAME, static_cast<WBHandleMessage>(&EFRecordWorker::on_timer_calling)) ;
}

void EFRecordWorker::on_timer_stop(NXTimer* timer)
{
	theWorkbeach.unregisterMessage(WORKBEACH_RECORDWORK_MESSAGE_RECORDFRAME) ;
}

void EFRecordWorker::on_timer_running(NXTimer* timer)
{
	theWorkbeach.postHighMessasge(WORKBEACH_RECORDWORK_MESSAGE_RECORDFRAME, this) ;
}


void EFRecordWorker::onActionStoped(CCAction* action)
{
	if(action->getContext1() == FILTERS_HIDE_CONTEXT) {
		for(uint32_t idx=0; idx<9; idx++)
			_nineptArrays[idx]->setVisible(_ninpetIndex==idx) ;
	}
}

////////////////////////////////////////////////////////////

EFRecordProgress::EFRecordProgress(jobject progress)
{
	_globalObject = progress ;
}

EFRecordProgress::~EFRecordProgress()
{
	getGlobalWchEnv()->DeleteGlobalRef(_globalObject) ;
}

void EFRecordProgress::onRecordSegmentsDuration(float duration)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onRecordSegmentsDuration", "(F)V");
	env->CallVoidMethod(_globalObject, methodId, (jfloat)duration) ;
	env->DeleteLocalRef(prgsClz) ;
}

void EFRecordProgress::onDeleteLastSegment(float duration)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onDeleteLastSegment", "(F)V");
	env->CallVoidMethod(_globalObject, methodId, (jfloat)duration) ;
	env->DeleteLocalRef(prgsClz) ;
}

void EFRecordProgress::onDeleteAllTheSegments()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onDeleteAllTheSegments", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}

////////////////////////////////////////////////////////////

EFBuildProgress::EFBuildProgress(jobject progress)
{
	_globalObject = progress ;
}

EFBuildProgress::~EFBuildProgress()
{
	getGlobalWchEnv()->DeleteGlobalRef(_globalObject) ;
}

void EFBuildProgress::onBuildProgressBegin()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onBuildProgressBegin", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}

void EFBuildProgress::onBuildProgressProgress(float progress)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onBuildProgressProgress", "(F)V");
	env->CallVoidMethod(_globalObject, methodId, (jfloat)progress) ;
	env->DeleteLocalRef(prgsClz) ;
}

void EFBuildProgress::onBuildProgressFailure()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onBuildProgressFailure", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}

void EFBuildProgress::onBuildProgressComplete(const char* filePath)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jstring jFilePath = env->NewStringUTF(filePath);
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onBuildProgressComplete", "(Ljava/lang/String;)V");
	env->CallVoidMethod(_globalObject, methodId, jFilePath) ;
	env->ReleaseStringUTFChars(jFilePath, filePath) ;
	env->DeleteLocalRef(prgsClz) ;
}
