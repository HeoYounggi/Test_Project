#include "pch.h"
//==============================================================================

#include "xTCPSocket.h"

#include "xWSAManager.h"
#include "xTrace.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//==============================================================================

CxTCPSocket::CxTCPSocket()
{
	m_hSocket = INVALID_SOCKET;
	m_nErrorCode = 0;
	memset(m_szIPAddress, 0x00, sizeof(m_szIPAddress));
	m_unPortNo = 0;
}
//==============================================================================

CxTCPSocket::~CxTCPSocket()
{
	Close();
}
//==============================================================================

bool CxTCPSocket::Close()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		m_nErrorCode = ::WSAGetLastError();
	}
	return true;
}
//==============================================================================

bool CxTCPSocket::Connect()
{
	int nDefaultTimeOut = 2000;
	int nResult;

	if(m_hSocket != INVALID_SOCKET)
	{
		Close();
	}

	m_hSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = ::WSAGetLastError();
		return false;
	}

	ULONG nonBlk = TRUE;

	if(ioctlsocket(m_hSocket, FIONBIO, &nonBlk ) == SOCKET_ERROR)
	{
		return false;
	}

	sockaddr_in	sa;
	memset(&sa, 0x00, sizeof(sa));
	sa.sin_family = PF_INET;
	sa.sin_port = ::htons(m_unPortNo);

	//sa.sin_addr.s_addr = ::inet_addr(m_szIPAddress);
	::inet_pton(AF_INET, m_szIPAddress, &sa.sin_addr.s_addr);

	nResult = ::connect(m_hSocket, (sockaddr*)&sa, sizeof(sa));
	if(nResult == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		m_nErrorCode = ::WSAGetLastError();
		Close();
		return false;
	}

	m_nErrorCode = 0;

	fd_set fdset;
	
	FD_ZERO(&fdset);
	FD_SET(m_hSocket, &fdset);

	struct timeval timevalue;

	timevalue.tv_sec = nDefaultTimeOut / 1000;
	timevalue.tv_usec = (nDefaultTimeOut % 1000) * 1000;

	if(select(0, NULL, &fdset, NULL, &timevalue) == SOCKET_ERROR)
	{
		return false;
	}

	if(!FD_ISSET(m_hSocket, &fdset))
	{
		m_nErrorCode = ::WSAGetLastError();
		Close();
		return false;
	}

	nonBlk = FALSE;
	if(ioctlsocket(m_hSocket, FIONBIO, &nonBlk ) == SOCKET_ERROR)
	{
		return false;
	}

	// 연결상태를 자동으로 유지하도록 옵션 설정
	BOOL biKeepAlive = TRUE;
	if(::setsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&biKeepAlive, sizeof(biKeepAlive)) == SOCKET_ERROR)
	{
		m_nErrorCode = ::WSAGetLastError();
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		return false;
	}

	tcp_keepalive tcpkl;
	DWORD dwRet;

	tcpkl.onoff = 1;			// KEEPALIVE ON
	tcpkl.keepalivetime = 1000; // 1초 마다 KEEPALIVE 신호를 보내겠다. (Default 는 2시간이다)
	tcpkl.keepaliveinterval = 200; // keepalive 신호를 보내고 응답이 없으면 1초마다 재 전송하겠다. (ms tcp 는 10회 재시도 한다)

	WSAIoctl(m_hSocket, SIO_KEEPALIVE_VALS, &tcpkl, sizeof(tcp_keepalive), 0, 0, &dwRet, NULL, NULL);

	return true;
}
//==============================================================================

bool CxTCPSocket::Readable(int nTimeoutSec, int nTimeoutUSec)
{
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return false;
	}

	int	nRet;
	fd_set fdset;

	fdset.fd_count = 1;
	fdset.fd_array[0] = m_hSocket;

	if(nTimeoutSec < 0)
	{
		nRet = ::select(0, &fdset, NULL, NULL, NULL);
	}
	else
	{
		timeval	tv;
		tv.tv_sec = nTimeoutSec;
		tv.tv_usec = nTimeoutUSec;
		nRet = ::select(0, &fdset, NULL, NULL, &tv);
	}

	m_nErrorCode = 0;

	switch(nRet)
	{
		case 0 : 
		{
			// timeout
			return false;
		}
		case 1 :
		{
			// received
			return true;
		}
		default:
		{
			// error
			m_nErrorCode = ::WSAGetLastError();
			Close();
			return false;
		}
	}
}
//==============================================================================

