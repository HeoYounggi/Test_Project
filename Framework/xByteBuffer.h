#ifndef _XBYTEBUFFER_H_
#define _XBYTEBUFFER_H_
//==============================================================================

typedef enum _E_BYTEBUFFER_ERROR
{
	eBYTEBUFFER_ERR_SUCCESS		= 0x00,
	eBYTEBUFFER_ERR_OVERFLOW	= 0x01,
	eBYTEBUFFER_ERR_UNDERFLOW	= 0x02,
	eBYTEBUFFER_ERR_UNKNOWN		= 0xFF,
} EBYTEBUFFER_ERROR, * PEBYTEBUFFER_ERROR;
//==============================================================================

class AFX_EXT_CLASS CxByteBuffer
{
protected:
	LPCRITICAL_SECTION m_pLock;
	unsigned char* m_pBuffer;

	int m_nMaxSize;
	int m_nFront;
	int m_nRear;
	int m_nSize;

public:
	CxByteBuffer(int nMaxSize);
	virtual ~CxByteBuffer(void);

	EBYTEBUFFER_ERROR Push(unsigned char* pData, int nLength);
	int Pop(unsigned char* pData, int nLength);
	int Read(unsigned char* pData, int nLength);
	void Clear();

	int GetSize();
	int GetMaxSize();
	int GetRemainSize();
};
//==============================================================================

#endif // _XBYTEBUFFER_H_
