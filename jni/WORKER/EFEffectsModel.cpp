/*
 * EFEffectsModel.cpp
 *
 *  Created on: 2016年1月6日
 *      Author: yangyk
 */

#include "EFEffectsModel.h"
#include "json/json.h"

#include "EFALGTracker.h"
#include "EFALGSaliency.h"

EFEffectData::EFEffectData()
{
	effectStart  = 0 ;
	effectCount  = 0 ;
	effectRotate = 0 ;
	effectScale  = 1 ;
	effectOffset = NXPointMake(0,0) ;
}
EFEffectData::~EFEffectData()
{

}


EFVideoFrame::EFVideoFrame()
{
    valid 	= false ;
    index 	= 0 ;
    type  	= trackBoxInvalid	 ;
    image 	= NULL ;
    angle  	= 0 ;
    posx   	= 0 ;
    posy  	= 0 ;
    memset(&rect, 0, sizeof(rect)) ;
}
EFVideoFrame::~EFVideoFrame()
{
	image 	= NULL ;
}





EFEffectsModel::EFEffectsModel(const NXSDKMediaFormat& mediaFormat, NXPtr<NXBuffer>& firstImage)
{
	_mediaFormat = mediaFormat ;
	_videoTracker = new EFALGTracker();
	_videoTracker->_trackFirst = -1 ;

	NXRect rect = NXRectMake(0,0,0,0) ;
	EFALGSaliency::static_saliency(firstImage, _mediaFormat.video_format.width, _mediaFormat.video_format.height, 9, rect) ;

#ifdef DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "saliency get rect at 0, x=%.2f, y=%.2f, w=%.2f, h=%.2f", rect.x, rect.y, rect.w, rect.h);
#endif

	if(_videoTracker->open(firstImage, _mediaFormat.video_format.width, _mediaFormat.video_format.height, rect)) {
		_videoTracker->_trackFirst = 0 ;
		setFirstFrame(0, firstImage, rect) ;
	}

#ifdef DEBUG
	else
		__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "tracker open at index = 0 failed !!!");
#endif

}

EFEffectsModel::~EFEffectsModel()
{
	_effectDataMap.clear() ;
	_vframeDataMap.clear() ;
	_vframeEffList.clear() ;
	_videoTracker->close();
	_videoTracker = NULL ;
}

void EFEffectsModel::getEffectsKeys(std::set<uint32_t>& keys)
{
	for(EFEffDataMap::iterator it = _effectDataMap.begin();
			it != _effectDataMap.end();
			it ++)
	{
		keys.insert(it->first);
	}
}

bool EFEffectsModel::syncEffectsModel(const char* ejson)
{
	std::set<uint32_t> existKeys ;
	getEffectsKeys(existKeys);

	if(ejson)
	{
		json::Value  jsonRoot ;
		json::Reader jsonReader ;
		jsonReader.parse(ejson, jsonRoot) ;

		json::Value& effects = jsonRoot["effects"] ;
		int count = effects.size() ;

		for(int idx=0; idx<count; idx++)
		{
			json::Value& effect = effects[idx] ;

			uint32_t	 effectKey  = effect["effect_key"].asUInt() ;

			if(existKeys.find(effectKey) == existKeys.end())
			{
				std::string  effectId   	= effect["effect_id"].asString() ;
				std::string  effectName 	= effect["effect_name"].asString() ;
				std::string  effectPath 	= effect["effect_path"].asString() ;
				float 		 effectWidth 	= effect["effect_width"].asFloat() ;
				float		 effectHeight 	= effect["effect_height"].asFloat() ;
				float 		 effectStart 	= effect["effect_start"].asFloat() ;
				float		 effectDuration = effect["effect_duration"].asFloat() ;

				NXPtr<EFEffectData> effData = new EFEffectData();

				effData->effectId   	= effectId ;
				effData->effectKey  	= effectKey ;
				effData->effectName  	= effectName ;
				effData->effectPath  	= effectPath ;
				effData->effectWidth 	= effectWidth ;
				effData->effectHeight 	= effectHeight ;
				effData->effectStart 	= effectStart * _mediaFormat.video_format.frame_rate ;
				effData->effectCount 	= effectDuration * _mediaFormat.video_format.frame_rate ;
				effData->effectOffset 	= NXPointMake(0, 0) ;
				effData->effectRotate 	= 0.0f ;
				effData->effectScale  	= 1.0f ;

				_effectDataMap[effectKey] = effData ;
			}
			else
			{
				existKeys.erase(effectKey);
			}
		}
	}

	for(std::set<uint32_t>::iterator it = existKeys.begin();
			it != existKeys.end();
			it ++)
	{
		_effectDataMap.erase(*it);
	}

	return true ;
}

void EFEffectsModel::getVideoFormat(NXSDKMediaFormat& mediaFormat)
{
	mediaFormat = _mediaFormat ;
}

bool EFEffectsModel::isFrameInTrack(uint32_t index, uint32_t* first /*= NULL*/)
{
	int iFirst = -1 ;
	for(EFFrmDataMap::iterator it =  _vframeDataMap.begin();
			it != _vframeDataMap.end();
			it ++)
	{
		if(it->second.type == trackBoxFirst)
			iFirst = it->first ;
		if(it->first >= index)
			break ;
	}

	if(first && iFirst >= 0)
		*first = iFirst ;

	return iFirst >= 0 ;
}

