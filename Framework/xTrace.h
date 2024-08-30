#ifndef _XTRACE_H_
#define _XTRACE_H_
//==============================================================================

#define CONSOLE_COLOR_BLACK			0x08
#define CONSOLE_COLOR_BLUE			0x09
#define CONSOLE_COLOR_GREEN			0x0A
#define CONSOLE_COLOR_SKY			0x0B
#define CONSOLE_COLOR_RED			0x0C
#define CONSOLE_COLOR_PURPLE		0x0D
#define CONSOLE_COLOR_YELLOW		0x0E
#define CONSOLE_COLOR_WHITE			0x0F
#define CONSOLE_COLOR_DEFAULT		0x0F
//==============================================================================

#define AxSetColor(color) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color)

#define AxTRACE_BLACK(szFormat, ...) AxSetColor(CONSOLE_COLOR_BLACK);AxTRACE(szFormat, __VA_ARGS__);AxSetColor(CONSOLE_COLOR_DEFAULT);
#define AxTRACE_BLUE(szFormat, ...) AxSetColor(CONSOLE_COLOR_BLUE);AxTRACE(szFormat, __VA_ARGS__);AxSetColor(CONSOLE_COLOR_DEFAULT);
#define AxTRACE_GREEN(szFormat, ...) AxSetColor(CONSOLE_COLOR_GREEN);AxTRACE(szFormat, __VA_ARGS__);AxSetColor(CONSOLE_COLOR_DEFAULT);
#define AxTRACE_SKY(szFormat, ...) AxSetColor(CONSOLE_COLOR_SKY);AxTRACE(szFormat, __VA_ARGS__);AxSetColor(CONSOLE_COLOR_DEFAULT);
#define AxTRACE_RED(szFormat, ...) AxSetColor(CONSOLE_COLOR_RED);AxTRACE(szFormat, __VA_ARGS__);AxSetColor(CONSOLE_COLOR_DEFAULT);
#define AxTRACE_PURPLE(szFormat, ...) AxSetColor(CONSOLE_COLOR_PURPLE);AxTRACE(szFormat, __VA_ARGS__);AxSetColor(CONSOLE_COLOR_DEFAULT);
#define AxTRACE_YELLOW(szFormat, ...) AxSetColor(CONSOLE_COLOR_YELLOW);AxTRACE(szFormat, __VA_ARGS__);AxSetColor(CONSOLE_COLOR_DEFAULT);
#define AxTRACE_WHITE(szFormat, ...) AxSetColor(CONSOLE_COLOR_WHITE);AxTRACE(szFormat, __VA_ARGS__);AxSetColor(CONSOLE_COLOR_DEFAULT);

#ifdef __cplusplus
extern "C" {
#endif

AFX_EXT_API void AxTRACEA(char* szMessage, ...);
AFX_EXT_API void AxTRACEW(wchar_t* szMessage, ...);

AFX_EXT_API void AxDEBUGA(char* szMessage, ...);
AFX_EXT_API void AxDEBUGW(wchar_t* szMessage, ...);

#ifdef UNICODE
#define AxTRACE AxTRACEW
#define AxDEBUG AxDEBUGW
#else
#define AxTRACE AxTRACEA
#define AxDEBUG AxDEBUGA
#endif

#ifdef __cplusplus
};
#endif
//==============================================================================

#endif //_XTRACE_H_
