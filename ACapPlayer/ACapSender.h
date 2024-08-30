#ifndef _ACAPSENDER_H_
#define _ACAPSENDER_H_
//==============================================================================

#include "ACapQueue.h"

#include "../Framework/xThread.h"
//==============================================================================

class CACapQueue;
//==============================================================================

typedef struct TSenderInfo
{
	unsigned int unFileSendCount;
	int nLoopCount;
	unsigned long long unSendingTime;
	unsigned int unSendBytes;
	unsigned long long unBitrateCount;
	bool bFinish;
} TSenderInfo, * PTSenderInfo;
//==============================================================================

class CACapSender : public CxThread
{
public:
	CACapSender(CACapQueue* pcBuffer, CACapQueue* pcPool);
	virtual ~CACapSender();

	bool Start();
	bool Stop();

	void SetPcapHandle(pcap_t* pPcapHandle);

	bool GetSenderInfo(TSenderInfo* ptSenderInfo);

protected:
	pcap_t* m_pPcapHandle;

	CACapQueue* m_pcBuffer;
	CACapQueue* m_pcPool;

	TSenderInfo m_tSenderInfo;

	virtual DWORD Execute();
};
//==============================================================================

#endif // _ACAPSENDER_H_