bool EFEffectsModel::trackVideoFrame(uint32_t index, NXPtr<NXBuffer>& image)
{
	NXRect rect = NXRectMake(0,0,0,0) ;
	EFVideoFrame& frameData = _vframeDataMap[index] ;

	if(_videoTracker->_trackFirst < 0)
	{
		EFALGSaliency::static_saliency(image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, 9, rect) ;
		if(!_videoTracker->open(image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, rect)) {
			__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "tracker open failed !!!");
			return false ;
		}
		_videoTracker->_trackFirst = index ;
		setFirstFrame(index, image, rect) ;
	}
	else if(frameData.valid && frameData.type == trackBoxFirst)
	{
		if(!_videoTracker->open(frameData.image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, frameData.rect))
			return false ;
		_videoTracker->_trackFirst = index ;
	}
	else if(!frameData.valid)
	{
#ifdef PRINT_TRACK_FRAME_TIME
		uint32_t tick1 = NXKit::get_time_tick32();
#endif
		if(_videoTracker->track(image, rect))
		{
#ifdef PRINT_TRACK_FRAME_TIME
			uint32_t tick2 = NXKit::get_time_tick32();
			__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "tracker work cost ticks = %u !!!", tick2-tick1);
#endif
			frameData.valid = true ;
			frameData.posx  = 0 ;
			frameData.posy  = 0 ;
			frameData.angle = 0 ;
			frameData.type  = trackBoxNormal ;
			frameData.rect  = rect ;
			frameData.index = index ;
		}
		else
		{
			EFALGSaliency::static_saliency(image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, 9, rect) ;
			if(!_videoTracker->open(image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, rect)) {
				__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "tracker open failed !!!");
				return false ;
			}

			_videoTracker->_trackFirst = index ;
			setFirstFrame(index, image, rect) ;
		}
	}
	return true ;
}

bool EFEffectsModel::seekToVideoFrame(uint32_t index, NXPtr<NXBuffer>& image)
{
	uint32_t first = 0 ;
	NXRect rect = NXRectMake(0,0,0,0) ;

	if(!isFrameInTrack(index, &first))
	{
		EFALGSaliency::static_saliency(image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, 9, rect) ;
		if(!_videoTracker->open(image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, rect)) {
			__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "tracker open failed !!!");
			return false ;
		}
		_videoTracker->_trackFirst = index ;
		setFirstFrame(index, image, rect) ;
	}
	else if(_videoTracker->_trackFirst != first)
	{
		EFVideoFrame& firstData = _vframeDataMap[first] ;
		EFVideoFrame& frameData = _vframeDataMap[index] ;
		if(!_videoTracker->open(firstData.image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, rect)) {
			__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "tracker open failed !!!");
			return false ;
		}
		_videoTracker->_trackFirst = first ;

		if(!frameData.valid)
		{
#ifdef PRINT_TRACK_FRAME_TIME
			uint32_t tick1 = NXKit::get_time_tick32();
#endif
			if(_videoTracker->track(image, rect))
			{
#ifdef PRINT_TRACK_FRAME_TIME
				uint32_t tick2 = NXKit::get_time_tick32();
				__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "tracker work cost ticks = %u !!!", tick2-tick1);
#endif
				frameData.valid = true ;
				frameData.posx  = 0 ;
				frameData.posy  = 0 ;
				frameData.angle = 0 ;
				frameData.type  = trackBoxNormal ;
				frameData.rect  = rect ;
				frameData.index = index ;
			}
			else
			{
				EFALGSaliency::static_saliency(image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, 9, rect) ;
				if(!_videoTracker->open(image, _mediaFormat.video_format.width, _mediaFormat.video_format.height, rect)) {
					__android_log_print(ANDROID_LOG_DEBUG, "TRACK", "tracker open failed !!!");
					return false ;
				}

				_videoTracker->_trackFirst = index ;
				setFirstFrame(index, image, rect) ;
			}
		}
	}
	return true ;
}

bool EFEffectsModel::cleanTracksAfter(uint32_t index)
{
	for(EFFrmDataMap::iterator it = _vframeDataMap.begin() ;
			it != _vframeDataMap.end();)
	{
		if(it->first > index)
			_vframeDataMap.erase(it++) ;
		else
			it++ ;
	}
	return false ;
}

void EFEffectsModel::setFirstFrame(uint32_t index, NXPtr<NXBuffer>& image, NXRect& rect)
{
	EFVideoFrame& frameData = _vframeDataMap[index] ;
	frameData.image = image ;
	frameData.valid = true ;
	frameData.posx  = 0 ;
	frameData.posy  = 0 ;
	frameData.angle = 0 ;
	frameData.type  = trackBoxFirst ;
	frameData.rect  = rect ;
	frameData.index = index ;
}

EFVideoFrame& EFEffectsModel::getVideoFrame(uint32_t index)
{
	return _vframeDataMap[index] ;
}

void EFEffectsModel::cleanVideoFrame()
{
	_vframeDataMap.clear();
}

std::vector< NXPtr<EFEffectData> >& EFEffectsModel::getFrameEffects(uint32_t index)
{
	_vframeEffList.clear();
	for(EFEffDataMap::iterator it = _effectDataMap.begin();
			it != _effectDataMap.end();
			it ++)
	{
		NXPtr<EFEffectData>& effectData = it->second ;
		if(effectData->effectStart <= index && effectData->effectStart + effectData->effectCount > index)
			_vframeEffList.push_back(effectData);
	}
	return _vframeEffList ;
}

