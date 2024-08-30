#ifndef _XTCPSERVER_H_
#define _XTCPSERVER_H_
//==============================================================================

#include "xThread.h"
//==============================================================================

class CxList;
//==============================================================================

typedef struct _TCPClientItem
{
	SOCKET hSocket;
	unsigned long unIPAddress;
	unsigned short unPort;
	int nUserData;
	void* pUserData;
} TCPClientItem, *PTCPClientItem;
//==============================================================================

class AFX_EXT_CLASS CxTCPServer : public CxThread
{
public: 
	CxTCPServer();
	virtual ~CxTCPServer();

	int GetPortNo();
	void SetPortNo(unsigned short unPortNo);

	int ErrorCode();
	bool Active();

	bool StartServer();
	bool StopServer();

	int	Recv(PTCPClientItem pClient, LPVOID pData, int nSize);
	int Send(PTCPClientItem pClient, LPVOID pData, int nSize);
	bool Close(PTCPClientItem pClient);

	unsigned int GetClientCount();
	PTCPClientItem GetClient(unsigned int unIdx);

protected:
	bool CreateServSock();
	bool CloseServSock();

	bool BindServSock();
	bool ListenServSock();
	bool Accept();

	virtual DWORD Execute(); // override

	virtual void OnAccept(PTCPClientItem pClient) = 0;
	virtual void OnDisconnect(PTCPClientItem pClient) = 0;
	virtual void OnRecv(PTCPClientItem pClient) = 0;

	SOCKET m_hSocket;
	int	 m_nErrorCode;

	unsigned short m_unPortNo;

	CxList* m_pClientList;

private:
	bool FindIdxClientItem(SOCKET hSocket, unsigned int* punIdx);
};
//==============================================================================

#endif // _XTCPSERVER_H_
