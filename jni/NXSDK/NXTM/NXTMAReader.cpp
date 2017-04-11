/*
 * NXTMAReader.cpp
 *
 *  Created on: 2016年1月20日
 *      Author: yangyk
 */

#include "NXTMAReader.h"


NXTMAReader::NXTMAReader()
 : _asyncCounts(10)
{
	_fileObj 	= NULL ;
	_readBufer  = NULL ;
	_avdecoder  = NULL ;
	_avcontext  = NULL ;
	_scaleCtxt 	= NULL ;
	_avdcframe  = NULL ;
	_reachend   = false ;
	_seekable   = false ;
}

NXTMAReader::~NXTMAReader()
{

}

bool NXTMAReader::open(const char* filepath, NXSDKPixelFormat pixfmt, bool seekable)
{
	_seekable = seekable ;

	if(_fileObj)
	{
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "NXTMAReader::open Failure 1 !!");
		return false;
	}

	_fileObj = new NXFile(filepath);

	if(!_fileObj->exist())
	{
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "NXTMAReader::open Failure 2 !!");
		return false;
	}

	if(!_fileObj->open("rb"))
	{
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "NXTMAReader::open Failure 3 !!");
		return false;
	}

	if(sizeof(_fileHeader) != _fileObj->read((uint8_t*)&_fileHeader, sizeof(_fileHeader)))
	{
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "NXTMAReader::open Failure 4 !!");
		return false;
	}

	int32_t firstPos = _fileObj->pos() ;
	if(firstPos < 0)
	{
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "NXTMAReader::open Failure 5 !!");
		return false;
	}

	//load all frame index
	if(_seekable)
	{
		MPTPacketHeader tempHeader ;
		uint32_t frameIndex = 0 ;
		do {
			if(sizeof(tempHeader) != _fileObj->read((uint8_t*)&tempHeader, sizeof(tempHeader)))
				break ;

			if(tempHeader.packet_track == NXSDKTrackVideo)
				_framesMap[frameIndex++] = (_fileObj->pos() - sizeof(tempHeader));

			_fileObj->seek(tempHeader.packet_length, false) ;
		}
		while(!_fileObj->ended()) ;

		_fileObj->seek(firstPos, true) ;
	}

	_avdecoder = avcodec_find_decoder(AV_CODEC_ID_H264) ;
	if(!_avdecoder) {
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Async Reader avcodec_find_decoder() Failure!!");
		return false ;
	}

	_avcontext = avcodec_alloc_context3(_avdecoder) ;
	if(!_avcontext) {
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Async Reader avcodec_alloc_context3() Failure!!");
		return false ;
	}

	if(avcodec_open2(_avcontext, _avdecoder, NULL) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Async Reader avcodec_open2() Failure!!");
		return false ;
	}

	_avdcframe = av_frame_alloc() ;
	if(!_avdcframe) {
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Async Reader avcodec_alloc_frame() Failure!!");
		return false ;
	}

	av_init_packet(&_avdcpacket);

	_outpixfmt = pixfmt ;
	_scaleCtxt = sws_getContext(_fileHeader.video_width, _fileHeader.video_height, (AVPixelFormat)NXKit::pixel_format_convert_nx(NXSDK_PIX_FMT_YUV420P),
									_fileHeader.video_width, _fileHeader.video_height, (AVPixelFormat)NXKit::pixel_format_convert_nx(_outpixfmt), SWS_BICUBIC, NULL, NULL, NULL);

	_readBufer = (uint8_t*)malloc(_fileHeader.video_width*_fileHeader.video_width*1.5);
	_reachend = false ;

	_asyncStatus = NXThreadUnknown ;

	return true ;
}

bool NXTMAReader::get_media_format(NXOUT NXSDKMediaFormat& mediaFormat)
{
	if(_fileObj == NULL)
		return false ;

	mediaFormat.track_mask 		= _fileHeader.track_masks ;
	mediaFormat.video_frames    = _fileHeader.video_frames ;
	mediaFormat.media_duration 	= _fileHeader.media_time ;

	mediaFormat.video_bit_rate  = 0 ;
	mediaFormat.video_pixfmt    = _outpixfmt ;
	mediaFormat.video_format.width = _fileHeader.video_width ;
	mediaFormat.video_format.height = _fileHeader.video_height ;
	mediaFormat.video_format.frame_rate = _fileHeader.frame_rate ;

	mediaFormat.audio_bit_rate  = 0 ;
	mediaFormat.audio_format.channels = _fileHeader.audio_channels ;
	mediaFormat.audio_format.sample_bits = _fileHeader.audio_smpbits ;
	mediaFormat.audio_format.sample_rate = _fileHeader.audio_smprate ;

	return true ;
}

