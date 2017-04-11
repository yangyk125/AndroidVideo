/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 
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

#ifndef __CCNUMBER_H__
#define __CCNUMBER_H__

#include "CCObject.h"

NS_CC_BEGIN

/**
 * @addtogroup data_structures
 * @{
 * @js NA
 * @lua NA
 */

class CC_DLL CCNumber : public CCObject
{
protected:
	CCNumber() : m_dValue(0) {}
protected:
    double 		m_dValue ;
public:
    int32_t   	getInt32Value() 	{return (int32_t)m_dValue ;}
    int64_t   	getInt64Value() 	{return (int64_t)m_dValue ;}
    uint32_t   	getUInt32Value() 	{return (uint32_t)m_dValue ;}
    uint64_t   	getUInt64Value() 	{return (uint64_t)m_dValue ;}
    float   	getFloatValue() 	{return (float)m_dValue ;}
    double   	getDoubleValue() 	{return (double)m_dValue ;}
};

// end of data_structure group
/// @}

NS_CC_END

#endif /* __CCNUMBER_H__ */
