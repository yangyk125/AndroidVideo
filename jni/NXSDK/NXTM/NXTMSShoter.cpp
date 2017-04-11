/*
 * NXTMSShoter.cpp
 *
 *  Created on: 2016-1-4
 *      Author: yangyk
 */

#include "NXTMSShoter.h"


NXTMSShoter::NXTMSShoter()
{
	_fileObject = NULL ;
	_avdecoder  = NULL ;
	_avcontext  = NULL ;
	_scaleCtxt  = NULL ;
	_avdcframe  = NULL ;
	_readBufer  = NULL ;
}

NXTMSShoter::~NXTMSShoter()
{

}

bool NXTMSShoter::open(const char* filepath, NXSDKPixelFormat outfmt)
{
	if(_fileObject)
		return false ;

	_fileObject = new NXFile(filepath);

	if(!_fileObject->exist())
		return false ;

	if(!_fileObject->open("rb"))
		return false ;

	if(sizeof(_fileHeader) != _fileObject->read((uint8_t*)&_fileHeader, sizeof(_fileHeader)))
		return false ;

	int32_t firstPos = _fileObject->pos() ;
	if(firstPos < 0)
		return false ;

	//load all frame index
	MPTPacketHeader tempHeader ;
	uint32_t frameIndex = 0 ;
	do {
		if(sizeof(tempHeader) != _fileObject->read((uint8_t*)&tempHeader, sizeof(tempHeader)))
			break ;

		if(tempHeader.packet_track == NXSDKTrackVideo) {
			TMVideoFrame& frame = _framesMap[frameIndex++] ;
			frame.pos = (_fileObject->pos() - sizeof(tempHeader));
			frame.key = (tempHeader.packet_flags & MPT_FILE_PACKET_H264KEYFRAME) ;
		}

		_fileObject->seek(tempHeader.packet_length, false) ;
	}
	while(!_fileObject->ended()) ;

	_fileObject->seek(firstPos, true) ;

	_avdecoder = avcodec_find_decoder(AV_CODEC_ID_H264) ;
	if(!_avdecoder) {
		__android_log_print(ANDROID_LOG_ERROR, "NXST", "TMPT Sync Shoter avcodec_find_decoder() Failure!!");
		return false ;
	}

	_avcontext = avcodec_alloc_context3(_avdecoder) ;
	if(!_avcontext) {
		__android_log_print(ANDROID_LOG_ERROR, "NXST", "TMPT Sync Shoter avcodec_alloc_context3() Failure!!");
		return false ;
	}

	if(avcodec_open2(_avcontext, _avdecoder, NULL) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXST", "TMPT Sync Shoter avcodec_open2() Failure!!");
		return false ;
	}

	_avdcframe = av_frame_alloc() ;
	if(!_avdcframe) {
		__android_log_print(ANDROID_LOG_ERROR, "NXST", "TMPT Sync Shoter avcodec_alloc_frame() Failure!!");
		return false ;
	}

	av_init_packet(&_avdcpacket);

	_outpixfmt = outfmt ;
	_scaleCtxt = sws_getContext(_fileHeader.video_width, _fileHeader.video_height, (AVPixelFormat)NXKit::pixel_format_convert_nx(NXSDK_PIX_FMT_YUV420P),
								_fileHeader.video_width, _fileHeader.video_height, (AVPixelFormat)NXKit::pixel_format_convert_nx(_outpixfmt), SWS_BICUBIC, NULL, NULL, NULL);

	_readBufer = (uint8_t*)malloc(_fileHeader.video_width*_fileHeader.video_width*1.5);
	return true ;
}

bool NXTMSShoter::get_media_format(NXOUT NXSDKMediaFormat& mediaFormat)
{
	if(_fileObject == NULL)
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

bool NXTMSShoter::shot(float time, bool exact, NXPtr<NXBuffer>& buffer)
{
	int32_t frmIndex = time*_fileHeader.frame_rate ;
	frmIndex = MIN(frmIndex, _fileHeader.video_frames) ;
	frmIndex = MAX(frmIndex, 0) ;

	TMFrameKeyMap::iterator cur = _framesMap.find(frmIndex) ;
	for(TMFrameKeyMap::iterator it = cur;
			it != _framesMap.begin();
			it --)
	{
		if(it->second.key > 0)
		{
			if(!exact || it == cur)
				return shot_iterator(it, buffer, true) ;
			else
			{
				shot_iterator(it, buffer, false) ;
				return shot_iterator(cur, buffer, true) ;
			}
		}
	}

	TMFrameKeyMap::iterator it = _framesMap.begin() ;
	if(it->second.key > 0)
	{
		return shot_iterator(it, buffer, true) ;
	}

	__android_log_print(ANDROID_LOG_ERROR, "NXST", "TMPT Sync Shoter shot none image !!");
	return false ;
}

bool NXTMSShoter::close()
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

	if(_fileObject) {
		_fileObject->close() ;
		delete _fileObject ;
		_fileObject = NULL ;
	}

	_framesMap.clear();
	return true ;
}

bool NXTMSShoter::shot_iterator(TMFrameKeyMap::iterator& it, NXPtr<NXBuffer>& buffer, bool fillbuffer)
{
	_fileObject->seek(it->second.pos, true) ;

	MPTPacketHeader tempHeader ;
	_fileObject->read((uint8_t*)&tempHeader, sizeof(tempHeader)) ;

	int32_t pkglen = tempHeader.packet_length ;
	int32_t redlen = 0 ;

	while(redlen < pkglen)
		redlen += _fileObject->read(_readBufer+redlen, pkglen-redlen);

	_avdcpacket.data = _readBufer ;
	_avdcpacket.size = pkglen ;

#ifdef PRINT_RD_TEMP_VIDEO_TIME
		uint32_t tickd1 = NXKit::get_time_tick32();
#endif

    int gotframe = 0 ;
    int retlen = avcodec_decode_video2(_avcontext, _avdcframe, &gotframe, &_avdcpacket);
    if(retlen < 0) {
    	__android_log_print(ANDROID_LOG_ERROR, "NXST", "TMPT Sync Shoter avcodec_decode_video2() Failure!!");
        return false ;
    }

    if(gotframe == 0) {
    	__android_log_print(ANDROID_LOG_ERROR, "NXST", "TMPT Sync Shoter avcodec_decode_video2() gotframe = 0!!");
    	return false ;
    }

#ifdef PRINT_RD_TEMP_VIDEO_TIME
		uint32_t tickd2 = NXKit::get_time_tick32();
		__android_log_print(ANDROID_LOG_DEBUG, "NXST", "TMPT Sync Shoter decode video cost ticks = %u !!", tickd2-tickd1);
#endif

	if(fillbuffer)
	{
	    buffer = new NXBuffer(_fileHeader.video_width, _fileHeader.video_height, _outpixfmt) ;

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
			case NXSDK_PIX_FMT_BGRA: {
				uint8_t* ousbox[4] = {buffer->getPlanePtr(0), NULL} ;
				int 	 ousize[4] = {_fileHeader.video_width*4, 0} ;
				int ret = sws_scale((SwsContext*)_scaleCtxt, _avdcframe->data, _avdcframe->linesize, 0, _fileHeader.video_height, ousbox, ousize);
				assert(ret == _fileHeader.video_height) ;
				break ;
			}
		}
	}

	return true ;
}
