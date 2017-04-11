/*
 * NXKit.h
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"
#include <string>


class NXKit {
public:
	NXKit();
	virtual ~NXKit();
public:
	//time
	static uint32_t get_time_tick32() ;
	static uint64_t get_time_tick64() ;

	static std::string get_timenow_string() ;
	static void	replace_string(std::string& str, char ochar, char nchar) ;

	//pixel
	static int pixel_format_convert_ff(int format);
	static int pixel_format_convert_nx(int format);

	//string
	static std::string convert_unicode_to_utf8(const std::wstring& unicode) ;
	static std::string convert_unicode_to_utf8(const wchar_t* unicode) ;

	static std::wstring convert_utf8_to_unicode(const std::string& utf8) ;
	static std::wstring convert_utf8_to_unicode(const char* utf8) ;

	static std::string& format_string(std::string& result, const char* format, ...) ;
};

