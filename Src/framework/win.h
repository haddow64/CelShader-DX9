////////////////////////////////////////////////////////////////
// File: win.h
// Desc: Windows class.
////////////////////////////////////////////////////////////////
#ifndef __WIN_H__
#define __WIN_H__

#include "stddef.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class CWindows
{
public:
	CWindows(CApplication *application);
	~CWindows();

	void go();

	HWND getHWND() const { return m_hWnd; }
	UINT getFPS() const { return m_uFramesPerSecond; }

private:
	//Create window
	void createWindow();
	//Initialize application
	void createApplication();
	//Enter message loop
	void enterMsgLoop();

	//Objects
	CApplication*	mApplication;

	//Handles
	HINSTANCE		m_hInstance;
	HWND			m_hWnd;

	//Parameters
	float			m_fTimeAccum;
	UINT			m_uFrameCount;
	UINT			m_uFramesPerSecond;
};

#endif