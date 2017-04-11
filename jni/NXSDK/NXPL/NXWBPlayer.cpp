/*
 * NXWBPlayer.cpp
 *
 *  Created on: 2015-11-23
 *      Author: yangyk
 */

#include "NXWBPlayer.h"
#include "../NXWB/NXWorkbeach.h"

static uint32_t NXWBPlayerCount = 0 ;

#define NX_SDK_WORKBEACH_PLAYER_MESSAGE 	0x4A000000

NXWBPlayer::NXWBPlayer()
{
	_natvWndow = NULL ;
	_plistener = NULL ;
	_sdkReader = NULL ;
	_sdkSpeakr = NULL ;
	_sdkDrawer = NULL ;
	_curntTime = NXTimeMake(0, 1) ;
	_playState = NXSDKPlayerUnknown ;
	_autoRepeat = false ;

	NXWBPlayerCount ++ ;
	_wbMessage = NX_SDK_WORKBEACH_PLAYER_MESSAGE + NXWBPlayerCount ;

	theWorkbeach.registerMessage(_wbMessage, static_cast<WBHandleMessage>(&NXWBPlayer::on_timer_calling)) ;
}

NXWBPlayer::~NXWBPlayer()
{
	_natvWndow = NULL ;
	_plistener = NULL ;
	_sdkReader = NULL ;
	_sdkSpeakr = NULL ;
	_sdkDrawer = NULL ;
	_curntTime = NXTimeMake(0, 1) ;
	_playState = NXSDKPlayerUnknown ;

	theWorkbeach.unregisterMessage(_wbMessage) ;
}

bool NXWBPlayer::open(const char* filepath, ANativeWindow* surface, NXSDKPlayerListener* listener, NXSDKPixelFormat pixfmt)
{
	_videoFile = filepath ;
	_natvWndow = surface ;
	_pixelFmat = pixfmt ;
	_plistener = listener ;
	_curntTime = NXTimeMake(0, 1) ;

	return true ;
}

bool NXWBPlayer::close()
{
	_natvWndow = NULL ;
	_plistener = NULL ;
	_sdkReader = NULL ;
	_sdkSpeakr = NULL ;
	_sdkDrawer = NULL ;
	_curntTime = NXTimeMake(0, 1) ;
	_playState = NXSDKPlayerUnknown ;

	return true ;
}

bool NXWBPlayer::set_auto_repeat(bool autorepeat)
{
	_autoRepeat = autorepeat ;
	return true ;
}

bool NXWBPlayer::get_media_format(NXOUT NXSDKMediaFormat& mediaFormat)
{
	mediaFormat = _mediaFmat;
	return true ;
}
bool NXWBPlayer::volume(float volume) //0.0-1.0
{
	return true ;
}
bool NXWBPlayer::load(NXPtr<NXBuffer>* firstImage /*= NULL*/)
{
	size_t pos = _videoFile.rfind('.');
	std::string suffix = _videoFile.substr(pos + 1, _videoFile.size() - pos - 1);

	if (suffix.compare("mpt") == 0)
	{
		_sdkReader = NXSDK_CreateReader("mpta");
	}
	else
	{
		_sdkReader = NXSDK_CreateReader("mp4s");
	}

	_sdkReader->open(_videoFile.c_str(), _pixelFmat, true) ;
	_sdkReader->get_media_format(_mediaFmat) ;
	_sdkReader->start() ;

	_curntTime = NXTimeMake(0, _mediaFmat.video_format.frame_rate) ;

	if(_natvWndow) {
		_sdkDrawer = NXSDK_CreateDrawer(NULL) ;
		_sdkDrawer->open(_mediaFmat.video_format.width, _mediaFmat.video_format.height, _pixelFmat, _natvWndow) ;
	}

	_sdkSpeakr = NXSDK_CreateSpeaker(NULL) ;
	_sdkSpeakr->open(_mediaFmat.audio_format, _mediaFmat.audio_format.sample_rate/_mediaFmat.video_format.frame_rate, 16) ;
	_sdkSpeakr->start() ;
	_sdkSpeakr->pause(true) ;

	_sdkTimer = new NXTimer() ;
	_sdkTimer->open(this, NXTimeMake(1, _mediaFmat.video_format.frame_rate), true) ;
	_sdkTimer->start() ;

	_playState = NXSDKPlayerReady ;
	if(_plistener) {
		_plistener->playerLoadCompleted(this, _mediaFmat.media_duration) ;
		_plistener->playerStatusChanged(this, NXSDKPlayerReady) ;
	}

	refresh_now_frame(firstImage);

	return true ;
}
bool NXWBPlayer::unload()
{
	_playState = NXSDKPlayerUnknown ;

	if(_sdkTimer) {
		_sdkTimer->stop() ;
		_sdkTimer->stoped() ;
		_sdkTimer = NULL ;
	}

	if(_sdkDrawer) {
		_sdkDrawer->close() ;
		_sdkDrawer = NULL ;
	}

	if(_sdkSpeakr) {
		_sdkSpeakr->stop() ;
		_sdkSpeakr->close() ;
		_sdkSpeakr = NULL ;
	}

	if(_sdkReader) {
		_sdkReader->stop() ;
		_sdkReader->stoped() ;
		_sdkReader->close() ;
		_sdkReader = NULL ;
	}

	return true ;
}
bool NXWBPlayer::play()
{
	_playState = NXSDKPlayerPlay ;
	if(_sdkSpeakr)
		_sdkSpeakr->pause(false) ;
	if(_plistener)
		_plistener->playerStatusChanged(this, _playState) ;
	return true ;
}
bool NXWBPlayer::pause()
{
	_playState = NXSDKPlayerPause ;
	theWorkbeach.cleanQueueMessage(_wbMessage) ;

	if(_sdkSpeakr)
		_sdkSpeakr->pause(true) ;
	if(_plistener)
		_plistener->playerStatusChanged(this, _playState) ;
	return true ;
}
bool NXWBPlayer::stop()
{
	_playState = NXSDKPlayerStop ;
	theWorkbeach.cleanQueueMessage(_wbMessage) ;

	if(_sdkSpeakr)
		_sdkSpeakr->pause(true) ;
	_sdkReader->seek(0) ;
	_curntTime.value = 0 ;
	refresh_now_frame() ;
	if(_plistener)
		_plistener->playerStatusChanged(this, _playState) ;
	return true ;
}

