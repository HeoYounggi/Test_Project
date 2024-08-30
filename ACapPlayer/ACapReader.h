#ifndef _ACAPREADER_H_
#define _ACAPREADER_H_
//==============================================================================

class CACapQueue;
//==============================================================================

#include "ACapQueue.h"

#include "../Framework/xThread.h"
//==============================================================================

class CACapReader : public CxThread
{
public:
	CACapReader(CACapQueue* pcBuffer, CACapQueue* pcPool);
	virtual ~CACapReader();

	bool SetFilePath(TCHAR* pszFilePath);
	bool GetFilePath(TCHAR* pszGetFilePath);
	bool SetLoopDelayCount(int nLoops, int nDelay);
	bool GetLoopDelayCount(int* pnLoops, int* pnDelay);

	bool Start();
	bool Pause();
	bool Continue();
	bool Stop();

	bool SetFileSize(unsigned long long unFileSize);
	bool GetFileSize(unsigned long long *punFileSize);

	bool ReadPacketFile(FILE* pFile, bool bFinis);

	void PushPool(PTACAPQueueItem ptPacketItem);

protected:
	CACapQueue* m_pcBuffer;
	CACapQueue* m_pcPool;

	bool m_bPause;

	TCHAR m_szFilePath[MAX_PATH];

	int m_nLoopCount;
	int m_nDelayTime;

	unsigned long long m_unFileSize;

	virtual DWORD Execute();
};
//==============================================================================

#endif // _ACAPREADER_H_
