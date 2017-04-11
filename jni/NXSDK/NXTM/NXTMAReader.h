/*
 * NXTMAReader.h
 *
 *  Created on: 2016年1月20日
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

typedef std::deque<TMBufferItem> TMRAsyncDeque ;

class NXTMAReader : public NXSDKReader
				  , public NXThreadRunner
{
private:
	NXFile*				_fileObj  ;
	bool 				_seekable ;
	NXLock				_seekLock ;

	MPTFileHeader		_fileHeader ;
	NXSDKPixelFormat	_outpixfmt ;
	TMFramePosMap 		_framesMap ;

	AVCodec*			_avdecoder ;
	AVCodecContext*		_avcontext ;
	SwsContext*			_scaleCtxt ;

	AVFrame*			_avdcframe  ;
	AVPacket 			_avdcpacket ;

	uint8_t*			_readBufer ;
	bool 				_reachend  ;
private:
	NXPtr<NXThread>		_asyncThread ;
	NXThreadStatus		_asyncStatus ;
	TMRAsyncDeque		_asyncQueuer ;
	NXLock				_asyncLocker ;
	NXEvent				_asyncCounts ;
public:
	NXTMAReader();
	virtual ~NXTMAReader();
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
public:
	virtual void on_thread_runing(const NXThread* thread, NXPtr<NXObject>& param) ;
private:
	bool read_buffer(NXOUT NXPtr<NXBuffer>& buffer, NXOUT NXSDKTrackType* trackType) ;
};

