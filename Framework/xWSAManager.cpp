#include "pch.h"
//==============================================================================

#include "xWSAManager.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

CxWSAManager CxWSAManager::m_Self = CxWSAManager();
//==============================================================================

CxWSAManager::CxWSAManager(void)
{
	WSADATA		stData;

	::WSAStartup(WINSOCK_VERSION, &stData);

	//int		nErrorCode = ::GetLastError();
	//WriteLog("Winsock error - [%d] %s", nErrorCode, SysErrorMessage(nErrorCode));
}
//==============================================================================

CxWSAManager::~CxWSAManager(void)
{
	WSACleanup();
}
//==============================================================================