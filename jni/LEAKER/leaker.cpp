/*
 * leaker.cpp
 *
 *  Created on: 2015-11-24
 *      Author: yangyk
 */

#include <map>
#include <stdlib.h>
#include <semaphore.h>
#include <android/log.h>

static sem_t g_meminfo_semaphore ;
static bool  g_meminfo_created = false ;

static std::map<void*, size_t> g_memInfo ;


void leaker_init_semaphore()
{
	sem_init(&g_meminfo_semaphore, 0, 1);
}

void leaker_uninit_semaphore()
{
	sem_destroy(&g_meminfo_semaphore);
}

void leaker_lock_semaphore()
{
	if(!g_meminfo_created)
	{
		leaker_init_semaphore() ;
		g_meminfo_created = true ;
	}

	sem_wait(&g_meminfo_semaphore);
}

void leaker_unlock_semaphore()
{
	sem_post(&g_meminfo_semaphore);
}



namespace leaker {

void* nx_malloc(size_t size)
{
	void* ptr = ::malloc(size) ;

	leaker_lock_semaphore() ;
	g_memInfo[ptr] = size ;
	leaker_unlock_semaphore() ;

	return ptr ;
}

void nx_free(void* ptr)
{
	leaker_lock_semaphore() ;
	std::map<void*, size_t>::iterator it = g_memInfo.find(ptr) ;
	if(it == g_memInfo.end())
		__android_log_print(ANDROID_LOG_ERROR, "LEAKER", "void operator delete() the pointer not in the map...");
	else
		g_memInfo.erase(it) ;
	leaker_unlock_semaphore() ;

	::free(ptr) ;
}

void* nx_memcpy(void* target, const void* source, size_t size)
{
	leaker_lock_semaphore() ;

	std::map<void*, size_t>::iterator itt = g_memInfo.find(target) ;
	std::map<void*, size_t>::iterator its = g_memInfo.find((void*)source) ;

	if(itt != g_memInfo.end() && size > itt->second)
		__android_log_print(ANDROID_LOG_ERROR, "LEAKER", "void memcpy() target size not enough!!");
	if(its != g_memInfo.end() && size > its->second)
		__android_log_print(ANDROID_LOG_ERROR, "LEAKER", "void memcpy() source size not enough!!");

	if(itt == g_memInfo.end())
	{
		std::map<void*, size_t>::iterator last = g_memInfo.end();
		for(std::map<void*, size_t>::iterator it = g_memInfo.begin();
				it != g_memInfo.end();
				++ it)
		{
			void* start = it->first ;
			void* end   = (char*)it->first + it->second ;

			if(target >= start && target < end)
			{
				last = it ;
				break ;
			}
		}

		if(last != g_memInfo.end())
		{
			size_t caps = last->second - ((char*)target - (char*)last->first) ;
			if(size > caps)
				__android_log_print(ANDROID_LOG_ERROR, "LEAKER", "void memcpy() capacity size not enough!!");
		}
	}

	leaker_unlock_semaphore() ;

	return ::memcpy(target, source, size) ;
}

void* nx_memccpy(void* target, const void* source, int num, size_t size)
{
	leaker_lock_semaphore() ;

	std::map<void*, size_t>::iterator itt = g_memInfo.find(target) ;
	std::map<void*, size_t>::iterator its = g_memInfo.find((void*)source) ;

	if(itt != g_memInfo.end() && size > itt->second)
		__android_log_print(ANDROID_LOG_ERROR, "LEAKER", "void memccpy() target size not enough!!");
	if(its != g_memInfo.end() && size > its->second)
		__android_log_print(ANDROID_LOG_ERROR, "LEAKER", "void memccpy() source size not enough!!");

	leaker_unlock_semaphore() ;

	return ::memccpy(target, source, num, size) ;
}

void* nx_memset(void* target, int value, size_t size)
{
	leaker_lock_semaphore() ;

	std::map<void*, size_t>::iterator itt = g_memInfo.find(target) ;

	if(itt != g_memInfo.end() && size > itt->second)
		__android_log_print(ANDROID_LOG_ERROR, "LEAKER", "void memset() target size not enough!!");

	if(itt == g_memInfo.end())
	{
		std::map<void*, size_t>::iterator last = g_memInfo.end();
		for(std::map<void*, size_t>::iterator it = g_memInfo.begin();
				it != g_memInfo.end();
				++ it)
		{
			void* start = it->first ;
			void* end   = (char*)it->first + it->second ;

			if(target >= start && target < end)
			{
				last = it ;
				break ;
			}
		}

		if(last != g_memInfo.end())
		{
			size_t caps = last->second - ((char*)target - (char*)last->first) ;
			if(size > caps)
				__android_log_print(ANDROID_LOG_ERROR, "LEAKER", "void memset() capacity size not enough!!");
		}
	}

	leaker_unlock_semaphore() ;

	return ::memset(target, value, size) ;
}

}
