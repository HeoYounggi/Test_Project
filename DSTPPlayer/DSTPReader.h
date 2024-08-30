#ifndef _DSTPREADER_H_
#define _DSTPREADER_H_
//==============================================================================

class CDSTPQueue;
//==============================================================================

#include "DSTPQueue.h"

#include "../Framework/xThread.h"
#include "../Framework/xTrace.h"
//==============================================================================

class CDSTPReader : public CxThread
{
public:
	CDSTPReader(CDSTPQueue* pcBuffer, CDSTPQueue* pcPool);
	virtual ~CDSTPReader();

	bool SetFilePath(TCHAR* pszFilePath);
	bool GetFilePath(TCHAR* pszGetFilePath);

	bool SetLoopDelay(int nLoopCount, int nDelay);
	bool GetLoopDelay(int* pnLoopCount, int* pnDelay);

	bool SetFileSize(unsigned long long unFileSize);
	bool GetFileSize(unsigned long long* punFileSize);

	bool Start();
	bool Stop();

	bool ReadPacketFile(FILE* pFile, bool bFinish);
	
	void PushPool(PTDSTPQueueItem ptPacketItem);

protected:
	CDSTPQueue* m_pcBuffer;
	CDSTPQueue* m_pcPool;

	TCHAR m_szFilePath[MAX_PATH];

	int m_nLoopCount;
	int m_nDelay;

	unsigned long long m_unFileSize;

	virtual DWORD Execute();
};
//==============================================================================
#endif // _DSTPREADER_H_


