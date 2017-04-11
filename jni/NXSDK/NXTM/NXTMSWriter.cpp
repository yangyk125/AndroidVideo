/*
 * NXTMSWriter.cpp
 *
 *  Created on: 2015-12-2
 *      Author: yangyk
 */

#include "NXTMSWriter.h"

NXTMSWriter::NXTMSWriter()
{
	memset(&_fileHead, 0, sizeof(_fileHead)) ;
	memset(&_nxformat, 0, sizeof(_nxformat)) ;
	memset(&_x264Param, 0, sizeof(_x264Param)) ;
	_fileObjt  = NULL ;
	_avscaler  = NULL ;
	_frmcount  = 0 ;
	_x264Encoder = NULL ;
	_x264ImageI  = NULL ;
	_x264ImageO  = NULL ;
}

NXTMSWriter::~NXTMSWriter()
{

}

bool NXTMSWriter::open(const char* filepath, const char* suffix, NXIN NXSDKMediaFormat& mediaFormat)
{
	_filepath = filepath ;
	_nxformat = mediaFormat ;
	_pxFormat = (AVPixelFormat)NXKit::pixel_format_convert_nx(mediaFormat.video_pixfmt) ;

	_fileObjt = new NXFile(_filepath.c_str());
	if(_fileObjt->exist())
		_fileObjt->remove() ;

	if(!_fileObjt->open("wb")) {
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer Open File Failure!!");
		return false ;
	}

	memcpy(_fileHead.headr_codec, "MPT ", 4) ;

	_fileHead.media_time  = 0 ;
	_fileHead.track_masks = mediaFormat.track_mask ;

	_fileHead.video_frames = 0 ;
	_fileHead.frame_rate   = mediaFormat.video_format.frame_rate ;
	_fileHead.video_width  = mediaFormat.video_format.width ;
	_fileHead.video_height = mediaFormat.video_format.height ;
	_fileHead.video_pixfmt = NXSDK_PIX_FMT_YUV420P ;

	_fileHead.audio_channels = mediaFormat.audio_format.channels ;
	_fileHead.audio_smpbits  = mediaFormat.audio_format.sample_bits ;
	_fileHead.audio_smprate  = mediaFormat.audio_format.sample_rate ;

	//init x264 param
	x264_param_default(&_x264Param) ;
	x264_param_default_preset(&_x264Param, x264_preset_names[0], x264_tune_names[0]);
	x264_param_apply_profile(&_x264Param, x264_profile_names[0]); //baseline, it is fast !!

	_x264Param.i_threads 	= X264_THREADS_AUTO ; //多线程编码，速度快的保证。
	_x264Param.i_width   	= _fileHead.video_width ;
	_x264Param.i_height  	= _fileHead.video_height ;
	_x264Param.i_keyint_min = 0 ;
	_x264Param.i_keyint_max = _fileHead.frame_rate/2 ;
	_x264Param.i_bframe     = 0 ;
	_x264Param.b_open_gop   = 0 ;
	_x264Param.i_bframe_pyramid = 0 ;
	_x264Param.i_bframe_adaptive = X264_B_ADAPT_NONE ;
	_x264Param.i_fps_den    = 1 ;
	_x264Param.i_fps_num    = _fileHead.frame_rate ;
	_x264Param.i_timebase_den = 1 ;
	_x264Param.i_timebase_num = _fileHead.frame_rate ;

	_x264Param.rc.i_rc_method 	= X264_RC_CRF ; //X264_RC_CQP:固定压缩质量, X264_RC_CRF:固定码率, X264_RC_ABR:平均码率
	//_x264Param.rc.i_qp_constant = 20 ;
	_x264Param.rc.i_qp_max	  	= 20 ; //尽量保证原始视频质量。
	_x264Param.rc.i_qp_min    	= 0 ;
	_x264Param.rc.i_bitrate   	= 0 ; //不设置码率.

	_x264Encoder = x264_encoder_open(&_x264Param) ;
	if(!_x264Encoder) {
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer Get X264 Encoder Failure!!");
		return false ;
	}

	_x264ImageI = new x264_picture_t ;
	_x264ImageO = new x264_picture_t ;

	x264_picture_init(_x264ImageI);
	x264_picture_init(_x264ImageO);
	x264_picture_alloc(_x264ImageI, X264_CSP_I420, _x264Param.i_width, _x264Param.i_height) ;

	_x264ImageI->img.i_csp   = X264_CSP_I420 ;
	_x264ImageI->img.i_plane = 3 ;
	_x264ImageI->img.i_stride[0] = _x264Param.i_width ;
	_x264ImageI->img.i_stride[1] = _x264Param.i_width>>1 ;
	_x264ImageI->img.i_stride[2] = _x264Param.i_width>>1 ;

	_avscaler = sws_getContext(_nxformat.video_format.width, _nxformat.video_format.height, _pxFormat,
							   _nxformat.video_format.width, _nxformat.video_format.height, AV_PIX_FMT_YUV420P, SWS_BICUBIC , NULL, NULL, NULL);
	if(_avscaler == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer sws_getContext() Failure!!");
		return false ;
	}

	_frmcount  = 0 ;

	return true ;
}

