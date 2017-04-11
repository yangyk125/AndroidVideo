/*
 * EFExtractMission.cpp
 *
 *  Created on: 2016-1-4
 *      Author: yangyk
 */

#include "EFExtractMission.h"
#include "EFEditorWorker.h"

EFExtractMission::EFExtractMission(const char* videoFile, int width, int height, int count, NXPtr<EFExtractProgress>& progress)
{
	_targetWidth  = width ;
	_targetHeight = height ;
	_targetCount  = count ;
	_targetIndex  = 0 ;
	_videoFile	  = videoFile ;
	_extProgress  = progress ;
}

EFExtractMission::~EFExtractMission()
{
	_extProgress = NULL ;
}

bool EFExtractMission::on_workitem_start()
{
	if(_extProgress)
		_extProgress->onExtractImagesStart(_targetCount, _targetWidth, _targetHeight) ;

	_extShoter = NXSDK_CreateShoter("mpts") ;
	_extShoter->open(_videoFile.c_str(), NXSDK_PIX_FMT_NV12) ;
	_extShoter->get_media_format(_videoFmat) ;

	_timepiece = _videoFmat.media_duration/_targetCount ;

	NXSDKTrimParam trimParam ;
	trimParam.hreverse = false ;
	trimParam.vreverse = false ;
	trimParam.cutrange = NXRectMake(0,0,1,1) ;
	trimParam.rotate   = 0.0f ;

	_extTrimer = NXSDK_CreateTrimer(NULL) ;
	_extTrimer->open(NXSizeMake(_videoFmat.video_format.width, _videoFmat.video_format.height), NXSizeMake(_targetWidth, _targetHeight), NXSDK_PIX_FMT_NV12, NXSDK_PIX_FMT_BGRA) ;
	_extTrimer->param(trimParam) ;

	return _targetIndex < _targetCount ;
}
bool EFExtractMission::on_workitem_step()
{
	NXPtr<NXBuffer> source = NULL ;
	_extShoter->shot(_timepiece*_targetIndex, false, source) ;

	NXPtr<NXBuffer> target = NULL ;
	if(source)
		_extTrimer->trim(source, target) ;

	if(_extProgress && target)
		_extProgress->onExtractImagesIndex(_targetIndex, _targetWidth, _targetHeight, target) ;

	_targetIndex ++ ;
	return _targetIndex < _targetCount && target != NULL ;
}
void EFExtractMission::on_workitem_stop()
{
	if(_extProgress)
		_extProgress->onExtractImagesStop() ;

	_extTrimer->close() ;
	_extTrimer = NULL ;

	_extShoter->close() ;
	_extShoter = NULL ;
}
