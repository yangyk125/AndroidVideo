/*
 * EFBuilderMission.cpp
 *
 *  Created on: 2015-12-21
 *      Author: yangyk
 */

#include "EFBuilderMission.h"
#include "EFRecordWorker.h"

EFBuilderMission::EFBuilderMission(NXPtr<EFBuildProgress>& buildProgrs, std::string& recordFolder, NXSDKMediaFormat& mediaFormat, NXSDKMediaFormat& recordFormat, std::vector<std::string>& recordSgmts, NXCCSpriteNV12* canvasLayer)
{
	_buildProgrs = buildProgrs ;
	_mediaFormat = mediaFormat ;
	_recrdFormat = recordFormat ;
	_recordFolder = recordFolder ;
	_recordSgmts.assign(recordSgmts.begin(), recordSgmts.end()) ;
	_recordCount = _recordSgmts.size() ;

	_canvasLayer = canvasLayer ;
	_canvasLayer->retain() ;
}

EFBuilderMission::~EFBuilderMission()
{
	_buildProgrs = NULL ;
	_recordFolder.clear() ;
	_buildrVideo.clear() ;
	_recordSgmts.clear() ;

	_canvasLayer->release() ;
	_canvasLayer = NULL ;
}

bool EFBuilderMission::on_workitem_start()
{
	if(_buildProgrs)
		_buildProgrs->onBuildProgressBegin() ;

	NXKit::format_string(_buildrVideo, "%s/%s", _recordFolder.c_str(), RECORD_BUILD_TARGET_FILE) ;

	_mediaFormat.video_pixfmt = NXSDK_PIX_FMT_RGBA ; //NXSDK_PIX_FMT_BGRA ;

	_buildWriter = NXSDK_CreateWriter("mpts") ;

	if(!_buildWriter->open(_buildrVideo.c_str(), "mpt",  _mediaFormat)){
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_start() writer open failure !!");
		if(_buildProgrs)
			_buildProgrs->onBuildProgressFailure() ;
		return false ;
	}

	if(!_buildWriter->start()){
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_start() writer start failure !!");
		if(_buildProgrs)
			_buildProgrs->onBuildProgressFailure() ;
		return false ;
	}

	_buildCanvas = NXSDK_CreateCanvas();

	if(!_buildCanvas->open(NXSizeMake(_mediaFormat.video_format.width, _mediaFormat.video_format.height))){
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_start() canvas open failure !!");
		if(_buildProgrs)
			_buildProgrs->onBuildProgressFailure() ;
		return false ;
	}

	_buildReader = NULL ;
	_buildIndex  = -1 ;
	_buildFrames = 0 ;

	return true ;
}

bool EFBuilderMission::on_workitem_step()
{
	if(_buildReader == NULL)
	{
		_buildIndex ++ ;
		if(_buildIndex < _recordCount)
		{
			const std::string& segmentFile = _recordSgmts[_buildIndex];

			_buildReader = NXSDK_CreateReader("mpts") ;

			if(!_buildReader->open(segmentFile.c_str(), NXSDK_PIX_FMT_NV21, false)) {
				__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_step() reader open failure !!");
				if(_buildProgrs)
					_buildProgrs->onBuildProgressFailure() ;
				return false ;
			}

			if(!_buildReader->get_media_format(_segmtFormat)) {
				__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_step() get media format failure !!");
				if(_buildProgrs)
					_buildProgrs->onBuildProgressFailure() ;
				return false ;
			}

			if(!_buildReader->start()) {
				__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_step() reader start failure !!");
				if(_buildProgrs)
					_buildProgrs->onBuildProgressFailure() ;
				return false ;
			}
		}
		else
			return false ;
	}

	NXPtr<NXBuffer> buffer = NULL ;
	NXSDKTrackType  track  = NXSDKTrackNone ;
	if(_buildReader->get_next_buffer(buffer, &track))
	{
		if(track == NXSDKTrackVideo)
		{
			NXCCDirector& director = NXCCDirector::getSharedDirector() ;
			CCContext* context = director.getCCContext() ;
			context->setCCCurrentContext() ;

			_buildCanvas->begin() ;

			_canvasLayer->updateImateData(buffer) ;

			float frameTime = 1.0f/_mediaFormat.video_format.frame_rate ;

			CCScene* scene = director.getRunningScene() ;
			float scaley = scene->getScaleY() ;

			scene->updateTime(frameTime) ;
			scene->setScaleY(scaley*-1) ;
			director.renderScene(true) ;
			scene->setScaleY(scaley) ;

			NXPtr<NXBuffer> newbuf = _buildCanvas->end() ;

			if(!_buildWriter->append_buffer(newbuf, track)){
				__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_step() writer append_buffer V failure!!");
				if(_buildProgrs)
					_buildProgrs->onBuildProgressFailure() ;
				return false ;
			}

			if(_buildProgrs)
				_buildProgrs->onBuildProgressProgress(1.0f*_buildIndex/_recordCount + 1.0f/_recordCount*(++_buildFrames)/_segmtFormat.video_frames) ;
		}
		else if(track == NXSDKTrackAudio1)
		{
			if(!_buildWriter->append_buffer(buffer, track)){
				__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_step() writer append_buffer A failure!!");
				if(_buildProgrs)
					_buildProgrs->onBuildProgressFailure() ;
				return false ;
			}
		}
	}
	else
	{
		if(!_buildReader->stop()) {
			__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_step() reader stop failure!!");
			if(_buildProgrs)
				_buildProgrs->onBuildProgressFailure() ;
			return false ;
		}

		if(!_buildReader->stoped()) {
			__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_step() reader stoped failure!!");
			if(_buildProgrs)
				_buildProgrs->onBuildProgressFailure() ;
			return false ;
		}

		if(!_buildReader->close()) {
			__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_step() reader close failure!!");
			if(_buildProgrs)
				_buildProgrs->onBuildProgressFailure() ;
			return false ;
		}

		_buildReader = NULL ;
		_buildFrames = 0 ;
	}

	return true ;
}

void EFBuilderMission::on_workitem_stop()
{
	if(!_buildWriter->stop()) {
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_stop() writer stop failure!!");
		if(_buildProgrs)
			_buildProgrs->onBuildProgressFailure() ;
		return ;
	}

	if(!_buildWriter->stoped()) {
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_stop() writer stoped failure!!");
		if(_buildProgrs)
			_buildProgrs->onBuildProgressFailure() ;
		return ;
	}

	if(!_buildWriter->close()) {
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_stop() writer close failure!!");
		if(_buildProgrs)
			_buildProgrs->onBuildProgressFailure() ;
		return ;
	}

	if(!_buildCanvas->close()) {
		__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_stop() close canvas failure!!");
		if(_buildProgrs)
			_buildProgrs->onBuildProgressFailure() ;
		return ;
	}

	_buildWriter = NULL ;
	_buildCanvas = NULL ;

#ifdef DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "EFBuilderMission::on_workitem_stop() build video file = %s", _buildrVideo.c_str());
#endif

	if(_buildProgrs)
		_buildProgrs->onBuildProgressComplete(_buildrVideo.c_str()) ;
}
