/*
 * EFEffectsLayer.cpp
 *
 *  Created on: 2016年1月6日
 *      Author: yangyk
 */

#include "EFEffectsLayer.h"
#include "EFRecordWorker.h"
#include "EFEditorWorker.h"

EFEffectsLayer::EFEffectsLayer(const char* rootFolder, EFEffectsListener* layerLister)
{
	_layerLister = layerLister ;
	_rootFolder  = rootFolder ;
	_effectModel = NULL ;
	_layerStatus = layerUnknown ;
	_touchEffect = NULL ;
	_touchButton = NULL ;
	_trackSprite = NULL ;
	_playerIndex = 0 ;
	_playerImage = NULL ;
}

EFEffectsLayer::~EFEffectsLayer()
{
	_layerLister = NULL ;
	_effectModel = NULL ;
	_efftSprites.clear();
	_playerIndex = 0 ;
	_playerImage = NULL ;
}

void EFEffectsLayer::setEffectsModel(NXPtr<EFEffectsModel>& effectModel, NXPtr<NXBuffer>& firstImage)
{
	_effectModel = effectModel ;
	_effectModel->getVideoFormat(_videoFormat) ;
	_playerIndex = 0 ;
	_playerImage = firstImage ;
	_layerStatus = layerEditing ;
}

NXPtr<EFEffectsModel>& EFEffectsLayer::getEffectModel()
{
	return _effectModel ;
}

void EFEffectsLayer::makeTrackSprite()
{
	if(_trackSprite == NULL) {

		_trackSprite = new NXCCSprite();
		_trackSprite->init() ;
		_trackSprite->setContext2(1000) ;
		_trackSprite->setOpacity(0x20) ;
		_trackSprite->setColor(ccc3(255,0,0)) ;

		_trackSprite->setDrawCorner(true) ;
		_trackSprite->setCornerLength(m_obContentSize.width/30) ;
		_trackSprite->setCornerWeight(m_obContentSize.width/200) ;

		this->addChild(_trackSprite, 0) ;
		_trackSprite->release() ;

		EFVideoFrame& frameData = _effectModel->getVideoFrame(0) ;
		if(frameData.valid) {
			CCSize  size = CCSizeMake(frameData.rect.w*m_obContentSize.width, frameData.rect.h*m_obContentSize.height) ;
			_trackSprite->setTextureRect(CCRectMake(0, 0, size.width, size.height)) ;
			_trackSprite->setPosition(ccp((frameData.rect.x+frameData.rect.w/2)*m_obContentSize.width, (1-frameData.rect.y-frameData.rect.h/2)*m_obContentSize.height)) ;
		}
		else {
			_trackSprite->setTextureRect(CCRectMake(0, 0, m_obContentSize.width/3, m_obContentSize.height/3)) ;
			_trackSprite->setPosition(ccp(m_obContentSize.width/2, m_obContentSize.height/2)) ;
		}

		float btnSize = m_obContentSize.width/15 ;
		const CCSize& spriteSize = _trackSprite->getContentSize() ;

		NXCCSprite* scaleSprite = new NXCCSprite();
		std::string scaleIcon ;
		NXKit::format_string(scaleIcon, "%s/icons/icon_effect_scale.png", _rootFolder.c_str()) ;
		scaleSprite->initWithFile(scaleIcon.c_str()) ;
		scaleSprite->setTextureRect(CCRectMake(0, 0, btnSize, btnSize)) ;
		scaleSprite->setContentSize(ccsz(btnSize, btnSize)) ;
		scaleSprite->setPosition(ccp(spriteSize.width, 0)) ;
		_trackSprite->addChild(scaleSprite) ;
		scaleSprite->release();

		_trackSprite->setContext5(scaleSprite) ;
	}
}

void EFEffectsLayer::setLayerStatus(EFLayerStatus status, bool refresh/* = true*/)
{
	_layerStatus = status ;
	if(refresh)
		syncAllSpriteStatus() ;
}

