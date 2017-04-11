/*
 * EFALGSaliency.cpp
 *
 *  Created on: 2016年1月12日
 *      Author: yangyk
 */

#include "EFALGSaliency.h"


EFALGSaliency::EFALGSaliency()
{

}

EFALGSaliency::~EFALGSaliency()
{

}

/*
 * 涉及到企业核心算法，出于保密目的不能公布！ 所以此类所有实现代码均删除。
 */

bool EFALGSaliency::open(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height)
{
	return true ;
}
bool EFALGSaliency::saliency(NXPtr<NXBuffer>& target, int precision, NXRect& rect, std::vector<NXPoint>& result)
{
	return true ;
}
bool EFALGSaliency::close()
{
	return true ;
}

bool EFALGSaliency::static_saliency(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height, int precision, NXRect& rect, std::vector<NXPoint>& result)
{
	rect = NXRectMake(0.3f,0.3f,0.3f,0.3f) ;
	return true ;
}

bool EFALGSaliency::static_saliency(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height, int precision, NXRect& rect)
{
	rect = NXRectMake(0.3f,0.3f,0.3f,0.3f) ;
	return true ;
}

bool EFALGSaliency::static_saliency(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height, int precision, std::vector<NXPoint>& result)
{
	return true ;
}
