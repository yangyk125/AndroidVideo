/*
 * NXWorkbeach.h
 *
 *  Created on: 2015-11-2
 *      Author: yangyk
 */

#pragma once

#include "../NXKT/NXDef.h"
#include "../NXKT/NXPtr.hpp"
#include "../NXKT/NXLock.h"
#include "../NXKT/NXEvent.h"

#include <map>
#include <deque>
#include <unistd.h>
#include <pthread.h>


//由workbeach调度的任务抽象接口。
class NXWorkitem : public NXCallback
{
private:
	int	 _status ; //0=ready, 1=running, 2=stoped
public:
	NXWorkitem();
	int  get_status();
	void set_status(int status) ;
public:
	virtual bool on_workitem_start() = 0 ; //return false means will stop ;
	virtual bool on_workitem_step() = 0 ; //return false means will stop ;
	virtual void on_workitem_stop() = 0 ;
} ;


//workbeach 异步处理消息的函数格式。
typedef void (NXObject::*WBHandleMessage)(NXPtr<NXObject>& param1, NXPtr<NXObject>& param2, void* param3, uint32_t param4, uint64_t param5, double param6) ;

class NXWorkbeach : public NXObject
{
private:
	NXLock								_wkbchMsgmapLock  ;
	std::map<uint32_t, WBHandleMessage>	_wkbchMessageMap  ;

	std::deque< NXPtr<NXObject> >		_wkbchMsgHigQueue ;
	std::deque< NXPtr<NXObject> >		_wkbchMsgNrmQueue ;
	std::deque< NXPtr<NXWorkitem> >		_wkbchWkitemQueue ;

	NXLock								_wkbchMsgHigLockr ;
	NXLock								_wkbchMsgNrmLockr ;
	NXLock								_wkbchWkitemLockr ;

	pthread_t							_wkbchMsgThread  ;
	NXThreadStatus						_wkbchMsgStatus  ;
public:
	NXWorkbeach();
	virtual ~NXWorkbeach();
private:
	static void* workbeach_thread_proc(void* context) ;
	void workbeach_thread_run();
public:
	//等级注册一个消息和一个消息的处理函数。
	void registerMessage(uint32_t msg, WBHandleMessage handleMethod) ;
	void unregisterMessage(uint32_t msg) ;
	//发送一个通知和通知相关的参数。
	void postHighMessasge(uint32_t msg, NXObject* obj,  NXPtr<NXObject>* param1 = NULL, NXPtr<NXObject>* param2 = NULL, void* param3 = NULL, uint32_t param4 = 0, uint64_t param5 = 0, double param6 = 0.0f) ;
	void postNormalMessasge(uint32_t msg, NXObject* obj,  NXPtr<NXObject>* param1 = NULL, NXPtr<NXObject>* param2 = NULL, void* param3 = NULL, uint32_t param4 = 0, uint64_t param5 = 0, double param6 = 0.0f) ;
	//同步执行通知处理函数，处理完才返回。
	void sendHighMessasge(uint32_t msg, NXObject* obj,  NXPtr<NXObject>* param1 = NULL, NXPtr<NXObject>* param2 = NULL, void* param3 = NULL, uint32_t param4 = 0, uint64_t param5 = 0, double param6 = 0.0f) ;
	void sendNormalMessasge(uint32_t msg, NXObject* obj,  NXPtr<NXObject>* param1 = NULL, NXPtr<NXObject>* param2 = NULL, void* param3 = NULL, uint32_t param4 = 0, uint64_t param5 = 0, double param6 = 0.0f) ;
	//撤销所有和obj相关的消息。
	void cleanObjectMessage(NXObject* obj) ;
	void cleanQueueMessage(uint32_t msg) ;

	//添加异步处理任务
	void addWorkerMission(NXPtr<NXWorkitem>& workitem) ;
	void delWorkerMission(NXPtr<NXWorkitem>& workitem) ; //强制移除一个workitem，不做任何处理。
};


extern NXWorkbeach theWorkbeach ;

