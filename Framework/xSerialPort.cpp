#include "pch.h"
//==============================================================================

//#include <crtdbg.h>

#include "xSerialPort.h"
#include "xTrace.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

CxSerialPort::CxSerialPort()
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_bOverlapped = FALSE;
}
//==============================================================================

CxSerialPort::~CxSerialPort()
{
  Close();
}
//==============================================================================

#ifdef _DEBUG
//void CxSerialPort::Dump(CDumpContext& dc) const
//{
//	CObject::Dump(dc);
//
//	dc << _T("m_hComm = ") << m_hComm << _T("\n");
//	dc << _T("m_bOverlapped = ") << m_bOverlapped;
//}
//==============================================================================
#endif

void CxSerialPort::Open(int nPort, DWORD dwBaud, Parity parity, BYTE DataBits, StopBits stopbits, FlowControl fc, BOOL bOverlapped)
{
	//Validate our parameters
	//_ASSERT(nPort>0 && nPort<=255);

	if(nPort <= 0)
	{
		m_hComm = INVALID_HANDLE_VALUE;
		return;
	}

	//Call CreateFile to open up the comms port
	TCHAR szPort[16];
	_stprintf_s(szPort, 16, _T("\\\\.\\COM%d"), nPort);
	m_hComm = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		AxTRACE(_T("Fail to open up the comms %d port. Error code %d\r\n"), nPort, GetLastError());
		return;
	}
  
	m_bOverlapped = bOverlapped;

	//Get the current state prior to changing it
	DCB dcb;
	GetState(dcb);

	//Setup the baud rate
	dcb.BaudRate = dwBaud; 

	//Setup the Parity
	switch (parity)
	{
		case EvenParity:  dcb.Parity = EVENPARITY;  break;
		case MarkParity:  dcb.Parity = MARKPARITY;  break;
		case NoParity:    dcb.Parity = NOPARITY;    break;
		case OddParity:   dcb.Parity = ODDPARITY;   break;
		case SpaceParity: dcb.Parity = SPACEPARITY; break;
		default:          _ASSERT(FALSE);           break;
	}

	//Setup the data bits
	dcb.ByteSize = DataBits;

	//Setup the stop bits
	switch (stopbits)
	{
		case OneStopBit:           dcb.StopBits = ONESTOPBIT;   break;
		case OnePointFiveStopBits: dcb.StopBits = ONE5STOPBITS; break;
		case TwoStopBits:          dcb.StopBits = TWOSTOPBITS;  break;
		default:                   _ASSERT(FALSE);              break;
	}

	//Setup the flow control 
	dcb.fDsrSensitivity = FALSE;
	switch (fc)
	{
		case NoFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
		case CtsRtsFlowControl:
		{
			dcb.fOutxCtsFlow = TRUE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
		case CtsDtrFlowControl:
		{
			dcb.fOutxCtsFlow = TRUE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
		case DsrRtsFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = TRUE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
		case DsrDtrFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = TRUE;
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
		case XonXoffFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = TRUE;
			dcb.fInX = TRUE;
			dcb.XonChar = 0x11;
			dcb.XoffChar = 0x13;
			dcb.XoffLim = 100;
			dcb.XonLim = 100;
			break;
		}
		default:
		{
			_ASSERT(FALSE);
			break;
		}
	}
  
	//Now that we have all the settings in place, make the changes
	SetState(dcb);
}
//==============================================================================

void CxSerialPort::Close()
{
	if (IsOpen())
	{
		BOOL bSuccess = CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		if (!bSuccess)
		{
			AxTRACE(_T("Failed to close up the comms port, GetLastError:%d\r\n"), GetLastError());
		}
		m_bOverlapped = FALSE;
	}
}
//==============================================================================

void CxSerialPort::Attach(HANDLE hComm)
{
	Close();
	m_hComm = hComm;  
}
//==============================================================================

HANDLE CxSerialPort::Detach()
{
	HANDLE hrVal = m_hComm;
	m_hComm = INVALID_HANDLE_VALUE;
	return hrVal;
}
//==============================================================================

DWORD CxSerialPort::Read(void* lpBuf, DWORD dwCount)
{
	_ASSERT(IsOpen());
	//_ASSERT(!m_bOverlapped);

	DWORD dwBytesRead = 0;
	if(!ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, NULL))
	{
		Sleep(100);
		AxTRACE(_T("Failed in call to ReadFile\r\n"));
	}

	return dwBytesRead;
}
//==============================================================================

BOOL CxSerialPort::Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped)
{
	_ASSERT(IsOpen());
	_ASSERT(m_bOverlapped);
	_ASSERT(overlapped.hEvent);

	DWORD dwBytesRead = 0;
	BOOL bSuccess = ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, &overlapped);
	if(!bSuccess)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			AxTRACE(_T("Failed in call to ReadFile\r\n"));
		}
	}
	return bSuccess;
}
//==============================================================================

