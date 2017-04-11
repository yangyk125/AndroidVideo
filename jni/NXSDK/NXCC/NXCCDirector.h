/*
 * CCAndroidNativeDirector.h
 *
 *  Created on: 2015-10-28
 *      Author: yangyk
 */

#pragma once

#include "cocos2d.h"

#include "NXCCContext.h"

#include "../NXKT/NXPtr.hpp"
#include "../NXKT/NXTimer.h"
#include "../NXWB/NXWorkbeach.h"


class NXCCDirector : public cocos2d::CCDirector
				   , public NXObject
				   , public NXTimerRunner
{
private:
	bool  				_animationRunning ;
	NXPtr<NXTimer>		_animationTimer ;
	NXCCContext*		_animationContext ;

#ifdef PRINT_COCOS2DX_ANIM_TIME
	uint32_t			_animationLastTick ;
#endif
public:
	NXCCDirector();
	virtual ~NXCCDirector();
public:
	const NXCCContext*	getAnimationContext();
	void				setAnimationSurface(NXPtr<NXGLSurface>& surface);
	void				delAnimationSurface();
	EGLConfig			getAnimationConfig();
	void				setCurrentContext() ;
public:
	virtual void setAnimationInterval(double dValue) ;
    virtual void stopAnimation(void) ;
    virtual void startAnimation(void) ;
    virtual void pauseAnimation(bool pause) ;
    virtual bool isAnimationRuning()  ;
    virtual void mainLoop(void) ;
public:
    static NXCCDirector& getSharedDirector() ;
public:
	virtual void on_timer_start(NXTimer* timer) ;
	virtual void on_timer_running(NXTimer* timer) ;
	virtual void on_timer_stop(NXTimer* timer) ;
public:
    void on_animation_runloop(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6) ;
};
