/*
 * NXWorkbeach.cpp
 *
 *  Created on: 2015-11-2
 *      Author: yangyk
 */

#include "NXSDK.h"
#include "NXWorkbeach.h"

class NXMessageItem : public NXObject
{
public:
	uint32_t					messag ;
	NXObject*					object ;
	NXPtr<NXEvent>				pevent ;
	NXPtr<NXObject> 			param1 ;
	NXPtr<NXObject>  			param2 ;
	void*						param3 ;
	uint32_t 					param4 ;
	uint64_t 					param5 ;
	double 						param6 ;
public:
	NXMessageItem()
	{
		clean() ;
	}
	virtual ~NXMessageItem()
	{
		clean() ;
	}
private:
	void clean()
	{
		messag = 0 ;
		object = NULL ;
		pevent = NULL ;
		param1 = NULL ;
		param2 = NULL ;
		param3 = NULL ;
		param4 = 0 ;
		param5 = 0 ;
		param6 = 0 ;
	}
} ;

NXWorkitem::NXWorkitem()
 : _status(0)
{}

int NXWorkitem::get_status()
{
	return _status ;
}

void NXWorkitem::set_status(int status)
{
	_status = status ;
}

NXWorkbeach::NXWorkbeach()
{
	_wkbchMsgThread = 0 ;
	_wkbchMsgStatus = NXThreadRunning ;
	pthread_create(&_wkbchMsgThread, NULL, workbeach_thread_proc, this) ;
}

NXWorkbeach::~NXWorkbeach()
{
	_wkbchMsgStatus = NXThreadExiting ;
	while(_wkbchMsgStatus != NXThreadExited)
		usleep(5) ;
}

void* NXWorkbeach::workbeach_thread_proc(void* context)
{
	NXWorkbeach* workbeach = (NXWorkbeach*)context ;
	workbeach->workbeach_thread_run();
	return NULL ;
}

void NXWorkbeach::workbeach_thread_run()
{
	uint32_t procMsgCount = 0 ;
	uint32_t hignLevCount = 0 ;
	uint32_t normLevCount = 0 ;
	uint32_t wrkitemCount = 0 ;

	while(_wkbchMsgStatus == NXThreadRunning)
	{
#ifdef PRINT_WORKBEACH_LOOP_TIME
		uint32_t tick1 = NXKit::get_time_tick32() ;
#endif

		procMsgCount = 0 ;

		_wkbchMsgHigLockr.lock() ;
		hignLevCount = _wkbchMsgHigQueue.size() ;
		_wkbchMsgHigLockr.unlock() ;

		for(uint32_t idx=0; idx<hignLevCount; idx++)
		{
			procMsgCount ++ ;

			_wkbchMsgHigLockr.lock() ;
			NXPtr<NXObject>& firstItem = _wkbchMsgHigQueue.front() ;
			NXPtr<NXMessageItem> firstMesg = (NXPtr<NXMessageItem>&)firstItem  ;
			_wkbchMsgHigQueue.pop_front() ;
			_wkbchMsgHigLockr.unlock() ;


			WBHandleMessage function = NULL ;
			_wkbchMsgmapLock.lock() ;
			std::map<uint32_t, WBHandleMessage>::iterator index = _wkbchMessageMap.find(firstMesg->messag);
			if(index != _wkbchMessageMap.end())
				function = index->second ;
			_wkbchMsgmapLock.unlock() ;

			if(function)
				(firstMesg->object->*function)(firstMesg->param1, firstMesg->param2, firstMesg->param3, firstMesg->param4, firstMesg->param5, firstMesg->param6);

			if(firstMesg->pevent)
				firstMesg->pevent->fire() ;
		}

		_wkbchMsgNrmLockr.lock() ;
		normLevCount = _wkbchMsgNrmQueue.size() ;
		_wkbchMsgNrmLockr.unlock() ;

		if(normLevCount > 0)
		{
			procMsgCount ++ ;

			_wkbchMsgNrmLockr.lock() ;
			NXPtr<NXObject>& firstItem = _wkbchMsgNrmQueue.front() ;
			NXPtr<NXMessageItem> firstMesg = (NXPtr<NXMessageItem>&)firstItem ;
			_wkbchMsgNrmQueue.pop_front() ;
			_wkbchMsgNrmLockr.unlock() ;

			WBHandleMessage function = NULL ;
			_wkbchMsgmapLock.lock() ;
			std::map<uint32_t, WBHandleMessage>::iterator index = _wkbchMessageMap.find(firstMesg->messag);
			if(index != _wkbchMessageMap.end())
				function = index->second ;
			_wkbchMsgmapLock.unlock() ;

			if(function)
				(firstMesg->object->*function)(firstMesg->param1, firstMesg->param2, firstMesg->param3, firstMesg->param4, firstMesg->param5, firstMesg->param6);

			if(firstMesg->pevent)
				firstMesg->pevent->fire() ;
		}

		_wkbchWkitemLockr.lock() ;
		wrkitemCount = _wkbchWkitemQueue.size() ;
		_wkbchWkitemLockr.unlock() ;

		if(wrkitemCount > 0)
		{
			procMsgCount ++ ;

			_wkbchWkitemLockr.lock() ;
			NXPtr<NXWorkitem> firstItem = _wkbchWkitemQueue.front() ;
			_wkbchWkitemQueue.pop_front() ;
			_wkbchWkitemLockr.unlock() ;

			if(firstItem->get_status() == 0)
			{
				if(firstItem->on_workitem_start())
					firstItem->set_status(1) ;
				else
					firstItem->set_status(2) ;
			}
			else if(firstItem->get_status() == 1)
			{
				if(firstItem->on_workitem_step())
					firstItem->set_status(1) ;
				else
					firstItem->set_status(2) ;
			}

			if(firstItem->get_status() == 1)
			{
				_wkbchWkitemLockr.lock() ;
				_wkbchWkitemQueue.push_back(firstItem) ;
				_wkbchWkitemLockr.unlock() ;
			}
			else if(firstItem->get_status() == 2)
			{
				firstItem->on_workitem_stop() ;
			}
		}

		if(procMsgCount == 0)
			usleep(5) ;

#ifdef PRINT_WORKBEACH_LOOP_TIME
		uint32_t tick2 = NXKit::get_time_tick32() ;
		if(tick2-tick1 > 5)
			__android_log_print(ANDROID_LOG_DEBUG, "WORKBEACH", "Workbeach main runloop cost ticks=%u ..", tick2-tick1);
#endif

	}
	_wkbchMsgStatus = NXThreadExited ;
}

