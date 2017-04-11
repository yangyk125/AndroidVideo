/*
 * NXLock.cpp
 *
 *  Created on: 2015-9-16
 *      Author: yangyk
 */

#include "NXLock.h"

#include <semaphore.h>

NXLock::NXLock()
{
	sem_init(&_semaphore, 0, 1);
}

NXLock::~NXLock()
{
	sem_destroy(&_semaphore);
}

void NXLock::lock()
{
	sem_wait(&_semaphore);
}

void NXLock::unlock()
{
	sem_post(&_semaphore);
}

bool NXLock::trylock()
{
	return 0 == sem_trywait(&_semaphore);
}
