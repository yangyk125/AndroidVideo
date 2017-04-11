/*
 * EFEffectsModel.h
 *
 *  Created on: 2016年1月6日
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

//effect data
class EFEffectData : public NXObject
{
private:
public:
	EFEffectData();
	virtual ~EFEffectData();
public:
	std::string		effectId    ;
	uint32_t		effectKey   ;

	uint32_t		effectStart ; 		//effect appear first frame
	uint32_t		effectCount ;		//effect running for frame count.

	float			effectWidth ;
	float 			effectHeight ;

	std::string 	effectName  ;		//effect name
	std::string		effectPath  ;		//effect native folder

	NXPoint	 		effectOffset ;		//effect center offset with tracker center.
	float    		effectRotate ;		//effect rotate angle
	float	 		effectScale  ;		//effect scale value.
};


//frame data
typedef enum tag_EFTrackBox {
    trackBoxInvalid = 0, //当前帧不属于任何标签，或者还没有进行过跟踪。
    trackBoxFirst  = 1,  //当前帧是某个片段的第一帧，当前框选区域作为后面跟踪的初始值。
    trackBoxUnable = 2,  //已经对当前帧进行跟踪，但是结果是跟踪失败，即目标丢失。
    trackBoxNormal = 3,  //已经对当前帧进行跟踪，结果是正常值，拥有一个框选区域。
} EFTrackBox ;


class EFVideoFrame : public NXObject
{
public:
	EFVideoFrame();
	virtual ~EFVideoFrame();
public:
    bool           	valid 	 ; //内部使用的成员，业务里面不要使用。
    uint32_t       	index 	 ; //当前帧的帧索引。
    EFTrackBox     	type  	 ; //当前帧框选区域的实际有效类型
    NXRect         	rect  	 ; //当前帧跟踪框的区域
    NXPtr<NXBuffer>	image 	 ; //如果是first，用于初始化的image.
    float           angle    ; //角度偏移
    float           posx     ; //坐标x偏移
    float           posy     ; //坐标y偏移
} ;

typedef std::map<uint32_t, NXPtr<EFEffectData> > EFEffDataMap ;
typedef std::map<uint32_t, EFVideoFrame > 		 EFFrmDataMap ;
typedef std::vector< NXPtr<EFEffectData> > 		 EFFrmEffList ;

class EFALGTracker ;
class EFEffectsModel : public NXObject
{

private:
	NXSDKMediaFormat 	_mediaFormat ;

private:
	EFEffDataMap 		_effectDataMap ; //effect key  -> effect data
	EFFrmDataMap		_vframeDataMap ; //frame index -> frame data
	EFFrmEffList	 	_vframeEffList ;

	NXPtr<EFALGTracker> _videoTracker  ;

public:
	EFEffectsModel(const NXSDKMediaFormat& mediaFormat, NXPtr<NXBuffer>& firstImage);
	virtual ~EFEffectsModel();

private:
	void getEffectsKeys(std::set<uint32_t>& keys) ;

//sync methods
public:
	bool syncEffectsModel(const char* ejson) ;

//get methods
public:
	bool isFrameInTrack(uint32_t index, uint32_t* first = NULL) ;
	bool trackVideoFrame(uint32_t index, NXPtr<NXBuffer>& image) ;
	bool seekToVideoFrame(uint32_t index, NXPtr<NXBuffer>& image) ;//seek
	bool cleanTracksAfter(uint32_t index) ;
	void setFirstFrame(uint32_t index, NXPtr<NXBuffer>& image, NXRect& rect) ;
	void getVideoFormat(NXSDKMediaFormat& mediaFormat) ;
	EFVideoFrame& getVideoFrame(uint32_t index) ;
	void cleanVideoFrame() ;
	std::vector< NXPtr<EFEffectData> >& getFrameEffects(uint32_t index) ;

};

