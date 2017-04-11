/*
 * NXFFAWriter.cpp
 *
 *  Created on: 2015-12-1
 *      Author: yangyk
 */

#include "NXFFAWriter.h"

NXFFAWriter::NXFFAWriter()
{
	_outformat = NULL ;
	_avcontext = NULL ;
	_avcodecV  = NULL ;
	_avcodecA  = NULL ;
	_avstreamV = NULL ;
	_avstreamA = NULL ;
	_avframeV  = NULL ;
	_avframeA  = NULL ;
	_avscaleV  = NULL ;
	_audififo  = NULL ;
	_vpktcount = 0 ;
	_apktcount = 0 ;
	_stsThread = NXThreadUnknown ;
}

NXFFAWriter::~NXFFAWriter()
{

}

bool NXFFAWriter::open(const char* filepath, const char* suffix, NXIN NXSDKMediaFormat& mediaFormat)
{
	_filepath = filepath ;
	_nxformat = mediaFormat ;
	_pxFormat = (AVPixelFormat)NXKit::pixel_format_convert_nx(mediaFormat.video_pixfmt) ;

	_yuvBuffer = new NXBuffer(mediaFormat.video_format.width, mediaFormat.video_format.height, NXSDK_PIX_FMT_YUV420P);

	_outformat = av_guess_format(suffix, NULL, NULL);
	if(_outformat == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer av_guess_format(%s) Failure!!", suffix);
		return false;
	}
	_avcontext = avformat_alloc_context();
	if(_avcontext== NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avformat_alloc_context() Failure!!");
		return false ;
	}
	int ret = avformat_alloc_output_context2(&_avcontext, _outformat, NULL, _filepath.c_str());
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avformat_alloc_output_context2() Failure!!");
		return false ;
	}

	if(_nxformat.track_mask & NXSDKTrackVideo) {
		_avcodecV = avcodec_find_encoder(AV_CODEC_ID_H264);
		if(_avcodecV == NULL) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avcodec_find_encoder(H264) Failure!!");
			return false ;
		}
	}

	if(_nxformat.track_mask & NXSDKTrackAudio1) {
		_avcodecA = avcodec_find_encoder(AV_CODEC_ID_AAC);
		if(_avcodecA == NULL) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avcodec_find_encoder(AAC) Failure!!");
			return false ;
		}
	}

	if((_nxformat.track_mask & NXSDKTrackVideo) && _avcodecV == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer V avcodec_find_encoder() Failure!!");
		return false ;
	}
	if((_nxformat.track_mask & NXSDKTrackAudio1) && _avcodecA == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer A avcodec_find_encoder() Failure!!");
		return false ;
	}

	_outformat->video_codec = _avcodecV->id ;
	if(_avcodecA)
		_outformat->audio_codec = _avcodecA->id ;

	_avstreamV = avformat_new_stream(_avcontext, _avcodecV);
	if(_avstreamV == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer V avformat_new_stream() Failure!!");
		return false ;
	}

	//quality setting !!
	_avstreamV->codec->flags   |= CODEC_FLAG_QSCALE ;
	_avstreamV->codec->qmin     = 0  ; //0是无损压缩。
	_avstreamV->codec->qmax     = 22 ; //20-30是比较合适的范围。
	_avstreamV->codec->bit_rate = 0  ;
	_avstreamV->codec->gop_size = _nxformat.video_format.frame_rate/2 ; //重要参数，编组长度 if set 0, means bit rate will controlling !!

	//other params setting
	_avstreamV->codec->width 	= _nxformat.video_format.width ;
	_avstreamV->codec->height 	= _nxformat.video_format.height ;
	_avstreamV->codec->time_base = av_make_q(1, _nxformat.video_format.frame_rate);
	_avstreamV->codec->pix_fmt 	= AV_PIX_FMT_YUV420P ; //h264 should be AV_PIX_FMT_YUV420P ;
	_avstreamV->codec->profile  = FF_PROFILE_H264_BASELINE ;
	_avstreamV->codec->max_b_frames = 0 ;

	if (_avcontext->oformat->flags & AVFMT_GLOBALHEADER)
		_avstreamV->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (avcodec_open2(_avstreamV->codec, _avcodecV, NULL)) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer V avcodec_open2() Failure!!");
		return false;
	}

	if(_avcodecA)
	{
		_avstreamA = avformat_new_stream(_avcontext, _avcodecA);
		if(_avstreamA == NULL) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer A avformat_new_stream() Failure!!");
			return false ;
		}

		_avstreamA->codec->bit_rate       = 0 ;
		_avstreamA->codec->channels       = _nxformat.audio_format.channels ;
		_avstreamA->codec->sample_rate    = _nxformat.audio_format.sample_rate ;
		_avstreamA->codec->time_base  	  = av_make_q(1, _nxformat.audio_format.sample_rate) ;
		_avstreamA->codec->channel_layout = av_get_default_channel_layout(_nxformat.audio_format.channels);
		_avstreamA->codec->sample_fmt     = AV_SAMPLE_FMT_S16 ;

		if (_avcontext->oformat->flags & AVFMT_GLOBALHEADER)
			_avstreamA->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		if (avcodec_open2(_avstreamA->codec, _avcodecA, NULL) < 0) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer A avcodec_open2() Failure!!");
			return false;
		}
	}


	_avscaleV = sws_getContext(_nxformat.video_format.width, _nxformat.video_format.height, _pxFormat,
							   _nxformat.video_format.width, _nxformat.video_format.height, AV_PIX_FMT_YUV420P, SWS_BICUBIC , NULL, NULL, NULL);
	if(_avscaleV == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer sws_getContext() Failure!!");
		return false ;
	}

	//打开创建文件
	if (avio_open(&_avcontext->pb, _filepath.c_str(), AVIO_FLAG_WRITE) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avio_open() Failure!!");
		return false;
	}

	//写入头信息
	if (avformat_write_header(_avcontext, NULL) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avformat_write_header() Failure!!");
		return false;
	}

	_avframeV = av_frame_alloc();
	if (_avframeV == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer av_frame_alloc() Failure!!");
		return false;
	}

	if(_avcodecA) {
		_avframeA = av_frame_alloc();
		if (_avframeA == NULL) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer av_frame_alloc() Failure!!");
			return false;
		}
	}

	av_init_packet (&_vpacket);
	_vpacket.dts = 0 ;
	_vpacket.pts = 0 ;

	if(_avcodecA)
	{
		av_init_packet (&_apacket);
		_apacket.dts = 0 ;
		_apacket.pts = 0 ;
	}

	_audififo = av_audio_fifo_alloc(AV_SAMPLE_FMT_S16, _nxformat.audio_format.channels, _nxformat.audio_format.sample_rate/4);
	if (_audififo == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer av_audio_fifo_alloc() Failure!!");
		return false;
	}

	_vpktcount = 0 ;
	_apktcount = 0 ;

	return true ;
}

