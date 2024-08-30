#include "pch.h"
//==============================================================================

#include "xThread.h"

#include "xTrace.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//==============================================================================

CxThread::CxThread()
{
	m_bFinished = false;
	m_bTerminated = false;

	m_dwExitCode = 0;
	m_bStarted = false;
	m_hThread = 0;
	m_uiThreadId = 0;
}
//==============================================================================

CxThread::~CxThread()
{
	if(m_hThread != 0)
	{
		StopThread();

		if(m_bFinished == false)
		{
			WaitFor();
		}

		::CloseHandle(m_hThread); // Thread Handle을 닫는다.
	}
}
//==============================================================================

unsigned WINAPI CxThread::ThreadProc(LPVOID pParameter)
{
	CxThread *pThread = (CxThread*)pParameter;

	if(pThread->m_bTerminated == false)
	{
		pThread->m_dwExitCode = pThread->Execute();
	}

	pThread->m_bStarted = false;
	pThread->m_bFinished = true;

	::_endthreadex(pThread->m_dwExitCode);
	::CloseHandle(pThread->m_hThread);
	pThread->m_hThread = NULL;
	return pThread->m_dwExitCode;
}
//==============================================================================

void CxThread::StartThread()
{
	if(m_bStarted == true)
	{
		AxTRACE(TEXT("[CxThread::StartThread] Already Started!!!\r\n"));
		return;
	}

	m_bStarted = true;
	m_bTerminated = false;
	m_bFinished = false;
	
	//::ResumeThread(m_hThread);
	//m_hThread = (HANDLE)::_beginthreadex(NULL, 0, ThreadProc, this, CREATE_SUSPENDED, &m_uiThreadId);
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, ThreadProc, this, 0, &m_uiThreadId);
}
//==============================================================================

void CxThread::StopThread()
{
	m_bTerminated = true;
	SwitchToThread();

	// Start함수가 call되지 않은 경우는 Suspended상태이기 때문에 Resume시켜줘야 종료할 수 있다.
	if(m_bStarted == false)
	{
		//::ResumeThread(m_hThread);
	}
}
//==============================================================================

void CxThread::WaitFor()
{
	DWORD dwResult;
	MSG msg;

	if(m_hThread != NULL)
	{
		unsigned long long unStartTick = GetTickCount();
		unsigned long long unTick;

		while(m_bFinished == false)
		{
			dwResult = MsgWaitForMultipleObjects(1, &m_hThread, FALSE, 500, QS_ALLINPUT);
			if(dwResult == WAIT_OBJECT_0 + 1)
			{
				if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			else if(dwResult == WAIT_OBJECT_0)
			{
				break;
			}

			unTick = GetTickCount();
			if(unTick < unStartTick)
			{
				unTick += 0x100000000ULL;
			}

			if(unTick - unStartTick > 3000)
			{
				AxTRACE(TEXT("WaitFor is TimeOut\r\n"));
				break;
			}
		}
	}
}
//==============================================================================
