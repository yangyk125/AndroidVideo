/*
 * NXEvent.cpp
 *
 *  Created on: 2015-10-15
 *      Author: yangyk
 */

#include "NXEvent.h"
#include <semaphore.h>

NXEvent::NXEvent(int initCount)
{
	sem_init(&_semaphore, 0, initCount);
}

NXEvent::~NXEvent()
{
	sem_destroy(&_semaphore);
}

void NXEvent::fire(uint32_t* count)
{
	sem_post(&_semaphore);
	if(count)
		*count = event_count() ;
}

void NXEvent::wait(uint32_t* count)
{
	sem_wait(&_semaphore);
	if(count)
		*count = event_count() ;
}

int NXEvent::event_count()
{
	int signals = 0 ;
	sem_getvalue(&_semaphore, &signals);
	return signals ;
}