DWORD CxSerialPort::Write(const void* lpBuf, DWORD dwCount)
{
	_ASSERT(IsOpen());
	//_ASSERT(!m_bOverlapped);

	DWORD dwBytesWritten = 0;
	if(!WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, NULL))
	{
		Sleep(100);
		AxTRACE(_T("Failed in call to WriteFile\r\n"));
	}

	return dwBytesWritten;
}
//==============================================================================

BOOL CxSerialPort::Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped)
{
	_ASSERT(IsOpen());
	_ASSERT(m_bOverlapped);
	_ASSERT(overlapped.hEvent);

	DWORD dwBytesWritten = 0;
	BOOL bSuccess = WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, &overlapped);
	if(!bSuccess)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			AxTRACE(_T("Failed in call to WriteFile\r\n"));
		}
	}
	return bSuccess;
}
//==============================================================================

void CxSerialPort::GetOverlappedResult(OVERLAPPED& overlapped, DWORD& dwBytesTransferred, BOOL bWait)
{
	_ASSERT(IsOpen());
	_ASSERT(m_bOverlapped);
	_ASSERT(overlapped.hEvent);

	DWORD dwBytesWritten = 0;
	if(!::GetOverlappedResult(m_hComm, &overlapped, &dwBytesTransferred, bWait))
	{
		if(GetLastError() != ERROR_IO_PENDING)
		{
			AxTRACE(_T("Failed in call to GetOverlappedResult\r\n"));
		}
	}
}
//==============================================================================

void CxSerialPort::_OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped)
{
	//Validate our parameters
	_ASSERT(lpOverlapped);

	//Convert back to the C++ world
	CxSerialPort* pSerialPort = (CxSerialPort*) lpOverlapped->hEvent;
	//_ASSERT(pSerialPort->IsKindOf(RUNTIME_CLASS(CxSerialPort)));

	//Call the C++ function
	pSerialPort->OnCompletion(dwErrorCode, dwCount, lpOverlapped);
}
//==============================================================================

void CxSerialPort::OnCompletion(DWORD /*dwErrorCode*/, DWORD /*dwCount*/, LPOVERLAPPED lpOverlapped)
{
	//Just free up the memory which was previously allocated for the OVERLAPPED structure
	delete lpOverlapped;

	//Your derived classes can do something useful in OnCompletion, but don't forget to
	//call CxSerialPort::OnCompletion to ensure the memory is freed up
}
//==============================================================================

void CxSerialPort::CancelIo()
{
	_ASSERT(IsOpen());

	if(!::CancelIo(m_hComm))
	{
		AxTRACE(_T("Failed in call to CancelIO\r\n"));
	}
}
//==============================================================================

void CxSerialPort::WriteEx(const void* lpBuf, DWORD dwCount)
{
	_ASSERT(IsOpen());

	OVERLAPPED* pOverlapped = new OVERLAPPED;
	ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
	pOverlapped->hEvent = (HANDLE) this;
	if(!WriteFileEx(m_hComm, lpBuf, dwCount, pOverlapped, _OnCompletion))
	{
		delete pOverlapped;
		AxTRACE(_T("Failed in call to WriteFileEx\r\n"));
	}
}
//==============================================================================

void CxSerialPort::ReadEx(void* lpBuf, DWORD dwCount)
{
	_ASSERT(IsOpen());

	OVERLAPPED* pOverlapped = new OVERLAPPED;
	ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
	pOverlapped->hEvent = (HANDLE) this;
	if(!ReadFileEx(m_hComm, lpBuf, dwCount, pOverlapped, _OnCompletion))
	{
		delete pOverlapped;
		AxTRACE(_T("Failed in call to ReadFileEx\r\n"));
	}
}
//==============================================================================

void CxSerialPort::TransmitChar(char cChar)
{
	_ASSERT(IsOpen());

	if(!TransmitCommChar(m_hComm, cChar))
	{
		AxTRACE(_T("Failed in call to TransmitCommChar\r\n"));
	}
}
//==============================================================================

void CxSerialPort::GetConfig(COMMCONFIG& config)
{
	_ASSERT(IsOpen());

	DWORD dwSize = sizeof(COMMCONFIG);
	if(!GetCommConfig(m_hComm, &config, &dwSize))
	{
		AxTRACE(_T("Failed in call to GetCommConfig\r\n"));
	}
}
//==============================================================================

