#ifndef _DSTPQUEUE_H_
#define _DSTPQUEUE_H_
//==============================================================================

#define DSTP_MAX_PACKET_SIZE			(8*1024+10)
//==============================================================================

#include "DSTPPcap.h"

#include "../Framework/xTrace.h"
//==============================================================================

typedef struct TDSTPQueueItem
{
	pcaprec_hdr_t tPacketHeader;
	unsigned char zbPacketData[DSTP_MAX_PACKET_SIZE];
	bool bFirstPacket;
	bool bFinishPacket;
}TDSTPQueueItem, *PTDSTPQueueItem;
//==============================================================================

class CDSTPQueue
{
public:
	CDSTPQueue(unsigned int unSize = 32);
	virtual ~CDSTPQueue();

	bool Push(PTDSTPQueueItem ptItem);
	bool Pop(PTDSTPQueueItem* pptItem);
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

	PTDSTPQueueItem* m_ptQueue;

	CRITICAL_SECTION m_csLock;
};
//==============================================================================
#endif // _DSTPQUEUE_H_


