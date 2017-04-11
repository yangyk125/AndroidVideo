/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

// standard includes
#include <string>

// cocos2d includes
#include "CCDirector.h"
#include "draw_nodes/CCDrawingPrimitives.h"
#include "CCConfiguration.h"
#include "cocoa/CCNS.h"
#include "scene_node/CCScene.h"
#include "cocoa/CCArray.h"
#include "CCScheduler.h"
#include "ccMacros.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "support/CCPointExtension.h"
#include "support/CCNotificationCenter.h"
#include "textures/CCTextureCache.h"
#include "sprite_nodes/CCSpriteFrameCache.h"
#include "cocoa/CCAutoreleasePool.h"
#include "platform/platform.h"
#include "platform/CCFileUtils.h"
#include "CCApplication.h"
#include "label_nodes/CCLabelBMFont.h"
#include "label_nodes/CCLabelAtlas.h"
#include "actions/CCActionManager.h"
#include "CCConfiguration.h"
#include "keypad_dispatcher/CCKeypadDispatcher.h"
#include "CCAccelerometer.h"
#include "sprite_nodes/CCAnimationCache.h"
#include "touch_dispatcher/CCTouch.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/CCShaderCache.h"
#include "kazmath/kazmath.h"
#include "kazmath/GL/matrix.h"
#include "support/CCProfiling.h"
#include "platform/CCImage.h"
#include "CCEGLView.h"
#include "CCConfiguration.h"



/**
 Position of the FPS
 
 Default: 0,0 (bottom-left corner)
 */
#ifndef CC_DIRECTOR_STATS_POSITION
#define CC_DIRECTOR_STATS_POSITION CCDirector::sharedDirector()->getVisibleOrigin()
#endif

using namespace std;

unsigned int g_uNumberOfDraws = 0;

NS_CC_BEGIN
// XXX it should be a Director ivar. Move it there once support for multiple directors is added

// singleton stuff
#define kDefaultFPS        30  // 60 frames per second

CCContext::~CCContext()
{

}

CCDirector::CCDirector(void)
{
	init();
}

bool CCDirector::init(void)
{
	// default FPS
	m_dOldAnimationInterval = m_dAnimationInterval = 1.0 / kDefaultFPS ;

	// GL projection
	m_eProjection = kCCDirectorProjection2D ;

	// Default pixel format for PNG images with alpha
	CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888) ;
	CCTexture2D::PVRImagesHavePremultipliedAlpha(true);

	m_pobOpenGLContext = NULL ;
    // scenes
    m_pRunningScene = NULL;

    m_pNotificationNode = NULL;

    // FPS
    m_fAccumDt = 0.0f;
    m_fFrameRate = 0.0f;
    m_uTotalFrames = m_uFrames = 0;
    m_pszFPS = new char[10];
    m_pLastUpdate = new struct cc_timeval();

    // paused ?
    m_bPaused = false;

    m_obWinSizeInPoints = CCSizeZero;    

    // scheduler
    m_pScheduler = new CCScheduler();
    // action manager
    m_pActionManager = new CCActionManager();
    m_pScheduler->scheduleUpdateForTarget(m_pActionManager, kCCPrioritySystem, false);
    // touchDispatcher
    m_pTouchDispatcher = new CCTouchDispatcher();
    m_pTouchDispatcher->init();

    // KeypadDispatcher
    m_pKeypadDispatcher = new CCKeypadDispatcher();

    // Accelerometer
    m_pAccelerometer = new CCAccelerometer();

    return true;
}
    
CCDirector::~CCDirector(void)
{
    CCLOG("cocos2d: deallocing CCDirector %p", this);

    CC_SAFE_RELEASE(m_pRunningScene);
    CC_SAFE_RELEASE(m_pNotificationNode);
    CC_SAFE_RELEASE(m_pScheduler);
    CC_SAFE_RELEASE(m_pActionManager);
    CC_SAFE_RELEASE(m_pTouchDispatcher);
    CC_SAFE_RELEASE(m_pKeypadDispatcher);
    CC_SAFE_DELETE(m_pAccelerometer);

    // delete m_pLastUpdate
    CC_SAFE_DELETE(m_pLastUpdate);
    // delete fps string
    delete []m_pszFPS;
}

void CCDirector::updateTime(float time)
{
	m_pScheduler->update(time);
}