bool NXWBPlayer::refresh()
{
	refresh_now_frame();
	return true ;
}

bool NXWBPlayer::seek(float time)
{
	if(_playState == NXSDKPlayerPlay)
		pause();
	_sdkReader->seek(time) ;
	_curntTime.value = time*_mediaFmat.video_format.frame_rate ;
	refresh_now_frame() ;
	return true ;
}

float NXWBPlayer::time()
{
	return 1.0f*_curntTime.value/_curntTime.scale ;
}

NXSDKPlayerStatus NXWBPlayer::status()
{
	return _playState ;
}

void NXWBPlayer::on_timer_start(NXTimer* timer)
{}

void NXWBPlayer::on_timer_running(NXTimer* timer)
{
	if(_playState == NXSDKPlayerPlay)
		theWorkbeach.postHighMessasge(_wbMessage, this) ;
}

void NXWBPlayer::on_timer_stop(NXTimer* timer)
{}

void NXWBPlayer::play_next_frame()
{
	NXPtr<NXBuffer> buffer = NULL ;
	NXSDKTrackType  trackr = NXSDKTrackNone ;

	do {
		if(_sdkReader->get_next_buffer(buffer, &trackr))
		{
			if(trackr == NXSDKTrackVideo) {
				if(_plistener)
					_plistener->playerPlayingProgess(this, buffer, 1.0f*_curntTime.value/_curntTime.scale) ;
				if(_sdkDrawer)
					_sdkDrawer->draw(buffer) ;
				_curntTime.value ++ ;
				break ;
			}
			else if(trackr == NXSDKTrackAudio1) {
				if(_sdkSpeakr)
					_sdkSpeakr->speak(buffer) ;
			}
		}
		else if(_sdkReader->reachend())
		{
			if(_autoRepeat)
			{
				_sdkReader->seek(0) ;
				_curntTime.value = 0 ;
			}
			else
			{
				_playState = NXSDKPlayerEnd ;
				if(_plistener)
					_plistener->playerStatusChanged(this, _playState) ;
				 break ;
			}
		}
		else
			break ;
	} while(!_sdkReader->reachend()) ;
}

void NXWBPlayer::refresh_now_frame(NXPtr<NXBuffer>* ptrNowImage /*= NULL*/)
{
	NXPtr<NXBuffer> buffer = NULL ;
	NXSDKTrackType  trackr = NXSDKTrackNone ;

	float lastTime = 1.0f*_curntTime.value/_curntTime.scale ;
	do {
		if(_sdkReader->get_next_buffer(buffer, &trackr))
		{
			if(trackr == NXSDKTrackVideo) {
				if(_plistener)
					_plistener->playerPlayingProgess(this, buffer, 1.0f*_curntTime.value/_curntTime.scale) ;
				if(_sdkDrawer)
					_sdkDrawer->draw(buffer) ;
				if(ptrNowImage)
					*ptrNowImage = buffer ;
				break ;
			}
		}
		else
			break ;
	} while(!_sdkReader->reachend()) ;

	_sdkReader->seek(lastTime) ;
}

void NXWBPlayer::on_timer_calling(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	if(_playState == NXSDKPlayerPlay) {

#ifdef PRINT_PLAYER_FRAME_TIME
		uint32_t tick1 = NXKit::get_time_tick32() ;
#endif

		play_next_frame() ;

#ifdef PRINT_PLAYER_FRAME_TIME
		uint32_t tick2 = NXKit::get_time_tick32() ;
		__android_log_print(ANDROID_LOG_DEBUG, "NXPL", "WBPlayer play_next_frame() cost ticks = %u !!", tick2-tick1);
#endif

	}
}