bool NXTMAReader::start()
{
	if(_fileObj == NULL)
		return false ;

	_asyncStatus = NXThreadRunning ;
	_asyncThread = new NXThread(this, NULL) ;
	_asyncThread->start() ;

	return true ;
}

bool NXTMAReader::stop()
{
	if(_fileObj == NULL)
		return false ;

	_asyncStatus = NXThreadExiting ;
	_asyncCounts.fire() ;

	return true ;
}

bool NXTMAReader::seek(float time)
{
	if(!_seekable)
		return false ;

	if(_fileObj == NULL)
		return false ;

	uint32_t frameIndex = time*_fileHeader.frame_rate ;
	TMFramePosMap::iterator it = _framesMap.find(frameIndex) ;
	if(it == _framesMap.end())
		return false ;

	_seekLock.lock() ;

	_fileObj->seek(it->second, true) ;
	_reachend = false ;

	_asyncLocker.lock() ;
	uint32_t count = _asyncQueuer.size() ;
	_asyncQueuer.clear() ;
	_asyncLocker.unlock() ;

	for(uint32_t idx=0; idx<count; idx++)
		_asyncCounts.fire() ;

	_seekLock.unlock() ;

	return true ;
}

bool NXTMAReader::stoped()
{
	if(_fileObj == NULL)
		return false ;

	_asyncThread->stoped() ;
	_asyncThread = NULL ;

	return true ;
}

bool NXTMAReader::close()
{
	if(_avcontext) {
		avcodec_close(_avcontext) ;
		avcodec_free_context(&_avcontext) ;
		_avcontext = NULL ;
		_avdecoder = NULL ;
	}

	if(_avdcframe) {
		av_frame_free(&_avdcframe) ;
		_avdcframe = NULL ;

		av_free_packet(&_avdcpacket);
	}

	if(_scaleCtxt) {
		sws_freeContext(_scaleCtxt) ;
		_scaleCtxt = NULL ;
	}

	if(_readBufer) {
		free(_readBufer) ;
		_readBufer = NULL ;
	}

	if(_fileObj) {
		_fileObj->close() ;
		delete _fileObj ;
		_fileObj = NULL ;
	}

	_framesMap.clear();

	return true ;
}

bool NXTMAReader::get_next_buffer(NXOUT NXPtr<NXBuffer>& buffer, NXOUT NXSDKTrackType* track)
{
#ifdef PRINT_RD_TEMP_VIDEO_TIME
	uint32_t tick1 = NXKit::get_time_tick32() ;
#endif

	if(_fileObj == NULL)
		return false ;

	while(_asyncStatus == NXThreadRunning) {

		_asyncLocker.lock() ;
		uint32_t ccount = _asyncQueuer.size() ;
		_asyncLocker.unlock() ;

		if(ccount == 0 && _reachend)
			return false ;

		if(ccount > 0)
			break ;

		usleep(1000) ;
	}

	_asyncLocker.lock() ;

	TMBufferItem& bufferItem = _asyncQueuer.front() ;
	buffer = bufferItem.buffer ;
	*track = bufferItem.track ;
	_asyncQueuer.pop_front() ;

	_asyncLocker.unlock() ;

	_asyncCounts.fire() ;

#ifdef PRINT_RD_TEMP_VIDEO_TIME
	uint32_t tick2 = NXKit::get_time_tick32() ;
	__android_log_print(ANDROID_LOG_DEBUG, "NXTM", "TMP4 Async Reader get_next_buffer() cost ticks = %u !!", tick2-tick1);
#endif

	return true ;
}

bool NXTMAReader::reachend()
{
	return _reachend ;
}

