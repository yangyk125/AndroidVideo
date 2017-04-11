/*
 * EFExtractMission.h
 *
 *  Created on: 2016-1-4
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

class EFExtractProgress ;
class EFExtractMission : public NXWorkitem
{
private:
	int 	_targetWidth  ;
	int 	_targetHeight ;
	int 	_targetCount ;
	int 	_targetIndex ;
	std::string			_videoFile ;
	NXSDKMediaFormat	_videoFmat ;
	float 				_timepiece ;
	NXPtr<NXSDKShoter>	_extShoter ;
	NXPtr<NXSDKTrimer>	_extTrimer ;
	NXPtr<EFExtractProgress> _extProgress ;
public:
	EFExtractMission(const char* videoFile, int width, int height, int count, NXPtr<EFExtractProgress>& progress);
	virtual ~EFExtractMission();
public:
	virtual bool on_workitem_start() ; //return false means will stop ;
	virtual bool on_workitem_step() ; //return false means will stop ;
	virtual void on_workitem_stop() ;
};

