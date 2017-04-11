/*
 * NXFFSReader.h
 *
 *  Created on: 2015年9月17日
 *      Author: lixiang2015
 */

#pragma once

#include "NXSDK.h"

#include <string>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}

class NXFFSReader : public NXSDKReader
{
	std::string			_videofile ;
	bool 				_seekable ;
	bool				_reachend ;
	NXSDKMediaFormat	_mediaFmt ;
	AVPixelFormat		_pixelout ;

	AVFormatContext*    _avcontext ;

	AVCodec*			_avcodecV  ;
	AVCodec*			_avcodecA  ;
	AVStream*			_avstreamV ;
	AVStream*			_avstreamA ;

	AVFrame*			_avframe  ;
	AVPacket 			_avpacket ;

	SwsContext*			_avscaleV ;
	SwrContext*			_avscaleA ;
public:
	NXFFSReader();
	virtual ~NXFFSReader();
public:
	virtual bool open(const char* filepath, NXSDKPixelFormat pixfmt, bool seekable) ;
	virtual bool get_media_format(NXOUT NXSDKMediaFormat& mediaFormat) ;
	virtual bool start() ;
	virtual bool stop() ;
	virtual bool seek(float time) ;
	virtual bool stoped() ;
	virtual bool close() ;
	virtual bool get_next_buffer(NXOUT NXPtr<NXBuffer>& buffer, NXOUT NXSDKTrackType* trackType) ;
	virtual bool reachend() ;
};