bool NXTMSWriter::start()
{
	if(_fileObjt == NULL)
		return false ;

	if(sizeof(_fileHead) != _fileObjt->write((uint8_t*)&_fileHead, sizeof(_fileHead)))
		return false ;

	return true ;
}

bool NXTMSWriter::append_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType track)
{
	if(_fileObjt == NULL)
		return false ;

	return encode_buffer(buffer, track) ;
}

bool NXTMSWriter::encode_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType track)
{
	if(track == NXSDKTrackVideo)
	{
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

		uint8_t* outbox[4] = {_x264ImageI->img.plane[0], _x264ImageI->img.plane[1], _x264ImageI->img.plane[2], NULL};
		int 	 ousize[4] = {_nxformat.video_format.width, _nxformat.video_format.width >>1 , _nxformat.video_format.width>> 1, 0};

#ifdef PRINT_SCALE_VIDEO_TIME
		uint32_t ticks1 = NXKit::get_time_tick32() ;
#endif

		int ret = sws_scale(_avscaler, insbox, insize, 0, _nxformat.video_format.height, outbox, ousize);

#ifdef PRINT_SCALE_VIDEO_TIME
		uint32_t ticks2 = NXKit::get_time_tick32() ;
		__android_log_print(ANDROID_LOG_DEBUG, "NXTM", "TMP4 Sync Writer Scale Video cost ticks = %u !!", ticks2-ticks1);
#endif

		if(ret != _nxformat.video_format.height) {
			__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer sws_scale() failure!!");
			return false ;
		}

		_x264ImageI->i_pts = _frmcount ;
		_x264ImageI->i_dts = _frmcount ;

		_frmcount ++ ;

#ifdef PRINT_WT_TEMP_VIDEO_TIME
		uint32_t ticke1 = NXKit::get_time_tick32() ;
#endif

		x264_nal_t* sNal = NULL ;
		int	iNal = 0 ;

		if(x264_encoder_encode(_x264Encoder, &sNal, &iNal, _x264ImageI, _x264ImageO) < 0) {
			__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer x264_encoder_encode() failure !!");
			return false;
		}

		if(iNal < 0) {
			__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer x264_encoder_encode() Failure !!");
			return false ;
		}
		else if(iNal == 0) {
			__android_log_print(ANDROID_LOG_WARN, "NXTM", "TMP4 Sync Writer x264_encoder_encode() got_packet=0 !!");

			TMWriteItem writeItem ;
			writeItem.track = track ;
			writeItem.bufer = NULL ;
			writeItem.flags = 0 ;
			writeItem.legth = 0 ;

			_aswtQueue.push_back(writeItem) ;
		}
		else if(iNal > 0) {

#ifdef PRINT_WT_TEMP_VIDEO_TIME
			uint32_t ticke2 = NXKit::get_time_tick32() ;
			__android_log_print(ANDROID_LOG_DEBUG, "NXTM", "TMP4 Sync Writer encode Video cost ticks = %u !!", ticke2-ticke1);
#endif

			//先计算长度
			uint32_t x264Length = 0 ;
			for(int idx=0; idx<iNal; idx++)
				x264Length += sNal[idx].i_payload ;

			//创建等大缓存
			NXPtr<NXBuffer> x264Buffer = new NXBuffer(x264Length);

			//拷贝x264数据
			uint8_t* x264buf = x264Buffer->getPlanePtr(0) ;
			uint8_t* tempbuf = x264buf ;
			for(int idx=0; idx<iNal; idx++) {
				memcpy(tempbuf, sNal[idx].p_payload, sNal[idx].i_payload) ;
				tempbuf += sNal[idx].i_payload ;
			}

			//从队列里面找第一个缓存为空的item，然后赋值进去
			bool fillBufferToQueue = false ;
			for(TMWBufferQueue::iterator it=_aswtQueue.begin();
					it != _aswtQueue.end();
					++ it)
			{
				if(it->legth == 0){

					it->bufer = x264Buffer ;
					it->legth = x264Length ;

					if(_x264ImageO->b_keyframe)
						it->flags |= MPT_FILE_PACKET_H264KEYFRAME ;

					fillBufferToQueue = true ;

					//同时补充一个空白的buffer位置。
					TMWriteItem writeItem ;
					writeItem.track = track ;
					writeItem.bufer = NULL ;
					writeItem.flags = 0 ;
					writeItem.legth = 0 ;
					_aswtQueue.push_back(writeItem) ;

					break ;
				}
			}

			if(!fillBufferToQueue) { //没有找到空白的item，那么就只能追加到最后，几乎是没有这种情况的。

				TMWriteItem writeItem ;
				writeItem.track = track ;
				writeItem.flags = 0 ;
				writeItem.bufer = x264Buffer ;
				writeItem.legth = x264Length ;
				_aswtQueue.push_back(writeItem) ;

			}
		}

#ifdef DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, "NXTM", "TMP4 Sync Writer append_buffer() Video success!!");
#endif

	}
	else if(track == NXSDKTrackAudio1)
	{
		TMWriteItem writeItem ;
		writeItem.bufer = buffer ;
		writeItem.track = track ;
		writeItem.flags = 0 ;
		writeItem.legth = buffer->getPlaneLength(0) ;

		_aswtQueue.push_back(writeItem) ;
	}
	else {
		__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer buffer track not supported !!");
		return false ;
	}

	while(_aswtQueue.size() > 0) {

		if(_aswtQueue.front().legth == 0)
			break ;

		TMWriteItem firstItem = _aswtQueue.front() ;
		_aswtQueue.pop_front() ;

		MPTPacketHeader apacket ;

		apacket.packet_track  = firstItem.track ;
		apacket.packet_flags  = firstItem.flags ;
		apacket.packet_length = firstItem.legth ;
		_fileObjt->write((uint8_t*)&apacket, sizeof(apacket));

		uint8_t* bufptr = firstItem.bufer->getPlanePtr(0) ;
		int      wrilen = 0 ;

		while(wrilen < firstItem.legth)
			wrilen += _fileObjt->write(bufptr+wrilen, firstItem.legth-wrilen) ;

	}

	return true ;
}

