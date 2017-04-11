/*
 * NXTMSWriter.h
 *
 *  Created on: 2015-12-2
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

#include "NXTMADef.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "x264.h"
}

class NXTMSWriter : public NXSDKWriter
{
private:
	std::string  		_filepath ;
	NXFile*				_fileObjt ;
	MPTFileHeader		_fileHead ;
	NXSDKMediaFormat	_nxformat ;
	AVPixelFormat		_pxFormat ;

	TMWBufferQueue		_aswtQueue ;

	x264_param_t		_x264Param ;
	x264_t*				_x264Encoder ;
	x264_picture_t*		_x264ImageI ;
	x264_picture_t*		_x264ImageO ;

	SwsContext*			_avscaler ;
	int64_t 			_frmcount ;
public:
	NXTMSWriter();
	virtual ~NXTMSWriter();
public:
	virtual bool open(const char* filepath, const char* suffix, NXIN NXSDKMediaFormat& mediaFormat) ;
	virtual bool start() ;
	virtual bool append_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType track) ;
	virtual bool stop() ;
	virtual bool stoped() ;
	virtual bool close() ;
private:
	bool encode_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType track) ;
	bool flush_buffers();
};
