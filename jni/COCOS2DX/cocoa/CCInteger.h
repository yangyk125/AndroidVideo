#ifndef __CCINTEGER_H__
#define __CCINTEGER_H__

#include "CCNumber.h"

NS_CC_BEGIN

/**
 * @addtogroup data_structures
 * @{
 * @js NA
 */

class CC_DLL CCInteger : public CCNumber
{
public:
    CCInteger(int v) { m_dValue = v ;}
    int getValue() const {return m_dValue;}

    static CCInteger* create(int v)
    {
        CCInteger* pRet = new CCInteger(v);
        pRet->autorelease();
        return pRet;
    }

    /* override functions 
     * @lua NA
     */
    virtual void acceptVisitor(CCDataVisitor &visitor) { visitor.visit(this); }
};

// end of data_structure group
/// @}

NS_CC_END

#endif /* __CCINTEGER_H__ */
