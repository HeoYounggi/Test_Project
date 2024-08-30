#include "pch.h"
//==============================================================================

#include "DSTPSender.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

CDSTPSender::CDSTPSender(CDSTPQueue* pcBuffer, CDSTPQueue* pcPool)
{
	m_pcBuffer = pcBuffer;
	m_pcPool = pcPool;

	memset(&m_tSenderInfo, 0x00, sizeof(TSenderInfo));
}
//==============================================================================

CDSTPSender::~CDSTPSender()
{
	m_pcBuffer = NULL;
	m_pcPool = NULL;
}
//==============================================================================

bool CDSTPSender::Start()
{
	StartThread();

	return true;
}
//==============================================================================

bool CDSTPSender::Stop()
{
	StopThread();
	WaitFor();

	return true;
}
//==============================================================================

DWORD CDSTPSender::Execute()
{
	AxTRACE(TEXT("Sender Thread Start!\n"));

	bool bResult;

	PTDSTPQueueItem ptPacketItem;

	unsigned long long unSendingTime = 0;
	unsigned int unFileSendCount = 0;

	int nLoopCount = 0;
	unsigned int unSendBytes = 0;

	unsigned long long unDiffSec = 0;

	unsigned long long unStartPacketTimeUS = 0;
	unsigned long long unCurrentPacketTimeUS = 0;

	unsigned long long unStartTickUS = GetTickCount64() * 1000;
	unsigned long long unCurrentTickUS = 0;

	unsigned long long unDiffPacketTimeUS = 0;
	unsigned long long unDiffTickTimeUS = 0;

	while (m_bTerminated == false)
	{
		bResult = m_pcBuffer->Pop(&ptPacketItem);
		if (bResult == false)
		{
			Sleep(10);

			continue;
		}

		if (ptPacketItem->bFirstPacket)
		{
			unSendingTime += unDiffPacketTimeUS;

			unStartPacketTimeUS = 0;
			unStartTickUS = GetTickCount64() * 1000;

			unFileSendCount = 0;
			nLoopCount++;

			m_tSenderInfo.unSendBytes = 0;
		}

		if (unStartPacketTimeUS == 0)
		{
			unStartPacketTimeUS = (ptPacketItem->tPacketHeader.ts_sec * 1000000) + ptPacketItem->tPacketHeader.ts_usec;
		}

		unCurrentPacketTimeUS = (ptPacketItem->tPacketHeader.ts_sec * 1000000) + ptPacketItem->tPacketHeader.ts_usec;
		unCurrentTickUS = GetTickCount64() * 1000;

		unDiffPacketTimeUS = unCurrentPacketTimeUS - unStartPacketTimeUS;
		unDiffTickTimeUS = unCurrentTickUS - unStartTickUS;

		while(unDiffPacketTimeUS > unDiffTickTimeUS)
		{
			Sleep(1);

			unCurrentTickUS = GetTickCount64() * 1000;

			unDiffPacketTimeUS = unCurrentPacketTimeUS - unStartPacketTimeUS;
			unDiffTickTimeUS = unCurrentTickUS - unStartTickUS;
		}

		bResult = m_pcPool->Push(ptPacketItem);
		while (bResult == false)
		{
			SwitchToThread();

			bResult = m_pcPool->Push(ptPacketItem);
		}

		unFileSendCount++;
		unSendBytes += ptPacketItem->tPacketHeader.orig_len;

		m_tSenderInfo.nLoopCount = nLoopCount;
		m_tSenderInfo.unFileSendCount = unFileSendCount;
		m_tSenderInfo.unSendingTime = unDiffPacketTimeUS + unSendingTime;

		if (unDiffSec != (unDiffPacketTimeUS / 500000))
		{
			m_tSenderInfo.unSendBytes = unSendBytes;
			unSendBytes = 0;

			unDiffSec = (unDiffPacketTimeUS / 500000);
			m_tSenderInfo.unBitrateCount = unDiffSec;
		}

		if (ptPacketItem->bFinishPacket)
		{
			m_tSenderInfo.bFinish = true;
		}
	}

	memset(&m_tSenderInfo, 0x00, sizeof(TSenderInfo));

	AxTRACE(TEXT("Sender Thread Stop!\n"));

	return 0;
}
//==============================================================================

bool CDSTPSender::GetSenderInfo(TSenderInfo* ptSenderInfo)
{
	memcpy(ptSenderInfo, &m_tSenderInfo, sizeof(TSenderInfo));

	return true;
}
//==============================================================================
