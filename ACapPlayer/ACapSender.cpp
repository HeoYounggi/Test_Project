#include "pch.h"
//==============================================================================

#include "ACapSender.h"

#include "../Framework/xTrace.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

CACapSender::CACapSender(CACapQueue* pcBuffer, CACapQueue* pcPool)
{
	m_pcBuffer = pcBuffer;
	m_pcPool = pcPool;

	m_pPcapHandle = NULL;

	memset(&m_tSenderInfo, 0x00, sizeof(TSenderInfo));
}
//==============================================================================

CACapSender::~CACapSender()
{
	m_pcBuffer = NULL;
	m_pcPool = NULL;
}
//==============================================================================

bool CACapSender::Start()
{
	StartThread();

	return true;
}
//==============================================================================

bool CACapSender::Stop()
{
	StopThread();
	WaitFor();

	return true;
}
//==============================================================================

void CACapSender::SetPcapHandle(pcap_t* pPcapHandle)
{
	m_pPcapHandle = pPcapHandle;
}
//==============================================================================

DWORD CACapSender::Execute()
{
	AxTRACE(TEXT("Sender Thread Start!\n"));

	bool bResult;

	PTACAPQueueItem ptPacketItem;

	unsigned long long unSendingTime = 0;

	unsigned int unFileSendCount = 0;
	int nLoopCount = 0;
	unsigned long long unDiffSec = 0;

	unsigned int unSendBytes = 0;
	
	unsigned long long unStartPacketTimeUS = 0; //us
	unsigned long long unCurrentPacketTimeUS = 0; //us

	unsigned long long unStartTickUS = GetTickCount64() * 1000; //us
	unsigned long long unCurrentTickUS = 0;

	unsigned long long unDiffPacketTimeUS = 0;
	unsigned long long unDiffTickTimeUS = 0;

	while (m_bTerminated == false)
	{
		//Get Item From Buffer
		bResult = m_pcBuffer->Pop(&ptPacketItem);
		if (bResult == false)
		{
			//AxDEBUG(TEXT("Sender pop Empty!\n"));
			Sleep(10);
			
			continue;
		}

		//Find First Packet
		if (ptPacketItem->bFirstPacket)
		{
			unSendingTime += unDiffPacketTimeUS;

			unStartPacketTimeUS = 0;
			unStartTickUS = GetTickCount64() * 1000;
			
			unFileSendCount = 0;
			nLoopCount++;

			m_tSenderInfo.unSendBytes = 0;
		}

		//Time Reset
		if (unStartPacketTimeUS == 0)
		{
			unStartPacketTimeUS = (ptPacketItem->tPacketHeader.ts.tv_sec * 1000000) + ptPacketItem->tPacketHeader.ts.tv_usec;
		}

		unCurrentPacketTimeUS = (ptPacketItem->tPacketHeader.ts.tv_sec * 1000000) + ptPacketItem->tPacketHeader.ts.tv_usec;

		unCurrentTickUS = GetTickCount64() * 1000;

		unDiffPacketTimeUS = unCurrentPacketTimeUS - unStartPacketTimeUS;
		unDiffTickTimeUS = unCurrentTickUS - unStartTickUS;

		while (unDiffPacketTimeUS > unDiffTickTimeUS)
		{
			Sleep(1);
			unCurrentTickUS = GetTickCount64() * 1000;

			unDiffPacketTimeUS = unCurrentPacketTimeUS - unStartPacketTimeUS;
			unDiffTickTimeUS = unCurrentTickUS - unStartTickUS;
		}

		//Send Packet
		if (pcap_sendpacket(m_pPcapHandle, ptPacketItem->zbPacketData, ptPacketItem->tPacketHeader.len) != 0)
		{
			//AxDEBUG(TEXT("Packet Error!!!!\n"));
		}

		//Push Item to Pool
		bResult = m_pcPool->Push(ptPacketItem);
		while (bResult == false)
		{
			SwitchToThread();
			bResult = m_pcPool->Push(ptPacketItem);
		}

		unFileSendCount++;
		unSendBytes += ptPacketItem->tPacketHeader.len;

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

	pcap_close(m_pPcapHandle);

	memset(&m_tSenderInfo, 0x00, sizeof(TSenderInfo));

	AxTRACE(TEXT("Sender Thread Stop!\n"));

	return 0;
}
//==============================================================================

bool CACapSender::GetSenderInfo(TSenderInfo* ptSenderInfo)
{
	memcpy(ptSenderInfo, &m_tSenderInfo, sizeof(TSenderInfo));

	return true;
}
//==============================================================================
