#ifndef _XUDPSOCKET_H_
#define _XUDPSOCKET_H_
//==============================================================================

typedef enum _EUDP_TYPE
{
	eUnicast,
	eBroadcast,
	eMulticast,
} EUDP_TYPE, * PEUDP_TYPE;
//==============================================================================

class AFX_EXT_CLASS CxUDPSocket
{
private:
	SOCKET m_hSocket;
	int	 m_nErrorCode;
	EUDP_TYPE m_eType;

	char m_szIPAddress[16];
	unsigned short m_unPortNo;

private: 
	int	m_nTTL;
	BOOL m_biAllowLoop;

public: 
	bool Create();
	bool Close();

	bool Readable(int nTimeoutSec, int nTimeoutUSec = 0);
	
	int	Recv(LPVOID pData, int nSize, char* pszIPAddress, unsigned short* punPortNo);
	int	Recv(LPVOID pData, int nSize, unsigned int* punIPAddress, unsigned short* punPortNo);
	bool Send(const LPVOID pData, int nSize, char* pszIPAddress, unsigned short unPortNo);
	bool Send(const LPVOID pData, int nSize, unsigned int unIPAddress, unsigned short unPortNo);
public:
	bool Bind();

public:
	int SetNetworkInterface(unsigned int unIdx);
	char* GetIPAddress() { return m_szIPAddress; }
	void SetIPAddress(char* pszIPAddress) { strcpy_s(m_szIPAddress, 16, pszIPAddress); }
	int GetPortNo() { return m_unPortNo; }
	void SetPortNo(unsigned short unPortNo) { m_unPortNo = unPortNo; }
	int GetTTL() { return m_nTTL; }
	void SetTTL(int nTTL) { m_nTTL = nTTL; }
	BOOL GetAllowLoop() { return m_biAllowLoop; }
	void SetAllowLoop(BOOL biAllowLoop) { m_biAllowLoop = biAllowLoop; }
	EUDP_TYPE GetType() { return m_eType; }
	void SetType(EUDP_TYPE eType) { m_eType = eType; }
	int ErrorCode() { return m_nErrorCode; }
	bool Active() { return m_hSocket != INVALID_SOCKET; }

public:
	CxUDPSocket();
	virtual ~CxUDPSocket();
};
//==============================================================================

#endif // _xUDPSOCKET_H_
