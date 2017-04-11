/*
 * NXTMSReader.h
 *
 *  Created on: 2015-12-3
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

#include "NXTMADef.h"

#include <map>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

class NXTMSReader : public NXSDKReader
{
private:
	NXFile*				_fileObj ;
	bool 				_seekable ;
	MPTFileHeader		_fileHeader ;
	NXSDKPixelFormat	_outpixfmt ;
	TMFramePosMap 		_framesMap ;

	AVCodec*			_avdecoder ;
	AVCodecContext*		_avcontext ;
	SwsContext*			_scaleCtxt ;

	AVFrame*			_avdcframe  ;
	AVPacket 			_avdcpacket ;

	uint8_t*			_readBufer ;
	bool 				_reachend ;
public:
	NXTMSReader();
	virtual ~NXTMSReader();
public:
	virtual bool open(const char* filepath, NXSDKPixelFormat pixfmt, bool seekable) ;
	virtual bool get_media_format(NXOUT NXSDKMediaFormat& mediaFormat) ;
	virtual bool start() ;
	virtual bool stop() ;
	virtual bool seek(float time) ;
	virtual bool stoped() ;
	virtual bool close() ;
	virtual bool get_next_buffer(NXOUT NXPtr<NXBuffer>& buffer, NXOUT NXSDKTrackType* track) ;
	virtual bool reachend() ;
private:
	bool read_buffer(NXOUT NXPtr<NXBuffer>& buffer, NXOUT NXSDKTrackType* trackType) ;
};

