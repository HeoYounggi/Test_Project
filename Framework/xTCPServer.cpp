#include "pch.h"
//==============================================================================

#include "xTCPServer.h"

#include "xWSAManager.h"
#include "xList.h"
#include "xTrace.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//==============================================================================

CxTCPServer::CxTCPServer()
{
	m_hSocket = INVALID_SOCKET;
	m_nErrorCode = 0;
	m_unPortNo = 0;

	m_pClientList = new CxList();
}
//==============================================================================

CxTCPServer::~CxTCPServer()
{
	// Stop Server..
	if(m_hSocket != INVALID_SOCKET)
	{
		StopServer();
	}
	delete m_pClientList;
	m_pClientList = NULL;

	m_hSocket = INVALID_SOCKET;
	m_nErrorCode = 0;
	m_unPortNo = 0;
}
//==============================================================================

int CxTCPServer::GetPortNo()
{
	return m_unPortNo;
}
//==============================================================================

void CxTCPServer::SetPortNo(unsigned short unPortNo)
{
	m_unPortNo = unPortNo;
}
//==============================================================================

int CxTCPServer::ErrorCode()
{
	return m_nErrorCode;
}
//==============================================================================

bool CxTCPServer::Active()
{
	return m_hSocket != INVALID_SOCKET;
}
//==============================================================================

bool CxTCPServer::StartServer()
{
	PTCPClientItem pClient;

	for(unsigned int i = 0; i < m_pClientList->GetCount(); i++)
	{
		pClient = (PTCPClientItem)m_pClientList->GetItem(i);
		if(pClient->hSocket != INVALID_SOCKET)
		{
			Close(pClient);
		}
		delete pClient;
	}
	m_pClientList->Clear();

	CreateServSock();
	BindServSock();
	ListenServSock();

	StartThread();

	return true;
}
//==============================================================================

bool CxTCPServer::StopServer()
{
	StopThread();
	Sleep(1);
	WaitFor();

	PTCPClientItem pClient;

	for(unsigned int i = 0; i < m_pClientList->GetCount(); i++)
	{
		pClient = (PTCPClientItem)m_pClientList->GetItem(i);
		if(pClient->hSocket != INVALID_SOCKET)
		{
			Close(pClient);
		}

		delete pClient;
	}
	m_pClientList->Clear();

	// Close...
	CloseServSock();

	return true;
}
//==============================================================================

int	CxTCPServer::Recv(PTCPClientItem pClient, LPVOID pData, int nSize)
{
	if(pClient->hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = WSAENOTSOCK;
		return false;
	}

	char* buffer = (char*)pData;
	int nResult, nLeft, idx;
	nLeft = nSize;
	idx = 0;

	while(nLeft > 0)
	{
		nResult = ::recv(pClient->hSocket, &buffer[idx], nLeft, 0);
		if (nResult == 0)
		{
			// the connection has been gracefully closed
			Close(pClient);
			return false;
		}
		else if (nResult == SOCKET_ERROR)
		{
			m_nErrorCode = ::WSAGetLastError();
			if(m_nErrorCode == WSAEWOULDBLOCK)
			{
				// ignore WSAEWOULDBLOCK error
				m_nErrorCode = 0;
				continue;
			}
			else if( (m_nErrorCode == WSAECONNABORTED) || (m_nErrorCode == WSAECONNRESET) )
			{
				// disconnect (10053, 10054)
				Close(pClient);
			}
			return false;
		}
		nLeft -= nResult;
		idx += nResult;
	}

	m_nErrorCode = 0;

	return idx;
}
//==============================================================================

int CxTCPServer::Send(PTCPClientItem pClient, LPVOID pData, int nSize)
{
	if(pClient->hSocket == INVALID_SOCKET)
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
		ret = ::send(pClient->hSocket, buffer, nLeft, 0);
		if (ret == 0)
		{
			// the connection has been gracefully closed
			Close(pClient);
			return false;
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
			else if( (m_nErrorCode == WSAECONNABORTED) || (m_nErrorCode == WSAECONNRESET) )
			{
				// disconnect (10053, 10054)
				Close(pClient);
			}
			return false;
		}
		nLeft -= ret;
		buffer += ret;
	}

	m_nErrorCode = 0;

	return (int)(buffer - (char*)pData);
}
//==============================================================================

bool CxTCPServer::Close(PTCPClientItem pClient)
{
	if(pClient->hSocket != INVALID_SOCKET)
	{
		unsigned int unIdx;
		bool bResult;

		OnDisconnect(pClient);

		bResult = FindIdxClientItem(pClient->hSocket, &unIdx);
		if(bResult == true)
		{
			::closesocket(pClient->hSocket);
			pClient->hSocket = INVALID_SOCKET;
		}

		m_nErrorCode = ::WSAGetLastError();
	}

	return true;
}
//==============================================================================

unsigned int CxTCPServer::GetClientCount()
{
	return m_pClientList->GetCount();
}
//==============================================================================

PTCPClientItem CxTCPServer::GetClient(unsigned int unIdx)
{
	PTCPClientItem pClient;
	if(unIdx < m_pClientList->GetCount())
	{
		pClient = (PTCPClientItem)m_pClientList->GetItem(unIdx);
		return pClient;
	}
	else
	{
		return NULL;
	}
}
//==============================================================================

bool CxTCPServer::CreateServSock()
{
	int nDefaultTimeOut = 2000;
	int nResult;

	if(m_hSocket != INVALID_SOCKET)
	{
		::closesocket(m_hSocket);
	}

	m_hSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
	if(m_hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = ::WSAGetLastError();
		return false;
	}

	nResult = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nDefaultTimeOut, sizeof(nDefaultTimeOut));
	if(nResult == SOCKET_ERROR)
	{
		m_nErrorCode = ::WSAGetLastError();
		return false;
	}

	return true;
}
//==============================================================================