void EFEffectsLayer::syncAllSpriteStatus()
{
	for(EFSpritesList::iterator it = _spritesList.begin();
					it != _spritesList.end();
					it ++)
	{
		NXCCSprite* effectSprite = *it ;
		syncTheSpriteStatus(effectSprite);
	}

	if(_trackSprite)
	{
		switch(_layerStatus)
		{
		case layerEditing:
			_trackSprite->setVisible(true) ;
			break ;
		case layerPlaying:
			_trackSprite->setVisible(false) ;
			break ;
		case layerTraking:
			_trackSprite->setVisible(true) ;
			break ;
		}
	}
}

void EFEffectsLayer::syncTheSpriteStatus(NXCCSprite* effectSprite)
{
	NXCCSprite* btnSprite1 = (NXCCSprite*)effectSprite->getContext5() ;
	NXCCSprite* btnSprite2 = (NXCCSprite*)effectSprite->getContext6() ;
	NXCCSprite* btnSprite3 = (NXCCSprite*)effectSprite->getContext7() ;

	switch(_layerStatus)
	{
	case layerEditing:
		btnSprite1->setVisible(true) ;
		btnSprite2->setVisible(true) ;
		btnSprite3->setVisible(true) ;
		effectSprite->setDrawBounds(true) ;
		break ;
	case layerPlaying:
		btnSprite1->setVisible(false) ;
		btnSprite2->setVisible(false) ;
		btnSprite3->setVisible(false) ;
		effectSprite->setDrawBounds(false) ;
		break ;
	case layerTraking:
		effectSprite->setVisible(false) ;
		break ;
	}
}

EFLayerStatus EFEffectsLayer::getLayerStatus()
{
	return _layerStatus ;
}

void EFEffectsLayer::getSpritesKeys(std::set<uint32_t>& keys)
{
	for(EFSpritesMap::iterator it = _efftSprites.begin();
			it != _efftSprites.end();
			it ++)
	{
		keys.insert(it->first);
	}
}