// Draw the Scene
void CCDirector::drawScene(void)
{
	CCAssert(m_pobOpenGLContext != NULL, "Invalid OpenGL Context Value !!");

	if(m_pobOpenGLContext)
	{
		m_pobOpenGLContext->setCCCurrentContext() ;
	    m_pobOpenGLContext->setCCViewport(0, 0, m_obWinSizeInPoints.width, m_obWinSizeInPoints.height) ;
	}

    // calculate "global" dt
    calculateDeltaTime();

    //tick before glClear: issue #533
    if (! m_bPaused)
    {
    	updateTime(m_fDeltaTime);
    }

    renderScene(false) ;

    m_uTotalFrames++;
}

void CCDirector::renderScene(bool finish)
{
	//alpha mode
	setAlphaBlending(true);

	//project matrix
	updateProjection() ;

	//disable depth test
	setDepthTest(false);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);

    kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLLoadIdentity();
    kmGLPushMatrix();

    // draw the scene
    if (m_pRunningScene)
    {
        m_pRunningScene->visit();
    }

    // draw the notifications node
    if (m_pNotificationNode)
    {
        m_pNotificationNode->visit();
    }
    
    kmGLPopMatrix();

    if(finish)
    	glFinish();
    else
    	glFlush();

    // swap buffers
    if(m_pobOpenGLContext)
    {
    	m_pobOpenGLContext->swapCCBuffers();
    }
}

void CCDirector::releaseObjects()
{
	CCAutoreleasePool::sharePool()->loopOnce() ;
}

void CCDirector::calculateDeltaTime(void)
{
    struct cc_timeval now;

    if (CCTime::gettimeofdayCocos2d(&now, NULL) != 0)
    {
        CCLOG("error in gettimeofday");
        m_fDeltaTime = 0;
        return;
    }

    // new delta time. Re-fixed issue #1277
    if (m_bNextDeltaTimeZero)
    {
        m_fDeltaTime = 0;
        m_bNextDeltaTimeZero = false;
    }
    else
    {
        m_fDeltaTime = (now.tv_sec - m_pLastUpdate->tv_sec) + (now.tv_usec - m_pLastUpdate->tv_usec) / 1000000.0f;
        m_fDeltaTime = MAX(0, m_fDeltaTime);
    }

#ifdef DEBUG
    // If we are debugging our code, prevent big delta time
    if(m_fDeltaTime > 0.2f)
    {
        m_fDeltaTime = 1 / 60.0f;
    }
#endif

    *m_pLastUpdate = now;
}
float CCDirector::getDeltaTime()
{
	return m_fDeltaTime;
}

void CCDirector::setNextDeltaTimeZero(bool bNextDeltaTimeZero)
{
    m_bNextDeltaTimeZero = bNextDeltaTimeZero;
}

void CCDirector::setProjection(ccDirectorProjection kProjection)
{
    m_eProjection = kProjection;
}

void CCDirector::updateProjection()
{
    CCSize size = m_obWinSizeInPoints;

    switch (m_eProjection)
    {
    case kCCDirectorProjection2D:
        {
            kmGLMatrixMode(KM_GL_PROJECTION);
            kmGLLoadIdentity();
            kmMat4 orthoMatrix;
            kmMat4OrthographicProjection(&orthoMatrix, 0, size.width, 0, size.height, -1024, 1024 );
            kmGLMultMatrix(&orthoMatrix);
            kmGLMatrixMode(KM_GL_MODELVIEW);
            kmGLLoadIdentity();
        }
        break;

    case kCCDirectorProjection3D:
        {
            float zeye = this->getZEye();

            kmMat4 matrixPerspective, matrixLookup;

            kmGLMatrixMode(KM_GL_PROJECTION);
            kmGLLoadIdentity();
            
            // issue #1334
            kmMat4PerspectiveProjection( &matrixPerspective, 60, (GLfloat)size.width/size.height, 0.1f, zeye*2);
            // kmMat4PerspectiveProjection( &matrixPerspective, 60, (GLfloat)size.width/size.height, 0.1f, 1500);

            kmGLMultMatrix(&matrixPerspective);

            kmGLMatrixMode(KM_GL_MODELVIEW);
            kmGLLoadIdentity();
            kmVec3 eye, center, up;
            kmVec3Fill( &eye, size.width/2, size.height/2, zeye );
            kmVec3Fill( &center, size.width/2, size.height/2, 0.0f );
            kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
            kmMat4LookAt(&matrixLookup, &eye, &center, &up);
            kmGLMultMatrix(&matrixLookup);
        }
        break;
            
    case kCCDirectorProjectionCustom:
    default:
        CCLOG("cocos2d: Director: unrecognized projection");
        break;
    }
}

void CCDirector::purgeCachedData(void)
{
    CCLabelBMFont::purgeCachedData();
    CCFileUtils::sharedFileUtils()->purgeCachedEntries();
}