bool NXTMAReader::read_buffer(NXOUT NXPtr<NXBuffer>& buffer, NXOUT NXSDKTrackType* trackType)
{
	if(_fileObj == NULL)
		return false ;

	if(_reachend)
		return false ;

	MPTPacketHeader pheader ;
	if(sizeof(pheader) != _fileObj->read((uint8_t*)&pheader, sizeof(pheader)))
	{
		_reachend = true ;
		return false ;
	}

	int32_t pkglen = pheader.packet_length ;
	int32_t redlen = 0 ;

	while(redlen < pkglen)
		redlen += _fileObj->read(_readBufer+redlen, pkglen-redlen);

	if(pheader.packet_track == NXSDKTrackVideo)
	{
		_avdcpacket.data = _readBufer ;
		_avdcpacket.size = pkglen ;

#ifdef PRINT_RD_TEMP_VIDEO_TIME
		uint32_t tickd1 = NXKit::get_time_tick32();
#endif

        int gotframe = 0 ;
        int retlen = avcodec_decode_video2(_avcontext, _avdcframe, &gotframe, &_avdcpacket);
        if(retlen < 0) {
        	__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Async Reader avcodec_decode_video2() Failure!!");
            return false ;
        }

        if(gotframe == 0) {
        	__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Async Reader avcodec_decode_video2() gotframe = 0!!");
        	return false ;
        }

#ifdef PRINT_RD_TEMP_VIDEO_TIME
		uint32_t tickd2 = NXKit::get_time_tick32();
		__android_log_print(ANDROID_LOG_DEBUG, "NXTM", "TMP4 Async Reader decode video cost ticks = %u !!", tickd2-tickd1);
#endif

		buffer = new NXBuffer(_fileHeader.video_width, _fileHeader.video_height, _outpixfmt) ;
		int pixelsize = _fileHeader.video_width*_fileHeader.video_height ;

		switch(_outpixfmt) {
			case NXSDK_PIX_FMT_YUV420P:
			{
				uint8_t* ousbox[4] = {buffer->getPlanePtr(0), buffer->getPlanePtr(1), buffer->getPlanePtr(2), NULL} ;
				int 	 ousize[4] = {_fileHeader.video_width, _fileHeader.video_width/2, _fileHeader.video_width/2, 0} ;
				int ret = sws_scale((SwsContext*)_scaleCtxt, _avdcframe->data, _avdcframe->linesize, 0, _fileHeader.video_height, ousbox, ousize);
				assert(ret == _fileHeader.video_height) ;
				break ;
			}
			case NXSDK_PIX_FMT_NV12:
			case NXSDK_PIX_FMT_NV21:
			{
				uint8_t* ousbox[4] = {buffer->getPlanePtr(0), buffer->getPlanePtr(1), NULL} ;
				int 	 ousize[4] = {_fileHeader.video_width, _fileHeader.video_width, 0} ;
				int ret = sws_scale((SwsContext*)_scaleCtxt, _avdcframe->data, _avdcframe->linesize, 0, _fileHeader.video_height, ousbox, ousize);
				assert(ret == _fileHeader.video_height) ;
				break ;
			}
			case NXSDK_PIX_FMT_ARGB:
			case NXSDK_PIX_FMT_RGBA:
			case NXSDK_PIX_FMT_ABGR:
			case NXSDK_PIX_FMT_BGRA:
			{
				uint8_t* ousbox[4] = {buffer->getPlanePtr(0), NULL} ;
				int 	 ousize[4] = {_fileHeader.video_width*4, 0} ;
				int ret = sws_scale((SwsContext*)_scaleCtxt, _avdcframe->data, _avdcframe->linesize, 0, _fileHeader.video_height, ousbox, ousize);
				assert(ret == _fileHeader.video_height) ;
				break ;
			}
		}
	}
	else if(pheader.packet_track >= NXSDKTrackAudio1 && pheader.packet_track <= NXSDKTrackAudio4)
	{
		uint32_t audioSamples = pheader.packet_length / (_fileHeader.audio_channels * _fileHeader.audio_smpbits / 8) ;
		buffer = new NXBuffer(audioSamples, _fileHeader.audio_smpbits, (uint8_t)_fileHeader.audio_channels) ;
		memcpy(buffer->getPlanePtr(0), _readBufer, pkglen) ;
	}

	*trackType = (NXSDKTrackType)pheader.packet_track ;

	return true ;
}

void NXTMAReader::on_thread_runing(const NXThread* thread, NXPtr<NXObject>& param)
{
	while(_asyncStatus == NXThreadRunning) {

		_asyncCounts.wait() ;

		if(_asyncStatus != NXThreadRunning)
			break ;

		NXPtr<NXBuffer> buffer = NULL ;
		NXSDKTrackType  trackr = NXSDKTrackNone ;

		_seekLock.lock() ;
		bool status = read_buffer(buffer, &trackr) ;
		if(status)
		{
			_asyncLocker.lock() ;

			TMBufferItem bufferItem ;
			bufferItem.buffer = buffer ;
			bufferItem.track  = trackr ;
			_asyncQueuer.push_back(bufferItem) ;

			_asyncLocker.unlock() ;
		}
		else if(this->reachend())
		{
			_asyncCounts.fire() ;
		}
		_seekLock.unlock() ;

		if(this->reachend())
			usleep(2000) ;

		if(_asyncStatus != NXThreadRunning)
			break ;
	}

	_asyncLocker.lock() ;
	_asyncQueuer.clear() ;
	_asyncLocker.unlock() ;

	_asyncStatus = NXThreadExited ;
}
