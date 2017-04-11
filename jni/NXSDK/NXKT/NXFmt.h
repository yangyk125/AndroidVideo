/*
 * NXFmt.h
 *
 *  Created on: 2015-11-2
 *      Author: yangyk
 */

#pragma once

#include "NXDef.h"

typedef enum tag_NXSDKPixelFormat {
	NXSDK_PIX_FMT_NONE = -1,
	NXSDK_PIX_FMT_YUV420P = 1 , ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
	NXSDK_PIX_FMT_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
	NXSDK_PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped
	NXSDK_PIX_FMT_ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
	NXSDK_PIX_FMT_RGBA,      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    NXSDK_PIX_FMT_ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
	NXSDK_PIX_FMT_BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
} NXSDKPixelFormat ;



typedef struct tag_NXSDKAudioFormat {
    uint8_t         channels	; //
    uint16_t        sample_rate	; //
    uint16_t        sample_bits	; //
} NXSDKAudioFormat;


typedef struct tag_NXSDKVideoFormat {
    uint16_t        width    	; //
    uint16_t        height  	; //
    uint16_t        frame_rate  ; //
} NXSDKVideoFormat;


typedef enum   tag_NXSDKTrackType {
    NXSDKTrackNone   = 0,
    NXSDKTrackVideo  = 0x01,
    NXSDKTrackAudio1 = 0x02,
    NXSDKTrackAudio2 = 0x04,
    NXSDKTrackAudio3 = 0x08,
    NXSDKTrackAudio4 = 0x10,
} NXSDKTrackType;


typedef struct tag_NXSDKMediaFormat {
	uint32_t			track_mask ;
	uint32_t			video_frames ;
	float 				media_duration ;
	float 				video_bit_rate ;
	float 				audio_bit_rate ;
	NXSDKAudioFormat	audio_format ;
	NXSDKVideoFormat	video_format ;
	NXSDKPixelFormat	video_pixfmt ;
}NXSDKMediaFormat;
