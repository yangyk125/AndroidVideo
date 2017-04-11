/*
 * NXCCSpriteNV12.cpp
 *
 *  Created on: 2015-11-3
 *      Author: yangyk
 */

#include "NXCCSpriteNV12.h"

NXCCSpriteNV12::NXCCSpriteNV12()
{
	_cc2dTextureY  = NULL ;
	_cc2dTextureUV = NULL ;
}

NXCCSpriteNV12::~NXCCSpriteNV12()
{
	if(_cc2dTextureY)
	{
		delete _cc2dTextureY ;
		_cc2dTextureY = NULL ;
	}

	if(_cc2dTextureUV)
	{
		delete _cc2dTextureUV ;
		_cc2dTextureUV = NULL ;
	}
}

// designated initializer
bool NXCCSpriteNV12::init(bool isNv12, const CCSize& contentSize, const CCSize& textureSize, const CCRect& cuttedRect)
{
    if (NXCCSprite::init())
    {
    	_cc2dContent = contentSize ;
    	_cc2dTexture = textureSize ;

        m_pobBatchNode = NULL;

        m_bRecursiveDirty = false;
        setDirty(false);

        m_bOpacityModifyRGB = true;

        m_sBlendFunc.src = CC_BLEND_SRC;
        m_sBlendFunc.dst = CC_BLEND_DST;

        m_bFlipX = m_bFlipY = false;

        // default transform anchor: center
        setAnchorPoint(ccp(0.5f, 0.5f));

        // zwoptex default values
        m_obOffsetPosition = CCPointZero;

        m_bHasChildren = false;

        // clean the Quad
        memset(&m_sQuad, 0, sizeof(m_sQuad));

        // Atlas: Color
        ccColor4B tmpColor = { 255, 255, 255, 255 };
        m_sQuad.bl.colors = tmpColor;
        m_sQuad.br.colors = tmpColor;
        m_sQuad.tl.colors = tmpColor;
        m_sQuad.tr.colors = tmpColor;

        // shader program
        if(isNv12)
        	CCSprite::setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColorNV12));
        else
        	CCSprite::setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColorNV21));

        CCSprite::setTexture(NULL) ;
        CCSprite::setTextureRect(CCRectMake(0, 0, _cc2dContent.width, _cc2dContent.height)) ;
        CCSprite::setBatchNode(NULL);

        m_sQuad.bl.vertices = (ccVertex3F) { 0, 0, 0 };
        m_sQuad.br.vertices = (ccVertex3F) { _cc2dContent.width, 0, 0 };
        m_sQuad.tl.vertices = (ccVertex3F) { 0, _cc2dContent.height, 0 };
        m_sQuad.tr.vertices = (ccVertex3F) { _cc2dContent.width, _cc2dContent.height, 0};

        m_sQuad.bl.texCoords = (ccTex2F){cuttedRect.origin.x,cuttedRect.origin.y+cuttedRect.size.height};
        m_sQuad.br.texCoords = (ccTex2F){cuttedRect.origin.x+cuttedRect.size.width,cuttedRect.origin.y+cuttedRect.size.height};
        m_sQuad.tl.texCoords = (ccTex2F){cuttedRect.origin.x,cuttedRect.origin.y};
        m_sQuad.tr.texCoords = (ccTex2F){cuttedRect.origin.x+cuttedRect.size.width,cuttedRect.origin.y};

        _cc2dTextureY = new CCTexture2D();
        _cc2dTextureY->initWithFormat(kCCTexture2DPixelFormat_I8, _cc2dTexture.width, _cc2dTexture.height, _cc2dTexture) ;

        _cc2dTextureUV = new CCTexture2D();
        _cc2dTextureUV->initWithFormat(kCCTexture2DPixelFormat_AI88, _cc2dTexture.width/2, _cc2dTexture.height/2, ccsz(_cc2dTexture.width/2, _cc2dTexture.height/2)) ;

        return true;
    }
    else
    {
        return false;
    }
}

void NXCCSpriteNV12::draw(void)
{
    CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "NXCCSpriteNV12 - draw");

    CCAssert(!m_pobBatchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");

    ccGLEnable(m_eGLServerState);

    CCAssert(m_pShaderProgram, "No shader program set for this node");

    m_pShaderProgram->use();
    m_pShaderProgram->setUniformsForBuiltins();

    ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );

    ccGLBindTexture2DN(0, _cc2dTextureY->getName()) ;
    ccGLBindTexture2DN(1, _cc2dTextureUV->getName()) ;

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );

#define kQuadSize sizeof(m_sQuad.bl)
#ifdef EMSCRIPTEN
    long offset = 0;
    setGLBufferData(&m_sQuad, 4 * kQuadSize, 0);
#else
    long offset = (long)&m_sQuad;
#endif // EMSCRIPTEN

    // vertex
    int diff = offsetof( ccV3F_C4B_T2F, vertices);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

    // texCoods
    diff = offsetof( ccV3F_C4B_T2F, texCoords);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

    // color
    diff = offsetof( ccV3F_C4B_T2F, colors);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));


    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    CHECK_GL_ERROR_DEBUG();


#if CC_SPRITE_DEBUG_DRAW == 1
    // draw bounding box
    CCPoint vertices[4]={
        ccp(m_sQuad.tl.vertices.x,m_sQuad.tl.vertices.y),
        ccp(m_sQuad.bl.vertices.x,m_sQuad.bl.vertices.y),
        ccp(m_sQuad.br.vertices.x,m_sQuad.br.vertices.y),
        ccp(m_sQuad.tr.vertices.x,m_sQuad.tr.vertices.y),
    };
    ccDrawPoly(vertices, 4, true);
#elif CC_SPRITE_DEBUG_DRAW == 2
    // draw texture box
    CCSize s = this->getTextureRect().size;
    CCPoint offsetPix = this->getOffsetPosition();
    CCPoint vertices[4] = {
        ccp(offsetPix.x,offsetPix.y), ccp(offsetPix.x+s.width,offsetPix.y),
        ccp(offsetPix.x+s.width,offsetPix.y+s.height), ccp(offsetPix.x,offsetPix.y+s.height)
    };
    ccDrawPoly(vertices, 4, true);
#endif // CC_SPRITE_DEBUG_DRAW

    CC_INCREMENT_GL_DRAWS(1);

    CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "NXCCSpriteNV12 - draw");
}

void NXCCSpriteNV12::updateImateData(NXPtr<NXBuffer>& buffer)
{
	uint8_t* bufferY = buffer->getPlanePtr(0) ;
	uint8_t* bufferUV = buffer->getPlanePtr(1) ;

	_cc2dTextureY->updateTextureData(_cc2dTexture.width, _cc2dTexture.height, bufferY) ;
	_cc2dTextureUV->updateTextureData(_cc2dTexture.width/2, _cc2dTexture.height/2, bufferUV) ;
}
