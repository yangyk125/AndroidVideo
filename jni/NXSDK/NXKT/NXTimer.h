/*
 * NXTimer.h
 *
 *  Created on: 2015-10-10
 *      Author: yangyk
 */

#pragma once


#include "NXDef.h"
#include "NXEvent.h"

#include <time.h>
#include <signal.h>
#include <pthread.h>

typedef struct tag_NXTime {
	uint64_t 	value ;
	uint64_t 	scale ;
} NXTime ;

inline NXTime NXTimeMake(uint64_t value, uint64_t scale)
{
	NXTime time ;
	time.value = value ;
	time.scale = scale ;
	return time ;
}

inline NXTime NXTimeMakeSeconds(float value)
{
	NXTime time ;
	time.value = value * 1000000000 ;
	time.scale = 1000000000 ;
	return time ;
}

typedef enum tag_NXTimerStatus {
	NXTimerUnknown = 0,
	NXTimerRunning = 1,
	NXTimerStoping = 2,
	NXTimerStoped = 3,
} NXTimerStatus;

class NXTimer ;

interface NXTimerRunner : public NXCallback {
	virtual void on_timer_start(NXTimer* timer) = 0 ;
	virtual void on_timer_running(NXTimer* timer) = 0 ;
	virtual void on_timer_stop(NXTimer* timer) = 0 ;
} ;


class NXTimer : public NXObject
{
private:
	bool 			_thread ;
	float 			_delays ;
	NXTimerRunner* 	_runner ;
	NXEvent			_eventr ;
	NXEvent			_exiter ;
	NXTimerStatus	_status ;
	pthread_t		_thrdid ;
	timer_t			_timert ;
public:
	NXTimer();
	virtual ~NXTimer();
private:
	static void* timer_thread_proc(void* context) ;
	void timer_thread_run();
private:
	/*
	static void singal_timer_proc(int signo, struct siginfo* info, void* context);
	void signal_timer_run(int signo, struct siginfo* info, void* context);
	*/
	static void singal_timer_proc(sigval_t sigvalue);
	void signal_timer_run();
public:
	bool open(NXTimerRunner* runner, NXTime delayTime, bool newThread) ;
	bool start();
	bool stop();
	bool stoped();//只对子线程模式有效
	bool close();
	NXTimerStatus status();
};

