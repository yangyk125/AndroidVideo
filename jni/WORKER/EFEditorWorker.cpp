/*
 * EFEditorWorker.cpp
 *
 *  Created on: 2016年1月6日
 *      Author: yangyk
 */

#include "NXWorker.h"
#include "EFEditorWorker.h"

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_createEditorWorker(JNIEnv* env, jobject obj, jobject surface, jstring rootfolder, jstring videopath)
{
	EFEditorWorker* editorWorker = new EFEditorWorker();
	setJavaNativeObject(env, obj, editorWorker);

	ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);

	const char* pRootPath = env->GetStringUTFChars(rootfolder, NULL) ;
	std::string strRootPath = pRootPath ;
	env->ReleaseStringUTFChars(rootfolder, pRootPath);

	const char* pVideoFile = env->GetStringUTFChars(videopath, NULL) ;
	std::string strVideoFile = pVideoFile ;
	env->ReleaseStringUTFChars(videopath, pVideoFile);

	return editorWorker->createEditorWorker(nativeWindow, strRootPath.c_str(), strVideoFile.c_str()) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_updateTouchEvent(JNIEnv* env, jobject obj, jint type, jfloat x, jfloat y)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->updateTouchEvent(type, x, y) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_destoryEditorWorker(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	delJavaNativeObject(env, obj);
	bool result = editorWorker->destoryEditorWorker() ;
	delete editorWorker ;
	return result ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_pausedEditorWorker(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->pausedEditorWorker() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_resumeEditorWorker(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->resumeEditorWorker() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_pauseLoopAnimation(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->pauseLoopAnimation() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_resumeLoopAnimation(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->resumeLoopAnimation() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_setPlayerProgress(JNIEnv* env, jobject obj, jobject progress)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->setPlayerProgress(env, progress) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_setEditorProgress(JNIEnv* env, jobject obj, jobject progress)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->setEditorProgress(env, progress) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_loadVideoPlayer(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->loadVideoPlayer() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_unloadVideoPlayer(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->unloadVideoPlayer() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_startPlayVideo(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->startPlayVideo() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_pausePlayVideo(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->pausePlayVideo() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_stopPlayVideo(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->stopPlayVideo() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_restartPlayVideo(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->restartPlayVideo() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_renderEffectVideo(JNIEnv* env, jobject obj, jobject progress)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->renderEffectVideo(env, progress) ;
}


JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_renderFinishVideo(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->renderFinishVideo() ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_syncEffectDatas(JNIEnv* env, jobject obj, jstring effects)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;

	if(effects) {
		const char* effectJson = env->GetStringUTFChars(effects, NULL) ;
		std::string strEffectJson = effectJson ;
		env->ReleaseStringUTFChars(effects, effectJson);
		return editorWorker->syncEffectDatas(strEffectJson.c_str()) ;
	}
	else
		return editorWorker->syncEffectDatas(NULL) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_seekPlayPosition(JNIEnv* env, jobject obj, jfloat position)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->seekPlayPosition(position) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_extractSerialImages(JNIEnv* env, jobject obj, jint count, jint width, jint height, jobject progress)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->extractSerialImages(count, width, height, env, progress) ;
}

JNIEXPORT jboolean JNICALL Java_com_android_video_worker_EFEditorWorker_cleanCocos2dXCache(JNIEnv* env, jobject obj)
{
	EFEditorWorker* editorWorker = (EFEditorWorker*)getJavaNativeObject(env, obj) ;
	if(editorWorker == NULL)
		return false ;
	return editorWorker->cleanCocos2dXCache() ;
}



EFEditorWorker::EFEditorWorker()
{
	_effectCanvas = NULL ;
	_effectLayers = NULL ;
}

EFEditorWorker::~EFEditorWorker()
{

}

bool EFEditorWorker::createEditorWorker(ANativeWindow* surface, const char* rootpath, const char* videofile)
{
	_rootsPath = rootpath  ;
	_videoFile = videofile ;

	NXKit::format_string(_rendrPath, "%s/videos", _rootsPath.c_str()) ;
	NXFile::mkdir(_rendrPath.c_str()) ;

	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_CREATEWORKER, static_cast<WBHandleMessage>(&EFEditorWorker::on_create_worker)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_TOUCHEVENT, static_cast<WBHandleMessage>(&EFEditorWorker::on_update_touch)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_DESTORYWORKER, static_cast<WBHandleMessage>(&EFEditorWorker::on_destory_worker)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_PAUSEDWORKER, static_cast<WBHandleMessage>(&EFEditorWorker::on_pause_worker)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_RESUMEWORKER, static_cast<WBHandleMessage>(&EFEditorWorker::on_resume_worker)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_PAUSEDANIMTN, static_cast<WBHandleMessage>(&EFEditorWorker::on_pause_animation)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_RESUMEANIMTN, static_cast<WBHandleMessage>(&EFEditorWorker::on_resume_animation)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_SETPLAYPROGS, static_cast<WBHandleMessage>(&EFEditorWorker::on_player_progress)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_SETEDITPROGS, static_cast<WBHandleMessage>(&EFEditorWorker::on_editor_progress)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_DOLOADVIDEO, static_cast<WBHandleMessage>(&EFEditorWorker::on_doload_video)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_UNLOADVIDEO, static_cast<WBHandleMessage>(&EFEditorWorker::on_unload_video)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_STARTPLAYER, static_cast<WBHandleMessage>(&EFEditorWorker::on_start_playing)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_PAUSEPLAYER, static_cast<WBHandleMessage>(&EFEditorWorker::on_pause_playing)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_STOPDPLAYER, static_cast<WBHandleMessage>(&EFEditorWorker::on_stopd_playing)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_RSTRTPLAYER, static_cast<WBHandleMessage>(&EFEditorWorker::on_restart_playing)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_RENDERVIDEO, static_cast<WBHandleMessage>(&EFEditorWorker::on_render_video)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_RENDERFINSH, static_cast<WBHandleMessage>(&EFEditorWorker::on_render_finsh)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_SYNCEFFECTS, static_cast<WBHandleMessage>(&EFEditorWorker::on_sync_effects)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_SEEKPOSITON, static_cast<WBHandleMessage>(&EFEditorWorker::on_seek_position)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_EXTRACTIMAGES, static_cast<WBHandleMessage>(&EFEditorWorker::on_exct_selimages)) ;
	theWorkbeach.registerMessage(WORKBEACH_EDITORWORK_MESSAGE_CLEANCC2DCACHE, static_cast<WBHandleMessage>(&EFEditorWorker::on_clean_cc2dcache)) ;

	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_CREATEWORKER, this, NULL, NULL, surface) ;
	return true ;
}

bool EFEditorWorker::updateTouchEvent(int type, float x, float y)
{
	EFTouchType64 touch(x, y) ;
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_TOUCHEVENT, this, NULL, NULL, NULL, type, touch.v) ;
	return true ;
}

bool EFEditorWorker::destoryEditorWorker()
{
	theWorkbeach.sendNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_DESTORYWORKER, this) ;
	return true ;
}

bool EFEditorWorker::pausedEditorWorker()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_PAUSEDWORKER, this) ;
	return true ;
}

bool EFEditorWorker::resumeEditorWorker()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_RESUMEWORKER, this) ;
	return true ;
}

bool EFEditorWorker::pauseLoopAnimation()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_PAUSEDANIMTN, this) ;
	return true ;
}

