/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "CCAutoreleasePool.h"
#include "ccMacros.h"

NS_CC_BEGIN

static CCAutoreleasePool* s_pShareReleasePool = NULL;

CCAutoreleasePool::CCAutoreleasePool(void)
{

}

CCAutoreleasePool::~CCAutoreleasePool(void)
{
	for(std::deque<CCObject*>::iterator it = _objArray.begin();
			it != _objArray.end();
			it ++)
	{
		(*it)->release() ;
	}
	_objArray.clear();
}

CCAutoreleasePool* CCAutoreleasePool::sharePool()
{
	if(s_pShareReleasePool == NULL)
		s_pShareReleasePool = new CCAutoreleasePool();
	return s_pShareReleasePool ;
}

void CCAutoreleasePool::addObject(CCObject* pObject)
{
	_objArray.push_back(pObject) ;
}

void CCAutoreleasePool::delObject(CCObject *pObject)
{
	//m_pObjectArray->removeObject(pObject, false) ;
}

void CCAutoreleasePool::loopOnce()
{
	int deals = 5 ;

	if(_objArray.size() <= deals)
	{
		for(std::deque<CCObject*>::iterator it = _objArray.begin();
				it != _objArray.end();
				it ++)
		{
			if((*it)->m_uReference == 1)
			{
				(*it)->release() ;
				it = _objArray.erase(it) ;
			}
			else
				it ++ ;
		}
	}
	else
	{
		for(int idx=0; idx<deals; idx++)
		{
			CCObject* obj = _objArray.front() ;
			_objArray.pop_front() ;

			if(obj->m_uReference == 1)
				obj->release() ;
			else
				_objArray.push_back(obj) ;
		}
	}
}


NS_CC_END
