#ifndef _XLIST_H_
#define _XLIST_H_
//==============================================================================

class AFX_EXT_CLASS CxList
{
public:
	CxList();
	virtual ~CxList();

	void Clear();

	void SetGrowUnit(unsigned int unGrowUnit);

	void Lock();
	void UnLock();

	void Append(void* pItem);
	void Insert(unsigned int unIdx, void* pItem);
	void Delete(unsigned int unIdx);

	unsigned int GetCount();

	void* GetItem(unsigned int unIdx);
	void* GetFirst();
	void* GetLast();

private:
	unsigned int m_unCapacity;
	unsigned int m_unGrowUnit;
	unsigned int m_unItemCount;
	
	CRITICAL_SECTION m_csLock;

	void** m_pItems;
};
//==============================================================================

#endif //_XLIST_H_
