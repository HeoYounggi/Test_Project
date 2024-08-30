#include "pch.h"
//==============================================================================

#include "xByteBuffer.h"
#include "xTrace.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//==============================================================================

CxByteBuffer::CxByteBuffer(int nMaxSize)
{
	m_pBuffer = new unsigned char[nMaxSize];
	memset(m_pBuffer, NULL, nMaxSize);

	m_nMaxSize = nMaxSize;
    m_nFront = 0;
    m_nRear = 0;
    m_nSize = 0;

	m_pLock =  new CRITICAL_SECTION;
	InitializeCriticalSection(m_pLock);
}
//==============================================================================

CxByteBuffer::~CxByteBuffer(void)
{
	DeleteCriticalSection(m_pLock);
	delete m_pLock;
	m_pLock = NULL;

	m_nMaxSize = 0;
    m_nFront = 0;
    m_nRear = 0;
    m_nSize = 0;

	delete[] m_pBuffer;
	m_pBuffer = NULL;
}
//==============================================================================

EBYTEBUFFER_ERROR CxByteBuffer::Push(unsigned char* pData, int nLength)
{
    int nRemainSize;
    
    nRemainSize = GetRemainSize();
    if(nRemainSize < nLength)
    {
        AxTRACE(TEXT("[Warning] Push Byte Buffer Overflow~!!! %d\r\n"), nRemainSize);
        return eBYTEBUFFER_ERR_OVERFLOW;
    }
    
	::EnterCriticalSection(m_pLock);

	if(m_nRear + nLength < m_nMaxSize)
	{
		memcpy(m_pBuffer + m_nRear, pData, nLength);
	}
	else
	{
		nRemainSize = m_nMaxSize - m_nRear;
		memcpy(m_pBuffer + m_nRear, pData, nRemainSize);
		memcpy(m_pBuffer, pData + nRemainSize, nLength - nRemainSize);
	}

	m_nRear += nLength;
	m_nRear %= m_nMaxSize;
	m_nSize += nLength;

	::LeaveCriticalSection(m_pLock);

    return eBYTEBUFFER_ERR_SUCCESS;
}
//==============================================================================

int CxByteBuffer::Pop(unsigned char* pData, int nLength)
{
    int nBufSize;

	nBufSize = GetSize();
	
	if(nBufSize == 0)
	{
		return nBufSize;
	}

	if(nBufSize < nLength)
	{
		nLength = nBufSize;
	}
	
	::EnterCriticalSection(m_pLock);
	
	if(m_nFront + nLength < m_nMaxSize)
	{
		memcpy(pData, m_pBuffer + m_nFront, nLength);
	}
	else
	{
		nBufSize = m_nMaxSize - m_nFront;
		memcpy(pData, m_pBuffer + m_nFront, nBufSize);
		memcpy(pData + nBufSize, m_pBuffer, nLength - nBufSize);
	}

	m_nFront += nLength;
	m_nFront %= m_nMaxSize;
	m_nSize -= nLength;

	::LeaveCriticalSection(m_pLock);

    return nLength;
}
//==============================================================================

int CxByteBuffer::Read(unsigned char* pData, int nLength)
{
    int nBufSize;

	nBufSize = GetSize();
	
	if(nBufSize == 0)
	{
		return nBufSize;
	}

	if(nBufSize < nLength)
	{
		nLength = nBufSize;
	}

	::EnterCriticalSection(m_pLock);

	if(m_nFront + nLength < m_nMaxSize)
	{
		memcpy(pData, m_pBuffer + m_nFront, nLength);
	}
	else
	{
		nBufSize = m_nMaxSize - m_nFront;
		memcpy(pData, m_pBuffer + m_nFront, nBufSize);
		memcpy(pData + nBufSize, m_pBuffer, nLength - nBufSize);
	}

	::LeaveCriticalSection(m_pLock);

    return eBYTEBUFFER_ERR_SUCCESS;
}
//==============================================================================

void CxByteBuffer::Clear()
{
	::EnterCriticalSection(m_pLock);
    m_nFront = 0;
    m_nRear = 0;
    m_nSize = 0;
	::LeaveCriticalSection(m_pLock);
}
//==============================================================================

int CxByteBuffer::GetSize()
{
    int nSize;

	//::EnterCriticalSection(m_pLock);
    nSize = m_nSize;
	//::LeaveCriticalSection(m_pLock);
    
    return nSize;
}
//==============================================================================

int CxByteBuffer::GetMaxSize()
{
    int nMaxSize;

	//::EnterCriticalSection(m_pLock);
    nMaxSize = m_nMaxSize;
	//::LeaveCriticalSection(m_pLock);
    
    return nMaxSize;
}
//==============================================================================

int CxByteBuffer::GetRemainSize()
{
    int nRemainSize;

	//::EnterCriticalSection(m_pLock);
    nRemainSize = m_nMaxSize - m_nSize;
	//::LeaveCriticalSection(m_pLock);

    return nRemainSize;
}
//==============================================================================
