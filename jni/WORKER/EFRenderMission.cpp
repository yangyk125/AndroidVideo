/*
 * EFRenderMission.cpp
 *
 *  Created on: 2015-11-25
 *      Author: yangyk
 */

#include "EFRenderMission.h"
#include "EFEditorWorker.h"

EFRenderMission::EFRenderMission(const NXSDKMediaFormat& settingFormat, NXPtr<EFRenderProgress>& progress, NXCCSpriteNV12* drawLayer, EFEffectsLayer* effectLayer, const char* sourceFile, const char* targetFile)
{
	_renderFormat = settingFormat ;
	_renderLayer  = drawLayer ;
	_renderEffect = effectLayer ;
	_renderProgrs = progress ;
	_renderSource = sourceFile ;
	_renderTarget = targetFile ;
	_renderNext   = false ;
}

EFRenderMission::~EFRenderMission()
{
	_renderLayer  = NULL ;
	_renderProgrs = NULL ;
	_renderSource.clear() ;
	_renderTarget.clear() ;
}

void EFRenderMission::render_begin()
{
	NXCCDirector::getSharedDirector().setCurrentContext() ;

	if(_renderProgrs)
		_renderProgrs->onRenderProgressBegin() ;

	_renderReader = NXSDK_CreateReader("mpts") ;
	_renderWriter = NXSDK_CreateWriter("mp4s");
	_renderCanvas = NXSDK_CreateCanvas();

	NXSDKMediaFormat fileFormat ;
	_renderReader->open(_renderSource.c_str(), NXSDK_PIX_FMT_NV21, false) ;
	_renderReader->get_media_format(fileFormat) ;
	_renderReader->start() ;

	_renderCanvas->open(NXSizeMake(fileFormat.video_format.width, fileFormat.video_format.height)) ;

	_renderFormat.video_pixfmt = NXSDK_PIX_FMT_RGBA ;//when canvas render, result is rgba ;
	_renderFormat.video_frames = fileFormat.video_frames ;
	_renderFormat.media_duration = fileFormat.media_duration ;

	_renderWriter->open(_renderTarget.c_str(), "mp4", _renderFormat) ;
	_renderWriter->start() ;

	_renderNext = true ;
	_renderCount = 0 ;
}

bool EFRenderMission::render_next()
{
	return _renderNext ;
}

void EFRenderMission::render_step()
{
	NXPtr<NXBuffer> buffer = NULL ;
	NXSDKTrackType  trackr = NXSDKTrackNone ;
	if(_renderReader->get_next_buffer(buffer, &trackr))
	{
		if(trackr == NXSDKTrackVideo)
		{
			NXCCDirector& director = NXCCDirector::getSharedDirector() ;
			CCContext* context = director.getCCContext() ;
			context->setCCCurrentContext() ;

			//update video image
			_renderCanvas->begin() ;

			_renderLayer->updateImateData(buffer) ;
			_renderEffect->refreshEffectSprites(_renderCount, buffer) ;

			if(_renderCount % 10 == 0)
				director.releaseObjects() ; //release some object

			float frameTime = 1.0f/_renderFormat.video_format.frame_rate ;

			CCScene* scene = director.getRunningScene() ;
			float scaley = scene->getScaleY() ;

			scene->updateTime(frameTime) ;
			scene->setScaleY(scaley*-1) ;
			director.renderScene(true) ;
			scene->setScaleY(scaley) ;

			NXPtr<NXBuffer> renderBuffer = _renderCanvas->end() ;

			_renderWriter->append_buffer(renderBuffer, NXSDKTrackVideo) ;

			_renderCount ++ ;

			if(_renderProgrs)
				_renderProgrs->onRenderProgressProgress(1.0f*_renderCount/_renderFormat.video_frames) ;
		}
		else if(trackr == NXSDKTrackAudio1)
		{
			_renderWriter->append_buffer(buffer, NXSDKTrackAudio1) ;
		}
	}
	else
	{
		_renderNext = false ;
	}
}

void EFRenderMission::render_finish()
{
	_renderWriter->stop() ;
	_renderWriter->stoped() ;
	_renderWriter->close() ;
	_renderWriter = NULL ;

	_renderReader->stop() ;
	_renderReader->stoped() ;
	_renderReader->close() ;
	_renderReader = NULL ;

	_renderCanvas->close() ;
	_renderCanvas = NULL ;

	if(_renderProgrs)
		_renderProgrs->onRenderProgressComplete(_renderTarget.c_str()) ;

	_renderProgrs = NULL ;
	_renderLayer  = NULL ;
}

bool EFRenderMission::on_workitem_start()
{
	render_begin() ;
	return _renderNext ;
}

bool EFRenderMission::on_workitem_step()
{
	render_step() ;
	return _renderNext ;
}

void EFRenderMission::on_workitem_stop()
{
	render_finish();
}
