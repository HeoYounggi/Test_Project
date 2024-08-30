#ifndef _DSRPSENDER_H_
#define _DSRPSENDER_H_
//==============================================================================

class CDSTPQueue;
//==============================================================================

#include "DSTPQueue.h"

#include "../Framework/xThread.h"
#include "../Framework/xTrace.h"
//==============================================================================

typedef struct TSenderInfo
{
	unsigned int unFileSendCount;
	int nLoopCount;
	unsigned long long unSendingTime;
	unsigned int unSendBytes;
	unsigned long long unBitrateCount;
	bool bFinish;
}TSenderInfo, *PTSenderInfo;
//==============================================================================

class CDSTPSender : public CxThread
{
public:
	CDSTPSender(CDSTPQueue* pcBuffer, CDSTPQueue* pcPool);
	virtual ~CDSTPSender();

	bool Start();
	bool Stop();

	bool GetSenderInfo(TSenderInfo* ptSenderInfo);

protected:
	CDSTPQueue* m_pcBuffer;
	CDSTPQueue* m_pcPool;

	TSenderInfo m_tSenderInfo;

	virtual DWORD Execute();
};
//==============================================================================
#endif // _DSRPSENDER_H_