bool EFEditorWorker::resumeLoopAnimation()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_RESUMEANIMTN, this) ;
	return true ;
}

bool EFEditorWorker::setPlayerProgress(JNIEnv* env, jobject progress)
{
	jobject globalRef = env->NewGlobalRef(progress) ;
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_SETPLAYPROGS, this, NULL, NULL, globalRef) ;
	return true ;
}

bool EFEditorWorker::setEditorProgress(JNIEnv* env, jobject progress)
{
	jobject globalRef = env->NewGlobalRef(progress) ;
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_SETEDITPROGS, this, NULL, NULL, globalRef) ;
	return true ;
}

bool EFEditorWorker::loadVideoPlayer()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_DOLOADVIDEO, this) ;
	return true ;
}

bool EFEditorWorker::unloadVideoPlayer()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_UNLOADVIDEO, this) ;
	return true ;
}

bool EFEditorWorker::startPlayVideo()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_STARTPLAYER, this) ;
	return true ;
}

bool EFEditorWorker::pausePlayVideo()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_PAUSEPLAYER, this) ;
	return true ;
}

bool EFEditorWorker::stopPlayVideo()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_STOPDPLAYER, this) ;
	return true ;
}

bool EFEditorWorker::restartPlayVideo()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_RSTRTPLAYER, this) ;
	return true ;
}

