/*
 * NXFFAWriter.h
 *
 *  Created on: 2015-12-1
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"
#include "NXFFADef.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
}

typedef std::deque<FFBufferItem> FFWAsyncDeque ;

class NXFFAWriter : public NXSDKWriter
				  , public NXThreadRunner
{
private:
	std::string  		_filepath ;
	NXSDKMediaFormat	_nxformat ;
	AVPixelFormat		_pxFormat ;
	NXPtr<NXBuffer>		_yuvBuffer ;
	NXPtr<NXThread>		_wrtThread ;
	NXThreadStatus		_stsThread ;
	FFWAsyncDeque		_wrtDequer ;
	NXLock				_wrtLocker ;

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
	NXFFAWriter();
	virtual ~NXFFAWriter();
public:
	virtual bool open(const char* filepath, const char* suffix, NXIN NXSDKMediaFormat& mediaFormat) ;
	virtual bool start() ;
	virtual bool append_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType track) ;
	virtual bool stop() ;
	virtual bool stoped() ;
	virtual bool close() ;
public:
	virtual void on_thread_runing(const NXThread* thread, NXPtr<NXObject>& param) ;
private:
	bool write_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType trackType) ;
	bool audio_trailer() ;
	bool flush_buffer(NXSDKTrackType track) ;
};