bool NXFFAWriter::start()
{
	if(!_wrtThread) {
		_stsThread = NXThreadRunning ;
		_wrtThread = new NXThread(this, NULL);
		_wrtThread->start() ;
		return true ;
	}
	return false ;
}

bool NXFFAWriter::append_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType track)
{
	if(_wrtThread == NULL)
		return false ;

	_wrtLocker.lock() ;

	FFBufferItem bufferItem ;
	bufferItem.buffer = buffer ;
	bufferItem.track  = track ;
	_wrtDequer.push_back(bufferItem) ;

	_wrtLocker.unlock() ;

	return true ;
}

void NXFFAWriter::on_thread_runing(const NXThread* thread, NXPtr<NXObject>& param)
{
	while(_stsThread == NXThreadRunning)
	{
		_wrtLocker.lock() ;
		uint32_t count = _wrtDequer.size() ;
		_wrtLocker.unlock() ;

		for(uint32_t idx=0; idx<count; idx++)
		{
			_wrtLocker.lock() ;
			FFBufferItem bufferItem = _wrtDequer.front() ;
			_wrtDequer.pop_front() ;
			_wrtLocker.unlock() ;

			if(!write_buffer(bufferItem.buffer, bufferItem.track)) {
				__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer write_buffer() Failure !!");
				return ;
			}
		}

		if(count == 0)
			usleep(1) ;
	}

	_wrtLocker.lock() ;
	uint32_t lastCount = _wrtDequer.size() ;
	for(uint32_t idx=0; idx<lastCount; idx++)
	{
		FFBufferItem bufferItem = _wrtDequer.front() ;
		_wrtDequer.pop_front() ;

		if(!write_buffer(bufferItem.buffer, bufferItem.track)) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer write_buffer() Failure !!");
			_wrtLocker.unlock() ;
			return ;
		}
	}
	_wrtLocker.unlock() ;

	if(!audio_trailer()) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer audio_trailer() Failure !!");
		return ;
	}

	bool flushv = true ;
	bool flusha = true ;

	while(flushv || flusha) {
		if(flushv)
			flushv = flush_buffer(NXSDKTrackVideo) ;
		if(flusha)
			flusha = flush_buffer(NXSDKTrackAudio1) ;
	}

	if(av_write_trailer(_avcontext) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer av_write_trailer() Failure !!");
		return ;
	}

	avio_flush(_avcontext->pb) ;

	if(avformat_flush(_avcontext) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avformat_flush() Failure !!");
		return ;
	}

	if(avio_close(_avcontext->pb) < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avio_close() Failure !!");
		return ;
	}

	__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Async Writer Thread Exit Success !!");
}

