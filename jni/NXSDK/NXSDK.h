/*
 * NXSDK.h
 *
 *  Created on: 2015-9-14
 *      Author: yangyk
 */

#pragma once

#include <jni.h>

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <android/native_window_jni.h>

#include <queue>
#include <deque>
#include <vector>
#include <string>

//#include "../LEAKER/leaker.h"
#include <stdlib.h>

#include "NXGL/NXGL.h"
#include "NXGL/NXGLContext.h"
#include "NXGL/NXGLSurface.h"
#include "NXKT/NXDef.h"
#include "NXKT/NXFmt.h"
#include "NXKT/NXKit.h"
#include "NXKT/NXPtr.hpp"
#include "NXKT/NXLock.h"
#include "NXKT/NXFile.h"
#include "NXKT/NXTimer.h"
#include "NXKT/NXEvent.h"
#include "NXKT/NXBuffer.h"
#include "NXKT/NXThread.h"
#include "NXKT/NXThrottle.h"
#include "NXCC/NXCCNode.h"
#include "NXCC/NXCCSprite.h"
#include "NXCC/NXCCDirector.h"
#include "NXCC/NXCCSpriteNV12.h"
#include "NXCC/NXCCShareNode.h"
#include "NXWB/NXWorkbeach.h"


/*
 * media file reader
 */
interface NXSDKReader : public NXObject
{
	virtual bool open(const char* filepath, NXSDKPixelFormat pixfmt, bool seekable) = 0 ;
	virtual bool get_media_format(NXOUT NXSDKMediaFormat& mediaFormat) = 0 ;
	virtual bool start() = 0 ;
	virtual bool seek(float time) = 0 ;
	virtual bool stop() = 0 ;
	virtual bool stoped() = 0 ;
	virtual bool close() = 0 ;
	virtual bool get_next_buffer(NXOUT NXPtr<NXBuffer>& buffer, NXOUT NXSDKTrackType* track) = 0 ;
	virtual bool reachend() = 0 ;
} ;


/*
 * media file writer
 */
interface NXSDKWriter : public NXObject
{
	virtual bool open(const char* filepath, const char* suffix, NXIN NXSDKMediaFormat& mediaFormat) = 0 ;

	virtual bool start() = 0 ;
	virtual bool append_buffer(NXPtr<NXBuffer>& buffer,NXIN NXSDKTrackType track) = 0 ;

	virtual bool stop() = 0 ;
	virtual bool stoped() = 0 ;
	virtual bool close() = 0 ;
} ;

/*
 * video image shoter.
 */
interface NXSDKShoter : public NXObject
{
	virtual bool open(const char* filepath, NXSDKPixelFormat outfmt) = 0 ;
	virtual bool get_media_format(NXOUT NXSDKMediaFormat& mediaFormat) = 0 ;
	virtual bool shot(float time, bool exact, NXPtr<NXBuffer>& buffer) = 0 ; //exact = false, get nearest key frame, exact = true get exact frame.
	virtual bool close() = 0 ;

} ;

/*
 * media player
 */
typedef enum tag_NXSDKPlayerStatus {
    NXSDKPlayerUnknown = 0,
    NXSDKPlayerReady = 1,   //加载之后，数据已经准备好。
    NXSDKPlayerPlay  = 2,   //播放进行中
    NXSDKPlayerPause = 3,   //暂停
    NXSDKPlayerStop  = 4,   //暂停在开始.
    NXSDKPlayerEnd   = 5,   //结束
} NXSDKPlayerStatus ;

interface NXSDKPlayer ;
interface NXSDKPlayerListener : public NXCallback
{
	virtual void playerLoadCompleted(NXSDKPlayer* player, float duration) = 0 ;
	virtual void playerStatusChanged(NXSDKPlayer* player, NXSDKPlayerStatus status) = 0 ;
	virtual void playerPlayingProgess(NXSDKPlayer* player,NXPtr<NXBuffer>& vbuffer, float currentTime) = 0 ;
	virtual void playerErrorGenerated(NXSDKPlayer* player, uint32_t errorCode) = 0 ;
};

interface NXSDKPlayer : public NXObject
{
	//surface or listener maybe null.
	virtual bool open(const char* filepath, ANativeWindow* surface, NXSDKPlayerListener* listener, NXSDKPixelFormat pixfmt) = 0 ;
	virtual bool close() = 0 ;

	virtual bool set_auto_repeat(bool autorepeat) = 0 ;

	virtual bool get_media_format(NXOUT NXSDKMediaFormat& mediaFormat) = 0 ;

	virtual bool volume(float volume) = 0 ; //0.0-1.0

	virtual bool load(NXOUT NXPtr<NXBuffer>* firstImage = NULL) = 0 ;
	virtual bool unload() = 0 ;

	virtual bool play() = 0 ;
	virtual bool pause() = 0 ;
	virtual bool stop() = 0 ;

	virtual bool refresh() = 0 ;
	virtual bool seek(float time) = 0 ;
	virtual float time() = 0 ;
	virtual NXSDKPlayerStatus status() = 0 ;
} ;




/*
 * pixel buffer drawer
 */
