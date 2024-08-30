#ifndef _XTHREAD_H_
#define _XTHREAD_H_
//==============================================================================

#pragma pack(push, 8)

const DWORD MS_VC_EXCEPTION = 0x406D1388;

typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
//==============================================================================

class AFX_EXT_CLASS CxThread
{
protected:
	HANDLE m_hThread;
	unsigned m_uiThreadId;

	bool m_bTerminated;
	bool m_bFinished;
	bool m_bStarted;

	DWORD m_dwExitCode;

	static unsigned WINAPI ThreadProc(LPVOID pParameter);	// thread procedure 
	virtual DWORD Execute() = 0;

public:
	CxThread();
	virtual ~CxThread();

	virtual void StartThread();
	virtual void StopThread();
	virtual void WaitFor();

	virtual bool GetTerminated() { return m_bTerminated; }
	virtual HANDLE GetHandle() { return m_hThread; }
	virtual unsigned GetThreadId() { return m_uiThreadId; }
	virtual DWORD GetExitCode() { return m_dwExitCode; }
};
//==============================================================================

#endif // _XTHREAD_H_