bool NXFFAWriter::write_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType trackType)
{
	if(trackType == NXSDKTrackVideo)
	{
		buffer->lockBuffer();

		uint8_t* insbox[4] = {NULL};
		int 	 insize[4] = {0};

		switch (_pxFormat) {
			case AV_PIX_FMT_BGRA:
			case AV_PIX_FMT_ABGR:
			case AV_PIX_FMT_RGBA:
			case AV_PIX_FMT_ARGB:
				insbox[0] = buffer->getPlanePtr(0) ;
				insize[0] = _nxformat.video_format.width * 4 ;
				break ;
			case AV_PIX_FMT_NV21:
			case AV_PIX_FMT_NV12:
				insbox[0] = buffer->getPlanePtr(0) ;
				insbox[1] = buffer->getPlanePtr(1) ;
				insize[0] = _nxformat.video_format.width ;
				insize[1] = _nxformat.video_format.width ;
				break ;
			case AV_PIX_FMT_YUV420P:
				insbox[0] = buffer->getPlanePtr(0);
				insbox[1] = buffer->getPlanePtr(1);
				insbox[2] = buffer->getPlanePtr(2);
				insize[0] = _nxformat.video_format.width;
				insize[1] = _nxformat.video_format.width >> 1 ;
				insize[2] = _nxformat.video_format.width >> 1 ;
				break ;
		}

		uint8_t* outbox[4] = {_yuvBuffer->getPlanePtr(0), _yuvBuffer->getPlanePtr(1), _yuvBuffer->getPlanePtr(2), NULL};
		int 	 ousize[4] = {_nxformat.video_format.width, _nxformat.video_format.width >>1 , _nxformat.video_format.width>> 1, 0};

#ifdef PRINT_SCALE_VIDEO_TIME
		uint32_t ticks1 = NXKit::get_time_tick32() ;
#endif

		int ret = sws_scale(_avscaleV, insbox, insize, 0, _nxformat.video_format.height, outbox, ousize);

#ifdef PRINT_SCALE_VIDEO_TIME
		uint32_t ticks2 = NXKit::get_time_tick32() ;
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Async Writer Scale Video cost ticks = %u !!", ticks2-ticks1);
#endif

		buffer->unlockBuffer() ;

		if(ret != _nxformat.video_format.height) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer sws_scale() failure!!");
			return false ;
		}

		_avframeV->data[0] = outbox[0];
		_avframeV->data[1] = outbox[1];
		_avframeV->data[2] = outbox[2];

		_avframeV->linesize[0] = _nxformat.video_format.width;
		_avframeV->linesize[1] = _nxformat.video_format.width >> 1;
		_avframeV->linesize[2] = _nxformat.video_format.width >> 1;

		av_free_packet(&_vpacket);
		av_init_packet(&_vpacket);

		_vpacket.pts = _vpktcount ;
		_vpacket.dts = _vpacket.pts ;
		_vpktcount ++ ;

#ifdef PRINT_ENCODE_VIDEO_TIME
		uint32_t tick1 = NXKit::get_time_tick32() ;
#endif

		int got_packet = 0;
		ret = avcodec_encode_video2(_avstreamV->codec, &_vpacket, _avframeV, &got_packet);
		if(ret < 0) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avcodec_encode_video2() failure!!");
			return false;
		}
		if(!got_packet) {
			__android_log_print(ANDROID_LOG_WARN, "NXFF", "FFMPEG Async Writer avcodec_encode_video2() got_packet=0 !!");
			return true;
		}

