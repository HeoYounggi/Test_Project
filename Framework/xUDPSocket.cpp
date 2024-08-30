#include "pch.h"
//==============================================================================

#include "xUDPSocket.h"

#include "xWSAManager.h"
#include "xTrace.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//==============================================================================

CxUDPSocket::CxUDPSocket()
{
	m_hSocket = INVALID_SOCKET;
	m_nErrorCode = 0;
	memset(m_szIPAddress, 0x00, sizeof(m_szIPAddress));
	m_unPortNo = 0;

	m_nTTL = 1;
	m_biAllowLoop = FALSE;
	m_eType = eUnicast;
}
//==============================================================================

CxUDPSocket::~CxUDPSocket()
{
	Close();
}
//==============================================================================

int CxUDPSocket::SetNetworkInterface(unsigned int unIdx)
{
	int nOption;

	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return 0;
	}

	nOption = htonl(unIdx);
	if(::setsockopt(m_hSocket, IPPROTO_IP, IP_MULTICAST_IF, (char*)&nOption, sizeof(nOption)) == SOCKET_ERROR)
	{
		AxTRACE(TEXT("Fail to setsockopt IF %d\r\n"), GetLastError());
		return 0;
	}

	return 1;
}

//==============================================================================
bool CxUDPSocket::Bind()
{
	sockaddr_in	sa;
	memset(&sa, 0x00, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = ::htons(m_unPortNo);
	sa.sin_addr.s_addr = INADDR_ANY;

	if(::bind(m_hSocket, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		m_nErrorCode = ::GetLastError();
		Close();
		return false;
	}

	if(m_eType == eBroadcast)
	{
		BOOL    bVal = TRUE;
		if(::setsockopt(m_hSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bVal, sizeof(bVal)) == SOCKET_ERROR)
		{
			m_nErrorCode = ::GetLastError();
			Close();
			return false;
		}
		if(::setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bVal, sizeof(bVal)) == SOCKET_ERROR)
		{
			m_nErrorCode = ::GetLastError();
			Close();
			return false;
		}
	}
	else if(m_eType == eMulticast)
	{
		struct  ip_mreq  mreq;

		//mreq.imr_multiaddr.s_addr = inet_addr(m_szIPAddress);	// IP multicast address of group
		::inet_pton(AF_INET, m_szIPAddress, &mreq.imr_multiaddr.s_addr); // IP multicast address of group
		mreq.imr_interface.s_addr = ::htonl(INADDR_ANY);		// local IP address of interface
		if(::setsockopt(m_hSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) == SOCKET_ERROR)
		{
			m_nErrorCode = ::GetLastError();
			Close();
			return false;
		}

		if(::setsockopt(m_hSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&m_nTTL, sizeof(m_nTTL)) == SOCKET_ERROR)
		{
			m_nErrorCode = ::GetLastError();
			Close();
			return false;
		}

		if(::setsockopt(m_hSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&m_biAllowLoop, sizeof(m_biAllowLoop)) == SOCKET_ERROR)
		{
			m_nErrorCode = ::GetLastError();
			Close();
			return false;
		}
	}

	return true;
}
//==============================================================================

bool CxUDPSocket::Create()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		::closesocket(m_hSocket);
	}

	m_hSocket = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = ::GetLastError();
		return false;
	}

	return true;
}
//==============================================================================

bool CxUDPSocket::Close()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		m_nErrorCode = ::GetLastError();
		return (m_nErrorCode == 0);
	}
	return true;
}
//==============================================================================

bool CxUDPSocket::Readable(int nTimeoutSec, int nTimeoutUSec)
{
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return false;
	}

	int	nRet;

	fd_set fdReads;
	timeval tTimeOut;

	FD_ZERO(&fdReads);
	FD_SET(m_hSocket, &fdReads);

	if(nTimeoutSec < 0)
	{
		nRet = ::select(0, &fdReads, NULL, NULL, NULL);
	}
	else
	{
		tTimeOut.tv_sec = nTimeoutSec + (nTimeoutUSec / 1000000);
		tTimeOut.tv_usec = (nTimeoutUSec % 1000000);
		nRet = ::select(0, &fdReads, NULL, NULL, &tTimeOut);
	}

	m_nErrorCode = 0;

	switch(nRet)
	{
		case 0: 
		{
			// timeout
			return false;
		}
		case 1:
		{
			// received
			return true;
		}
		default:
		{
			// error
			m_nErrorCode = ::GetLastError();
			Close();
			return false;
		}
	}
}
//==============================================================================