bool CxTCPServer::CloseServSock()
{
	if(m_hSocket != INVALID_SOCKET)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		m_nErrorCode = ::WSAGetLastError();
		return (m_nErrorCode == 0);
	}

	return true;
}
//==============================================================================

bool CxTCPServer::BindServSock()
{
	sockaddr_in	sa;
	
	memset(&sa, 0x00, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = ::htons(m_unPortNo);
	sa.sin_addr.s_addr = INADDR_ANY;

	if(::bind(m_hSocket, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		m_nErrorCode = ::WSAGetLastError();
		CloseServSock();
		return false;
	}

	return true;
}
//==============================================================================

bool CxTCPServer::ListenServSock()
{
	if(::listen(m_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		m_nErrorCode = ::WSAGetLastError();
		CloseServSock();
		return false;
	}
	return true;
}
//==============================================================================

bool CxTCPServer::Accept()
{
	int nClientAddrLength;
	struct sockaddr_in tClientAddr;

	int nDefaultTimeOut = 1000;
	int nResult;

	SOCKET hSocket;

	nClientAddrLength = sizeof(tClientAddr);
	hSocket = ::accept(m_hSocket, (struct sockaddr*)&tClientAddr, &nClientAddrLength);

	if(hSocket == INVALID_SOCKET)
	{
		m_nErrorCode = ::WSAGetLastError();
		return false;
	}

	m_nErrorCode = 0;

	nResult = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nDefaultTimeOut, sizeof(nDefaultTimeOut));
	if(nResult == SOCKET_ERROR)
	{
		m_nErrorCode = ::WSAGetLastError();
		return false;
	}

	// 연결상태를 자동으로 유지하도록 옵션 설정
	BOOL biKeepAlive = TRUE;
	if(::setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&biKeepAlive, sizeof(biKeepAlive)) == SOCKET_ERROR)
	{
		m_nErrorCode = ::WSAGetLastError();
		::closesocket(hSocket);
		hSocket = INVALID_SOCKET;

		return false;
	}

	tcp_keepalive tcpkl;
	DWORD dwRet;

	tcpkl.onoff = 1;			// KEEPALIVE ON
	tcpkl.keepalivetime = 1000; // 1초 마다 KEEPALIVE 신호를 보내겠다. (Default 는 2시간이다)
	tcpkl.keepaliveinterval = 200; // keepalive 신호를 보내고 응답이 없으면 1초마다 재 전송하겠다. (ms tcp 는 10회 재시도 한다)

	WSAIoctl(hSocket, SIO_KEEPALIVE_VALS, &tcpkl, sizeof(tcp_keepalive), 0, 0, &dwRet, NULL, NULL);

	PTCPClientItem pClient = new TCPClientItem;
	pClient->hSocket = hSocket;
	pClient->unPort = ::ntohs(tClientAddr.sin_port);
	pClient->unIPAddress = ::ntohl(tClientAddr.sin_addr.s_addr);
	pClient->nUserData = 0;
	pClient->pUserData = NULL;

	m_pClientList->Lock();
	m_pClientList->Append(pClient);
	m_pClientList->UnLock();

	OnAccept(pClient);

	return true;
}
//==============================================================================

DWORD CxTCPServer::Execute()
{
	fd_set fdReads;
	timeval tTimeOut;

	int nResult;

	while(m_bTerminated == false)
	{
		FD_ZERO(&fdReads);
		FD_SET(m_hSocket, &fdReads);
		
		for(unsigned int i = 0; i < m_pClientList->GetCount(); i++)
		{
			PTCPClientItem pClient;
			pClient = (PTCPClientItem)m_pClientList->GetItem(i);

			FD_SET(pClient->hSocket, &fdReads);
		}

		tTimeOut.tv_sec = 2;
		tTimeOut.tv_usec = 0;

		nResult = select(0, &fdReads, NULL, NULL, &tTimeOut);
		if(nResult == 0)
		{
			// Timeout
			continue;
		}
		else if(nResult < 0)
		{
			m_nErrorCode = ::WSAGetLastError();

			AxTRACE(TEXT("Select Failed.\r\n"));
			break;
		}

		if(FD_ISSET(m_hSocket, &fdReads))
		{
			// Accept
			Accept();
		}

		for(unsigned int i = m_pClientList->GetCount(); i > 0 ; i--)
		{
			PTCPClientItem pClient;
			pClient = (PTCPClientItem)m_pClientList->GetItem(i - 1);

			if(FD_ISSET(pClient->hSocket, &fdReads))
			{
				OnRecv(pClient);
			}
		}

		for(unsigned int i = m_pClientList->GetCount(); i > 0 ; i--)
		{
			PTCPClientItem pClient;
			pClient = (PTCPClientItem)m_pClientList->GetItem(i - 1);

			if(pClient->hSocket == INVALID_SOCKET)
			{
				m_pClientList->Lock();
				m_pClientList->Delete(i - 1);
				m_pClientList->UnLock();
				
				delete pClient;
			}
		}
	}

	AxTRACE(TEXT("TCP Server Terminated\r\n"));
	CloseServSock();

	return 0;
}
//==============================================================================

bool CxTCPServer::FindIdxClientItem(SOCKET hSocket, unsigned int* punIdx)
{
	bool bResult = false;

	for(unsigned int i = 0; i < m_pClientList->GetCount(); i++)
	{
		PTCPClientItem pClient;

		pClient = (PTCPClientItem)m_pClientList->GetItem(i);
		if(pClient->hSocket == hSocket)
		{
			*punIdx = i;
			bResult = true;
			break;
		}
	}
	
	return bResult;
}
//==============================================================================
