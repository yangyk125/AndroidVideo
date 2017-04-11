/*
 * EFALGTracker.h
 *
 *  Created on: 2016年1月12日
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

class EFALGTracker
{
private:
	NXRect		_testRect ;
public:
	int32_t	    _trackFirst ;
public:
	EFALGTracker();
	virtual ~EFALGTracker();
private:
	bool scaleGray(NXPtr<NXBuffer>& source);
public:
	bool open(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height, NXIN NXRect& rect) ;
	bool track(NXPtr<NXBuffer>& target, NXOUT NXRect& result) ;
	bool close() ;
};