int CxUDPSocket::Recv(LPVOID pData, int nSize, char* pszIPAddress, unsigned short* punPortNo)
{
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return 0;
	}

	while(true)
	{
		sockaddr_in	sa;
		int nLen = sizeof(sa);

		m_nErrorCode = 0;
		
		nSize = ::recvfrom(m_hSocket, (char*)pData, nSize, 0, (sockaddr*)&sa, &nLen);
		if(nSize == 0)
		{
			//Close();
			return 0;
		}
		else if(nSize < 0)
		{
			m_nErrorCode = ::GetLastError();
			if(m_nErrorCode == WSAEWOULDBLOCK)
			{
				m_nErrorCode = 0;
				continue;
			}
			else if(m_nErrorCode == WSAECONNRESET)
			{
				Close();
				return 0;
			}
			return nSize;
		}

		if(pszIPAddress != NULL)
		{
			//strcpy_s(pszIPAddress, 16, ::inet_ntoa(sa.sin_addr));
			::inet_ntop(AF_INET, &sa.sin_addr, pszIPAddress, 16);
		}

		if(punPortNo != NULL)
		{
			*punPortNo = ::ntohs(sa.sin_port);
		}

		return nSize;
	}
}
//==============================================================================

int CxUDPSocket::Recv(LPVOID pData, int nSize, unsigned int* punIPAddress, unsigned short* punPortNo)
{
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK; 
		return 0;
	}

	while(true)
	{
		sockaddr_in	sa;
		int nLen = sizeof(sa);

		m_nErrorCode = 0;

		nSize = ::recvfrom(m_hSocket, (char*)pData, nSize, 0, (sockaddr*)&sa, &nLen);
		if(nSize == 0)
		{
			return 0;
		}
		else if(nSize < 0)
		{
			m_nErrorCode = ::GetLastError();
			if(m_nErrorCode == WSAEWOULDBLOCK)
			{
				m_nErrorCode = 0;
				continue;
			}
			else if(m_nErrorCode == WSAECONNRESET)
			{
				Close();
				return 0;
			}

			return nSize;
		}

		if(punIPAddress != NULL)
		{
			*punIPAddress = ::ntohl(sa.sin_addr.s_addr);
		}

		if(punPortNo != NULL)
		{
			*punPortNo = ::ntohs(sa.sin_port);
		}

		return nSize;
	}
}
//==============================================================================

bool CxUDPSocket::Send(const LPVOID pData, int nSize, char* pszIPAddress, unsigned short unPortNo)
{
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return false;
	}

	while(true)
	{
		sockaddr_in	sa;

		m_nErrorCode = 0;
		
		memset(&sa, 0x00, sizeof(sa));
		sa.sin_family = AF_INET;
		//sa.sin_addr.s_addr = ::inet_addr(pszIPAddress);
		::inet_pton(AF_INET, pszIPAddress, &sa.sin_addr.s_addr);

		if(unPortNo == 0)
		{
			unPortNo = m_unPortNo;
		}
		sa.sin_port = ::htons(unPortNo);

		nSize = ::sendto(m_hSocket, (char*)pData, nSize, 0, (sockaddr*)&sa, sizeof(sa));
		if(nSize == 0)
		{
			Close();
			return false;
		}
		else if(nSize < 0)
		{
			m_nErrorCode = ::GetLastError();
			if(m_nErrorCode == WSAEWOULDBLOCK)
			{
				m_nErrorCode = 0;
				continue;
			}
			else if( (m_nErrorCode == WSAECONNRESET) || (m_nErrorCode == WSAECONNABORTED) )
			{
				Close();
			}
			return false;
		}

		return true;
	}
}
//==============================================================================

bool CxUDPSocket::Send(const LPVOID pData, int nSize, unsigned int unIPAddress, unsigned short unPortNo)
{
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return false;
	}

	while(true)
	{
		sockaddr_in	sa;

		m_nErrorCode = 0;

		memset(&sa, 0x00, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = ::htonl(unIPAddress);
		if(unPortNo == 0)
		{
			unPortNo = m_unPortNo;
		}
		sa.sin_port = ::htons(unPortNo);

		nSize = ::sendto(m_hSocket, (char*)pData, nSize, 0, (sockaddr*)&sa, sizeof(sa));
		if(nSize == 0)
		{
			Close();
			return false;
		}
		else if(nSize < 0)
		{
			m_nErrorCode = ::GetLastError();
			if(m_nErrorCode == WSAEWOULDBLOCK)
			{
				m_nErrorCode = 0;
				continue;
			}
			else if( (m_nErrorCode == WSAECONNRESET) || (m_nErrorCode == WSAECONNABORTED) )
			{
				Close();
			}
			return false;
		}

		return true;
	}
}
//==============================================================================
