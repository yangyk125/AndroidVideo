/*
 * EFALGSaliency.h
 *
 *  Created on: 2016年1月12日
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

class EFALGSaliency
{
private:
public:
	EFALGSaliency();
	virtual ~EFALGSaliency();
public:
	bool open(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height) ;
	bool saliency(NXPtr<NXBuffer>& target, int precision, NXRect& rect, std::vector<NXPoint>& result) ;
	bool close();
public:
	static bool static_saliency(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height, int precision, NXRect& rect, std::vector<NXPoint>& result) ;
	static bool static_saliency(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height, int precision, NXRect& rect) ;
	static bool static_saliency(NXPtr<NXBuffer>& source, uint32_t width, uint32_t height, int precision, std::vector<NXPoint>& result) ;
};

