/*
 * NXCCShareNode.cpp
 *
 *  Created on: 2015-11-19
 *      Author: yangyk
 */

#include "NXCCShareNode.h"

NXCCShareNode::NXCCShareNode()
{
	setAnchorPoint(ccp(0.5, 0.5)) ;
}

NXCCShareNode::~NXCCShareNode()
{

}

void NXCCShareNode::addChild(CCNode* child)
{
	this->addChild(child, 1, 0) ;
}

void NXCCShareNode::addChild(CCNode* child, int zOrder)
{
	this->addChild(child, zOrder, 0) ;
}

void NXCCShareNode::addChild(CCNode* child, int zOrder, int tag)
{
    //CCAssert( child != NULL, "Argument must be non-nil");
    //CCAssert( child->m_pParent == NULL, "child already added. It can't be added again");

    if( ! m_pChildren )
    {
        this->childrenAlloc();
    }

    this->insertChild(child, zOrder);

    child->setTag(tag) ;

    //child->setParent(this);
    child->setOrderOfArrival(CCNodeGlobalOrderOfArrival());

    if( m_bRunning )
    {
        child->onEnter();
        child->onEnterTransitionDidFinish();
    }
}
