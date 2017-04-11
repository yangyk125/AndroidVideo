/*
 * NXThread.h
 *
 *  Created on: 2015-10-22
 *      Author: yangyk
 */

#pragma once

#include "NXDef.h"
#include "NXEvent.h"
#include "NXPtr.hpp"

#include <time.h>
#include <signal.h>
#include <pthread.h>

class NXThread ;
interface NXThreadRunner : public NXCallback
{
	virtual void on_thread_runing(const NXThread* thread, NXPtr<NXObject>& param) = 0 ;
} ;

class NXThread : public NXObject
{
private:
	NXThreadRunner*	_runner ;
	NXPtr<NXObject> _paramr ;
	NXEvent			_eventr ;
	pthread_t		_thrdid ;
	timer_t			_timert ;
public:
	NXThread(NXThreadRunner* callRunner, NXPtr<NXObject>* param = NULL) ;
	virtual ~NXThread() ;
private:
	static void* thread_proc(void* context) ;
	void thread_run();
public:
	bool start();
	void stoped();
};


