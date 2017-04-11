/*
 * NXFFSReader.cpp
 *
 *  Created on: 2015年9月17日
 *      Author: lixiang2015
 */

#include "NXFFSReader.h"

NXFFSReader::NXFFSReader()
{
	_pixelout  = AV_PIX_FMT_NONE ;
	_avcontext = NULL ;
	_reachend  = false ;

	_avcodecV  = NULL ;
	_avcodecA  = NULL ;
	_avstreamV = NULL ;
	_avstreamA = NULL ;

	_avframe  = NULL ;

	_avscaleV  = NULL ;
	_avscaleA  = NULL ;
}

NXFFSReader::~NXFFSReader()
{

}

bool NXFFSReader::open(const char* filepath, NXSDKPixelFormat pixfmt, bool seekable)
{
	_seekable 	= seekable ;
	_reachend   = false ;

	_videofile 	= filepath ;
	_pixelout   = (AVPixelFormat)NXKit::pixel_format_convert_nx(pixfmt) ;
	_mediaFmt.video_pixfmt = pixfmt ;

	_avcontext = avformat_alloc_context();
	if(_avcontext== NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader avformat_alloc_context() Failure !!");
		return false ;
	}

	if(avformat_open_input(&_avcontext, _videofile.c_str(), NULL, NULL) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader avformat_open_input() Failure !!");
		return false ;
	}

	for(int idx=0; idx<_avcontext->nb_streams; idx++)
	{
		AVStream* stream = _avcontext->streams[idx] ;
		if(stream->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			if(_avcodecV == NULL) {
				_avcodecV = avcodec_find_decoder(stream->codec->codec_id) ;
				_avstreamV = stream ;
			}
		}
		else if(stream->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			if(_avcodecA == NULL) {
				_avcodecA = avcodec_find_decoder(stream->codec->codec_id) ;
				_avstreamA = stream ;
			}
		}
	}

	if(_avcodecV == NULL || _avcodecA == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader Media File Unsupported !!");
		return false ;
	}

	if(avcodec_open2(_avstreamV->codec, _avcodecV, NULL) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Open Video Decoder Failure !!");
		return false ;
	}

	if(avcodec_open2(_avstreamA->codec, _avcodecA, NULL) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Open Audio Decoder Failure !!");
		return false ;
	}

	_mediaFmt.track_mask = NXSDKTrackVideo | NXSDKTrackAudio1 ;
	_mediaFmt.media_duration = 1.0f*_avstreamV->duration*_avstreamV->time_base.num/_avstreamV->time_base.den ;

	_mediaFmt.video_format.width = _avstreamV->codec->width ;
	_mediaFmt.video_format.height = _avstreamV->codec->height ;
	_mediaFmt.video_format.frame_rate = _avstreamV->avg_frame_rate.num/_avstreamV->avg_frame_rate.den ;
	_mediaFmt.video_frames = _avstreamV->nb_frames ;
	_mediaFmt.video_bit_rate = _avstreamV->codec->bit_rate ;

	_mediaFmt.audio_format.channels = _avstreamA->codec->channels ;
	_mediaFmt.audio_format.sample_bits = 16 ;//will scale to s16 format
	_mediaFmt.audio_format.sample_rate = _avstreamA->codec->sample_rate ;
	_mediaFmt.audio_bit_rate = _avstreamA->codec->bit_rate ;

	_avscaleV = sws_getContext(_mediaFmt.video_format.width, _mediaFmt.video_format.height, AV_PIX_FMT_YUV420P ,
							   _mediaFmt.video_format.width, _mediaFmt.video_format.height, _pixelout, SWS_BICUBIC , NULL, NULL, NULL);
	if(_avscaleV == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader sws_getContext() Failure!!");
		return false ;
	}

	int64_t channelLayout = (_mediaFmt.audio_format.channels == 1) ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO ;
	_avscaleA = swr_alloc_set_opts(NULL, channelLayout, AV_SAMPLE_FMT_S16, _mediaFmt.audio_format.sample_rate,
										 channelLayout, _avcodecA->sample_fmts[0], _mediaFmt.audio_format.sample_rate, 0, NULL);
	if(_avscaleA == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader swr_alloc_set_opts() Failure!!");
		return false ;
	}

	if(swr_init(_avscaleA) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader swr_init() Failure!!");
		return false ;
	}

	av_init_packet(&_avpacket);

	_avframe = av_frame_alloc() ;

	return true;
}