#ifdef PRINT_ENCODE_VIDEO_TIME
		uint32_t tick2 = NXKit::get_time_tick32() ;
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Async Writer encode Video cost ticks = %u !!", tick2-tick1);
#endif

		_vpacket.stream_index = _avstreamV->index ;

		//int ret = av_write_frame(_avcontext, &_apacket); //no reorder the packets
		ret = av_interleaved_write_frame(_avcontext, &_vpacket);
		if(ret < 0)
		{
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer av_write_frame() failure, v packet size = %d!!", _vpacket.size);
			return false;
		}

#ifdef DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Async Writer append_buffer() Video success!!");
#endif
	}
	else if(trackType == NXSDKTrackAudio1)
	{
		buffer->lockBuffer() ;

		uint8_t* ptrbuf  = buffer->getPlanePtr(0) ;
		int 	 alllen  = buffer->getPlaneLength(0) ;
		int      smplen  = (_nxformat.audio_format.channels*_nxformat.audio_format.sample_bits/8) ;
		int 	 allsmp  = alllen/smplen ;

		//fill data into buffer
		int retsmp = av_audio_fifo_write(_audififo, (void**)&ptrbuf, allsmp) ;
		if(retsmp != allsmp) {
			__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer A av_audio_fifo_write() failure!!");
			return false;
		}

		int maxsmps = _avstreamA->codec->frame_size ;

		NXPtr<NXBuffer> ptrtmp = new NXBuffer(maxsmps * smplen);

		//while to read data from buffer
		while(av_audio_fifo_size(_audififo) > maxsmps)
		{
			uint8_t* pptrtmp = ptrtmp->getPlanePtr(0) ;
			uint32_t pptrlen = ptrtmp->getPlaneLength(0) ;

			if(maxsmps != av_audio_fifo_read(_audififo, (void**)&pptrtmp, maxsmps)) {
				__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer A av_audio_fifo_read() failure!!");
				return false;
			}

			_avframeA->channels = _nxformat.audio_format.channels ;
			_avframeA->sample_rate = _nxformat.audio_format.sample_rate ;
			_avframeA->nb_samples  = maxsmps ;

			if (_avstreamA->codec->sample_fmt == AV_SAMPLE_FMT_S16) {
				_avframeA->data[0] = pptrtmp ;
				_avframeA->data[1] = NULL ;
				_avframeA->linesize[0] = pptrlen ;
				_avframeA->linesize[1] = 0 ;
			}
			else {
				__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer A unsupported PCM Format !!!");
				return false ;
			}

			av_free_packet(&_apacket);
			av_init_packet(&_apacket);

			_apacket.pts = _apktcount ;
			_apacket.dts = _apacket.pts ;
			_apktcount += maxsmps ;

#ifdef PRINT_ENCODE_AUDIO_TIME
		uint32_t tick1 = NXKit::get_time_tick32() ;
#endif

			int got_packet = 0;
			int retlen = avcodec_encode_audio2(_avstreamA->codec, &_apacket, _avframeA, &got_packet);

			if (retlen < 0) {
				__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer avcodec_encode_audio2() failure error = %d", retlen);
				return false;
			}
			if (!got_packet) {
				__android_log_print(ANDROID_LOG_WARN, "NXFF", "FFMPEG Async Writer avcodec_encode_audio2() got_packet=0 !!");
				return true;
			}

#ifdef PRINT_ENCODE_AUDIO_TIME
		uint32_t tick2 = NXKit::get_time_tick32() ;
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Async Writer encode Audio cost ticks = %u !!", tick2-tick1);
#endif

			_apacket.stream_index = _avstreamA->index;

			//int ret = av_write_frame(_avcontext, &_apacket); //no reorder the packets
			int ret = av_interleaved_write_frame(_avcontext, &_apacket); //do reorder the packets
			if (ret < 0) {
				__android_log_print(ANDROID_LOG_ERROR, "NXFF", "FFMPEG Async Writer av_write_frame() failure, a packet size = %d!!", _apacket.size);
				return false;
			}
		}

#ifdef DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Async Writer append_buffer() Audio success!!");
#endif
	}
	return true ;
}