typedef enum tag_NXSDKDrawType{
	NXSDKDrawError   = 0,
    NXSDKDrawTwist   = 1, //拉伸铺满
    NXSDKDrawCenter  = 2, //不拉伸高度或者宽度铺满，不够的地方黑色。
    NXSDKDrawCutout  = 3, //不拉伸覆盖完全铺满，多余的部分剪切掉。
} NXSDKDrawType ;

interface NXSDKDrawer : public NXObject
{
	virtual bool open(uint32_t width, uint32_t height, NXSDKPixelFormat pixfmt, ANativeWindow* surface) = 0 ;
	virtual bool mode(NXSDKDrawType mode) = 0 ;
	virtual bool rotate(float radian) = 0 ;
	virtual bool draw(NXPtr<NXBuffer>& vbuffer) = 0 ;
	virtual bool draw(NXPtr<NXBuffer>& vbuffer, const LPNXRect rect, uint32_t color) = 0 ;
	virtual bool close() = 0 ;
} ;


interface NXSDKCapturer : public NXObject
{
	virtual bool 	open() = 0 ;
	virtual NXSize 	size() = 0 ;
	virtual bool    start() = 0 ;
	virtual bool 	focus(NXPoint pt) = 0 ;
	virtual bool    pause(bool pause) = 0 ;
	virtual bool 	stop() = 0 ;
	virtual bool 	close() = 0 ;
	virtual NXSDKPixelFormat format() = 0 ;
	virtual NXPtr<NXBuffer> capture() = 0 ;
} ;

/*
 * audio speaker
 */
interface NXSDKSpeaker : public NXObject
{
	virtual bool open(const NXSDKAudioFormat& audioFormat, uint32_t samplesPerCache, uint32_t cacheCount) = 0 ;
	virtual bool volume(float volume) = 0 ;
	virtual bool start() = 0 ;
	virtual bool pause(bool pause) = 0 ;
	virtual bool speak(NXPtr<NXBuffer>& buffer) = 0 ;
	virtual bool stop() = 0 ;
	virtual bool close() = 0 ;
};


/*
 * audio recorder
 */

interface NXSDKRecorder : public NXObject
{
	virtual bool open(NXINOUT NXSDKAudioFormat& audioFormat, uint32_t samplesPerPacket, uint32_t cachePackets) = 0 ;
	virtual bool start() = 0 ;
	virtual bool pause(bool pause) = 0 ;
	virtual bool record_packet(NXOUT NXPtr<NXBuffer>& abuffer) = 0 ;
	virtual bool stop() = 0 ;
	virtual bool close() = 0 ;
} ;


/*
 * image trimer
 */
typedef struct tag_NXSDKTrimParam {
    NXRect          cutrange ; //从原始视频中剪切的范围，然后拉伸填满到目标区域。第一步执行。
    bool            hreverse ; //水平画面反向。
    bool            vreverse ; //垂直画面反向。
    float           rotate   ; //画面旋转弧度。0-2PI
} NXSDKTrimParam ;

interface NXSDKTrimer : public NXObject
{
	virtual bool open(const NXSize& sourceSize, const NXSize& targetSize, NXSDKPixelFormat infmt, NXSDKPixelFormat outfmt) = 0 ;
	virtual bool param(const NXSDKTrimParam& param) = 0 ;
	virtual bool trim(NXPtr<NXBuffer>& source, NXOUT NXPtr<NXBuffer>& result) = 0 ;
	virtual bool close() = 0 ;
};


interface NXSDKCanvas : public NXObject
{
	virtual bool open(const NXSize& viewport) = 0 ;
	virtual bool begin() = 0  ;
	virtual NXPtr<NXBuffer> end() = 0 ;
	virtual bool close() = 0 ;
};


/*
 * mpta ==> async read temp file (not stable)
 * mpts ==> sync  read temp file
 *
 * NULL ==> sync  read mp4  file
 */
NXPtr<NXSDKReader> NXSDK_CreateReader(const char* suffix = NULL) ;

/*
 * mpta ==> async write temp file
 * mpts ==> sync  write temp file (no support)
 * mp4a ==> async write mp4  file
 * mp4s ==> sync  write mp4  file
 */
NXPtr<NXSDKWriter> NXSDK_CreateWriter(const char* suffix = NULL) ;

/*
 * only support mpt
 */
NXPtr<NXSDKShoter> NXSDK_CreateShoter(const char* suffix = NULL) ;

NXPtr<NXSDKDrawer> NXSDK_CreateDrawer(const char* suffix = NULL) ;
NXPtr<NXSDKTrimer> NXSDK_CreateTrimer(const char* suffix = NULL) ;
NXPtr<NXSDKCanvas> NXSDK_CreateCanvas(const char* suffix = NULL) ;
NXPtr<NXSDKPlayer> NXSDK_CreatePlayer(const char* suffix = NULL) ;
NXPtr<NXSDKSpeaker> NXSDK_CreateSpeaker(const char* suffix = NULL) ;
NXPtr<NXSDKRecorder> NXSDK_CreateRecorder(const char* suffix = NULL) ;
NXPtr<NXSDKCapturer> NXSDK_CreateCapturer(const char* suffix = NULL) ;

