/*
 * NXCameraThrottle.cpp
 *
 *  Created on: 2015-10-21
 *      Author: yangyk
 */

#include "NXSDK.h"
#include "NXThrottle.h"

#include <assert.h>

NXThrottle::NXThrottle()
{
	_buffer = NULL ;
}

NXThrottle::~NXThrottle()
{
	_locker.lock() ;
	_buffer = NULL ;
	_locker.unlock() ;
}

void NXThrottle::push(NXPtr<NXBuffer>& data)
{
	assert(data != NULL);
	_locker.lock();
	_buffer = data ;
	_locker.unlock();
}

void NXThrottle::pop(NXPtr<NXBuffer>& data)
{
	assert(data == NULL);
	_locker.lock();
	data = _buffer ;
	_locker.unlock();
}