bool EFEditorWorker::renderEffectVideo(JNIEnv* env, jobject progress)
{
	jobject globalRef = env->NewGlobalRef(progress) ;
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_RENDERVIDEO, this, NULL, NULL, globalRef) ;
	return true ;
}

bool EFEditorWorker::renderFinishVideo()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_RENDERFINSH, this) ;
	return true ;
}

bool EFEditorWorker::syncEffectDatas(const char* effects)
{
	if(effects) {
		int strLength = strlen(effects) ;
		NXPtr<NXBuffer> effectJson = new NXBuffer(strLength+1) ;
		uint8_t* ptrJson = effectJson->getPlanePtr(0) ;
		ptrJson[strLength] = 0 ;
		memcpy(ptrJson, effects, strLength) ;
		theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_SYNCEFFECTS, this, (NXPtr<NXObject>*)&effectJson) ;
	}
	else
		theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_SYNCEFFECTS, this) ;
	return true ;
}

bool EFEditorWorker::seekPlayPosition(float position)
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_SEEKPOSITON, this, NULL, NULL, NULL, 0, 0, position) ;
	return true ;
}

bool EFEditorWorker::extractSerialImages(int count, int width, int height, JNIEnv* env, jobject progress)
{
	jobject globalRef = env->NewGlobalRef(progress) ;
	uint64_t size64 = (((uint64_t)width) << 32) | ((uint64_t)height)  ;
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_EXTRACTIMAGES, this, NULL, NULL, globalRef, count, size64) ;
	return true ;
}

bool EFEditorWorker::cleanCocos2dXCache()
{
	theWorkbeach.postNormalMessasge(WORKBEACH_EDITORWORK_MESSAGE_CLEANCC2DCACHE, this) ;
	return true ;
}


// - - - - - -// - - - - - -// - - - - - -// - - - - - -// - - - - - -
void EFEditorWorker::on_create_worker(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	ANativeWindow* nativeWindow = (ANativeWindow*)param3 ;

	EGLConfig config = NXCCDirector::getSharedDirector().getAnimationConfig() ;
	_drawSurface = new NXGLSurface(nativeWindow, config);

	NXCCDirector::getSharedDirector().setAnimationSurface(_drawSurface) ;
	NXCCDirector::getSharedDirector().drawSurfaceChanged() ;
	NXCCDirector::getSharedDirector().setCurrentContext() ;
}

void EFEditorWorker::on_update_touch(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	EFTouchType64 touch(param5) ;
	int type = param4 ;

	if(_effectLayers != NULL)
		_effectLayers->updateLayerTouch(type, touch.x, touch.y) ;
}

void EFEditorWorker::on_destory_worker(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_drawSurface  = NULL ;

	_playerImage  = NULL ;
	_playerProgrs = NULL ;
	_effectLister = NULL ;
	_renderProgrs = NULL ;
}

void EFEditorWorker::on_pause_worker(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_playerPlayer->pause() ;

	_effectCanvas->removeFromParent() ;
	_effectLayers->removeFromParent() ;
}

void EFEditorWorker::on_resume_worker(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXCCDirector::getSharedDirector().setAnimationSurface(_drawSurface) ;
	NXCCDirector::getSharedDirector().drawSurfaceChanged() ;
	NXCCDirector::getSharedDirector().setCurrentContext() ;

	NXCCDirector::getSharedDirector().getRunningScene()->addChild(_effectCanvas, 1) ;
	NXCCDirector::getSharedDirector().getRunningScene()->addChild(_effectLayers, 2) ;
}

void EFEditorWorker::on_pause_animation(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXCCDirector::getSharedDirector().pauseAnimation(true) ;
}

void EFEditorWorker::on_resume_animation(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXCCDirector::getSharedDirector().pauseAnimation(false) ;
}

void EFEditorWorker::on_player_progress(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_playerProgrs = new EFPlayerProgress((jobject)param3);
}

void EFEditorWorker::on_editor_progress(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_effectLister = new EFEditorProgress((jobject)param3);
}

