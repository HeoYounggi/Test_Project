#include "pch.h"
//==============================================================================

#include "xList.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//==============================================================================

#define MX_UTIL_LIST_DEFALUT_GROWUNIT 16
//==============================================================================

CxList::CxList()
{
	m_unGrowUnit = MX_UTIL_LIST_DEFALUT_GROWUNIT;
	m_unCapacity = MX_UTIL_LIST_DEFALUT_GROWUNIT;
	m_unItemCount = 0x00;
	m_pItems = (void**)malloc(sizeof(void*) * MX_UTIL_LIST_DEFALUT_GROWUNIT);

	InitializeCriticalSection(&m_csLock);
}
//==============================================================================

CxList::~CxList()
{
	DeleteCriticalSection(&m_csLock);

	free(m_pItems);
}
//==============================================================================

void CxList::Clear()
{
	free(m_pItems);

	m_unItemCount = 0x00;
	m_unCapacity = m_unGrowUnit;
	m_pItems = (void**)malloc(sizeof(void*) * m_unGrowUnit);
}
//==============================================================================

void CxList::SetGrowUnit(unsigned int unGrowUnit)
{
	m_unGrowUnit = unGrowUnit;
}
//==============================================================================

void CxList::Lock()
{
	EnterCriticalSection(&m_csLock);
}
//==============================================================================

void CxList::UnLock()
{
	LeaveCriticalSection(&m_csLock);
}
//==============================================================================

void CxList::Append(void* pItem)
{
	if(m_unItemCount == m_unCapacity)
	{
		m_unCapacity = m_unCapacity + m_unGrowUnit;
		m_pItems = (void**)realloc(m_pItems, sizeof(void*) * m_unCapacity);
	}

	m_pItems[m_unItemCount] = pItem;
	m_unItemCount++;
}
//==============================================================================

void CxList::Insert(unsigned int unIdx, void* pItem)
{
	if(m_unItemCount < unIdx)
	{
		return;
	}

	if(m_unItemCount == m_unCapacity)
	{
		m_unCapacity = m_unCapacity + m_unGrowUnit;
		m_pItems = (void**)realloc(m_pItems, sizeof(void*) * m_unCapacity);
	}

	memmove(m_pItems + unIdx + 1, m_pItems + unIdx, sizeof(void*) * (m_unItemCount - unIdx));

	m_pItems[unIdx] = pItem;
	m_unItemCount++;
}
//==============================================================================

void CxList::Delete(unsigned int unIdx)
{
	if(m_unItemCount < unIdx)
	{
		return;
	}

	m_unItemCount--;
	memmove(m_pItems + unIdx, m_pItems + unIdx + 1, sizeof(void*) * (m_unItemCount - unIdx));
}
//==============================================================================

unsigned int CxList::GetCount()
{
	unsigned int unResult;

	unResult = m_unItemCount;

	return unResult;
}
//==============================================================================

void* CxList::GetItem(unsigned int unIdx)
{
	return m_pItems[unIdx];
}
//==============================================================================

void* CxList::GetFirst()
{
	if(m_unItemCount == 0x00)
	{
		return NULL;
	}

	return m_pItems[0];
}
//==============================================================================

void* CxList::GetLast()
{
	if(m_unItemCount == 0x00)
	{
		return NULL;
	}

	return m_pItems[m_unItemCount - 1];
}
//==============================================================================