void EFEffectsLayer::refreshEffectSprites(uint32_t frameIndex, NXPtr<NXBuffer>& playerImage)
{
	if(_effectModel == NULL)
		return ;

	_playerIndex = frameIndex ;
	_playerImage = playerImage ;

	std::vector<NXPtr<EFEffectData> >& frameEffects = _effectModel->getFrameEffects(_playerIndex) ;

	std::set<uint32_t> existKeys ;
	getSpritesKeys(existKeys);

	_effectModel->trackVideoFrame(_playerIndex, _playerImage) ;
	EFVideoFrame& frameData = _effectModel->getVideoFrame(_playerIndex) ;

	if(frameData.valid) {
		CCSize  size = CCSizeMake(frameData.rect.w*m_obContentSize.width, frameData.rect.h*m_obContentSize.height) ;
		_trackSprite->setTextureRect(CCRectMake(0, 0, size.width, size.height)) ;
		_trackSprite->setPosition(ccp((frameData.rect.x+frameData.rect.w/2)*m_obContentSize.width, (1-frameData.rect.y-frameData.rect.h/2)*m_obContentSize.height)) ;

		const CCSize& trackSize = _trackSprite->getContentSize() ;
		NXCCSprite* trackScale = (NXCCSprite*)_trackSprite->getContext5() ;
		trackScale->setPosition(ccp(trackSize.width, 0)) ;
	}

#ifdef DEBUG
	//__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "frame=%u track rect x=%.2f, y=%.2f, w=%.2f, h=%.2f",_playerIndex, frameData.rect.x, frameData.rect.y, frameData.rect.w, frameData.rect.h);
#endif

	for(uint32_t idx=0; idx<frameEffects.size(); idx++)
	{
		NXPtr<EFEffectData>& effectData = frameEffects[idx] ;
		std::set<uint32_t>::iterator it = existKeys.find(effectData->effectKey);
		if(it == existKeys.end())
		{
			NXCCSprite* effectSprite = new NXCCSprite();
			effectSprite->init() ;
			effectSprite->setContext2(3000) ;
			effectSprite->setOpacity(0x0) ;
			effectSprite->setDrawBounds(true) ;
			effectSprite->setScale(effectData->effectScale) ;
			effectSprite->setRotation(effectData->effectRotate) ;

			if(frameData.type == trackBoxFirst || frameData.type == trackBoxNormal) {
				float centerX = (frameData.rect.x+frameData.rect.w/2) * m_obContentSize.width ;
				float centerY = (1.0f-(frameData.rect.y+frameData.rect.h/2)) * m_obContentSize.height ;
				effectSprite->setPosition(ccp(centerX+effectData->effectOffset.x, centerY+effectData->effectOffset.y)) ;
			}
			else
				effectSprite->setPosition(ccp(m_obContentSize.width/2+effectData->effectOffset.x, m_obContentSize.height/2+effectData->effectOffset.y)) ;

			effectSprite->setContext1(effectData->effectKey) ;
			effectSprite->setContext9((EFEffectData*)effectData) ;

			createSpriteEffect(effectSprite, effectData);
			syncTheSpriteStatus(effectSprite) ;

			NXCCSprite* btnSprite1 = (NXCCSprite*)effectSprite->getContext5() ;
			NXCCSprite* btnSprite2 = (NXCCSprite*)effectSprite->getContext6() ;
			NXCCSprite* btnSprite3 = (NXCCSprite*)effectSprite->getContext7() ;

			btnSprite1->setScale(1.0f/effectData->effectScale) ;
			btnSprite2->setScale(1.0f/effectData->effectScale) ;
			btnSprite3->setScale(1.0f/effectData->effectScale) ;

			_efftSprites[effectData->effectKey] = effectSprite ;
			this->addChild(effectSprite, 1) ;
			_spritesList.push_back(effectSprite) ;

			effectSprite->release() ;
		}
		else
		{
			NXCCSprite* effectSprite = _efftSprites[effectData->effectKey] ;
			if(frameData.type == trackBoxFirst || frameData.type == trackBoxNormal) {
				float centerX = (frameData.rect.x+frameData.rect.w/2) * m_obContentSize.width ;
				float centerY = (1.0f-(frameData.rect.y+frameData.rect.h/2)) * m_obContentSize.height ;
				effectSprite->setPosition(ccp(centerX+effectData->effectOffset.x, centerY+effectData->effectOffset.y)) ;
			}

			existKeys.erase(effectData->effectKey);
		}
	}

	for(std::set<uint32_t>::iterator it = existKeys.begin();
			it != existKeys.end();
			it ++)
	{
		NXCCSprite* effectSprite = _efftSprites[*it] ;
		_efftSprites.erase(*it);
		this->removeChild(effectSprite) ;

		for(EFSpritesList::iterator it = _spritesList.begin();
				it != _spritesList.end();
				it ++)
		{
			if(*it == effectSprite) {
				_spritesList.erase(it);
				break ;
			}
		}
	}
}

