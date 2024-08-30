#include "pch.h"
//==============================================================================

#include "ACapQueue.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

CACapQueue::CACapQueue(unsigned int unSize/* = 32 */)
{
	m_unMaxSize = unSize;
	m_unSize = 0;

	m_unFront = 0;
	m_unRear = 0;

	m_ptQueue = new PTACAPQueueItem[m_unMaxSize];

	InitializeCriticalSection(&m_csLock);
}
//==============================================================================

CACapQueue::~CACapQueue()
{
	DeleteCriticalSection(&m_csLock);

	delete[] m_ptQueue;
	m_ptQueue = NULL;
}
//==============================================================================

bool CACapQueue::Push(PTACAPQueueItem ptItem)
{
	if (IsFull() == true)
	{
		//AxDEBUG(TEXT("Queue Full\n"));
		return false;
	}

	EnterCriticalSection(&m_csLock);

	m_ptQueue[m_unRear] = ptItem;

	m_unSize++;
	m_unRear++;
	m_unRear = m_unRear % m_unMaxSize;

	LeaveCriticalSection(&m_csLock);

	return true;
}
//==============================================================================

bool CACapQueue::Pop(PTACAPQueueItem* pptItem)
{
	if (IsEmpty() == true)
	{
		//AxDEBUG(TEXT("Queue Empty\n"));
		return false;
	}

	EnterCriticalSection(&m_csLock);

	*pptItem = m_ptQueue[m_unFront];

	m_unSize--;
	m_unFront++;
	m_unFront = m_unFront % m_unMaxSize;

	LeaveCriticalSection(&m_csLock);

	return true;
}
//==============================================================================

bool CACapQueue::Clear()
{
	EnterCriticalSection(&m_csLock);

	m_unSize = 0;
	m_unFront=0;
	m_unRear = 0;

	LeaveCriticalSection(&m_csLock);

	return true;
}
//==============================================================================

unsigned int CACapQueue::GetMaxSize()
{
	return m_unMaxSize;
}
//==============================================================================

unsigned int CACapQueue::GetSize()
{
	unsigned int unResult;

	EnterCriticalSection(&m_csLock);

	unResult = m_unSize;

	LeaveCriticalSection(&m_csLock);

	return unResult;
}
//==============================================================================

bool CACapQueue::IsEmpty()
{
	bool bResult;

	EnterCriticalSection(&m_csLock);

	bResult = (m_unSize == 0);

	LeaveCriticalSection(&m_csLock);

	return bResult;
}
//==============================================================================

bool CACapQueue::IsFull()
{
	bool bResult;

	EnterCriticalSection(&m_csLock);

	bResult = (m_unSize == m_unMaxSize);

	LeaveCriticalSection(&m_csLock);

	return bResult;
}
//==============================================================================
