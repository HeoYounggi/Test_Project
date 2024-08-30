#include "pch.h"
//==============================================================================

#include "xService.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

// thread procedure와 같은 형태의 함수에서는 class instance를 넘길수 있다.
// 하지만 ServiceHandler에는 DWORD형태의 상태 코드값만 넘어온다.
// 그래서 어쩔수 없이 global로 변수를 하나 선언했다.
static CxService* LService = NULL;
//==============================================================================

CxService::CxService()
{
	LService = this;
	
	memset(m_szLoadOrderGroup, 0x00, sizeof(TCHAR) * 256);
	memset(m_szDependencies, 0x00, sizeof(TCHAR) * 256);

	m_bConsoleMode = false;
	m_bTerminated = false;

	m_dwStartType = SERVICE_AUTO_START;
}
//==============================================================================

CxService::~CxService()
{
	LService = NULL;
}
//==============================================================================

void CxService::Main(DWORD dwArgc, LPTSTR* lpszArgv)
{
	if(dwArgc > 1)
	{
		if(_tcscmp(lpszArgv[1], TEXT("install")) == 0)
		{
			Install(true);
			return;
		}
		else if(_tcscmp(lpszArgv[1], TEXT("uninstall")) == 0)
		{
			Uninstall(true);
			return;
		}
		else if(_tcscmp(lpszArgv[1], TEXT("console")) == 0)
		{
			m_bConsoleMode = true;
		}
	}

	if(m_bConsoleMode == true)
	{
		Register_Signal();
		CxService::ServiceMain(dwArgc, lpszArgv);
	}
	else
	{
		SERVICE_TABLE_ENTRY tEntry[] =
		{
			{m_szServiceName, CxService::ServiceMain},
			{NULL,NULL}
		};

		if(StartServiceCtrlDispatcher(tEntry) == FALSE)
		{
			return;
		}
	}
}
//==============================================================================

bool CxService::Terminated()
{
	return m_bTerminated;
}
//==============================================================================

bool CxService::Install(bool bShowMsg)
{
	// 이미 install되어 있는지 확인
	if (IsInstalled())
		return true;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		if(bShowMsg)
		{
			TCHAR szMessage[256];
			_tprintf(TEXT("Service \"%s\" failed to install with error : \"%d\""), m_szServiceName, ::GetLastError());
			::MessageBox(NULL, szMessage, m_szServiceName, MB_OK | MB_ICONSTOP);
		}
		return false;
	}

	// Get the executable file path
	TCHAR szFilePath[MAX_PATH+1] = {0, };
	GetModuleFileName(NULL, szFilePath, _MAX_PATH);

	SC_HANDLE hService = ::CreateService(hSCM, 
		m_szServiceName, 
		m_szServiceDisplay,
		SERVICE_ALL_ACCESS, 
		SERVICE_WIN32_OWN_PROCESS,
		m_dwStartType,
		SERVICE_ERROR_NORMAL,
		szFilePath, 
		NULL, 
		NULL, 
		NULL, 
		NULL, 
		NULL);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		if(bShowMsg)
		{
			TCHAR szMessage[256];
			_tprintf(TEXT("Service \"%s\" failed to install with error : \"%d\""), m_szServiceName, ::GetLastError());
			::MessageBox(NULL, szMessage, m_szServiceName, MB_OK | MB_ICONSTOP);
		}
		return false;
	}

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	if(bShowMsg)
	{
		::MessageBox(NULL, TEXT("Service installed successfully"), m_szServiceName, MB_OK | MB_ICONINFORMATION);
	}

	return true;
}
//==============================================================================

bool CxService::Uninstall(bool bShowMsg)
{
	if (!IsInstalled())
		return true;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
	{
		if(bShowMsg)
		{
			TCHAR szMessage[256];
			_tprintf(TEXT("Service \"%s\" failed to uninstall with error : \"%d\""), m_szServiceName, ::GetLastError());
			::MessageBox(NULL, szMessage, m_szServiceName, MB_OK | MB_ICONSTOP);
		}
		return false;
	}

	SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		if(bShowMsg)
		{
			TCHAR szMessage[256];
			_tprintf(TEXT("Service \"%s\" failed to uninstall with error : \"%d\""), m_szServiceName, ::GetLastError());
			::MessageBox(NULL, szMessage, m_szServiceName, MB_OK | MB_ICONSTOP);
		}
		return false;
	}

	SERVICE_STATUS status;
	::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	bool bDelete = ::DeleteService(hService) ? true : false;
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	if (bDelete)
	{
		::MessageBox(NULL, TEXT("Service uninstalled successfully"), m_szServiceName, MB_OK | MB_ICONINFORMATION);
		return true;
	}

	::MessageBox(NULL, TEXT("Service could not be deleted"), m_szServiceName, MB_OK | MB_ICONSTOP);
	return false;
}
//==============================================================================

bool CxService::IsInstalled()
{
	bool bResult = false;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM != NULL)
	{
		SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
		if (hService != NULL)
		{
			bResult = true;
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(hSCM);
	}
	return bResult;
}
//==============================================================================

