////////////////////////////////////////////////////////////////
// File: win.cpp
// Desc: Windows class.
////////////////////////////////////////////////////////////////
#include "win.h"
#include "exception.h"
#include "application.h"
#include "d3d.h"

CWindows::CWindows(CApplication *application) : 
	mApplication(application), m_hWnd(0), m_fTimeAccum(0.0f), 
	m_uFrameCount(0), m_uFramesPerSecond(0)
{
	assert(mApplication != NULL);
	m_hInstance = GetModuleHandle(NULL);
	createWindow();
}

CWindows::~CWindows()
{
}

//Create a window and display it then enter the message loop
void CWindows::go()
{
	enterMsgLoop();
}

//Create window
void CWindows::createWindow()
{
	WNDCLASS wc;
	std::wstring windowTitle = L"Cel Shading Demo :: ";
	windowTitle.append(mApplication->getTitle());

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)WndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"Cel Shading Demo";

	if( !RegisterClass(&wc) ) 
	{
		throw CException(L"RegisterClass failed", LINE_AND_FILE);
	}
	
	m_hWnd = CreateWindow(L"Cel Shading Demo", windowTitle.c_str(), 
		WS_EX_TOPMOST,
		CW_USEDEFAULT, CW_USEDEFAULT, mApplication->getWidth(), mApplication->getHeight(),
		0, 0, m_hInstance, 0); 

	if( !m_hWnd )
	{
		throw CException(L"CreateWindow failed", LINE_AND_FILE);
	}
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
}

//Enter the message loop
void CWindows::enterMsgLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();

	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
        {	
			float currTime  = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;
			m_fTimeAccum += timeDelta;
			m_uFrameCount++;
			if(m_fTimeAccum >= 1.0f)
			{
				m_fTimeAccum -= 1.0f;
				m_uFramesPerSecond = m_uFrameCount;
				m_uFrameCount = 0;
			}

			mApplication->update(timeDelta);
			mApplication->render();

			lastTime = currTime;
        }
    }
}