/*
 * NXCCShareNode.h
 *
 *  Created on: 2015-11-19
 *      Author: yangyk
 */

#pragma once

#include "cocos2d.h"

using namespace cocos2d ;

class NXCCShareNode: public cocos2d::CCNode
{
public:
	NXCCShareNode();
	virtual ~NXCCShareNode();
public:
	virtual void addChild(CCNode* child);
	virtual void addChild(CCNode* child, int zOrder);
	virtual void addChild(CCNode* child, int zOrder, int tag);
};

