#ifndef _ACAPQUEUE_H_
#define _ACAPQUEUE_H_
//==============================================================================

#define ACAP_MAX_PACKET_SIZE			(9*1024+10)
//==============================================================================

#include "../Framework/xTrace.h"
//==============================================================================

typedef struct TACAPQueueItem
{
	pcap_pkthdr tPacketHeader;
	unsigned char zbPacketData[ACAP_MAX_PACKET_SIZE];
	bool bFirstPacket;
	bool bFinishPacket;
}TACAPQueueItem, *PTACAPQueueItem;
//==============================================================================

class CACapQueue
{
public:
	CACapQueue(unsigned int unSize = 32);
	virtual ~CACapQueue();

	bool Push(PTACAPQueueItem ptItem);
	bool Pop(PTACAPQueueItem* pptItem);
	bool Clear();

	unsigned int GetMaxSize();
	unsigned int GetSize();

	bool IsEmpty();
	bool IsFull();

protected:
	unsigned int m_unMaxSize;
	unsigned int m_unSize;

	unsigned int m_unFront;
	unsigned int m_unRear;

	PTACAPQueueItem* m_ptQueue;

	CRITICAL_SECTION m_csLock;
};
//==============================================================================

#endif // _ACAPQUEUE_H_