float CCDirector::getZEye(void)
{
    return (m_obWinSizeInPoints.height / 1.1566f);
}

void CCDirector::setAlphaBlending(bool bOn)
{
    if (bOn)
    {
        ccGLBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
    }
    else
    {
        ccGLBlendFunc(GL_ONE, GL_ZERO);
    }

    CHECK_GL_ERROR_DEBUG();
}

void CCDirector::setDepthTest(bool bOn)
{
    if (bOn)
    {
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
//        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    CHECK_GL_ERROR_DEBUG();
}

/*
static void GLToClipTransform(kmMat4 *transformOut)
{
	kmMat4 projection;
	kmGLGetMatrix(KM_GL_PROJECTION, &projection);

	kmMat4 modelview;
	kmGLGetMatrix(KM_GL_MODELVIEW, &modelview);
	
	kmMat4Multiply(transformOut, &projection, &modelview);
}
*/

CCPoint CCDirector::convertToGL(const CCPoint& uiPoint)
{
	/*
    kmMat4 transform;
	GLToClipTransform(&transform);
	
	kmMat4 transformInv;
	kmMat4Inverse(&transformInv, &transform);
	
	// Calculate z=0 using -> transform*[0, 0, 0, 1]/w
	kmScalar zClip = transform.mat[14]/transform.mat[15];
	
    CCSize glSize = m_pobOpenGLView->getDesignResolutionSize();
	kmVec3 clipCoord = {2.0f*uiPoint.x/glSize.width - 1.0f, 1.0f - 2.0f*uiPoint.y/glSize.height, zClip};
	
	kmVec3 glCoord;
	kmVec3TransformCoord(&glCoord, &clipCoord, &transformInv);
	
	return ccp(glCoord.x, glCoord.y);
	*/
	return ccp(0,0) ;
}

CCPoint CCDirector::convertToUI(const CCPoint& glPoint)
{
	/*
    kmMat4 transform;
	GLToClipTransform(&transform);
    
	kmVec3 clipCoord;
	// Need to calculate the zero depth from the transform.
	kmVec3 glCoord = {glPoint.x, glPoint.y, 0.0};
	kmVec3TransformCoord(&clipCoord, &glCoord, &transform);
	
	CCSize glSize = m_pobOpenGLView->getDesignResolutionSize();
	return ccp(glSize.width*(clipCoord.x*0.5 + 0.5), glSize.height*(-clipCoord.y*0.5 + 0.5));
	*/

	return ccp(0, 0) ;
}

CCSize CCDirector::getWinSize(void)
{
    return m_obWinSizeInPoints ;
}

CCSize CCDirector::getWinSizeInPixels()
{
    return m_obWinSizeInPoints ;
}

CCSize CCDirector::getVisibleSize()
{
    if (m_pobOpenGLContext)
    {
        return m_pobOpenGLContext->getCCWindowSize() ;
    }
    else 
    {
        return CCSizeZero;
    }
}

CCPoint CCDirector::getVisibleOrigin()
{
    if (m_pobOpenGLContext)
    {
        return m_pobOpenGLContext->getCCWindowOrigin() ;
    }
    else 
    {
        return CCPointZero;
    }
}

// scene management
void CCDirector::makeRunningScene(void)
{
	getRunningScene();
}

CCScene* CCDirector::getRunningScene(void)
{
	if(m_pRunningScene == NULL)
		m_pRunningScene = new CCScene();
	m_pRunningScene->setContentSize(m_obWinSizeInPoints) ;
	return m_pRunningScene ;
}

void CCDirector::setWithScene(CCScene *pScene)
{
    CCAssert(pScene != NULL, "This command can only be used to start the CCDirector. There is already a scene present.");
    CCAssert(m_pRunningScene == NULL, "m_pRunningScene should be null");
    m_pRunningScene = pScene ;
}

void CCDirector::end()
{

}

void CCDirector::purgeDirector()
{
    // cleanup scheduler
    getScheduler()->unscheduleAll();
    
    // don't release the event handlers
    // They are needed in case the director is run again
    m_pTouchDispatcher->removeAllDelegates();

    if (m_pRunningScene)
    {
        m_pRunningScene->onExitTransitionDidStart();
        m_pRunningScene->onExit();
        m_pRunningScene->cleanup();
        m_pRunningScene->release();
        m_pRunningScene = NULL;
    }

    stopAnimation();

    // purge bitmap cache
    CCLabelBMFont::purgeCachedData();

    // purge all managed caches
    ccDrawFree();
    CCAnimationCache::purgeSharedAnimationCache();
    CCSpriteFrameCache::purgeSharedSpriteFrameCache();
    CCTextureCache::purgeSharedTextureCache();
    CCShaderCache::purgeSharedShaderCache();
    CCFileUtils::purgeFileUtils();
    CCConfiguration::purgeConfiguration();

    // cocos2d-x specific data structures
    CCNotificationCenter::purgeNotificationCenter();

    ccGLInvalidateStateCache();
    
    CHECK_GL_ERROR_DEBUG();
    
    // OpenGL view
    m_pobOpenGLContext = NULL;

    // delete CCDirector
    release();
}

void CCDirector::pause(void)
{
    if (m_bPaused)
    {
        return;
    }

    m_dOldAnimationInterval = m_dAnimationInterval;

    // when paused, don't consume CPU
    setAnimationInterval(1 / 4.0);
    m_bPaused = true;
}

void CCDirector::resume(void)
{
    if (! m_bPaused)
    {
        return;
    }

    setAnimationInterval(m_dOldAnimationInterval);

    if (CCTime::gettimeofdayCocos2d(m_pLastUpdate, NULL) != 0)
    {
        CCLOG("cocos2d: Director: Error in gettimeofday");
    }

    m_bPaused = false;
    m_fDeltaTime = 0;
}

void CCDirector::calculateMPF()
{

}

CCNode* CCDirector::getNotificationNode() 
{ 
    return m_pNotificationNode; 
}

void CCDirector::setNotificationNode(CCNode *node)
{
    CC_SAFE_RELEASE(m_pNotificationNode);
    m_pNotificationNode = node;
    CC_SAFE_RETAIN(m_pNotificationNode);
}

void CCDirector::setScheduler(CCScheduler* pScheduler)
{
    if (m_pScheduler != pScheduler)
    {
        CC_SAFE_RETAIN(pScheduler);
        CC_SAFE_RELEASE(m_pScheduler);
        m_pScheduler = pScheduler;
    }
}

CCScheduler* CCDirector::getScheduler()
{
    return m_pScheduler;
}

void CCDirector::setActionManager(CCActionManager* pActionManager)
{
    if (m_pActionManager != pActionManager)
    {
        CC_SAFE_RETAIN(pActionManager);
        CC_SAFE_RELEASE(m_pActionManager);
        m_pActionManager = pActionManager;
    }    
}

CCActionManager* CCDirector::getActionManager()
{
    return m_pActionManager;
}

void CCDirector::setTouchDispatcher(CCTouchDispatcher* pTouchDispatcher)
{
    if (m_pTouchDispatcher != pTouchDispatcher)
    {
        CC_SAFE_RETAIN(pTouchDispatcher);
        CC_SAFE_RELEASE(m_pTouchDispatcher);
        m_pTouchDispatcher = pTouchDispatcher;
    }    
}

CCTouchDispatcher* CCDirector::getTouchDispatcher()
{
    return m_pTouchDispatcher;
}

void CCDirector::setKeypadDispatcher(CCKeypadDispatcher* pKeypadDispatcher)
{
    CC_SAFE_RETAIN(pKeypadDispatcher);
    CC_SAFE_RELEASE(m_pKeypadDispatcher);
    m_pKeypadDispatcher = pKeypadDispatcher;
}

CCKeypadDispatcher* CCDirector::getKeypadDispatcher()
{
    return m_pKeypadDispatcher;
}

void CCDirector::setAccelerometer(CCAccelerometer* pAccelerometer)
{
    if (m_pAccelerometer != pAccelerometer)
    {
        CC_SAFE_DELETE(m_pAccelerometer);
        m_pAccelerometer = pAccelerometer;
    }
}

CCAccelerometer* CCDirector::getAccelerometer()
{
    return m_pAccelerometer;
}

void CCDirector::setCCContext(CCContext* glContext)
{
	m_pobOpenGLContext = glContext ;
	if(m_pobOpenGLContext)
		m_pobOpenGLContext->setCCCurrentContext() ;
}

void CCDirector::drawSurfaceChanged()
{
	if(m_pobOpenGLContext)
	{
		m_obWinPointInPoints = m_pobOpenGLContext->getCCWindowOrigin() ;
		m_obWinSizeInPoints = m_pobOpenGLContext->getCCWindowSize() ;
	}

	if(m_pRunningScene)
		m_pRunningScene->setContentSize(m_obWinSizeInPoints) ;
}

NS_CC_END

//must implement by user
extern cocos2d::CCDirector* CCGetGlobalDirector() ;

NS_CC_BEGIN

CCDirector* CCDirector::sharedDirector()
{
	return CCGetGlobalDirector() ;
}

NS_CC_END

