////////////////////////////////////////////////////////////////
// File: stddef.h
// Desc: Application wide definitions and forward declarations.
////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4996 ) //get rid of the annoying "consider using wcsncpy_s instead"

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <exception>
#include "singleton.h"
#include "exception.h"
#include "d3dfont.h"

// Framework
std::wstring TranslateHResult(HRESULT hResult);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
class CApplication;
class CWindows;
class CD3D;