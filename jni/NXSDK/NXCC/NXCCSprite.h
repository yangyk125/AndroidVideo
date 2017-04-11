/*
 * NXCCSprite.h
 *
 *  Created on: 2015-10-28
 *      Author: yangyk
 */

#ifndef NXCCSPRITE_H_
#define NXCCSPRITE_H_

#include "cocos2d.h"

using namespace cocos2d ;

class NXCCSprite: public cocos2d::CCSprite
{
protected:
	CC_SYNTHESIZE(bool, m_bDrawBounds, DrawBounds) ;
	CC_SYNTHESIZE(bool, m_bDrawCorner, DrawCorner) ;
	CC_SYNTHESIZE(uint32_t, m_bCornerLength, CornerLength) ;
	CC_SYNTHESIZE(uint32_t, m_bCornerWeight, CornerWeight) ;

	CC_SYNTHESIZE(uint32_t, m_context1, Context1) ;
	CC_SYNTHESIZE(uint64_t, m_context2, Context2) ;
	CC_SYNTHESIZE(float,    m_context3, Context3) ;
	CC_SYNTHESIZE(double,   m_context4, Context4) ;
	CC_SYNTHESIZE(void*,    m_context5, Context5) ;
	CC_SYNTHESIZE(void*,    m_context6, Context6) ;
	CC_SYNTHESIZE(void*,    m_context7, Context7) ;
	CC_SYNTHESIZE(void*,    m_context8, Context8) ;
	CC_SYNTHESIZE(void*,    m_context9, Context9) ;
public:
	NXCCSprite() ;
	virtual ~NXCCSprite() ;
public:
	virtual void draw(void) ;
	virtual void setTextureRect(const CCRect& rect) ;
};

#endif /* NXCCSPRITE_H_ */