void EFEffectsLayer::createSpriteEffect(NXCCSprite* effectSprite, NXPtr<EFEffectData>& effectPath)
{
	std::string effectFile ;
	NXKit::format_string(effectFile, "%s/effect.plist", effectPath->effectPath.c_str()) ;
	CCDictionary* effectDict = CCDictionary::createWithContentsOfFile(effectFile.c_str()) ;

	int    effectType   = effectDict->valueForKey("type")->intValue();
	float  effectWidth  = effectPath->effectWidth ;
	float  effectHeight = effectPath->effectHeight ;

	if(effectWidth == 0.0f && effectHeight == 0.0f) {
		effectWidth = 0.3f ;
		effectHeight = 0.3f ;
	}

	CCArray* plistFiles = (CCArray*)effectDict->objectForKey("plists") ;
	CCString* textrFile = (CCString*)effectDict->valueForKey("texture") ;
	CCString* textrName = (CCString*)effectDict->valueForKey("texname") ;
	uint32_t  frameRate = effectDict->valueForKey("fpsrate")->intValue() ;

	if(effectType == effectChartle)
	{
		if(plistFiles->count() == 0)
			return ;

		if(frameRate == 0)
			frameRate = 15 ;

		CCString* plistName = (CCString*)plistFiles->objectAtIndex(0) ;

		std::string plistPath ;
		NXKit::format_string(plistPath, "%s/%s", effectPath->effectPath.c_str(), plistName->getCString()) ;

		std::string textrPath ;
		NXKit::format_string(textrPath, "%s/%s", effectPath->effectPath.c_str(), textrFile->getCString()) ;

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plistPath.c_str(), textrPath.c_str()) ;
	    //加载序列帧到缓存，可以根据name得到序列帧对象。
		std::string textureName ;
		NXKit::format_string(textureName, textrName->getCString(), 0) ;

	    //根据序列帧第一帧创建一个Sprite对象。
	    CCSprite* frameSprite = CCSprite::createWithSpriteFrameName(textureName.c_str()) ;
	    CCSize frameOSize = frameSprite->getContentSize() ;

	    float frameWidth = 0.0f ;
	    float frameHeight = 0.0f ;

	    if(effectWidth>0)
	    	frameWidth = m_obContentSize.width*effectWidth ;
	    else if(effectHeight>0)
	    	frameWidth = m_obContentSize.height*effectHeight*frameOSize.width/frameOSize.height ;

	    if(effectWidth>0)
	    	frameHeight = m_obContentSize.width*effectWidth*frameOSize.height/frameOSize.width ;
	    else if(effectHeight>0)
	    	frameHeight = m_obContentSize.height*effectHeight ;

	    frameSprite->setScale(frameWidth/frameOSize.width) ;
	    frameSprite->setPosition(ccp(frameWidth/2, frameHeight/2)) ;

	    effectSprite->addChild(frameSprite) ;
	    effectSprite->setContentSize(ccsz(frameWidth, frameHeight)) ;
	    effectSprite->setTextureRect(CCRectMake(0, 0, frameWidth, frameHeight)) ;

	    // load the ship's animation frames as textures and create a sprite frame
		CCArray* frames = CCArray::createWithCapacity(30) ;
	    CCSpriteFrameCache* frameCache = CCSpriteFrameCache::sharedSpriteFrameCache() ;
		for (int idx=0; idx<200; idx++)
		{
			std::string path ;
			NXKit::format_string(path, textrName->getCString(), idx) ;
			CCSpriteFrame* frame = frameCache->spriteFrameByName(path.c_str()) ;
	        if(!frame)
	            break;
	        frames->addObject(frame) ;
		}

		CCAnimation* animation = CCAnimation::createWithSpriteFrames(frames, 1.0f/frameRate) ;
		CCAction* action = CCRepeatForever::create(CCAnimate::create(animation)) ;
		frameSprite->runAction(action) ;
	}
	else if(effectType == effectParticle)
	{
		int count = plistFiles->count() ;
		for(int idx=0; idx<count; idx++)
		{
			float particleWidth = m_obContentSize.width/5;
			float particleHeight = m_obContentSize.height/5;

			if(effectWidth == 0)
				effectWidth = effectHeight ;
			else if(effectHeight == 0)
				effectHeight = effectWidth ;

		    if(effectWidth > 0)
		    	particleWidth = m_obContentSize.width*effectWidth ;
		    if(effectHeight > 0)
		    	particleHeight = m_obContentSize.height*effectHeight ;

			CCString* plistFile = (CCString*)plistFiles->objectAtIndex(idx) ;

			std::string plistPath ;
			NXKit::format_string(plistPath, "%s/%s", effectPath->effectPath.c_str(), plistFile->getCString()) ;

		    CCParticleSystemQuad* particleSprite = CCParticleSystemQuad::create(plistPath.c_str()) ;
		    particleSprite->registerScheduler() ;
		    particleSprite->setPosition(particleWidth/2, particleHeight/2) ;

		    CCParticleBatchNode* ptrBatchNode = CCParticleBatchNode::createWithParticle(particleSprite) ;
		    ptrBatchNode->addChild(particleSprite) ;

		    effectSprite->addChild(ptrBatchNode) ;
		    effectSprite->setContentSize(ccsz(particleWidth, particleHeight)) ;
		    effectSprite->setTextureRect(CCRectMake(0, 0, particleWidth, particleHeight)) ;
		}
	}

	//add adjust buttons
	float btnSize = m_obContentSize.width/15 ;
	const CCSize& spriteSize = effectSprite->getContentSize() ;

	NXCCSprite* closeSprite = new NXCCSprite();
	std::string closeIcon ;
	NXKit::format_string(closeIcon, "%s/icons/icon_effect_delete.png", _rootFolder.c_str()) ;
	closeSprite->initWithFile(closeIcon.c_str()) ;
	closeSprite->setTextureRect(CCRectMake(0, 0, btnSize, btnSize)) ;
	closeSprite->setContentSize(ccsz(btnSize, btnSize)) ;
	closeSprite->setPosition(ccp(0, spriteSize.height)) ;
	effectSprite->addChild(closeSprite, 10) ;
	closeSprite->release() ;

	NXCCSprite* rotateSprite = new NXCCSprite();
	std::string rotateIcon ;
	NXKit::format_string(rotateIcon, "%s/icons/icon_effect_rotate.png", _rootFolder.c_str()) ;
	rotateSprite->initWithFile(rotateIcon.c_str()) ;
	rotateSprite->setTextureRect(CCRectMake(0, 0, btnSize, btnSize)) ;
	rotateSprite->setContentSize(ccsz(btnSize, btnSize)) ;
	rotateSprite->setPosition(ccp(spriteSize.width, spriteSize.height)) ;
	effectSprite->addChild(rotateSprite, 11) ;
	rotateSprite->release();

	NXCCSprite* scaleSprite = new NXCCSprite();
	std::string scaleIcon ;
	NXKit::format_string(scaleIcon, "%s/icons/icon_effect_scale.png", _rootFolder.c_str()) ;
	scaleSprite->initWithFile(scaleIcon.c_str()) ;
	scaleSprite->setTextureRect(CCRectMake(0, 0, btnSize, btnSize)) ;
	scaleSprite->setContentSize(ccsz(btnSize, btnSize)) ;
	scaleSprite->setPosition(ccp(spriteSize.width, 0)) ;
	effectSprite->addChild(scaleSprite, 12) ;
	scaleSprite->release();

	effectSprite->setContext5(closeSprite) ;
	effectSprite->setContext6(rotateSprite) ;
	effectSprite->setContext7(scaleSprite) ;
}

