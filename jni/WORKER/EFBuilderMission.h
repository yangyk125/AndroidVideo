/*
 * EFBuilderMission.h
 *
 *  Created on: 2015-12-21
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"


class EFBuildProgress ;
class EFBuilderMission : public NXWorkitem
{
private:
	NXPtr<EFBuildProgress> 		_buildProgrs ;
	NXSDKMediaFormat 			_mediaFormat ;
	NXSDKMediaFormat 			_recrdFormat ;
	NXCCSpriteNV12*				_canvasLayer ;
	std::vector<std::string>	_recordSgmts ;
	uint32_t					_recordCount ;
	std::string 				_recordFolder ;
	std::string					_buildrVideo ;

	NXPtr<NXSDKWriter> 			_buildWriter ;
	NXPtr<NXSDKCanvas>			_buildCanvas ;

	NXPtr<NXSDKReader> 			_buildReader ;
	NXSDKMediaFormat			_segmtFormat ;
	int							_buildIndex  ;
	int							_buildFrames ;

public:
	EFBuilderMission(NXPtr<EFBuildProgress>& buildProgrs, std::string& recordFolder, NXSDKMediaFormat& mediaFormat, NXSDKMediaFormat& recordFormat, std::vector<std::string>& recordSgmts, NXCCSpriteNV12* canvasLayer);
	virtual ~EFBuilderMission();
public:
	virtual bool on_workitem_start() ; //return false means will stop ;
	virtual bool on_workitem_step() ; //return false means will stop ;
	virtual void on_workitem_stop() ;
};