//等级注册一个消息和一个消息的处理函数。内部是异步注册的，函数立即返回。
void NXWorkbeach::registerMessage(uint32_t msg, WBHandleMessage handleMethod)
{
	_wkbchMsgmapLock.lock() ;
	_wkbchMessageMap[msg] = handleMethod ;
	_wkbchMsgmapLock.unlock() ;
}

void NXWorkbeach::unregisterMessage(uint32_t msg)
{
	_wkbchMsgmapLock.lock() ;
	std::map<uint32_t, WBHandleMessage>::iterator it = _wkbchMessageMap.find(msg) ;
	if(it != _wkbchMessageMap.end())
		_wkbchMessageMap.erase(it);
	_wkbchMsgmapLock.unlock() ;
}

//发送一个通知和通知相关的参数。
void NXWorkbeach::postHighMessasge(uint32_t msg, NXObject* obj,  NXPtr<NXObject>* param1, NXPtr<NXObject>* param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXPtr<NXMessageItem> pItem = new NXMessageItem() ;

	pItem->messag = msg ;
	pItem->object = obj ;
	if(param1)
		pItem->param1 = *param1 ;
	if(param2)
		pItem->param2 = *param2 ;
	pItem->param3 = param3 ;
	pItem->param4 = param4 ;
	pItem->param5 = param5 ;
	pItem->param6 = param6 ;

	_wkbchMsgHigLockr.lock() ;
	_wkbchMsgHigQueue.push_back(*((NXPtr<NXObject>*)&pItem)) ;
	_wkbchMsgHigLockr.unlock() ;
}

void NXWorkbeach::postNormalMessasge(uint32_t msg, NXObject* obj,  NXPtr<NXObject>* param1, NXPtr<NXObject>* param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXPtr<NXMessageItem> pItem = new NXMessageItem();

	pItem->messag = msg ;
	pItem->object = obj ;
	if(param1)
		pItem->param1 = *param1 ;
	if(param2)
		pItem->param2 = *param2 ;
	pItem->param3 = param3 ;
	pItem->param4 = param4 ;
	pItem->param5 = param5 ;
	pItem->param6 = param6 ;

	_wkbchMsgNrmLockr.lock() ;
	_wkbchMsgNrmQueue.push_back(*((NXPtr<NXObject>*)&pItem)) ;
	_wkbchMsgNrmLockr.unlock() ;
}

void NXWorkbeach::sendHighMessasge(uint32_t msg, NXObject* obj,  NXPtr<NXObject>* param1, NXPtr<NXObject>* param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXPtr<NXMessageItem> pItem = new NXMessageItem();

	pItem->messag = msg ;
	pItem->object = obj ;
	if(param1)
		pItem->param1 = *param1 ;
	if(param2)
		pItem->param2 = *param2 ;
	pItem->param3 = param3 ;
	pItem->param4 = param4 ;
	pItem->param5 = param5 ;
	pItem->param6 = param6 ;

	pItem->pevent = new NXEvent(0) ;

	_wkbchMsgHigLockr.lock() ;
	_wkbchMsgHigQueue.push_back(*((NXPtr<NXObject>*)&pItem)) ;
	_wkbchMsgHigLockr.unlock() ;

	pItem->pevent->wait() ;
}