void CxSerialPort::SetConfig(COMMCONFIG& config)
{
	_ASSERT(IsOpen());

	DWORD dwSize = sizeof(COMMCONFIG);
	if(!SetCommConfig(m_hComm, &config, dwSize))
	{
		AxTRACE(_T("Failed in call to SetCommConfig\r\n"));
	}
}
//==============================================================================

void CxSerialPort::SetBreak()
{
	_ASSERT(IsOpen());

	if(!SetCommBreak(m_hComm))
	{
		AxTRACE(_T("Failed in call to SetCommBreak\r\n"));
	}
}
//==============================================================================

void CxSerialPort::ClearBreak()
{
	_ASSERT(IsOpen());

	if(!ClearCommBreak(m_hComm))
	{
		AxTRACE(_T("Failed in call to SetCommBreak\r\n"));
	}
}
//==============================================================================

void CxSerialPort::ClearError(DWORD& dwErrors)
{
	_ASSERT(IsOpen());

	if(!ClearCommError(m_hComm, &dwErrors, NULL))
	{
		AxTRACE(_T("Failed in call to ClearCommError\n"));
	}
}
//==============================================================================

void CxSerialPort::GetDefaultConfig(int nPort, COMMCONFIG& config)
{
	//Validate our parameters
	_ASSERT(nPort>0 && nPort<=255);

	//Create the device name as a string
	TCHAR szPort[16];
	_stprintf_s(szPort, 16, _T("COM%d"), nPort);

	DWORD dwSize = sizeof(COMMCONFIG);
	if (!GetDefaultCommConfig(szPort, &config, &dwSize))
	{
		AxTRACE(_T("Failed in call to GetDefaultCommConfig\r\n"));
	}
}
//==============================================================================

void CxSerialPort::SetDefaultConfig(int nPort, COMMCONFIG& config)
{
	//Validate our parameters
	_ASSERT(nPort>0 && nPort<=255);

	//Create the device name as a string
	TCHAR szPort[16];
	_stprintf_s(szPort, 16, _T("COM%d"), nPort);

	DWORD dwSize = sizeof(COMMCONFIG);
	if (!SetDefaultCommConfig(szPort, &config, dwSize))
	{
		AxTRACE(_T("Failed in call to GetDefaultCommConfig\r\n"));
	}
}
//==============================================================================

bool CxSerialPort::GetStatus(COMSTAT& stat)
{
	if(IsOpen() == TRUE)
	{
		DWORD dwErrors;
		if(!ClearCommError(m_hComm, &dwErrors, &stat))
		{
			AxTRACE(_T("Failed in call to ClearCommError\r\n"));
			return false;
		}

		return true;
	}

	return false;
}
//==============================================================================

bool CxSerialPort::GetState(DCB& dcb)
{
	if(IsOpen() == TRUE)
	{
		if(!GetCommState(m_hComm, &dcb))
		{
			AxTRACE(_T("Failed in call to GetCommState\r\n"));
			return false;
		}

		return true;
	}

	return false;
}
//==============================================================================

void CxSerialPort::SetState(DCB& dcb)
{
	_ASSERT(IsOpen());

	if(!SetCommState(m_hComm, &dcb))
	{
		AxTRACE(_T("Failed in call to SetCommState\r\n"));
	}
}
//==============================================================================

void CxSerialPort::Escape(DWORD dwFunc)
{
	_ASSERT(IsOpen());

	if(!EscapeCommFunction(m_hComm, dwFunc))
	{
		AxTRACE(_T("Failed in call to EscapeCommFunction\r\n"));
	}
}
//==============================================================================

void CxSerialPort::ClearDTR()
{
	Escape(CLRDTR);
}
//==============================================================================

void CxSerialPort::ClearRTS()
{
	Escape(CLRRTS);
}
//==============================================================================

void CxSerialPort::SetDTR()
{
	Escape(SETDTR);
}
//==============================================================================

void CxSerialPort::SetRTS()
{
	Escape(SETRTS);
}
//==============================================================================

void CxSerialPort::SetXOFF()
{
	Escape(SETXOFF);
}
//==============================================================================

void CxSerialPort::SetXON()
{
	Escape(SETXON);
}
//==============================================================================

void CxSerialPort::GetProperties(COMMPROP& properties)
{
	_ASSERT(IsOpen());

	if(!GetCommProperties(m_hComm, &properties))
	{
		AxTRACE(_T("Failed in call to GetCommProperties\r\n"));
	}
}
//==============================================================================

void CxSerialPort::GetModemStatus(DWORD& dwModemStatus)
{
  _ASSERT(IsOpen());

	if (!GetCommModemStatus(m_hComm, &dwModemStatus))
	{
		AxTRACE(_T("Failed in call to GetCommModemStatus\r\n"));
	}
}
//==============================================================================