void CxService::SetStatus(DWORD dwStatus, DWORD dwCheckPoint, DWORD dwWaitHint)
{
	SERVICE_STATUS	status;

	status.dwCheckPoint = dwCheckPoint;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	status.dwCurrentState = dwStatus;
	status.dwServiceSpecificExitCode = 0;
	status.dwServiceType = SERVICE_WIN32;
	status.dwWaitHint = dwWaitHint;
	status.dwWin32ExitCode = NO_ERROR;
	::SetServiceStatus(m_hStatus, &status);
}
//==============================================================================

DWORD CxService::GetServiceStatus(TCHAR* pszServiceName)
{
	SERVICE_STATUS Status;     
	SC_HANDLE hService = NULL;
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
	{
		return 0;
	}

	hService = ::OpenService(hSCM, pszServiceName, SERVICE_QUERY_STATUS);

	if (!hService)
	{
		::CloseServiceHandle(hSCM);
		return 0;
	}

	if (!::QueryServiceStatus(hService,  &Status) )
	{
		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
		return 0;
	} 

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);    

	return Status.dwCurrentState;    
}
//==============================================================================

int CxService::ServiceStart(TCHAR* pszServiceName)
{
	int	nResult = 0;
	SC_HANDLE hService = NULL;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
		return ::GetLastError();

	hService = ::OpenService(hSCM, pszServiceName, SERVICE_START | SERVICE_QUERY_STATUS);
	if (hService == NULL)
	{
		nResult = ::GetLastError();
		::CloseServiceHandle(hSCM);
		return nResult;
	}

	if( ::StartService(hService, 0, NULL) == FALSE )
		nResult = ::GetLastError();

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);

	return nResult;
}
//==============================================================================

int CxService::ServiceStop(TCHAR* pszServiceName)
{
	int	nResult = 0;
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM == NULL)
		return ::GetLastError();

	SC_HANDLE hService = NULL;

	hService = ::OpenService(hSCM, pszServiceName, SERVICE_STOP);
	if (hService == NULL)
	{
		nResult = ::GetLastError();
		::CloseServiceHandle(hSCM);
		return nResult;
	}

	SERVICE_STATUS status;
	if( ::ControlService(hService, SERVICE_CONTROL_STOP, &status) == FALSE )
	{
		nResult = ::GetLastError();
		if (status.dwCurrentState == SERVICE_STOPPED) 
			nResult = 0;
	}

	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);
	return nResult;
}
//==============================================================================

void WINAPI CxService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	if(LService->m_bConsoleMode == false)
	{
		// ServiceHandler등록
		LService->m_hStatus = ::RegisterServiceCtrlHandler(LService->m_szServiceName, CxService::ServiceHandler);
		LService->SetStatus(SERVICE_RUNNING);
	}

	LService->Execute();

	if(LService->m_bConsoleMode == false)
	{
		LService->SetStatus(SERVICE_STOPPED);
	}
}
//==============================================================================

void WINAPI CxService::ServiceHandler(DWORD fdwControl)
{
	switch(fdwControl)
	{
	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		{
			LService->SetStatus(SERVICE_STOP_PENDING);
			LService->m_bTerminated = true;
		}
		return;
	case SERVICE_CONTROL_PAUSE:
		{
			LService->SetStatus(SERVICE_PAUSED);
		}
		break;
	case SERVICE_CONTROL_CONTINUE:
		{
			LService->SetStatus(SERVICE_RUNNING);
		}
		break;
	case SERVICE_CONTROL_INTERROGATE:
		{
			// not supperted...
		}
		break;
	default:
		break;
	}
}
//==============================================================================

void CxService::sig_func(int sig)
{
	switch(sig)
	{
	case SIGABRT   : printf("signal detected : SIGABRT \n"); break;
	case SIGFPE    : printf("signal detected : SIGFPE  \n"); break;
	case SIGILL    : printf("signal detected : SIGILL  \n"); break;
	case SIGINT    : printf("signal detected : SIGINT  \n"); break;
	case SIGSEGV   : printf("signal detected : SIGSEGV \n"); break;
	case SIGTERM   : printf("signal detected : SIGTERM \n"); break;
//	case SIGUSR1   : printf("signal detected : SIGUSR1 \n"); break;
//	case SIGUSR2   : printf("signal detected : SIGUSR2 \n"); break;
//	case SIGUSR3   : printf("signal detected : SIGUSR3 \n"); break;
	case SIGBREAK  : printf("signal detected : SIGBREAK\n"); break;
	}
	LService->m_bTerminated = true;
}
//==============================================================================

void CxService::Register_Signal()
{
	signal(SIGABRT , sig_func);
	signal(SIGFPE  , sig_func);
	signal(SIGILL  , sig_func);
	signal(SIGINT  , sig_func);
	signal(SIGSEGV , sig_func);
	signal(SIGTERM , sig_func);
//	signal(SIGUSR1 , sig_func);
//	signal(SIGUSR2 , sig_func);
//	signal(SIGUSR3 , sig_func);
	signal(SIGBREAK, sig_func);
}
//==============================================================================
