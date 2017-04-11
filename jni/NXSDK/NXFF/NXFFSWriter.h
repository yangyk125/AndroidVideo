/*
 * NXFFSWriter.h
 *
 *  Created on: 2015-9-21
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

#include <string>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
}

class NXFFSWriter : public NXSDKWriter
{
private:
	std::string  		_filepath ;
	NXSDKMediaFormat	_nxformat ;
	AVPixelFormat		_pxFormat ;
	NXPtr<NXBuffer>		_yuvBuffer ;

	AVOutputFormat*     _outformat ;
	AVFormatContext*    _avcontext ;
	AVCodec*			_avcodecV  ;
	AVCodec*			_avcodecA  ;
	AVStream*			_avstreamV ;
	AVStream*			_avstreamA ;
	AVFrame*			_avframeV ;
	AVFrame*			_avframeA ;
	SwsContext*			_avscaleV ;
	AVAudioFifo*		_audififo ;
	AVPacket 			_vpacket  ;
	AVPacket 			_apacket  ;
	int64_t 			_vpktcount ;
	int64_t 			_apktcount ;
public:
	NXFFSWriter();
	virtual ~NXFFSWriter();
public:
	virtual bool open(const char* filepath, const char* suffix, NXIN NXSDKMediaFormat& mediaFormat) ;
	virtual bool start() ;
	virtual bool append_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType trackType) ;
	virtual bool stop() ;
	virtual bool stoped() ;
	virtual bool close() ;
private:
	bool audio_trailer();
	bool flush_buffer(NXSDKTrackType track) ;

};

