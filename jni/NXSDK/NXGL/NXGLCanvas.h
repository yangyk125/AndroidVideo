/*
 * NXCCCanvas.h
 *
 *  Created on: 2015-11-4
 *      Author: yangyk
 */

#pragma once

#include "NXSDK.h"

class NXGLCanvas : public NXSDKCanvas
{
private:
	NXSize		_targetSize  ;
	GLuint		_frameBuffer ;
	GLuint		_rendrBuffer ;
	GLuint		_rendrTextur ;
	GLuint		_oldRndBuffer ;
	GLuint		_oldFrmBuffer ;
public:
	NXGLCanvas();
	virtual ~NXGLCanvas();
public:
	virtual bool open(const NXSize& viewport) ;
	virtual bool begin();
	virtual NXPtr<NXBuffer> end();
	virtual bool close();
};

