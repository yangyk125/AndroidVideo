/*
 * NXCCDirector.cpp
 *
 *  Created on: 2015-10-28
 *      Author: yangyk
 */

#include "NXCCDirector.h"

#include "NXSDK.h"
#include <android/log.h>


#define NX_COCOS2DX_ANIMATION_RUNLOOP_MESSAGE 	0x7FEFEFEF

NXCCDirector::NXCCDirector()
{
	_animationRunning  = false ;
	_animationTimer    = NULL ;

#ifdef PRINT_COCOS2DX_ANIM_TIME
	_animationLastTick = 0 ;
#endif

	_animationContext = new NXCCContext();
	this->setCCContext(_animationContext) ;

	theWorkbeach.registerMessage(NX_COCOS2DX_ANIMATION_RUNLOOP_MESSAGE,  static_cast<WBHandleMessage>(&NXCCDirector::on_animation_runloop)) ;
}

NXCCDirector::~NXCCDirector()
{
	this->setCCContext(NULL) ;

	delete _animationContext ;
	_animationContext = NULL ;

	theWorkbeach.unregisterMessage(NX_COCOS2DX_ANIMATION_RUNLOOP_MESSAGE) ;
}

const NXCCContext* NXCCDirector::getAnimationContext()
{
	return _animationContext ;
}

void NXCCDirector::setCurrentContext()
{
	_animationContext->setCCCurrentContext() ;
}

void NXCCDirector::setAnimationSurface(NXPtr<NXGLSurface>& surface)
{
	_animationContext->setDrawSurface(surface) ;
	_animationContext->setCCCurrentContext() ;
}

void NXCCDirector::delAnimationSurface()
{
	NXPtr<NXGLSurface> surface = NULL ;
	_animationContext->setDrawSurface(surface) ;
	_animationContext->setCCCurrentContext() ;
}

EGLConfig NXCCDirector::getAnimationConfig()
{
	return _animationContext->getDrawContext()->getCurrentConfig() ;
}

void NXCCDirector::setAnimationInterval(double dValue)
{
	m_dAnimationInterval = dValue ;
	if(_animationRunning)
	{
		stopAnimation();
		startAnimation();
	}
}

void NXCCDirector::startAnimation(void)
{
#ifdef DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, "DIRECTOR", "NXCCDirector::startAnimation()") ;
#endif

	if(_animationTimer == NULL) {
		_animationTimer = new NXTimer();
		_animationTimer->open(this, NXTimeMakeSeconds(m_dAnimationInterval), true) ;
		_animationTimer->start() ;
		_animationRunning = true ;
	}
}

void NXCCDirector::pauseAnimation(bool pause)
{
	_animationRunning = !pause ;
	if(!_animationRunning)
		theWorkbeach.cleanQueueMessage(NX_COCOS2DX_ANIMATION_RUNLOOP_MESSAGE) ;
	else
		_animationContext->setCCCurrentContext() ;
}

void NXCCDirector::stopAnimation(void)
{
#ifdef DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, "DIRECTOR", "NXCCDirector::stopAnimation()") ;
#endif

	if(_animationTimer != NULL) {
		_animationRunning = false ;
		_animationTimer->stop() ;
		_animationTimer->stoped();
		_animationTimer->close() ;
		_animationTimer = NULL ;

		theWorkbeach.cleanQueueMessage(NX_COCOS2DX_ANIMATION_RUNLOOP_MESSAGE) ;
	}
}

bool NXCCDirector::isAnimationRuning()
{
	return _animationRunning ;
}

void NXCCDirector::mainLoop(void)
{
	if(_animationRunning && _animationContext->hasDrawSurface())
	{

#ifdef PRINT_COCOS2DX_ANIM_TIME
		uint32_t tick1 = NXKit::get_time_tick32() ;
#endif
		//cocos2d main loop
		this->drawScene();

		//release some objects .
		this->releaseObjects() ;

#ifdef PRINT_COCOS2DX_ANIM_TIME
		uint32_t tick2 = NXKit::get_time_tick32() ;
		__android_log_print(ANDROID_LOG_DEBUG, "ANIMATION", "COCOS2DX-animation drawScene after ticks %u cost ticks %u..", tick1-_animationLastTick, tick2-tick1);
		_animationLastTick = tick1 ;
#endif
	}
}

cocos2d::CCDirector* CCGetGlobalDirector()
{
	NXCCDirector& director = NXCCDirector::getSharedDirector() ;
	return &director ;
}

NXCCDirector& NXCCDirector::getSharedDirector()
{
	static NXCCDirector diretor ;
	return diretor ;
}

void NXCCDirector::on_timer_start(NXTimer* timer)
{

}

void NXCCDirector::on_timer_running(NXTimer* timer)
{
	if(_animationRunning)
		theWorkbeach.postHighMessasge(NX_COCOS2DX_ANIMATION_RUNLOOP_MESSAGE, this) ;
}

void NXCCDirector::on_timer_stop(NXTimer* timer)
{
}

void NXCCDirector::on_animation_runloop(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(m_pRunningScene && !m_pRunningScene->isRunning()) {
		m_pRunningScene->onEnter() ;
		m_pRunningScene->onEnterTransitionDidFinish() ;
	}

	this->mainLoop() ;
}
