/*
 * NXLock.h
 *
 *  Created on: 2015-9-16
 *      Author: yangyk
 */

#pragma once

#include "NXDef.h"
#include <semaphore.h>

class NXLock : public NXObject
{
private:
	sem_t _semaphore ;
public:
	NXLock();
	virtual ~NXLock();
public:
	void lock() ;
	void unlock();
	bool trylock();
};

