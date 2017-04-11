/*
 * leaker.h
 *
 *  Created on: 2015-11-24
 *      Author: yangyk
 */

#pragma once

#include <stddef.h>

namespace leaker {

void*  nx_malloc(size_t size);
void   nx_free(void* ptr);

void*  nx_memcpy(void* target, const void* source, size_t size);
void*  nx_memset(void* target, int value, size_t size);

void*  nx_memccpy(void *, const void *, int, size_t);

}

#define malloc(x) 			leaker::nx_malloc(x)
#define free(x) 			leaker::nx_free(x)
#define memcpy(x,y,z)		leaker::nx_memcpy(x,y,z)
#define memset(x,y,z)		leaker::nx_memset(x,y,z)
#define memccpy(x,y,z)		leaker::nx_memccpy(x,y,z)