bool NXFFSReader::get_media_format(NXOUT NXSDKMediaFormat& mediaFormat)
{
	mediaFormat = _mediaFmt ;
	return true ;
}

bool NXFFSReader::start()
{
	return false ;
}

bool NXFFSReader::stop()
{
	return true ;
}

bool NXFFSReader::seek(float time)
{
	if(!_seekable)
		return false ;

	if(_reachend)
		return false ;

	//seek video
	int64_t videotime = time/(1.0f*_avstreamV->time_base.num/_avstreamV->time_base.den) ;
	if(0 < avformat_seek_file(_avcontext, _avstreamV->index, videotime, videotime, videotime, AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_ANY)) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader seek() for video  Failure!!");
		return false ;
	}

	//seek audio
	int64_t audiotime = time/(1.0f*_avstreamA->time_base.num/_avstreamA->time_base.den) ;
	if(0 < avformat_seek_file(_avcontext, _avstreamA->index, audiotime, audiotime, audiotime, AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_ANY)) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader seek() for audio Failure!!");
		return false ;
	}

	_reachend = false ;

	return true ;
}

bool NXFFSReader::stoped()
{
	return true ;
}

bool NXFFSReader::get_next_buffer(NXOUT NXPtr<NXBuffer>& buffer,NXOUT NXSDKTrackType* trackType)
{
	av_free_packet(&_avpacket) ;
	av_init_packet(&_avpacket) ;

	int retcode = av_read_frame(_avcontext, &_avpacket) ;
	if(retcode < 0) {
		if(retcode == AVERROR_EOF) {
			_reachend = true ;
			__android_log_print(ANDROID_LOG_WARN, "NXFF", "FFMPEG Sync Reader av_read_frame() End of file !!");
			return false ;
		}
		else {
			__android_log_print(ANDROID_LOG_WARN, "NXFF", "FFMPEG Sync Reader av_read_frame() Failure !!");
			return false ;
		}
	}

	if(_avpacket.stream_index == _avstreamV->index) {

#ifdef PRINT_DECODE_VIDEO_TIME
		uint32_t tickd1 = NXKit::get_time_tick32();
#endif
        int getframe = 0 ;
        int retlen = avcodec_decode_video2(_avstreamV->codec, _avframe, &getframe, &_avpacket);
        if(retlen < 0) {
        	__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader avcodec_decode_video2() Failure!!");
            return false ;
        }

    	if(trackType)
    		*trackType = NXSDKTrackVideo ;

        if(!getframe) {
        	__android_log_print(ANDROID_LOG_WARN, "NXFF", "FFMPEG Sync Reader decode video getframe = 0 !!");
            return false ;
        }

#ifdef PRINT_DECODE_VIDEO_TIME
		uint32_t tickd2 = NXKit::get_time_tick32();
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Sync Reader decode video cost ticks = %u !!", tickd2-tickd1);
#endif

    	buffer = new NXBuffer(_mediaFmt.video_format.width, _mediaFmt.video_format.height, _mediaFmt.video_pixfmt);

    	uint8_t* ouboxs[4] = {NULL} ;
    	int		 ousize[4] = {0} ;

    	switch(_pixelout) {
			case AV_PIX_FMT_YUV420P:
				ouboxs[0] = buffer->getPlanePtr(0) ;
				ouboxs[1] = buffer->getPlanePtr(1) ;
				ouboxs[2] = buffer->getPlanePtr(2) ;
				ousize[0] = _mediaFmt.video_format.width ;
				ousize[1] = _mediaFmt.video_format.width/2 ;
				ousize[2] = _mediaFmt.video_format.width/2 ;
				break ;
    		case AV_PIX_FMT_NV12:
    		case AV_PIX_FMT_NV21:
				ouboxs[0] = buffer->getPlanePtr(0) ;
				ouboxs[1] = buffer->getPlanePtr(1) ;
				ousize[0] = _mediaFmt.video_format.width ;
				ousize[1] = _mediaFmt.video_format.width ;
    			break ;
    		case AV_PIX_FMT_RGBA:
    		case AV_PIX_FMT_BGRA:
    		case AV_PIX_FMT_ARGB:
    		case AV_PIX_FMT_ABGR:
				ouboxs[0] = buffer->getPlanePtr(0) ;
				ousize[0] = _mediaFmt.video_format.width*4 ;
    			break ;
    	}

#ifdef PRINT_SCALE_VIDEO_TIME
		uint32_t ticks1 = NXKit::get_time_tick32() ;
#endif

    	int lines = sws_scale(_avscaleV, _avframe->data, _avframe->linesize, 0, _avframe->height, ouboxs, ousize);

#ifdef PRINT_SCALE_VIDEO_TIME
		uint32_t ticks2 = NXKit::get_time_tick32() ;
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Sync Reader Scale Video cost ticks = %u !!", ticks2-ticks1);
#endif

    	if(lines != _avframe->height) {
    		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader sws_scale() Failure!!");
    		return false ;
    	}
	}
	else if(_avpacket.stream_index == _avstreamA->index) {

        int got_frame = 0 ;
        int retlen = avcodec_decode_audio4(_avstreamA->codec, _avframe, &got_frame, &_avpacket);
        if(retlen < 0) {
        	__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader avcodec_decode_audio4() Failure!!");
            return false ;
        }

    	if(trackType)
    		*trackType = NXSDKTrackAudio1 ;

        if(!got_frame) {
        	__android_log_print(ANDROID_LOG_WARN, "NXFF", "FFMPEG Sync Reader decode audio getframe = 0 !!");
            return false ;
        }

        buffer = new NXBuffer(_avframe->nb_samples, 16, _mediaFmt.audio_format.channels);
        uint8_t* ouboxs[4] = {buffer->getPlanePtr(0), NULL} ;

#ifdef PRINT_RESMPLE_AUDIO_TIME
		uint32_t ticks1 = NXKit::get_time_tick32() ;
#endif
        retlen = swr_convert(_avscaleA, ouboxs, _avframe->nb_samples, (const uint8_t**)_avframe->data , _avframe->nb_samples);

#ifdef PRINT_RESMPLE_AUDIO_TIME
		uint32_t ticks2 = NXKit::get_time_tick32() ;
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Sync Reader Resmple Audio cost ticks = %u !!", ticks2-ticks1);
#endif

        if(retlen < 0) {
        	__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Sync Reader swr_convert() Failure!!");
            return false ;
        }
	}

	return true ;
}

bool NXFFSReader::reachend()
{
	return _reachend ;
}

bool NXFFSReader::close()
{
	if(_avcontext) {

		avcodec_close(_avstreamV->codec);
		avcodec_close(_avstreamA->codec);

		avformat_close_input(&_avcontext);

		av_frame_free(&_avframe);
		av_free_packet(&_avpacket);

		_avcontext = NULL ;

		_avstreamV = NULL ;
		_avstreamA = NULL ;

		_avcodecV  = NULL ;
		_avcodecA  = NULL ;
	}

	if(_avscaleV) {
		sws_freeContext(_avscaleV);
		_avscaleV = NULL ;
	}

	if(_avscaleA) {
		swr_close(_avscaleA);
		swr_free(&_avscaleA);
		_avscaleA = NULL ;
	}

	_videofile.clear() ;
	_pixelout = AV_PIX_FMT_NONE ;
	memset(&_mediaFmt, 0, sizeof(_mediaFmt)) ;

	return true;
}

