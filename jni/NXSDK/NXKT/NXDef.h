/*
 * NXDEF.h
 *
 *  Created on: 2015-9-14
 *      Author: yangyk
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifndef interface
#define interface struct
#endif

//output param
#ifndef NXOUT
#define NXOUT
#endif

//input param
#ifndef NXIN
#define NXIN
#endif

#ifndef NXINOUT
#define NXINOUT
#endif


#ifndef NXINLINE
#define NXINLINE inline
#endif


#define NXMAX(A,B) (A)>(B)?(A):(B)
#define NXMIN(A,B) (A)<(B)?(A):(B)


typedef enum tag_NXThreadStatus {
	NXThreadUnknown = 0,
	NXThreadRunning = 1,
	NXThreadExiting = 2,
	NXThreadExited  = 3
} NXThreadStatus ;

typedef struct tag_NXRect{
	float x ;
	float y ;
	float w ;
	float h ;
}NXRect, *LPNXRect ;

NXINLINE NXRect NXRectMake(float x, float y, float w, float h)
{
	NXRect rect ;
	rect.x = x ;
	rect.y = y ;
	rect.w = w ;
	rect.h = h ;
	return rect ;
}

typedef struct tag_NXPoint{
	float x ;
	float y ;
}NXPoint, *LPNXPoint ;

NXINLINE NXPoint NXPointMake(float x, float y)
{
	NXPoint point ;
	point.x = x ;
	point.y = y ;
	return point ;
}

typedef struct tag_NXSize{
	float w ;
	float h ;
}NXSize, *LPNXSize ;

NXINLINE NXSize NXSizeMake(float w, float h)
{
	NXSize size ;
	size.w = w ;
	size.h = h ;
	return size ;
}

/*
 * sdk object
 */
interface NXObject
{
	NXObject();
	virtual ~NXObject() ;
} ;

/*
 * sdk object
 */
interface NXCallback
{
	NXCallback();
	virtual ~NXCallback() ;
} ;

class NXValue : public NXObject
{
public:
	union {
		NXRect   rect ;
		NXSize   size ;
		NXPoint point ;
	} value ;
} ;


