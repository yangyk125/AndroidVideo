/*
 * NXCCSpriteNV12.h
 *
 *  Created on: 2015-11-3
 *      Author: yangyk
 */

#pragma once

#include "cocos2d.h"

#include "NXCCSprite.h"

#include "../NXKT/NXPtr.hpp"
#include "../NXKT/NXBuffer.h"


class NXCCSpriteNV12 : public NXCCSprite
{
private:
	CCSize				_cc2dContent ;
	CCSize				_cc2dTexture ;
	CCTexture2D*		_cc2dTextureY  ;
	CCTexture2D*		_cc2dTextureUV ;
public:
	NXCCSpriteNV12();
	virtual ~NXCCSpriteNV12();
public:
    virtual bool init(bool isNv12, const CCSize& contentSize, const CCSize& textureSize, const CCRect& cuttedRect = CCRectMake(0,0,1,1)) ;
	virtual void draw(void) ;
public:
	void updateImateData(NXPtr<NXBuffer>& buffer) ;
};

