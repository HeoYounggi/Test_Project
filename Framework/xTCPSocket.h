#ifndef _XTCPSOCKET_H_
#define _XTCPSOCKET_H_


//=============================================================================
//
// Class Declare ( CxTCPSocket )
// - TCP Socket class
//
//=============================================================================

class AFX_EXT_CLASS CxTCPSocket
{
	/**************************o
	* Constructor / Destructor *
	o**************************/
public:
	CxTCPSocket();
	virtual ~CxTCPSocket();

	/*****************o
	* Member variable *
	o*****************/
protected:
	SOCKET					m_hSocket;
	int						m_nErrorCode;

	char					m_szIPAddress[16];
	unsigned short			m_unPortNo;

	/***********o
	* Functions *
	o***********/
public:
	bool					Connect();
	bool 					Close();

	bool 					Readable(int nTimeoutSec, int nTimeoutUSec = 0);
	unsigned int			RecvSize();

	int						Recv(LPVOID pData, int nSize);
	int 					Send(const LPVOID pData, int nSize);

	char* 					GetIPAddress();
	void 					SetIPAddress(char* pszIPAddress);

	int 					GetPortNo();
	void 					SetPortNo(unsigned short unPortNo);

	void 					SetKeppAliveTime(unsigned long unTime, unsigned long unInterval);

	int 					ErrorCode();
	bool 					Active();

	SOCKET 					GetSocket() { return m_hSocket; }

	/*******************o
	* Virtual Functions *
	o*******************/
};
//==============================================================================

#endif // _xTCPSOCKET_H_
