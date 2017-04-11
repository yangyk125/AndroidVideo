/*
 * NXThread.cpp
 *
 *  Created on: 2015-10-22
 *      Author: yangyk
 */

#include "NXThread.h"
#include <unistd.h>

NXThread::NXThread(NXThreadRunner* callRunner, NXPtr<NXObject>* param) : _eventr(0)
{
	if(param)
		_paramr = *param ;
	else
		_paramr = NULL ;

	_runner = callRunner ;
	_thrdid = 0 ;
	_timert = 0 ;
}

NXThread::~NXThread()
{
	_runner = NULL ;
	_paramr = NULL ;
	_thrdid = 0 ;
	_timert = 0 ;
}

void* NXThread::thread_proc(void* context)
{
	NXThread* pThis = (NXThread*)context ;
	pThis->thread_run();
	pthread_exit(NULL);
	return NULL ;
}

void NXThread::thread_run()
{
	if(_runner)
		_runner->on_thread_runing(this, _paramr) ;
	_eventr.fire();
}

bool NXThread::start()
{
	if(_thrdid == 0)
	{
		pthread_create(&_thrdid, NULL, thread_proc, this) ;
		return true ;
	}
	return false ;
}

void NXThread::stoped()
{
	_eventr.wait() ;
	_thrdid = 0 ;
}