void CxSerialPort::SetMask(DWORD dwMask)
{
	_ASSERT(IsOpen());

	if(!SetCommMask(m_hComm, dwMask))
	{
		AxTRACE(_T("Failed in call to SetCommMask\r\n"));
	}
}
//==============================================================================

void CxSerialPort::GetMask(DWORD& dwMask)
{
	_ASSERT(IsOpen());

	if(!GetCommMask(m_hComm, &dwMask))
	{
		AxTRACE(_T("Failed in call to GetCommMask\r\n"));
	}
}
//==============================================================================

void CxSerialPort::Flush()
{
	_ASSERT(IsOpen());

	if(!FlushFileBuffers(m_hComm))
	{
		AxTRACE(_T("Failed in call to FlushFileBuffers\r\n"));
	}
}
//==============================================================================

void CxSerialPort::Purge(DWORD dwFlags)
{
	_ASSERT(IsOpen());

	if(!PurgeComm(m_hComm, dwFlags))
	{
		AxTRACE(_T("Failed in call to PurgeComm\r\n"));
	}
}
//==============================================================================

void CxSerialPort::TerminateOutstandingWrites()
{
	Purge(PURGE_TXABORT);
}
//==============================================================================

void CxSerialPort::TerminateOutstandingReads()
{
	Purge(PURGE_RXABORT);
}
//==============================================================================

void CxSerialPort::ClearWriteBuffer()
{
	Purge(PURGE_TXCLEAR);
}
//==============================================================================

void CxSerialPort::ClearReadBuffer()
{
	Purge(PURGE_RXCLEAR);
}
//==============================================================================

void CxSerialPort::Setup(DWORD dwInQueue, DWORD dwOutQueue)
{
	_ASSERT(IsOpen());

	if(!SetupComm(m_hComm, dwInQueue, dwOutQueue))
	{
		AxTRACE(_T("Failed in call to SetupComm\r\n"));
	}
}
//==============================================================================

void CxSerialPort::SetTimeouts(COMMTIMEOUTS& timeouts)
{
	_ASSERT(IsOpen());

	if(!SetCommTimeouts(m_hComm, &timeouts))
	{
		AxTRACE(_T("Failed in call to SetCommTimeouts\r\n"));
	}
}
//==============================================================================

void CxSerialPort::GetTimeouts(COMMTIMEOUTS& timeouts)
{
	_ASSERT(IsOpen());

	if(!GetCommTimeouts(m_hComm, &timeouts))
	{
		AxTRACE(_T("Failed in call to GetCommTimeouts\r\n"));
	}
}
//==============================================================================

void CxSerialPort::Set0Timeout()
{
	COMMTIMEOUTS Timeouts;
	ZeroMemory(&Timeouts, sizeof(COMMTIMEOUTS));
	Timeouts.ReadIntervalTimeout = MAXDWORD;
	Timeouts.ReadTotalTimeoutMultiplier = 0;
	Timeouts.ReadTotalTimeoutConstant = 0;
	Timeouts.WriteTotalTimeoutMultiplier = 0;
	Timeouts.WriteTotalTimeoutConstant = 0;
	SetTimeouts(Timeouts);
}
//==============================================================================

void CxSerialPort::Set0WriteTimeout()
{
	COMMTIMEOUTS Timeouts;
	GetTimeouts(Timeouts);
	Timeouts.WriteTotalTimeoutMultiplier = 0;
	Timeouts.WriteTotalTimeoutConstant = 0;
	SetTimeouts(Timeouts);
}
//==============================================================================

void CxSerialPort::Set0ReadTimeout()
{
	COMMTIMEOUTS Timeouts;
	GetTimeouts(Timeouts);
	Timeouts.ReadIntervalTimeout = MAXDWORD;
	Timeouts.ReadTotalTimeoutMultiplier = 0;
	Timeouts.ReadTotalTimeoutConstant = 0;
	SetTimeouts(Timeouts);
}
//==============================================================================

void CxSerialPort::WaitEvent(DWORD& dwMask)
{
	_ASSERT(IsOpen());
	//_ASSERT(!m_bOverlapped);

	if(!WaitCommEvent(m_hComm, &dwMask, NULL))
	{
		AxTRACE(_T("Failed in call to WaitCommEvent\r\n"));
	}
}
//==============================================================================

void CxSerialPort::WaitEvent(DWORD& dwMask, OVERLAPPED& overlapped)
{
	_ASSERT(IsOpen());
	_ASSERT(m_bOverlapped);
	_ASSERT(overlapped.hEvent);

	if(!WaitCommEvent(m_hComm, &dwMask, &overlapped))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			AxTRACE(_T("Failed in call to WaitCommEvent\r\n"));
		}
	}
}
//==============================================================================