void NXWorkbeach::sendNormalMessasge(uint32_t msg, NXObject* obj,  NXPtr<NXObject>* param1, NXPtr<NXObject>* param2, void* param3, uint32_t param4, uint64_t param5, double param6)
{
	NXPtr<NXMessageItem> pItem = new NXMessageItem();

	pItem->messag = msg ;
	pItem->object = obj ;
	if(param1)
		pItem->param1 = *param1 ;
	if(param2)
		pItem->param2 = *param2 ;
	pItem->param3 = param3 ;
	pItem->param4 = param4 ;
	pItem->param5 = param5 ;
	pItem->param6 = param6 ;

	pItem->pevent = new NXEvent(0) ;

	_wkbchMsgNrmLockr.lock() ;
	_wkbchMsgNrmQueue.push_back(*((NXPtr<NXObject>*)&pItem)) ;
	_wkbchMsgNrmLockr.unlock() ;

	pItem->pevent->wait() ;
}

void NXWorkbeach::cleanObjectMessage(NXObject* obj)
{
	_wkbchMsgHigLockr.lock() ;
	for(std::deque< NXPtr<NXObject> >::iterator it = _wkbchMsgHigQueue.begin(); it != _wkbchMsgHigQueue.end();)
	{
		NXPtr<NXObject>& item = *it ;
		NXPtr<NXMessageItem> pItem = *((NXPtr<NXMessageItem>*)&item) ;
		if(pItem->object == obj)
			it = _wkbchMsgHigQueue.erase(it);
		else
			it ++ ;
	}
	_wkbchMsgHigLockr.unlock() ;

	_wkbchMsgNrmLockr.lock() ;
	for(std::deque< NXPtr<NXObject> >::iterator it = _wkbchMsgNrmQueue.begin(); it != _wkbchMsgNrmQueue.end();)
	{
		NXPtr<NXObject>& item = *it ;
		NXPtr<NXMessageItem> pItem = *((NXPtr<NXMessageItem>*)&item) ;
		if(pItem->object == obj)
			it = _wkbchMsgNrmQueue.erase(it);
		else
			it ++ ;
	}
	_wkbchMsgNrmLockr.unlock() ;
}

void NXWorkbeach::cleanQueueMessage(uint32_t msg)
{
	_wkbchMsgHigLockr.lock() ;
	for(std::deque< NXPtr<NXObject> >::iterator it = _wkbchMsgHigQueue.begin(); it != _wkbchMsgHigQueue.end();)
	{
		NXPtr<NXObject>& item = *it ;
		NXPtr<NXMessageItem> pItem = *((NXPtr<NXMessageItem>*)&item) ;
		if(pItem->messag == msg)
			it = _wkbchMsgHigQueue.erase(it);
		else
			it ++ ;
	}
	_wkbchMsgHigLockr.unlock() ;

	_wkbchMsgNrmLockr.lock() ;
	for(std::deque< NXPtr<NXObject> >::iterator it = _wkbchMsgNrmQueue.begin(); it != _wkbchMsgNrmQueue.end();)
	{
		NXPtr<NXObject>& item = *it ;
		NXPtr<NXMessageItem> pItem = *((NXPtr<NXMessageItem>*)&item) ;
		if(pItem->messag == msg)
			it = _wkbchMsgNrmQueue.erase(it);
		else
			it ++ ;
	}
	_wkbchMsgNrmLockr.unlock() ;
}

void NXWorkbeach::addWorkerMission(NXPtr<NXWorkitem>& workitem)
{
	_wkbchWkitemLockr.lock() ;
	_wkbchWkitemQueue.push_back(workitem) ;
	_wkbchWkitemLockr.unlock() ;
}

void NXWorkbeach::delWorkerMission(NXPtr<NXWorkitem>& workitem)
{
	_wkbchWkitemLockr.lock() ;
	for(std::deque< NXPtr<NXWorkitem> >::iterator it = _wkbchWkitemQueue.begin(); it != _wkbchWkitemQueue.end();)
	{
		NXPtr<NXWorkitem>& item = *it ;
		if(item == workitem)
			it = _wkbchWkitemQueue.erase(it);
		else
			it ++ ;
	}
	_wkbchWkitemLockr.unlock() ;
}

//定义全局共享的工作台对象，即副消息循环。
NXWorkbeach theWorkbeach ;
