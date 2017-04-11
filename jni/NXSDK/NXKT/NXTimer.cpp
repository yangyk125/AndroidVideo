/*
 * NXTimer.cpp
 *
 *  Created on: 2015-10-10
 *      Author: yangyk
 */

#include "NXTimer.h"
#include "NXKit.h"
#include <map>
#include <android/log.h>
#include <asm/siginfo.h>

NXTimer::NXTimer() : _eventr(1), _exiter(1)
{
	_runner = NULL ;
	_delays = 0 ;
	_timert = 0 ;
	_status = NXTimerUnknown ;
}

NXTimer::~NXTimer()
{
	_runner = NULL ;
	_delays = 0 ;
	_timert = 0 ;
	_status = NXTimerUnknown ;
}

bool NXTimer::open(NXTimerRunner* runner, NXTime delayTime, bool newThread)
{
	_status = NXTimerUnknown ;
	_runner = runner ;
	_delays = 1.0f*delayTime.value/delayTime.scale ;
	_thread = newThread ;

	struct sigevent  evp ;
	//struct sigaction act ;

	memset(&evp, 0, sizeof(evp));
	//memset(&act, 0, sizeof(act));

	//act.sa_sigaction = singal_timer_proc ;
	//act.sa_flags = SA_SIGINFO ;

	//if(sigemptyset(&act.sa_mask) < 0)
	//	__android_log_print(ANDROID_LOG_DEBUG, "TIMER", "sigemptyset() faild !!");

	//if(sigaction(SIGALRM, &act, NULL) < 0)
	//	__android_log_print(ANDROID_LOG_DEBUG, "TIMER", "sigaction() faild !!");

	evp.sigev_signo = SIGALRM ;
	evp.sigev_notify = SIGEV_THREAD ;
	evp.sigev_value.sival_ptr = this ;
	evp.sigev_notify_function = singal_timer_proc ;
	evp.sigev_notify_attributes = NULL ;

	if(timer_create(CLOCK_REALTIME, &evp, &_timert) < 0)
		__android_log_print(ANDROID_LOG_DEBUG, "TIMER", "timer_create() faild !!");

	return _delays > 0 ;
}

bool NXTimer::start()
{
	_status = NXTimerRunning ;

	struct itimerspec it  ;
	it.it_value.tv_sec = 0 ;
	it.it_value.tv_nsec = _delays * 1000000000 ;
	it.it_interval.tv_sec = 0 ;
	it.it_interval.tv_nsec = _delays * 1000000000 ;

	if(_thread)
		pthread_create(&_thrdid, NULL, timer_thread_proc, this) ;
	else if(_runner)
		_runner->on_timer_start(this) ;

	return (timer_settime(_timert, 0, &it, NULL) < 0)  ;
}

bool NXTimer::stop()
{
	if(_timert)
	{
		timer_delete(_timert) ;
		_timert = 0 ;
	}

	if(_status == NXTimerRunning)
	{
		_status = NXTimerStoping ;
		if(_thread)
		{
			_eventr.fire() ;
			_thrdid = 0 ;
		}
		else if(_runner)
		{
			_runner->on_timer_stop(this) ;
		}
	}

	return true ;
}

bool NXTimer::stoped()
{
	if(_status == NXTimerStoping)
	{
		_exiter.wait();
		_status = NXTimerStoped ;
		return true ;
	}
	return false ;
}

bool NXTimer::close()
{
	_status = NXTimerUnknown ;
	_runner = NULL ;
	_delays = 0 ;
	return true ;
}

NXTimerStatus NXTimer::status()
{
	return _status ;
}

void* NXTimer::timer_thread_proc(void* context)
{
	NXTimer* pThis = (NXTimer*)context ;
	pThis->timer_thread_run();
	pthread_exit(NULL);
	return NULL ;
}

void NXTimer::timer_thread_run()
{
	_exiter.wait();

	if(_runner)
		_runner->on_timer_start(this) ;

	while(_status == NXTimerRunning)
	{
		_eventr.wait();

		if(_runner)
			_runner->on_timer_running(this);
	}

	if(_runner)
		_runner->on_timer_stop(this) ;

	_exiter.fire() ;
}



//void NXTimer::singal_timer_proc(int signo, struct siginfo* info, void* context)
void NXTimer::singal_timer_proc(sigval_t sigvalue)
{
	NXTimer* pThis = (NXTimer*)sigvalue.sival_ptr ;
	if(pThis)
		pThis->signal_timer_run() ;
}


//void NXTimer::signal_timer_run(int signo, struct siginfo* info, void* context)
void NXTimer::signal_timer_run()
{
	if(_thread)
		_eventr.fire();
	else if(_runner)
		_runner->on_timer_running(this);
}
