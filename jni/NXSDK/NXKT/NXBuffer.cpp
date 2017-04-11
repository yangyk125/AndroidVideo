/*
 * NXBuffer.cpp
 *
 *  Created on: 2015-9-16
 *      Author: yangyk
 */

#include "NXSDK.h"
#include "NXBuffer.h"


#ifdef PRINT_NXSDK_BUFFER_COUNT

static volatile uint32_t  NXBufferObjectCount = 0 ;
static void 	NXBufferCreate()
{
	__android_log_print(ANDROID_LOG_DEBUG, "NXKT", "NXBuffer Create Objects Count = %u", ++NXBufferObjectCount);
}
static void 	NXBufferDestory()
{
	__android_log_print(ANDROID_LOG_DEBUG, "NXKT", "NXBuffer Destory Objects Count = %u", --NXBufferObjectCount);
}

#endif

//private
NXBuffer::NXBuffer()
{
	_bufferCont  = 0 ;
	_bufferType  = NXBufferError ;
	_bufferPixel = NXSDK_PIX_FMT_NONE ;

	memset(_bufferArry,0,sizeof(_bufferArry));
	memset(_bufferLeng,0,sizeof(_bufferLeng));

#ifdef PRINT_NXSDK_BUFFER_COUNT
	NXBufferCreate() ;
#endif
}

//NXBufferData
NXBuffer::NXBuffer(uint32_t bytes)
{
	_bufferType = NXBufferData ;
	_bufferPixel = NXSDK_PIX_FMT_NONE ;

	memset(_bufferArry,0,sizeof(_bufferArry));
	memset(_bufferLeng,0,sizeof(_bufferLeng));

	_bufferArry[0] = (uint8_t*)malloc(bytes) ;
	_bufferLeng[0] = bytes ;
	_bufferCont    = 1 ;

	memset(_bufferArry[0], 0, _bufferLeng[0]) ;

#ifdef PRINT_NXSDK_BUFFER_COUNT
	NXBufferCreate() ;
#endif
}

//NXBufferAudio
NXBuffer::NXBuffer(uint32_t sampleCount, uint32_t sampleBits, uint8_t channels)
{
	_bufferType = NXBufferAudio ;
	_bufferPixel = NXSDK_PIX_FMT_NONE ;

	memset(_bufferArry,0,sizeof(_bufferArry));
	memset(_bufferLeng,0,sizeof(_bufferLeng));

	_bufferLeng[0] = sampleCount*channels*sampleBits/8 ;
	_bufferArry[0] = (uint8_t*)malloc(_bufferLeng[0]);
	_bufferCont    = 1 ;

	memset(_bufferArry[0], 0, _bufferLeng[0]) ;

#ifdef PRINT_NXSDK_BUFFER_COUNT
	NXBufferCreate() ;
#endif
}

