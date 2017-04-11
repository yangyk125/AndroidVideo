/*
 * NXCCSprite.cpp
 *
 *  Created on: 2015-10-28
 *      Author: yangyk
 */

#include "NXCCSprite.h"

NXCCSprite::NXCCSprite()
{
	m_bDrawBounds = false ;
	m_bDrawCorner = false ;
	m_bCornerLength = 10 ;
	m_bCornerWeight = 3 ;
	m_context1 = 0 ;
	m_context2 = 0 ;
	m_context3 = 0 ;
	m_context4 = 0 ;
	m_context5 = 0 ;
	m_context6 = 0 ;
	m_context7 = 0 ;
	m_context8 = 0 ;
	m_context9 = 0 ;

}

NXCCSprite::~NXCCSprite()
{

}

void NXCCSprite::draw(void)
{
	CCSprite::draw() ;

	if(m_bDrawBounds)
	{
		CCPoint vertices[4]={
		      ccp(m_sQuad.tl.vertices.x,m_sQuad.tl.vertices.y),
		      ccp(m_sQuad.bl.vertices.x,m_sQuad.bl.vertices.y),
		      ccp(m_sQuad.br.vertices.x,m_sQuad.br.vertices.y),
		      ccp(m_sQuad.tr.vertices.x,m_sQuad.tr.vertices.y),
		};

		ccDrawColor4F(1, 1, 1, 1);
		ccDrawPoly(vertices, 4, true);
	}

    if(m_bDrawCorner)
    {
        ccColor4F color = ccc4f(1, 1, 1, 1) ;

        CCPoint ptlt = ccp(m_sQuad.tl.vertices.x,m_sQuad.tl.vertices.y) ;
        CCPoint ptbl = ccp(m_sQuad.bl.vertices.x,m_sQuad.bl.vertices.y) ;
        CCPoint ptbr = ccp(m_sQuad.br.vertices.x,m_sQuad.br.vertices.y) ;
        CCPoint pttr = ccp(m_sQuad.tr.vertices.x,m_sQuad.tr.vertices.y) ;

        //左上
        ccDrawSolidRect(ccp(ptlt.x+1, ptlt.y-1), ccp(ptlt.x+1+m_bCornerLength, ptlt.y-m_bCornerWeight), color);
        ccDrawSolidRect(ccp(ptlt.x+1, ptlt.y-1), ccp(ptlt.x+m_bCornerWeight, ptlt.y-1-m_bCornerLength), color);

        //左下
        ccDrawSolidRect(ccp(ptbl.x+1, ptbl.y+1), ccp(ptbl.x+1+m_bCornerLength, ptbl.y+m_bCornerWeight), color);
        ccDrawSolidRect(ccp(ptbl.x+1, ptbl.y+1), ccp(ptbl.x+m_bCornerWeight, ptbl.y+1+m_bCornerLength), color);

        //右下角
        ccDrawSolidRect(ccp(ptbr.x-1, ptbr.y+1), ccp(ptbr.x-1-m_bCornerLength, ptbr.y+m_bCornerWeight), color);
        ccDrawSolidRect(ccp(ptbr.x-1, ptbr.y+1), ccp(ptbr.x-m_bCornerWeight, ptbr.y+1+m_bCornerLength), color);

        //右上角
        ccDrawSolidRect(ccp(pttr.x-1, pttr.y-1), ccp(pttr.x-m_bCornerWeight, pttr.y-1-m_bCornerLength), color);
        ccDrawSolidRect(ccp(pttr.x-1, pttr.y-1), ccp(pttr.x-1-m_bCornerLength, pttr.y-m_bCornerWeight), color);
    }
}

void NXCCSprite::setTextureRect(const CCRect& rect)
{
    CCSprite::setTextureRect(rect) ;

    m_sQuad.bl.vertices = (ccVertex3F) { 0, 0, 0 };
    m_sQuad.br.vertices = (ccVertex3F) { rect.size.width, 0, 0 };
    m_sQuad.tl.vertices = (ccVertex3F) { 0, rect.size.height, 0 };
    m_sQuad.tr.vertices = (ccVertex3F) { rect.size.width, rect.size.height, 0};

    m_sQuad.bl.texCoords = (ccTex2F){0,1};
    m_sQuad.br.texCoords = (ccTex2F){1,1};
    m_sQuad.tl.texCoords = (ccTex2F){0,0};
    m_sQuad.tr.texCoords = (ccTex2F){1,0};
}