unsigned int CxTCPSocket::RecvSize()
{
	unsigned int unRead;

	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return false;
	}

	ioctlsocket(m_hSocket, FIONREAD, (unsigned long*)&unRead);

	return unRead;
}
//==============================================================================

int CxTCPSocket::Recv(LPVOID pData, int nSize)
{
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return false;
	}

	char* buffer = (char*)pData;
	int ret, nLeft, idx;
	nLeft = nSize;
	idx = 0;

	do
	{
		ret = ::recv(m_hSocket, &buffer[idx], nLeft, 0);
		if (ret == 0)
		{
			// the connection has been gracefully closed
			AxTRACE(TEXT("[Recv]Disconnected from Server\r\n"));
			Close();
			return 0;
		}
		else if (ret == SOCKET_ERROR)
		{
			m_nErrorCode = ::WSAGetLastError();
			if(m_nErrorCode == WSAEWOULDBLOCK)
			{
				// ignore WSAEWOULDBLOCK error
				m_nErrorCode = 0;
				continue;
			}
			else //if( (m_nErrorCode == WSAECONNABORTED) || (m_nErrorCode == WSAECONNRESET) )
			{
				// disconnect (10053, 10054)
				AxTRACE(TEXT("[Recv]Disconnected by Error : %d\r\n"), m_nErrorCode);
				Close();
			}

			return 0;
		}
		nLeft -= ret;
		idx += ret;
	} while(nLeft > 0);

	m_nErrorCode = 0;

	return idx;
}
//==============================================================================

int CxTCPSocket::Send(const LPVOID pData, int nSize)
{
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return false;
	}

	char* buffer = (char*)pData;
	int ret, nLeft, idx;    
	nLeft = nSize;
	idx = 0;

	while(nLeft > 0)
	{
		ret = ::send(m_hSocket, &buffer[idx], nLeft, 0);
		if (ret == 0)
		{
			// the connection has been gracefully closed
			Close();
			AxTRACE(TEXT("[Send]Disconnected from Server\r\n"));
			return 0;
		}
		else if (ret == SOCKET_ERROR)
		{
			m_nErrorCode = ::WSAGetLastError();
			if(m_nErrorCode == WSAEWOULDBLOCK)
			{
				// ignore WSAEWOULDBLOCK error
				m_nErrorCode = 0;
				continue;
			}
			else //if( (m_nErrorCode == WSAECONNABORTED) || (m_nErrorCode == WSAECONNRESET) )
			{
				// disconnect (10053, 10054)
				Close();
				AxTRACE(TEXT("[Send]Disconnected by Error : %d\r\n"), m_nErrorCode);
			}
			return SOCKET_ERROR;
		}

		nLeft -= ret;
		idx += ret;
	}

	m_nErrorCode = 0;

	return idx;
}
//==============================================================================

char* CxTCPSocket::GetIPAddress()
{
	return m_szIPAddress;
}
//==============================================================================

void CxTCPSocket::SetIPAddress(char* pszIPAddress)
{
	strcpy_s(m_szIPAddress, 16, pszIPAddress);
}
//==============================================================================

int CxTCPSocket::GetPortNo()
{
	return m_unPortNo;
}
//==============================================================================

void CxTCPSocket::SetPortNo(unsigned short unPortNo)
{
	m_unPortNo = unPortNo;
}
//==============================================================================

void CxTCPSocket::SetKeppAliveTime(unsigned long unTime, unsigned long unInterval)
{
	tcp_keepalive tcpkl;
	DWORD dwRet;

	if(m_hSocket == INVALID_SOCKET)
	{
		AxTRACE(TEXT("SetKeppAliveTime Error. INVALID_SOCKET\r\n"));
		return;
	}

	tcpkl.onoff = 1;			// KEEPALIVE ON
	tcpkl.keepalivetime = unTime; // keepalivetime ms 마다 KEEPALIVE 신호를 보내겠다. (Default 는 2시간이다)
	tcpkl.keepaliveinterval = unInterval; // keepalive 신호를 보내고 응답이 없으면 keepaliveinterval ms 마다가 재 전송하겠다. (ms tcp 는 10회 재시도 한다)

	WSAIoctl(m_hSocket, SIO_KEEPALIVE_VALS, &tcpkl, sizeof(tcp_keepalive), 0, 0, &dwRet, NULL, NULL);
}
//==============================================================================

int CxTCPSocket::ErrorCode()
{
	return m_nErrorCode;
}
//==============================================================================

bool CxTCPSocket::Active()
{
	return m_hSocket != INVALID_SOCKET;
}
//==============================================================================
