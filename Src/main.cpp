////////////////////////////////////////////////////////////////
// File: main.cpp
// Desc: Application entry
////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN

#include "stddef.h"
#include "application.h"
#include "framework/win.h"

#ifdef _DEBUG
#include <io.h>
#include <fcntl.h>
#endif

//Win32 message proc
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYUP:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			return 0;
		}
		break;

	case WM_ACTIVATEAPP:
		if(wParam)
		{
			CApplication::isRunning = true;
		}
		else
		{
			CApplication::isRunning = false;
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//Win32 entry function
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
#ifdef _DEBUG
	//Enable console for debugging
	//Remember do disable this when compiling release exe
	AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
	printf("Debug:\n");
#endif

	try
	{
		CApplication app;
		app.start();
	}
	catch(CException& e)
	{
		MessageBox(NULL, e.getErrorMsg(), L"Fatal Error", MB_OK | MB_ICONERROR);
	}

	return 0;
}

//Get HRESULT string representation
std::wstring TranslateHResult(HRESULT hResult)
{
	wchar_t* message = NULL;
	if( FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		hResult,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<wchar_t*>(&message),
		0, 
		NULL)
		&& message != NULL )
	{
		std::wstring strMessage(message);
		LocalFree(reinterpret_cast<HLOCAL>(message));
		return strMessage;
	}
	else
	{
		return std::wstring(L"Unknown Error");
	}
}