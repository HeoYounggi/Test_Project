#include "pch.h"
//==============================================================================

#include "xUtilSystem.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

bool IsX64Windows()
{
	BOOL bIsWow64 = FALSE;
	if (IsX64Process() == true )
	{
		return true;
	}

	IsWow64Process(GetCurrentProcess(), &bIsWow64);
	if (bIsWow64 == TRUE)
	{
		return true;
	}

	return false;
}
//==============================================================================

bool IsX64Process()
{
#ifdef _WIN64
	return true;
#else
	return false;
#endif
}
//==============================================================================