void EFEditorWorker::on_doload_video(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXPtr<NXBuffer> firstImage = NULL ;
	_playerPlayer = NXSDK_CreatePlayer("wb") ;
	_playerPlayer->open(_videoFile.c_str(), NULL, this, NXSDK_PIX_FMT_NV21) ;
	_playerPlayer->load(&firstImage) ;
	_playerPlayer->set_auto_repeat(true);
	_playerPlayer->get_media_format(_mediaFormat) ;

	create_edit_element() ;

	NXPtr<EFEffectsModel> effectModel = new EFEffectsModel(_mediaFormat, firstImage) ;
	_effectLayers->setEffectsModel(effectModel, firstImage) ;
	_effectLayers->makeTrackSprite() ;

	_effectCanvas->updateImateData(firstImage) ;
}

void EFEditorWorker::on_unload_video(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	_playerPlayer->stop() ;
	_playerPlayer->unload() ;
	_playerPlayer->close() ;
	_playerPlayer = NULL ;

	destory_edit_element() ;
}

void EFEditorWorker::on_start_playing(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_playerPlayer)
		_playerPlayer->play() ;
}

void EFEditorWorker::on_pause_playing(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_playerPlayer)
		_playerPlayer->pause() ;
}

void EFEditorWorker::on_stopd_playing(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_playerPlayer)
		_playerPlayer->stop() ;
}

void EFEditorWorker::on_restart_playing(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_playerPlayer) {
		_playerPlayer->stop() ;
		_playerPlayer->play() ;
	}
}

void EFEditorWorker::on_render_video(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_playerPlayer) {
		if(_playerPlayer->status() == NXSDKPlayerPlay)
			_playerPlayer->pause() ;
	}

	if(_effectLayers)
		_effectLayers->setLayerStatus(layerPlaying) ;

	if(_renderMisson == NULL)
	{
		NXCCDirector::getSharedDirector().pauseAnimation(true) ;

		std::string renderVideo ;
		NXKit::format_string(renderVideo, "%s/video_%s.mp4", _rendrPath.c_str(), NXKit::get_timenow_string().c_str()) ;
		NXKit::replace_string(renderVideo, ':', '_') ;

		_renderProgrs = new EFRenderProgress((jobject)param3);
		_renderMisson = new EFRenderMission(_mediaFormat, _renderProgrs, _effectCanvas, _effectLayers, _videoFile.c_str(), renderVideo.c_str()) ;

		theWorkbeach.addWorkerMission((NXPtr<NXWorkitem>&)_renderMisson) ;
	}
}

void EFEditorWorker::on_render_finsh(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_renderMisson)
	{
		_renderMisson = NULL ;
		_renderProgrs = NULL ;

		if(_effectLayers)
			_effectLayers->setLayerStatus(layerPlaying) ;

		if(_playerPlayer)
			_playerPlayer->play() ;

		NXCCDirector::getSharedDirector().pauseAnimation(false) ;
	}
}

void EFEditorWorker::on_sync_effects(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(param1 != NULL) {

		NXPtr<NXBuffer> effectJson = (NXPtr<NXBuffer>&)param1 ;
		const char* strEffJson = (const char*)effectJson->getPlanePtr(0) ;

#ifdef DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFMainWorker::on_sync_effects() effect json = %s", strEffJson) ;
#endif

		_effectLayers->getEffectModel()->syncEffectsModel(strEffJson) ;
		_effectLayers->refreshEffectSprites(_playerIndex, _playerImage) ;

	}
	else {

#ifdef DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFMainWorker::on_sync_effects() effect json = NULL ..") ;
#endif

		_effectLayers->getEffectModel()->syncEffectsModel(NULL) ;
		_effectLayers->getEffectModel()->cleanVideoFrame() ;
		_effectLayers->refreshEffectSprites(0, _playerImage) ;
	}
}

void EFEditorWorker::on_seek_position(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	float position = (float)param6 ;

	__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFMainWorker::on_seek_position() position = %.2f ..", position) ;
}

void EFEditorWorker::on_exct_selimages(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	int width = (int)(param5 >> 32) ;
	int height = (int)(param5 & 0xFFFFFFFF) ;

	NXPtr<EFExtractProgress> extractProgress = new EFExtractProgress((jobject)param3);
	NXPtr<EFExtractMission> extractMission = new EFExtractMission(_videoFile.c_str(), width, height, param4, extractProgress) ;

	theWorkbeach.addWorkerMission((NXPtr<NXWorkitem>&)extractMission) ;
}