bool NXFFAWriter::audio_trailer()
{
	int smplen  = (_nxformat.audio_format.channels*_nxformat.audio_format.sample_bits/8) ;
	int fifosmp = av_audio_fifo_size(_audififo) ;

	NXPtr<NXBuffer> ptrtmp = new NXBuffer(fifosmp*smplen);

	uint8_t* pptrtmp = ptrtmp->getPlanePtr(0) ;
	uint32_t pptrlen = ptrtmp->getPlaneLength(0) ;

	if(fifosmp != av_audio_fifo_read(_audififo, (void**)&pptrtmp, fifosmp))
		return false ;

	_avframeA->channels = _nxformat.audio_format.channels ;
	_avframeA->sample_rate = _nxformat.audio_format.sample_rate ;
	_avframeA->nb_samples  = fifosmp ;

	if (_avstreamA->codec->sample_fmt == AV_SAMPLE_FMT_S16) {
		_avframeA->data[0] = pptrtmp ;
		_avframeA->data[1] = NULL ;
		_avframeA->linesize[0] = pptrlen ;
		_avframeA->linesize[1] = 0 ;
	}
	else {
		return false ;
	}

	av_free_packet(&_apacket);
	av_init_packet(&_apacket);

	_apacket.pts = _apktcount ;
	_apacket.dts = _apacket.pts ;
	_apktcount += fifosmp ;

	int got_packet = 0 ;
	int retlen = avcodec_encode_audio2(_avstreamA->codec, &_apacket, _avframeA, &got_packet);

	if (retlen < 0) {
		return false ;
	}
	if (!got_packet) {
		return false ;
	}

	_apacket.stream_index = _avstreamA->index;

	//int ret = av_write_frame(_avcontext, &_apacket); //no reorder the packets
	int ret = av_interleaved_write_frame(_avcontext, &_apacket); //do reorder the packets
	if (ret < 0)
		return false ;

	return true ;
}

bool NXFFAWriter::flush_buffer(NXSDKTrackType track)
{
	if(track == NXSDKTrackVideo)
	{
		int got_packet = 0;
		int ret_length = avcodec_encode_video2(_avstreamV->codec, &_vpacket, NULL, &got_packet);
		if(ret_length < 0)
			return false ;

		if(!got_packet)
			return false ;

		_vpacket.stream_index = _avstreamV->index ;

		ret_length = av_interleaved_write_frame(_avcontext, &_vpacket);
		if(ret_length < 0)
		{
			return false ;
		}

#ifdef DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Async Writer Flush Video success!!");
#endif
		return true ;
	}
	else if(track == NXSDKTrackAudio1)
	{
		av_free_packet(&_apacket);
		av_init_packet(&_apacket);

		int got_packet = 0 ;
		int retlen = avcodec_encode_audio2(_avstreamA->codec, &_apacket, NULL, &got_packet);
		if (retlen < 0)
			return false ;

		if (!got_packet)
			return false ;

		int ret = av_interleaved_write_frame(_avcontext, &_apacket); //do reorder the packets
		if (ret < 0)
			return false ;

#ifdef DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, "NXFF", "FFMPEG Async Writer Flush Audio success!!");
#endif
		return true ;
	}

	return false ;
}


bool NXFFAWriter::stop()
{
	_stsThread = NXThreadExiting ;

	return true ;
}

bool NXFFAWriter::stoped()
{
	_wrtThread->stoped() ;
	_wrtThread = NULL ;
	_stsThread = NXThreadExited ;

	return true ;
}

bool NXFFAWriter::close()
{
	memset(&_nxformat, 0, sizeof(_nxformat)) ;
	_pxFormat = AV_PIX_FMT_NONE ;

	_filepath.clear() ;
	_yuvBuffer = NULL ;

	_outformat = NULL ;//global var, no delete ;

	if(_avcontext)
	{
		avcodec_close(_avstreamV->codec) ;
		avcodec_close(_avstreamA->codec) ;

		avformat_free_context(_avcontext) ;
		_avcontext = NULL ;

		av_free_packet(&_vpacket);
		av_free_packet(&_apacket);

		_avcodecV  = NULL ;
		_avcodecA  = NULL ;

		_avstreamV = NULL ;
		_avstreamA = NULL ;
	}

	if(_avframeV)
	{
		av_frame_free(&_avframeV) ;
		_avframeV = NULL ;
	}

	if(_avframeA)
	{
		av_frame_free(&_avframeA) ;
		_avframeA = NULL ;
	}

	if(_avscaleV)
	{
		sws_freeContext(_avscaleV) ;
		_avscaleV = NULL ;
	}

	if(_audififo)
	{
		av_audio_fifo_free(_audififo);
		_audififo = NULL ;
	}

	return true ;
}


