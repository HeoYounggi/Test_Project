#include "pch.h"

#include "xTrace.h"
//==============================================================================

//#define MAX_MESSAGE_LENGTH 1024
#define MAX_MESSAGE_LENGTH 8191
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//==============================================================================

void AxTRACEW(wchar_t* szMessage, ...)
{
	wchar_t wszDebugMsg[MAX_MESSAGE_LENGTH];

	va_list va;
	
	//memset(wszDebugMsg, 0x00, sizeof(wchar_t) * MAX_MESSAGE_LENGTH);
		
	va_start(va, szMessage);
	vswprintf_s(wszDebugMsg, MAX_MESSAGE_LENGTH, szMessage, (va_list)va);
	va_end(va);

	OutputDebugStringW(wszDebugMsg);

	if(GetConsoleWindow())
	{
		wprintf(wszDebugMsg);
	}
}
//==============================================================================

void AxTRACEA(char* szMessage, ...)
{
	char szDebugMsg[MAX_MESSAGE_LENGTH];

	va_list va;

	//memset(szDebugMsg, 0x00, sizeof(char) * MAX_MESSAGE_LENGTH);

	va_start(va, szMessage);
	vsprintf_s(szDebugMsg, MAX_MESSAGE_LENGTH, szMessage, va);
	va_end(va);

	OutputDebugStringA(szDebugMsg);

	if(GetConsoleWindow())
	{
		printf(szDebugMsg);
	}
}
//==============================================================================

void AxDEBUGW(wchar_t* szMessage, ...)
{
#ifdef _DEBUG
	wchar_t wszDebugMsg[MAX_MESSAGE_LENGTH];

	va_list va;

	//memset(wszDebugMsg, 0x00, sizeof(wchar_t) * MAX_MESSAGE_LENGTH);

	va_start(va, szMessage);
	vswprintf_s(wszDebugMsg, MAX_MESSAGE_LENGTH, szMessage, (va_list)va);
	va_end(va);

	OutputDebugStringW(wszDebugMsg);

	if(GetConsoleWindow())
	{
		wprintf(wszDebugMsg);
	}
#endif
}
//==============================================================================

void AxDEBUGA(char* szMessage, ...)
{
#ifdef _DEBUG
	char szDebugMsg[MAX_MESSAGE_LENGTH];

	va_list va;

	//memset(szDebugMsg, 0x00, sizeof(char) * MAX_MESSAGE_LENGTH);

	va_start(va, szMessage);
	vsprintf_s(szDebugMsg, MAX_MESSAGE_LENGTH, szMessage, va);
	va_end(va);

	OutputDebugStringA(szDebugMsg);

	if(GetConsoleWindow())
	{
		printf(szDebugMsg);
	}
#endif
}
//==============================================================================
