#ifndef _XSERVICE_H_
#define _XSERVICE_H_
//==============================================================================

class AFX_EXT_CLASS CxService
{
public:
	CxService();
	virtual ~CxService();

	void Main(DWORD dwArgc, LPTSTR* lpszArgv);

	bool Terminated();

	bool Install(bool bShowMsg);
	bool Uninstall(bool bShowMsg);
	bool IsInstalled();

	void SetStatus(DWORD dwStatus, DWORD dwCheckPoint = 0, DWORD dwWaitHint = 0);
	static DWORD GetServiceStatus(TCHAR* pszServiceName);
	static int ServiceStart(TCHAR* pszServiceName);
	static int ServiceStop(TCHAR* pszServiceName);

protected:
	SERVICE_STATUS_HANDLE m_hStatus;
	TCHAR		m_szServiceName[256];
	TCHAR		m_szServiceDisplay[256];

	TCHAR		m_szLoadOrderGroup[256];
	TCHAR		m_szDependencies[256];

	bool		m_bConsoleMode;
	bool		m_bTerminated;

	DWORD		m_dwStartType;

	// Service Main
	virtual void Execute() = 0;

private:
	static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	static void WINAPI ServiceHandler(DWORD fdwControl);

	// Console Mode..
	void Register_Signal();
	static void sig_func(int sig);
};
//==============================================================================

#endif // _XSERVICE_H_
