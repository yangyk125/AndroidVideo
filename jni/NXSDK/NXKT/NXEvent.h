/*
 * NXEvent.h
 *
 *  Created on: 2015-10-15
 *      Author: yangyk
 */

#pragma once

#include "NXDef.h"
#include <semaphore.h>

class NXEvent : public NXObject
{
private:
	sem_t _semaphore ;
public:
	NXEvent(int initCount);
	virtual ~NXEvent();
public:
	void fire(uint32_t* count = NULL);
	void wait(uint32_t* count = NULL);
	int  event_count();
};

