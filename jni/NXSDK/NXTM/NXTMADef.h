/*
 * NXTPDef.h
 *
 *  Created on: 2015-12-1
 *      Author: yangyk
 */

#pragma once


//YWM = yowo media, yuv420p => lz4 compress to save file, s16 audio => lz4 compress to save file

typedef struct tag_MPTFileHeader {
	char				headr_codec[4] ;//'M','P','T',' '
	float 				media_time  ;
	uint32_t 			track_masks ;

	uint32_t			video_width  ;
	uint32_t 			video_height ;
	uint32_t 			video_pixfmt ;
	uint32_t			video_frames ;
	uint32_t 			frame_rate   ;

	uint32_t         	audio_channels ; //
	uint32_t        	audio_smprate  ; //
	uint32_t        	audio_smpbits  ; //
} MPTFileHeader ;


typedef struct tag_MPTPacketHeader {
	uint32_t		packet_track  ;
	uint32_t		packet_flags  ;
	uint32_t 		packet_length ;
} MPTPacketHeader;

#define MPT_FILE_PACKET_H264KEYFRAME  0x1


typedef struct tag_TMBufferItem {
	NXPtr<NXBuffer> buffer ;
	NXSDKTrackType  track ;
} TMBufferItem ;

typedef struct tag_TMWriteItem {
	NXPtr<NXBuffer> bufer ;	//待写入的缓存, 如果为NULL，表示该帧的视频还没有压缩完，那么就会阻塞这一个缓存以及后面所有缓存的写入。
	NXSDKTrackType  track ; //所属轨道
	uint32_t		flags ;
	uint32_t		legth ;
} TMWriteItem ;

typedef std::deque<TMWriteItem> TMWBufferQueue ;

typedef std::map<int32_t, int32_t> TMFramePosMap ;


typedef struct tag_TMVideoFrame {
	int32_t		pos ;
	int32_t		key ;
} TMVideoFrame ;

typedef std::map<int32_t, TMVideoFrame> TMFrameKeyMap ;
