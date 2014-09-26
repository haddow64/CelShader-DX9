////////////////////////////////////////////////////////////////
// File: exception.h
// Desc: Exception class.
////////////////////////////////////////////////////////////////
#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <string>
#include <stdio.h>
#include <stdarg.h>

#define LINE_AND_FILE __LINE__, __FILE__

class CException
{
public:
	CException()
	{
		m_strError = std::wstring(L"Unknown exception");		
	}

	CException(const wchar_t* description)
	{
		m_strError = description;
	}

	CException(const wchar_t* description, int lineNumber, const char* fileName)
	{
		WCHAR wtBuffer [255];
		wsprintf(wtBuffer, L"\nLine: %d\nFile: %hs", lineNumber, fileName);
		m_strError = description;
		m_strError.append(wtBuffer);
	}

	CException(const char* description)
	{
		WCHAR wtBuffer [255];
		wsprintf(wtBuffer, L"%hs", description);
		m_strError = wtBuffer;
	}

	CException(const char* description, int lineNumber, const char* fileName)
	{
		WCHAR wtBuffer [255];
		wsprintf(wtBuffer, L"%hs\nLine: %d\nFile: %hs", description, lineNumber, fileName);
		m_strError = wtBuffer;
	}
	
	CException(std::wstring& description)
	{
		m_strError = description;
	}

	CException(std::wstring& description, int lineNumber, const char* fileName)
	{
		WCHAR wtBuffer [255];
		wsprintf(wtBuffer, L"\nLine: %d\nFile: %hs", lineNumber, fileName);
		m_strError = description.c_str();
		m_strError.append(wtBuffer);
	}

	LPCWSTR getErrorMsg() const { return m_strError.c_str(); }

	void fill(const WCHAR* text, ...)
	{
		WCHAR wtBuffer [256];
		va_list argList;
		va_start(argList, text);
		vswprintf(wtBuffer, 255, text, argList);
		va_end(argList);
		m_strError = wtBuffer;
	}

private:
	std::wstring			m_strError;

};

#endif