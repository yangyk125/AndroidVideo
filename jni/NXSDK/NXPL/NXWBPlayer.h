/*
 * NXWBPlayer.h
 *
 *  Created on: 2015-11-23
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

class NXWBPlayer : public NXSDKPlayer
				 , public NXTimerRunner
{
private:
	uint32_t 				_wbMessage ;
	std::string				_videoFile ;
	ANativeWindow*			_natvWndow ;
	NXSDKPlayerListener*	_plistener ;
	NXSDKPixelFormat		_pixelFmat ;
	NXSDKMediaFormat		_mediaFmat ;
	NXSDKPlayerStatus		_playState ;
	NXTime 					_curntTime ;

	NXPtr<NXTimer>			_sdkTimer  ;
	NXPtr<NXSDKReader>		_sdkReader ;
	NXPtr<NXSDKSpeaker>		_sdkSpeakr ;
	NXPtr<NXSDKDrawer>		_sdkDrawer ;

	bool 					_autoRepeat ;

public:
	NXWBPlayer();
	virtual ~NXWBPlayer();
public:
	virtual bool open(const char* filepath, ANativeWindow* surface, NXSDKPlayerListener* listener, NXSDKPixelFormat pixfmt) ;
	virtual bool close() ;
	virtual bool set_auto_repeat(bool autorepeat) ;
	virtual bool get_media_format(NXOUT NXSDKMediaFormat& mediaFormat) ;
	virtual bool volume(float volume) ; //0.0-1.0
	virtual bool load(NXPtr<NXBuffer>* firstImage = NULL) ;
	virtual bool unload() ;
	virtual bool play() ;
	virtual bool pause() ;
	virtual bool stop() ;
	virtual bool refresh();
	virtual bool seek(float time) ;
	virtual float time() ;
	virtual NXSDKPlayerStatus status() ;
public:
	virtual void on_timer_start(NXTimer* timer) ;
	virtual void on_timer_running(NXTimer* timer) ;
	virtual void on_timer_stop(NXTimer* timer) ;
public:
	void on_timer_calling(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6) ;
	void play_next_frame() ;
	void refresh_now_frame(NXPtr<NXBuffer>* ptrNowImage = NULL);
};

