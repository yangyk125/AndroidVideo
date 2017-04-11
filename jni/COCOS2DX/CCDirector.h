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

#ifndef __CCDIRECTOR_H__
#define __CCDIRECTOR_H__

#include "platform/CCPlatformMacros.h"
#include "cocoa/CCObject.h"
#include "ccTypes.h"
#include "cocoa/CCGeometry.h"
#include "cocoa/CCArray.h"
#include "CCGL.h"
#include "kazmath/mat4.h"
#include "label_nodes/CCLabelAtlas.h"
#include "ccTypeInfo.h"


NS_CC_BEGIN

/**
 * @addtogroup base_nodes
 * @{
 */

/** @typedef ccDirectorProjection
 Possible OpenGL projections used by director
 */
typedef enum {
    /// sets a 2D projection (orthogonal projection)
    kCCDirectorProjection2D,
    
    /// sets a 3D projection with a fovy=60, znear=0.5f and zfar=1500.
    kCCDirectorProjection3D,
    
    /// it calls "updateProjection" on the projection delegate.
    kCCDirectorProjectionCustom,
    
    /// Default projection is 3D projection
    kCCDirectorProjectionDefault = kCCDirectorProjection3D,
} ccDirectorProjection;

/* Forward declarations. */
class CCLabelAtlas;
class CCScene;
class CCNode;
class CCScheduler;
class CCActionManager;
class CCTouchDispatcher;
class CCKeypadDispatcher;
class CCAccelerometer;

/**
@brief Class that creates and handle the main Window and manages how
and when to execute the Scenes.
 
 The CCDirector is also responsible for:
  - initializing the OpenGL context
  - setting the OpenGL pixel format (default on is RGB565)
  - setting the OpenGL buffer depth (default one is 0-bit)
  - setting the projection (default one is 3D)
  - setting the orientation (default one is Portrait)
 
 Since the CCDirector is a singleton, the standard way to use it is by calling:
  _ CCDirector::sharedDirector()->methodName();
 
 The CCDirector also sets the default OpenGL context:
  - GL_TEXTURE_2D is enabled
  - GL_VERTEX_ARRAY is enabled
  - GL_COLOR_ARRAY is enabled
  - GL_TEXTURE_COORD_ARRAY is enabled
*/

extern const char* cocos2dVersion(void);

class CCContext
{
public:
	virtual ~CCContext() ;
public:
	virtual void setCCCurrentContext() = 0;
	virtual void setCCViewport(int x, int y, int w, int h) = 0 ;
	virtual void swapCCBuffers() = 0 ;
	virtual CCPoint getCCWindowOrigin() = 0 ;
	virtual CCSize getCCWindowSize() = 0 ;
};


