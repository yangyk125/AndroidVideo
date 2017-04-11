/*
 * NXKit.cpp
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#include "NXKit.h"
#include <signal.h>
#include <sys/time.h>
#include <iostream>
#include <time.h>

extern "C"
{
#include "libavutil/pixfmt.h"
}

NXKit::NXKit()
{}

NXKit::~NXKit()
{}

uint32_t NXKit::get_time_tick32()
{
    struct timeval time_tick ;
    gettimeofday(&time_tick, NULL);
    uint32_t tick = ((uint32_t)time_tick.tv_sec)*1000+(uint32_t)time_tick.tv_usec/1000 ;
    return tick ;
}

uint64_t NXKit::get_time_tick64()
{
    struct timeval time_tick ;
    gettimeofday(&time_tick, NULL);
    uint64_t tick = ((uint64_t)time_tick.tv_sec)*1000+(uint64_t)time_tick.tv_usec/1000 ;
    return tick ;
}

std::string NXKit::get_timenow_string()
{
	time_t ticks ;
	time(&ticks);

	struct tm fields ;
	localtime_r(&ticks, &fields);
	std::string result ;

	NXKit::format_string(result, "%04d-%02d-%02d %02d:%02d:%02d", 1900+fields.tm_year, fields.tm_mon+1, fields.tm_mday, fields.tm_hour, fields.tm_min, fields.tm_sec) ;
	return result ;
}

void NXKit::replace_string(std::string& str, char ochar, char nchar)
{
	for(int idx=0; idx<str.length(); idx++)
	{
		if(str[idx] == ochar)
			str[idx] = nchar ;
	}
}

int NXKit::pixel_format_convert_ff(int format)
{
	switch (format) {
		case AV_PIX_FMT_BGRA:
			return NXSDK_PIX_FMT_BGRA;
		case AV_PIX_FMT_ABGR:
			return NXSDK_PIX_FMT_ABGR;
		case AV_PIX_FMT_RGBA:
			return NXSDK_PIX_FMT_RGBA;
		case AV_PIX_FMT_ARGB:
			return NXSDK_PIX_FMT_ARGB;
		case AV_PIX_FMT_NV21:
			return NXSDK_PIX_FMT_NV21;
		case AV_PIX_FMT_NV12:
			return NXSDK_PIX_FMT_NV12;
		case AV_PIX_FMT_YUV420P:
			return NXSDK_PIX_FMT_YUV420P;
		default:
			return NXSDK_PIX_FMT_NONE;
		}
}
int NXKit::pixel_format_convert_nx(int format)
{
	switch (format) {
		case NXSDK_PIX_FMT_BGRA:
			return AV_PIX_FMT_BGRA;
		case NXSDK_PIX_FMT_ABGR:
			return AV_PIX_FMT_ABGR;
		case NXSDK_PIX_FMT_RGBA:
			return AV_PIX_FMT_RGBA;
		case NXSDK_PIX_FMT_ARGB:
			return AV_PIX_FMT_ARGB;
		case NXSDK_PIX_FMT_NV21:
			return AV_PIX_FMT_NV21;
		case NXSDK_PIX_FMT_NV12:
			return AV_PIX_FMT_NV12;
		case NXSDK_PIX_FMT_YUV420P:
			return AV_PIX_FMT_YUV420P;
		default:
			return AV_PIX_FMT_NONE;
		}
}

std::string NXKit::convert_unicode_to_utf8(const std::wstring& unicode)
{
	return std::string() ;
}
std::string NXKit::convert_unicode_to_utf8(const wchar_t* unicode)
{
	return std::string() ;
}
std::wstring NXKit::convert_utf8_to_unicode(const std::string& utf8)
{
	return std::wstring() ;
}
std::wstring NXKit::convert_utf8_to_unicode(const char* utf8)
{
	return std::wstring() ;
}

std::string& NXKit::format_string(std::string& result, const char* format, ...)
{
    char buf[1024];

    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    result = buf ;
    return result ;
}
