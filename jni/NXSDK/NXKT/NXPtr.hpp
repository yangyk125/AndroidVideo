/*
 * NXPtr.h
 *
 *  Created on: 2015-9-14
 *      Author: yangyk
 */

#pragma once

#include "NXDef.h"

#include "../NXKT/NXLock.h"
/**
 * reference count auto ptr .
 */

template <typename T>
class NXPtr
{
private:
	volatile int* 	ref ;
	NXLock* 		lck ;
	T*				ptr ;
public:
	NXPtr() ;
	NXPtr(T* ptr) ;
	NXPtr(const NXPtr<T>& ptr) ;
	~NXPtr() ;
private:
	void clean();
public:
	bool is_null();
	int  ref_count();
public:
	operator T*() ;
	T* operator->();
	T* operator+(int len) ;
	bool operator==(T* aptr);
	bool operator==(const NXPtr<T>& aptr);
	bool operator!=(T* aptr);
	bool operator!=(const NXPtr<T>& aptr);
	const NXPtr<T>& operator=(T* aptr);
	const NXPtr<T>& operator=(const NXPtr<T>& aptr);
};


template <typename T>
void NXPtr<T>::clean()
{
	if(this->ref && this->ptr && this->lck)
	{
		this->lck->lock() ;

		(*this->ref)-- ;

		if(*this->ref == 0)
		{
			delete this->ptr ;
			delete this->ref ;
			this->ref = NULL ;
			this->ptr = NULL ;

			this->lck->unlock() ;
			delete this->lck ;
			this->lck = NULL ;
		}
		else
		{
			this->ref = NULL ;
			this->ptr = NULL ;

			this->lck->unlock() ;
			this->lck = NULL ;
		}
	}
}

template <typename T>
NXPtr<T>::NXPtr()
{
	this->ref = NULL ;
	this->ptr = NULL ;
	this->lck = NULL ;
}


template <typename T>
NXPtr<T>::NXPtr(T* aptr)
{
	if(aptr)
	{
		this->ref = new int(1) ;
		this->lck = new NXLock() ;
		this->ptr = aptr ;
	}
	else
	{
		this->ref = NULL ;
		this->lck = NULL ;
		this->ptr = NULL ;
	}
}

template <typename T>
NXPtr<T>::NXPtr(const NXPtr<T>& aptr)
{
	this->ref = aptr.ref ;
	this->ptr = aptr.ptr ;
	this->lck = aptr.lck ;

	if(this->ref)
	{
		this->lck->lock();
		(*this->ref) ++ ;
		this->lck->unlock();
	}
}

template <typename T>
NXPtr<T>::~NXPtr()
{
	clean();
}

template <typename T>
T* NXPtr<T>::operator->()
{
	return this->ptr ;
}

template <typename T>
T* NXPtr<T>::operator+(int len)
{
	if(ptr)
		return this->ptr + len ;
	return NULL ;
}

template <typename T>
bool NXPtr<T>::operator==(const NXPtr<T>& aptr)
{
	return this->ptr == aptr.ptr ;
}

template <typename T>
bool NXPtr<T>::operator==(T* aptr)
{
	return this->ptr == aptr ;
}

template <typename T>
bool NXPtr<T>::operator!=(const NXPtr<T>& aptr)
{
	return this->ptr != aptr.ptr ;
}

template <typename T>
bool NXPtr<T>::operator!=(T* aptr)
{
	return this->ptr != aptr ;
}

template <typename T>
bool NXPtr<T>::is_null()
{
	return this->ptr == NULL ;
}

template <typename T>
int NXPtr<T>::ref_count()
{
	return *this->ref ;
}

template <typename T>
NXPtr<T>::operator T*()
{
	return this->ptr ;
}

template <typename T>
const NXPtr<T>& NXPtr<T>::operator=(T* aptr)
{
	clean();

	if(aptr)
	{
		this->ref = new int(1) ;
		this->lck = new NXLock() ;
		this->ptr = aptr ;
	}

	return *this ;
}

template <typename T>
const NXPtr<T>& NXPtr<T>::operator=(const NXPtr<T>& aptr)
{
	clean();

	this->ref = aptr.ref ;
	this->ptr = aptr.ptr ;
	this->lck = aptr.lck ;

	if(this->ref)
	{
		this->lck->lock();
		(*this->ref) ++ ;
		this->lck->unlock();
	}

	return *this ;
}


