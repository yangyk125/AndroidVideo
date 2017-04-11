/*
 * EFALGTracker.cpp
 *
 *  Created on: 2016年1月12日
 *      Author: yangyk
 */

#include "EFALGTracker.h"

EFALGTracker::EFALGTracker()
{
	_trackFirst = -1 ;
}

EFALGTracker::~EFALGTracker()
{

}

/*
 * 涉及到企业核心算法，出于保密目的不能公布！ 所以此类所有实现代码均删除。
 */

bool EFALGTracker::scaleGray(NXPtr<NXBuffer>& source)
{
	return true ;
}

bool EFALGTracker::open(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height, NXRect& rect)
{
	_testRect = rect ;
	return true ;
}

bool EFALGTracker::track(NXPtr<NXBuffer>& target, NXRect& result)
{
	result = _testRect ;
	return true ;
}

bool EFALGTracker::close()
{
	return true ;
}