class CC_DLL CCDirector : public CCObject, public TypeInfo
{
public:
    /**
     *  @js ctor
     */
    CCDirector(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~CCDirector(void);
    virtual bool init(void);
    /**
     * @js NA
     * @lua NA
     */
    virtual long getClassTypeInfo() {
		static const long id = cocos2d::getHashCodeByString(typeid(cocos2d::CCDirector).name());
		return id;
    }
    // attribute

    /** Get the FPS value */
    inline double getAnimationInterval(void) { return m_dAnimationInterval; }
    /** Set the FPS value. */
    virtual void setAnimationInterval(double dValue) = 0;

    /** seconds per frame */
    inline float getSecondsPerFrame() { return m_fSecondsPerFrame; }

    inline bool isNextDeltaTimeZero(void) { return m_bNextDeltaTimeZero; }
    void setNextDeltaTimeZero(bool bNextDeltaTimeZero);

    /** Whether or not the Director is paused */
    inline bool isPaused(void) { return m_bPaused; }

    /** How many frames were called since the director started */
    inline unsigned int getTotalFrames(void) { return m_uTotalFrames; }
    
    /** Sets an OpenGL projection
     @since v0.8.2
     @js NA
     */
    inline ccDirectorProjection getProjection(void) { return m_eProjection; }
    void setProjection(ccDirectorProjection kProjection);
    void updateProjection();
    
    /** How many frames were called since the director started */

    /** This object will be visited after the main scene is visited.
     This object MUST implement the "visit" selector.
     Useful to hook a notification object, like CCNotifications (http://github.com/manucorporat/CCNotifications)
     @since v0.99.5
     */
    CCNode* getNotificationNode();
    void setNotificationNode(CCNode *node);

    // window size

    /** returns the size of the OpenGL view in points.
    */
    CCSize getWinSize(void);

    /** returns the size of the OpenGL view in pixels.
    */
    CCSize getWinSizeInPixels(void);
    
    /** returns visible size of the OpenGL view in points.
     *  the value is equal to getWinSize if don't invoke
     *  CCEGLView::setDesignResolutionSize()
     */
    CCSize getVisibleSize();
    
    /** returns visible origin of the OpenGL view in points.
     */
    CCPoint getVisibleOrigin();

    /** converts a UIKit coordinate to an OpenGL coordinate
     Useful to convert (multi) touch coordinates to the current layout (portrait or landscape)
     */
    CCPoint convertToGL(const CCPoint& obPoint);

    /** converts an OpenGL coordinate to a UIKit coordinate
     Useful to convert node points to window points for calls such as glScissor
     */
    CCPoint convertToUI(const CCPoint& obPoint);

    /// XXX: missing description 
    float getZEye(void);

    void makeRunningScene(void) ;

    // Scene Managemen
    /** Get current running Scene. Director can only run one Scene at the time */
    CCScene* getRunningScene(void) ;

    /** Enters the Director's main loop with the given Scene.
     * Call it to run only your FIRST scene.
     * Don't call it if there is already a running scene.
     *
     * It will call pushScene: and then it will call startAnimation
     */
    void setWithScene(CCScene *pScene);

    /** Ends the execution, releases the running scene.
     It doesn't remove the OpenGL view from its parent. You have to do it manually.
     */
    void end(void);

    /** Pauses the running scene.
     The running scene will be _drawed_ but all scheduled timers will be paused
     While paused, the draw rate will be 4 FPS to reduce CPU consumption
     */
    void pause(void);

    /** Resumes the paused scene
     The scheduled timers will be activated again.
     The "delta time" will be 0 (as if the game wasn't paused)
     */
    void resume(void);

    /** Stops the animation. Nothing will be drawn. The main loop won't be triggered anymore.
     If you don't want to pause your animation call [pause] instead.
     */
    virtual void stopAnimation(void) = 0;

    /** The main loop is triggered again.
     Call this function only if [stopAnimation] was called earlier
     @warning Don't call this function to start the main loop. To run the main loop call runWithScene
     */
    virtual void startAnimation(void) = 0;

    //animation running
    virtual bool isAnimationRuning() = 0;

    //update time
    void updateTime(float time) ;

    /** Draw the scene.
    This method is called every frame. Don't call it manually.
    */
    void drawScene(void);


    //render the scene
    void renderScene(bool finish) ;

    // Memory Helper
    void releaseObjects() ;

    /** Removes cached all cocos2d cached data.
     It will purge the CCTextureCache, CCSpriteFrameCache, CCLabelBMFont cache
     @since v0.99.3
     */
    void purgeCachedData(void);

    // OpenGL Helper

    /** enables/disables OpenGL alpha blending */
    void setAlphaBlending(bool bOn);

    /** enables/disables OpenGL depth test */
    void setDepthTest(bool bOn);

    virtual void mainLoop(void) = 0;

public:
    static CCDirector* sharedDirector() ;


public:
    /** CCScheduler associated with this director
     @since v2.0
     */
    CC_PROPERTY(CCScheduler*, m_pScheduler, Scheduler);

    /** CCActionManager associated with this director
     @since v2.0
     */
    CC_PROPERTY(CCActionManager*, m_pActionManager, ActionManager);

    /** CCTouchDispatcher associated with this director
     @since v2.0
     */
    CC_PROPERTY(CCTouchDispatcher*, m_pTouchDispatcher, TouchDispatcher);

    /** CCKeypadDispatcher associated with this director
     @since v2.0
     */
    CC_PROPERTY(CCKeypadDispatcher*, m_pKeypadDispatcher, KeypadDispatcher);

    /** CCAccelerometer associated with this director
     @since v2.0
     @js NA
     @lua NA
     */
    CC_PROPERTY(CCAccelerometer*, m_pAccelerometer, Accelerometer);

    /* delta time since last tick to main loop */
	CC_PROPERTY_READONLY(float, m_fDeltaTime, DeltaTime);

protected:

    void purgeDirector();
    
    void calculateMPF();
    
    /** calculates delta time since last time it was called */    
    void calculateDeltaTime();
protected:

    CCContext*	m_pobOpenGLContext ;

    double m_dAnimationInterval;
    double m_dOldAnimationInterval;

    /* landscape mode ? */
    bool m_bLandscape;
    
    float m_fAccumDt;
    float m_fFrameRate;

    /** Whether or not the Director is paused */
    bool m_bPaused;

    /* How many frames were called since the director started */
    unsigned int m_uTotalFrames;
    unsigned int m_uFrames;
    float m_fSecondsPerFrame;
     
    /* The running scene */
    CCScene *m_pRunningScene;
    
    /* last time the main loop was updated */
    struct cc_timeval *m_pLastUpdate;

    /* whether or not the next delta time will be zero */
    bool m_bNextDeltaTimeZero;
    
    /* projection used */
    ccDirectorProjection m_eProjection;

    /* window size in points */
    CCPoint   m_obWinPointInPoints ;
    CCSize    m_obWinSizeInPoints;

    /* store the fps string */
    char *m_pszFPS;

    /* This object will be visited after the scene. Useful to hook a notification node */
    CCNode *m_pNotificationNode;

public:
    inline CCContext* getCCContext() {return m_pobOpenGLContext ; } ;
    void setCCContext(CCContext* glContext) ;
    void drawSurfaceChanged();
};

NS_CC_END

#endif // __CCDIRECTOR_H__
