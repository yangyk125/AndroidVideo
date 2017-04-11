/*
 * EFRenderMission.h
 *
 *  Created on: 2015-11-25
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

class EFRenderProgress ;
class EFEffectsLayer ;
class EFRenderMission : public NXWorkitem
{
private:
	NXSDKMediaFormat			_renderFormat ;

	NXCCSpriteNV12* 			_renderLayer  ;
	EFEffectsLayer*				_renderEffect ;
	NXPtr<EFRenderProgress>	 	_renderProgrs ;
	std::string	 				_renderSource ;
	std::string	 				_renderTarget ;

	NXPtr<NXSDKReader>			_renderReader ;
	NXPtr<NXSDKWriter>			_renderWriter ;
	NXPtr<NXSDKCanvas>			_renderCanvas ;

	bool 						_renderNext   ;
	uint32_t					_renderCount  ;

public:
	EFRenderMission(const NXSDKMediaFormat& settingFormat, NXPtr<EFRenderProgress>& progress, NXCCSpriteNV12* drawLayer, EFEffectsLayer* effectLayer, const char* sourceFile, const char* targetFile);
	virtual ~EFRenderMission();
public:
	void render_begin(); //绘制开始
	bool render_next(); //是否还需要调用step
	void render_step(); //由消息队列调用
	void render_finish(); //最终结束。
public:
	virtual bool on_workitem_start() ; //return false means will stop ;
	virtual bool on_workitem_step() ; //return false means will stop ;
	virtual void on_workitem_stop() ;
};

