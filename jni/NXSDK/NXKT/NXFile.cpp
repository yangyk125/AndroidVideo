/*
 * NXFile.cpp
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#include "NXFile.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <android/log.h>

NXFile::NXFile(const char* path)
{
	_filepath = path ;
	_fileobj  = NULL ;
}

NXFile::~NXFile()
{
	this->close();
}

bool NXFile::exist(const char* path)
{
	return access(path, F_OK) == 0 ;
}

bool NXFile::delfile(const char* path)
{
	return (0 == ::remove(path)) ;
}

int32_t  NXFile::length(const char* path)
{
	struct stat tbuf ;
	stat(path, &tbuf);
	return tbuf.st_size ;
}
bool NXFile::mkdir(const char* path)
{
	if(!NXFile::exist(path))
		return 0 == ::mkdir(path, S_IRWXU);
	return true ;
}
bool NXFile::open(const char* mode)
{
	if(_fileobj)
		return false ;
	_fileobj = fopen(_filepath.c_str(), mode);
	return _fileobj != NULL ;
}

bool NXFile::exist()
{
	return access(_filepath.c_str(), F_OK) == 0 ;
}

bool NXFile::remove()
{
	return ::remove(_filepath.c_str()) == 0 ;
}
bool NXFile::flush()
{
	if(_fileobj)
	{
		fflush(_fileobj);
		return true ;
	}
	return false ;
}
bool NXFile::close()
{
	if (_fileobj)
	{
		fclose(_fileobj);
		_fileobj = NULL;
		return true ;
	}
	return false ;
}

bool NXFile::ended()
{
	int32_t len = this->length() ;
	int32_t pos = this->pos() ;
	return pos >= len ;
}

int32_t NXFile::length()
{
	if(_fileobj) {
		struct stat tbuf ;
		stat(_filepath.c_str(), &tbuf);
		return tbuf.st_size ;
	}
	return -1 ;
}

int32_t NXFile::pos()
{
	if(_fileobj)
		return (int32_t)(ftell(_fileobj));
	return -1 ;
}

int32_t NXFile::read(uint8_t* buffer, int32_t buflen)
{
	if(_fileobj)
		return (int32_t)fread(buffer, 1, buflen, _fileobj);
	return -1 ;
}

int32_t NXFile::write(const uint8_t* buffer, int32_t buflen)
{
	if(_fileobj)
		return (int32_t)fwrite(buffer, 1, buflen, _fileobj);
	return -1 ;
}

bool NXFile::seek(int32_t offset, bool fromStart)
{
    if(_fileobj)
    {
        if(fromStart)
            return (0 == fseek(_fileobj, offset, SEEK_SET));
        else
            return (0 == fseek(_fileobj, offset, SEEK_CUR));
    }
    else
        return false ;
}
