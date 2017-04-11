/*
 * EFEffectsLayer.h
 *
 *  Created on: 2016年1月6日
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"
#include "EFEffectsModel.h"

typedef std::map<uint32_t, NXCCSprite*>  EFSpritesMap ;
typedef std::list<NXCCSprite*>  EFSpritesList ;

typedef enum tag_EFEffectType {
	effectUnknown  = 0,
	effectChartle  = 1,  //贴图贴图
	effectParticle = 2,  //粒子效果。
} EFEffectType ;


typedef enum tag_EFLayerStatus {
	layerUnknown = 0,
	layerPlaying = 1,
	layerEditing = 2,
	layerTraking = 3
} EFLayerStatus ;

class EFEffectsLayer ;
interface EFEffectsListener : public NXCallback
{
	virtual void onTouchEffectClose(EFEffectsLayer* effectLayer, uint32_t effectKey) = 0 ;
};


class EFEffectsLayer : public NXCCNode
{
private:
	std::string				_rootFolder    ;
	NXPtr<EFEffectsModel>	_effectModel   ;
	NXSDKMediaFormat		_videoFormat   ;
	EFSpritesMap			_efftSprites   ;
	EFSpritesList			_spritesList   ;
	EFLayerStatus			_layerStatus   ;
	NXCCSprite*				_trackSprite   ;

	uint32_t				_playerIndex   ;
	NXPtr<NXBuffer>			_playerImage   ;

	EFEffectsListener*		_layerLister   ;
	//temp fields
	NXCCSprite*				_touchEffect   ;
	NXCCSprite*				_touchButton   ;
	CCPoint					_touchPoint    ;
public:
	EFEffectsLayer(const char* rootFolder, EFEffectsListener* layerLister);
	virtual ~EFEffectsLayer();
public:
	void setEffectsModel(NXPtr<EFEffectsModel>& effectModel, NXPtr<NXBuffer>& firstImage) ;
	NXPtr<EFEffectsModel>& getEffectModel() ;
	void makeTrackSprite();
	void setLayerStatus(EFLayerStatus status, bool refresh = true);
	EFLayerStatus getLayerStatus();
	void syncAllSpriteStatus() ;
	void syncTheSpriteStatus(NXCCSprite* effectSprite) ;
private:
	void getSpritesKeys(std::set<uint32_t>& keys) ;
	void createSpriteEffect(NXCCSprite* effectSprite, NXPtr<EFEffectData>& effectPath) ;
	bool isRectContainPoint(const CCRect& rect, const CCPoint& point) ;
	void hitTouchEffectSprite(const CCPoint& worldPoint, NXCCSprite** returnParent, NXCCSprite** returnChildren) ;
	bool isWorldPointInSprite(const CCPoint& worldPoint, NXCCSprite* sprite) ;
public:
	void updateLayerTouch(int type, float x, float y) ;
	void refreshEffectSprites(uint32_t frameIndex, NXPtr<NXBuffer>& playerImage) ;

};