void EFEditorWorker::on_clean_cc2dcache(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	CCTextureCache::sharedTextureCache()->removeAllTextures() ;
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrames() ;
}


void EFEditorWorker::create_edit_element()
{
	CCSize screenSize = NXCCDirector::getSharedDirector().getRunningScene()->getContentSize() ;

	float imageWidth = _mediaFormat.video_format.width ;
	float imageHeight = _mediaFormat.video_format.height ;

	_effectCanvas = new NXCCSpriteNV12();
	_effectCanvas->init(false, ccp(screenSize.width, screenSize.height), ccsz(imageWidth, imageHeight), CCRectMake(0, 0, 1, 1)) ;
	_effectCanvas->setPosition(ccp(screenSize.width/2, screenSize.height/2)) ;
	NXCCDirector::getSharedDirector().getRunningScene()->addChild(_effectCanvas, 1) ;

	_effectLayers = new EFEffectsLayer(_rootsPath.c_str(), this);
	_effectLayers->init() ;
	_effectLayers->setPosition(screenSize.width/2, screenSize.height/2) ;
	_effectLayers->setContentSize(screenSize) ;
	NXCCDirector::getSharedDirector().getRunningScene()->addChild(_effectLayers, 2) ;
}

void EFEditorWorker::destory_edit_element()
{
	NXCCDirector::getSharedDirector().setCurrentContext() ;

	_effectCanvas->removeFromParent() ;
	_effectLayers->removeFromParent() ;

	_effectCanvas->removeAllChildren() ;
	_effectLayers->removeAllChildren() ;

	_effectCanvas->release();
	_effectLayers->release();

	_effectCanvas = NULL ;
	_effectLayers = NULL ;
}



// - - - - - // - - - - - // - - - - - // - - - - - // - - - - - // - - - - -
void EFEditorWorker::playerLoadCompleted(NXSDKPlayer* player, float duration)
{
	if(_playerProgrs != NULL)
		_playerProgrs->onPlayerLoadedCompleted(duration) ;
}

void EFEditorWorker::playerStatusChanged(NXSDKPlayer* player, NXSDKPlayerStatus status)
{
	switch(status)
	{
	case NXSDKPlayerReady:
		if(_effectLayers != NULL)
			_effectLayers->setLayerStatus(layerEditing) ;
		break ;
	case NXSDKPlayerPlay:
		if(_playerProgrs != NULL)
			_playerProgrs->onPlayerPlayingStart() ;
		if(_effectLayers != NULL)
			_effectLayers->setLayerStatus(layerPlaying) ;
		break ;
	case NXSDKPlayerPause:
		if(_playerProgrs != NULL)
			_playerProgrs->onPlayerPlayingPause() ;
		if(_effectLayers != NULL)
			_effectLayers->setLayerStatus(layerEditing) ;
		break ;
	case NXSDKPlayerStop:
		if(_playerProgrs != NULL)
			_playerProgrs->onPlayerPlayingStoped() ;
		if(_effectLayers != NULL)
			_effectLayers->setLayerStatus(layerEditing) ;
		break ;
	case NXSDKPlayerEnd:
		if(_effectLayers != NULL)
			_effectLayers->setLayerStatus(layerEditing) ;
		break ;
	}
}

void EFEditorWorker::playerPlayingProgess(NXSDKPlayer* player, NXPtr<NXBuffer>& vbuffer, float currentTime)
{
	_playerIndex = currentTime * _mediaFormat.video_format.frame_rate ;
	_playerImage = vbuffer ;

	if(_effectCanvas && _playerImage)
		_effectCanvas->updateImateData(_playerImage) ;

	if(_effectLayers)
		_effectLayers->refreshEffectSprites(_playerIndex, _playerImage) ;

	if(_playerProgrs != NULL)
		_playerProgrs->onPlayerPlayingProgress(currentTime) ;
}

void EFEditorWorker::playerErrorGenerated(NXSDKPlayer* player, uint32_t errorCode)
{

}

void EFEditorWorker::onTouchEffectClose(EFEffectsLayer* effectLayer, uint32_t effectKey)
{
	if(_effectLister)
		_effectLister->onDeleteEffectItem(effectKey) ;
}



