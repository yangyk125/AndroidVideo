/*
 * NXCameraThrottle.h
 *
 *  Created on: 2015-10-21
 *      Author: yangyk
 */

#pragma once

#include "NXDef.h"
#include "NXLock.h"
#include "NXPtr.hpp"
#include "NXBuffer.h"

/*
 * 上层CameraView得到的视频帧率并不一定稳定，并且帧率不可控。
 * 传入底层的时候，先经过此类中转，达到重新采样的效果，改变帧率。
 * 而此类的功能非常简单：两个线程的数据共享区，一个负责写，一个负责读，读写的速度不要求一样。
 */

class NXThrottle : public NXObject
{
private:
	NXPtr<NXBuffer>	_buffer ;
	NXLock			_locker ;
public:
	NXThrottle();
	virtual ~NXThrottle();
public:
	void push(NXPtr<NXBuffer>& data) ; //input
	void pop(NXPtr<NXBuffer>& data) ; //output
};