bool NXTMSWriter::flush_buffers()
{
	int delayFrames = x264_encoder_delayed_frames(_x264Encoder);
	for(int idx=0; idx<delayFrames; idx++) {

		x264_nal_t* sNal = NULL ;
		int	iNal = 0 ;

		if(x264_encoder_encode(_x264Encoder, &sNal, &iNal, _x264ImageI, _x264ImageO) < 0) {
			__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer x264_encoder_encode() failure !!");
			return false;
		}

		if(iNal < 0) {
			__android_log_print(ANDROID_LOG_ERROR, "NXTM", "TMP4 Sync Writer x264_encoder_encode() Failure !!");
			return false ;
		}
		else if(iNal == 0) {
			__android_log_print(ANDROID_LOG_WARN, "NXTM", "TMP4 Sync Writer x264_encoder_encode() got_packet=0 !!");

			TMWriteItem writeItem ;
			writeItem.track = NXSDKTrackVideo ;
			writeItem.bufer = NULL ;
			writeItem.flags = 0 ;
			writeItem.legth = 0 ;

			_aswtQueue.push_back(writeItem) ;
		}
		else if(iNal > 0) {

			//先计算长度
			uint32_t x264Length = 0 ;
			for(int idx=0; idx<iNal; idx++)
				x264Length += sNal[idx].i_payload ;

			//创建等大缓存
			NXPtr<NXBuffer> x264Buffer = new NXBuffer(x264Length);

			//拷贝x264数据
			uint8_t* x264buf = x264Buffer->getPlanePtr(0) ;
			uint8_t* tempbuf = x264buf ;
			for(int idx=0; idx<iNal; idx++) {
				memcpy(tempbuf, sNal[idx].p_payload, sNal[idx].i_payload) ;
				tempbuf += sNal[idx].i_payload ;
			}

			//从队列里面找第一个缓存为空的item，然后赋值进去
			for(TMWBufferQueue::iterator it=_aswtQueue.begin();
				it != _aswtQueue.end();
				++ it)
			{
				if(it->legth == 0){

					it->bufer = x264Buffer ;
					it->legth = x264Length ;

					if(_x264ImageO->b_keyframe)
						it->flags |= MPT_FILE_PACKET_H264KEYFRAME ;

					break ;
				}
			}
		}

		while(_aswtQueue.size() > 0) {

			if(_aswtQueue.front().legth == 0)
				break ;

			TMWriteItem firstItem = _aswtQueue.front() ;
			_aswtQueue.pop_front() ;

			MPTPacketHeader apacket ;

			apacket.packet_track  = firstItem.track ;
			apacket.packet_flags  = firstItem.flags ;
			apacket.packet_length = firstItem.legth ;
			_fileObjt->write((uint8_t*)&apacket, sizeof(apacket));

			uint8_t* bufptr = firstItem.bufer->getPlanePtr(0) ;
			int      wrilen = 0 ;

			while(wrilen < firstItem.legth)
				wrilen += _fileObjt->write(bufptr+wrilen, firstItem.legth-wrilen) ;
		}

#ifdef DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, "NXTM", "TMP4 Sync Writer flush_buffer() Video Success, Last Buffer = %u !!", (uint32_t)_aswtQueue.size());
#endif
	}

	return true ;
}

bool NXTMSWriter::stop()
{
	return flush_buffers() ;
}

bool NXTMSWriter::stoped()
{
	_fileHead.video_frames = _frmcount ;
	_fileHead.media_time   = 1.0f*_frmcount/_fileHead.frame_rate ;

	_fileObjt->flush() ;

	_fileObjt->seek(0, true) ;
	_fileObjt->write((uint8_t*)&_fileHead, sizeof(_fileHead)) ;
	_fileObjt->flush() ;

	return true ;
}

bool NXTMSWriter::close()
{
	_filepath.clear();

	if(_fileObjt) {
		delete _fileObjt ;
		_fileObjt = NULL ;
	}

	_aswtQueue.clear() ;

	if(_x264Encoder) {
		x264_encoder_close(_x264Encoder) ;
		_x264Encoder = NULL ;
	}

	if(_x264ImageI) {
		x264_picture_clean(_x264ImageI);
		delete _x264ImageI ;
		_x264ImageI = NULL ;
	}

	if(_x264ImageO) {
		delete _x264ImageO ;
		_x264ImageO = NULL ;
	}

	if(_avscaler) {
		sws_freeContext(_avscaler) ;
		_avscaler = NULL ;
	}

	_frmcount = 0 ;

	return true ;
}