//////////////////////////////////////////////////////////////////////
EFPlayerProgress::EFPlayerProgress(jobject progress)
{
	_globalObject = progress ;
}
EFPlayerProgress::~EFPlayerProgress()
{
	getGlobalWchEnv()->DeleteGlobalRef(_globalObject) ;
}
void EFPlayerProgress::onPlayerLoadedCompleted(float duration)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onPlayerLoadedCompleted", "(F)V");
	env->CallVoidMethod(_globalObject, methodId, (jfloat)duration) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFPlayerProgress::onPlayerPlayingProgress(float progress)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onPlayerPlayingProgress", "(F)V");
	env->CallVoidMethod(_globalObject, methodId, (jfloat)progress) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFPlayerProgress::onPlayerPlayingStart()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onPlayerPlayingStart", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFPlayerProgress::onPlayerPlayingPause()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onPlayerPlayingPause", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFPlayerProgress::onPlayerPlayingStoped()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onPlayerPlayingStoped", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}
//////////////////////////////////////////////////////////////////////

EFEditorProgress::EFEditorProgress(jobject progress)
{
	_globalObject = progress ;
}
EFEditorProgress::~EFEditorProgress()
{
	getGlobalWchEnv()->DeleteGlobalRef(_globalObject) ;
}
void EFEditorProgress::onDeleteEffectItem(int effectKey)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onDeleteEffectItem", "(I)V");
	env->CallVoidMethod(_globalObject, methodId, (jint)effectKey) ;
	env->DeleteLocalRef(prgsClz) ;
}

//////////////////////////////////////////////////////////////////////
EFExtractProgress::EFExtractProgress(jobject progress)
{
	_globalObject = progress ;
}
EFExtractProgress::~EFExtractProgress()
{
	getGlobalWchEnv()->DeleteGlobalRef(_globalObject) ;
}
void EFExtractProgress::onExtractImagesStart(int count, int width, int height)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onExtractImagesStart", "(III)V");
	env->CallVoidMethod(_globalObject, methodId, count, width, height) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFExtractProgress::onExtractImagesIndex(int index, int width, int height, NXPtr<NXBuffer>& image)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onExtractImagesIndex", "(III[I)V");
	jintArray colorsArray = env->NewIntArray(width*height) ;
	jint* rgbaBuffer = (jint*)image->getPlanePtr(0) ;
	env->SetIntArrayRegion(colorsArray, 0, width*height, rgbaBuffer) ;
	env->CallVoidMethod(_globalObject, methodId, index, width, height, colorsArray) ;
	env->ReleaseIntArrayElements(colorsArray, rgbaBuffer, 0) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFExtractProgress::onExtractImagesStop()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onExtractImagesStop", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}

//////////////////////////////////////////////////////////////////////
EFRenderProgress::EFRenderProgress(jobject progress)
{
	_globalObject = progress ;
}
EFRenderProgress::~EFRenderProgress()
{
	getGlobalWchEnv()->DeleteGlobalRef(_globalObject) ;
}
void EFRenderProgress::onRenderProgressBegin()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onRenderProgressBegin", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFRenderProgress::onRenderProgressProgress(float progress)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onRenderProgressProgress", "(F)V");
	env->CallVoidMethod(_globalObject, methodId, (jfloat)progress) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFRenderProgress::onRenderProgressFailure()
{
	JNIEnv* env = getGlobalWchEnv() ;
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onRenderProgressFailure", "()V");
	env->CallVoidMethod(_globalObject, methodId) ;
	env->DeleteLocalRef(prgsClz) ;
}
void EFRenderProgress::onRenderProgressComplete(const char* buildFile)
{
	JNIEnv* env = getGlobalWchEnv() ;
	jstring jFilePath = env->NewStringUTF(buildFile);
	jclass prgsClz = env->GetObjectClass(_globalObject) ;
	jmethodID methodId = env->GetMethodID(prgsClz, "onRenderProgressComplete", "(Ljava/lang/String;)V");
	env->CallVoidMethod(_globalObject, methodId, jFilePath) ;
	env->ReleaseStringUTFChars(jFilePath, buildFile) ;
	env->DeleteLocalRef(prgsClz) ;
}