bool EFEffectsLayer::isRectContainPoint(const CCRect& rect, const CCPoint& point)
{
	if(point.x >= 0 && point.x <= rect.size.width && point.y >= 0 && point.y <= rect.size.height)
		return true ;
	return false ;
}

void EFEffectsLayer::hitTouchEffectSprite(const CCPoint& worldPoint, NXCCSprite** returnParent, NXCCSprite** returnChildren)
{
	for(EFSpritesList::reverse_iterator it = _spritesList.rbegin();
			it != _spritesList.rend();
			it ++)
	{
		NXCCSprite* effectSprite = *it ;

		NXCCSprite* closeSprite = (NXCCSprite*)effectSprite->getContext5() ;
		NXCCSprite* rotateSprite = (NXCCSprite*)effectSprite->getContext6() ;
		NXCCSprite* scaleSprite = (NXCCSprite*)effectSprite->getContext7() ;

		if(isWorldPointInSprite(worldPoint, closeSprite))
		{
			*returnParent = effectSprite ;
			*returnChildren = closeSprite ;
			return ;
		}
		else if(isWorldPointInSprite(worldPoint, rotateSprite))
		{
			*returnParent = effectSprite ;
			*returnChildren = rotateSprite ;
			return ;
		}
		else if(isWorldPointInSprite(worldPoint, scaleSprite))
		{
			*returnParent = effectSprite ;
			*returnChildren = scaleSprite ;
			return ;
		}
		else if(isWorldPointInSprite(worldPoint, effectSprite))
		{
			*returnParent = effectSprite ;
			*returnChildren = NULL ;
			return ;
		}
	}

	NXCCSprite* trackScale = (NXCCSprite*)_trackSprite->getContext5() ;
	if(_trackSprite->isVisible() && isWorldPointInSprite(worldPoint, trackScale))
	{
		*returnParent = _trackSprite ;
		*returnChildren = trackScale ;
		return ;
	}
	if(_trackSprite->isVisible() && isWorldPointInSprite(worldPoint, _trackSprite))
	{
		*returnParent = _trackSprite ;
		*returnChildren = NULL ;
		return ;
	}

	*returnParent = NULL ;
	*returnChildren = NULL ;
	return ;
}