//NXBufferVideo
NXBuffer::NXBuffer(uint32_t width, uint32_t height, NXSDKPixelFormat pixfmt)
{
	_bufferType = NXBufferVideo ;
	_bufferPixel = pixfmt ;

	memset(_bufferArry,0,sizeof(_bufferArry));
	memset(_bufferLeng,0,sizeof(_bufferLeng));

	switch(pixfmt)
	{
		case NXSDK_PIX_FMT_YUV420P :
		{
		_bufferArry[0] = (uint8_t*)malloc(width*height) ;
		_bufferLeng[0] = width*height ;
		_bufferArry[1] = (uint8_t*)malloc(width*height/4) ;
		_bufferLeng[1] = width*height/4 ;
		_bufferArry[2] = (uint8_t*)malloc(width*height/4) ;
		_bufferLeng[2] = width*height/4 ;
		_bufferCont    = 3 ;
		memset(_bufferArry[0], 0, _bufferLeng[0]) ;
		memset(_bufferArry[1], 0, _bufferLeng[1]) ;
		memset(_bufferArry[2], 0, _bufferLeng[2]) ;
		break ;
		}
		case NXSDK_PIX_FMT_NV12:
		case NXSDK_PIX_FMT_NV21:
		{
		_bufferArry[0] = (uint8_t*)malloc(width*height) ;
		_bufferLeng[0] = width*height ;
		_bufferArry[1] = (uint8_t*)malloc(width*height/2) ;
		_bufferLeng[1] = width*height/2 ;
		_bufferCont    = 2 ;
		memset(_bufferArry[0], 0, _bufferLeng[0]) ;
		memset(_bufferArry[1], 0, _bufferLeng[1]) ;
		break ;
		}
		case NXSDK_PIX_FMT_ARGB:
		case NXSDK_PIX_FMT_RGBA:
		case NXSDK_PIX_FMT_ABGR:
		case NXSDK_PIX_FMT_BGRA:
		{
		_bufferArry[0] = (uint8_t*)malloc(width*height*4) ;
		_bufferLeng[0] = width*height*4 ;
		_bufferCont    = 1 ;
		memset(_bufferArry[0], 0, _bufferLeng[0]) ;
		break ;
		}
	}

#ifdef PRINT_NXSDK_BUFFER_COUNT
	NXBufferCreate() ;
#endif
}

//NXBufferCanvas
NXBuffer::NXBuffer(uint32_t width, uint32_t height)
{
	_bufferType = NXBufferCanvas ;
	_bufferPixel = NXSDK_PIX_FMT_BGRA ;

	memset(_bufferArry,0,sizeof(_bufferArry));
	memset(_bufferLeng,0,sizeof(_bufferLeng));

	_bufferArry[0] = (uint8_t*)malloc(width*height*4) ;
	_bufferLeng[0] = width*height*4 ;
	_bufferCont    = 1 ;

	memset(_bufferArry[0], 0, _bufferLeng[0]) ;

#ifdef PRINT_NXSDK_BUFFER_COUNT
	NXBufferCreate() ;
#endif
}


//Destructor
NXBuffer::~NXBuffer()
{
	for(int idx=0; idx<4; idx++)
	{
		if(_bufferArry[idx])
		{
			free(_bufferArry[idx]) ;
			_bufferArry[idx] = NULL ;
			_bufferLeng[idx] = 0 ;
		}
	}

#ifdef PRINT_NXSDK_BUFFER_COUNT
	NXBufferDestory() ;
#endif
}


NXBuffer* NXBuffer::copy()
{
	NXBuffer* nbuffer = new NXBuffer();

	nbuffer->_bufferType  = _bufferType ;
	nbuffer->_bufferPixel = _bufferPixel ;
	nbuffer->_bufferCont  = _bufferCont ;

	memcpy(nbuffer->_bufferLeng, _bufferLeng, sizeof(_bufferLeng));

	for(uint32_t idx=0; idx<nbuffer->_bufferCont; idx++)
	{
		if(nbuffer->_bufferLeng[idx] > 0)
		{
			nbuffer->_bufferArry[idx] = (uint8_t*)malloc(nbuffer->_bufferLeng[idx]) ;
			memcpy(nbuffer->_bufferArry[idx], _bufferArry[idx], nbuffer->_bufferLeng[idx]) ;
		}
	}

	return nbuffer ;
}

//buffer type
NXBufferType NXBuffer::getBufferType()
{
	return _bufferType ;
}

NXSDKPixelFormat NXBuffer::getPixelFormat()
{
	return _bufferPixel ;
}

//lock this buffer
bool NXBuffer::lockBuffer()
{
	return false ;
}

//unlock this buffer
void NXBuffer::unlockBuffer()
{

}

//plane count
uint32_t NXBuffer::getPlaneCount()
{
	return _bufferCont ;
}

//plane length
uint32_t NXBuffer::getPlaneLength(uint32_t index)
{
	return _bufferLeng[index] ;
}

//plane pointer
uint8_t* NXBuffer::getPlanePtr(uint32_t index)
{
	return _bufferArry[index] ;
}
