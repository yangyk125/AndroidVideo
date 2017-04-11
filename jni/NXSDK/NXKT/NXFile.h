/*
 * NXFile.h
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#pragma once

#include "NXDef.h"

#include <stdio.h>
#include <string>


/*
    r 打开只读文件，该文件必须存在。

 　　r+ 打开可读写的文件，该文件必须存在。

    rb 读打开一个二进制文件，只允许读数据。

 　　rb+ 读写打开一个二进制文件，只允许读写数据。

 　　rt+ 读写打开一个文本文件，允许读和写。

 　　w 打开只写文件，若文件存在则文件长度清为0，即该文件内容会消失。若文件不存在则建立该文件。

 　　w+ 打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。

 　　a 以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留。（EOF符保留）

 　　a+ 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留。 （原来的EOF符不保留）

 　　wb 只写打开或新建一个二进制文件；只允许写数据。

 　　wb+ 读写打开或建立一个二进制文件，允许读和写。

 　　wt+ 读写打开或着建立一个文本文件；允许读写。

 　　at+ 读写打开一个文本文件，允许读或在文本末追加数据。

 　　ab+ 读写打开一个二进制文件，允许读或在文件末追加数据。
*/

class NXFile : public NXObject
{
protected:
	std::string 	_filepath ;
	FILE*			_fileobj  ;
public:
	NXFile(const char* path);
	virtual ~NXFile();
public:
	static bool 	exist(const char* path) ;
	static bool 	delfile(const char* path) ;
	static int32_t 	length(const char* path) ;
	static bool 	mkdir(const char* path) ;
public:
	bool open(const char* mode) ;
	bool exist() ;
	bool remove() ;
	bool ended() ;
	bool flush();
	bool close() ;
	int32_t length() ;
	int32_t pos();
	int32_t read(uint8_t* buffer, int32_t buflen) ;
	int32_t write(const uint8_t* buffer, int32_t buflen) ;
	bool seek(int32_t offset, bool fromStart) ;
};