bool EFEffectsLayer::isWorldPointInSprite(const CCPoint& worldPoint, NXCCSprite* sprite)
{
	return isRectContainPoint(sprite->getRectToSelf(), sprite->convertToNodeSpace(worldPoint)) ;
}

void EFEffectsLayer::updateLayerTouch(int type, float x, float y)
{
	float posx = m_obContentSize.width * x ;
	float posy = m_obContentSize.height * y ;

	CCPoint touchPoint = ccp(posx, m_obContentSize.height-posy) ;

	switch(type)
	{
		case EDITOR_WORKER_TOUCH_DOWN:
		{
			_touchEffect = NULL ;
			_touchButton = NULL ;

			hitTouchEffectSprite(touchPoint, &_touchEffect, &_touchButton);

			if(_touchEffect)
				_touchPoint = touchPoint ;

			break ;
		}
		case EDITOR_WORKER_TOUCH_MOVE:
		{
			if(_touchEffect)
			{
				EFEffectData* effectData = (EFEffectData*)_touchEffect->getContext9() ;
				if(_touchEffect->getContext2() == 3000) //effect sprite
				{
					if(_touchButton == _touchEffect->getContext6()) //touch rotate sprite !!
					{
						CCPoint ceterNode = _touchEffect->getPosition() ;
						CCPoint vectrYaxi = ccp(0, 1.0f) ;
						CCPoint _vectorTouch = ccp(_touchPoint.x-ceterNode.x, _touchPoint.y-ceterNode.y) ;
						CCPoint vectorTouch = ccp(touchPoint.x-ceterNode.x, touchPoint.y-ceterNode.y) ;

						float _radian = ccpAngle(_vectorTouch, vectrYaxi);
						float radian = ccpAngle(vectorTouch, vectrYaxi);

						if(_vectorTouch.x < 0)
							_radian = 2*M_PI - _radian ;
						if(vectorTouch.x < 0)
							radian = 2*M_PI - radian ;

						effectData->effectRotate += (radian-_radian)/M_PI*180.0f ;
						_touchEffect->setRotation(effectData->effectRotate) ;
					}
					else if(_touchButton == _touchEffect->getContext7()) //touch scale sprite !!
					{
						CCPoint ceterNode = _touchEffect->getPosition() ;
						float _lineLength = sqrt(pow(_touchPoint.x-ceterNode.x, 2) + pow(_touchPoint.y-ceterNode.y, 2));
						float lineLength = sqrt(pow(touchPoint.x-ceterNode.x, 2) + pow(touchPoint.y-ceterNode.y, 2));
						float oldScale = effectData->effectScale ;
						float newScale = oldScale+oldScale*(lineLength-_lineLength)/_lineLength ;

						effectData->effectScale = MAX(0.5f, newScale) ;
						_touchEffect->setScale(effectData->effectScale) ;

						NXCCSprite* btnSprite1 = (NXCCSprite*)_touchEffect->getContext5() ;
						NXCCSprite* btnSprite2 = (NXCCSprite*)_touchEffect->getContext6() ;
						NXCCSprite* btnSprite3 = (NXCCSprite*)_touchEffect->getContext7() ;

						btnSprite1->setScale(1.0f/effectData->effectScale) ;
						btnSprite2->setScale(1.0f/effectData->effectScale) ;
						btnSprite3->setScale(1.0f/effectData->effectScale) ;
					}
					else if(_touchButton == NULL) //touch effect sprite !!!
					{
						CCPoint offset = touchPoint - _touchPoint ;
						effectData->effectOffset = NXPointMake(effectData->effectOffset.x+offset.x, effectData->effectOffset.y+offset.y) ;

						CCPoint position = _touchEffect->getPosition() ;
						position = position + offset ;
						_touchEffect->setPosition(position) ;
					}
				}
				else if(_touchEffect->getContext2() == 1000)
				{
					if(_touchButton == _trackSprite->getContext5())
					{
						float ofx = touchPoint.x-_touchPoint.x ;
						float ofy = touchPoint.y-_touchPoint.y ;
						CCSize size = _trackSprite->getContentSize() ;
						size.width  += 2*ofx ;
						size.height -= 2*ofy ;
						size.width = MAX(size.width, m_obContentSize.width/10) ;
						size.height = MAX(size.height, m_obContentSize.height/10) ;
						_trackSprite->setTextureRect(CCRectMake(0,0,size.width, size.height)) ;
						_touchButton->setPosition(ccp(size.width, 0)) ;
					}
					else if(_touchButton == NULL)
					{
						CCPoint offset = touchPoint - _touchPoint ;
						CCPoint position = _trackSprite->getPosition() ;
						position = position + offset ;
						_trackSprite->setPosition(position) ;
					}
				}
			}
			else
			{

			}
			_touchPoint = touchPoint ;
			break ;

		case EDITOR_WORKER_TOUCH_UP:
		{
			if(_touchEffect)
			{
				EFEffectData* effectData = (EFEffectData*)_touchEffect->getContext9() ;
				if(_touchEffect->getContext2() == 3000
						&& _touchButton == _touchEffect->getContext5()
						&& isWorldPointInSprite(touchPoint, _touchButton)) //touch close sprite !!!
				{
					if(_layerLister)
						_layerLister->onTouchEffectClose(this, _touchEffect->getContext1()) ;

					_touchEffect = NULL ;
					_touchButton = NULL ;
				}

				else if(_touchEffect->getContext2() == 1000)
				{
					const CCSize& size = _touchEffect->getContentSize() ;
					const CCPoint& point = _touchEffect->getPosition() ;

					NXRect trackRect = NXRectMake((point.x-size.width/2)/m_obContentSize.width, 1.0f-(point.y+size.height/2)/m_obContentSize.height, size.width/m_obContentSize.width, size.height/m_obContentSize.height) ;
					_effectModel->setFirstFrame(_playerIndex, _playerImage, trackRect) ;
					_effectModel->cleanTracksAfter(_playerIndex) ;

					__android_log_print(ANDROID_LOG_DEBUG, "WORKER", "frame=%u track first x=%.2f, y=%.2f, w=%.2f, h=%.2f",_playerIndex, trackRect.x, trackRect.y, trackRect.w, trackRect.h);
				}
			}

			_touchEffect = NULL ;
			_touchButton = NULL ;

			break ;
		}
		}
	}
}
