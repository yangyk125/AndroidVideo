/*
 * NXTMSShoter.h
 *
 *  Created on: 2016-1-4
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"
#include "NXTMADef.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

class NXTMSShoter : public NXSDKShoter
{
private:
	NXFile*				_fileObject ;
	MPTFileHeader		_fileHeader ;
	NXSDKPixelFormat	_outpixfmt ;
	TMFrameKeyMap 		_framesMap ;

	AVCodec*			_avdecoder ;
	AVCodecContext*		_avcontext ;
	SwsContext*			_scaleCtxt ;

	AVFrame*			_avdcframe  ;
	AVPacket 			_avdcpacket ;
	uint8_t*			_readBufer ;
public:
	NXTMSShoter();
	virtual ~NXTMSShoter();
public:
	virtual bool open(const char* filepath, NXSDKPixelFormat outfmt) ;
	virtual bool get_media_format(NXOUT NXSDKMediaFormat& mediaFormat) ;
	virtual bool shot(float time, bool exact, NXPtr<NXBuffer>& buffer) ;
	virtual bool close() ;
protected:
	bool shot_iterator(TMFrameKeyMap::iterator& it, NXPtr<NXBuffer>& buffer, bool fillbuffer) ;
};

