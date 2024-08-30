#include "pch.h"
//==============================================================================

#include "ACapReader.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

CACapReader::CACapReader(CACapQueue* pcBuffer, CACapQueue* pcPool)
{
	m_pcBuffer = pcBuffer;
	m_pcPool = pcPool;

	memset(m_szFilePath, 0, sizeof(m_szFilePath));

	m_bPause = false;

	m_nLoopCount = 0;
	m_nDelayTime = 0;

	m_unFileSize = 0;
}
//==============================================================================

CACapReader::~CACapReader()
{
	m_pcBuffer = NULL;
	m_pcPool = NULL;
}
//==============================================================================

bool CACapReader::SetFilePath(TCHAR* pszFilePath)
{
	_tcscpy_s(m_szFilePath, MAX_PATH, pszFilePath);

	return true;
}
//==============================================================================

bool CACapReader::GetFilePath(TCHAR* pszGetFilePath)
{
	_tcscpy_s(pszGetFilePath, MAX_PATH, m_szFilePath);

	return true;
}
//==============================================================================

bool CACapReader::SetLoopDelayCount(int nLoops, int nDelay)
{
	m_nLoopCount = nLoops;
	m_nDelayTime = nDelay;

	return true;
}
//==============================================================================

bool CACapReader::GetLoopDelayCount(int* pnLoops, int* pnDelay)
{
	*pnLoops = m_nLoopCount;
	*pnDelay = m_nDelayTime;

	return true;
}
//==============================================================================

bool CACapReader::Start()
{
	StartThread();

	return true;
}
//==============================================================================

bool CACapReader::Pause()
{
	//::SuspendThread(m_hThread);
	m_bPause = true;

	return true;
}
//==============================================================================

bool CACapReader::Continue()
{
	//::ResumeThread(m_hThread);
	m_bPause = false;

	return true;
}
//==============================================================================

bool CACapReader::Stop()
{
	StopThread();
	WaitFor();

	return true;
}
//==============================================================================

bool CACapReader::SetFileSize(unsigned long long unFileSize)
{
	m_unFileSize = unFileSize;

	return true;
}
//==============================================================================

bool CACapReader::GetFileSize(unsigned long long *punFileSize)
{
	*punFileSize = m_unFileSize;

	return true;
}
//==============================================================================

DWORD CACapReader::Execute()
{
	AxTRACE(TEXT("Reader Thread Start!\n"));

	FILE* pFile = NULL;
	errno_t eError;

	pcap_file_header tGlobalHeader;

	bool bFinish = false;
	m_bPause = false;

	int nLoopCount = 0;

	eError = _tfopen_s(&pFile, m_szFilePath, TEXT("rb"));
	if (eError != 0 || pFile == NULL)
	{
		if (pFile)
		{
			fclose(pFile);
		}

		return -1;
	}

	//Read Global Header
	if (fread(&tGlobalHeader, 1, sizeof(pcap_file_header), pFile) != sizeof(pcap_file_header))
	{
		fclose(pFile);
		pFile = NULL;

		return -1;
	}

	while (m_bTerminated == false)
	{
		AxTRACE(TEXT("Read Loop : %d Buffer Size : %d Pool Size : %d\n"), nLoopCount, m_pcBuffer->GetSize(), m_pcPool->GetSize());


		if (nLoopCount == m_nLoopCount - 1)
		{
			bFinish = true;
		}
		
		ReadPacketFile(pFile, bFinish);

		Sleep(m_nDelayTime);
		
		nLoopCount++;

		if (m_nLoopCount == 0)
		{
			continue;
		}

		if (nLoopCount >= m_nLoopCount)
		{
			break;
		}
	}

	fclose(pFile);
	pFile = NULL;

	AxTRACE(TEXT("Reader Thread Stop!\n"));

	return 0;
}
//==============================================================================

bool CACapReader::ReadPacketFile(FILE* pFile, bool bFinish)
{
	bool bResult;
	bool bFirstPacket;

	PTACAPQueueItem ptPacketItem;

	unsigned long long unTotalSize = 0;

	bFirstPacket = true;

	fseek(pFile, sizeof(pcap_file_header), SEEK_SET);

	while (m_bTerminated == false)
	{
		//Get Item from Pool
		bResult = m_pcPool->Pop(&ptPacketItem);
		if (bResult == false)
		{
			//AxDEBUG(TEXT("Reader pop empty!\n"));
			Sleep(10);

			continue;
		}

		//Read One by One Packet
		//Packet Header
		if (fread(&ptPacketItem->tPacketHeader, 1, sizeof(pcap_pkthdr), pFile) != sizeof(pcap_pkthdr))
		{
			PushPool(ptPacketItem);

			break;
		}

		//Packet Data
		if (fread(ptPacketItem->zbPacketData, 1, ptPacketItem->tPacketHeader.caplen, pFile) != ptPacketItem->tPacketHeader.caplen)
		{
			PushPool(ptPacketItem);

			break;
		}

		ptPacketItem->bFirstPacket = false;
		ptPacketItem->bFinishPacket = false;

		//Find First Packet
		if (bFirstPacket)
		{
			ptPacketItem->bFirstPacket = true;
			bFirstPacket = false;
		}

		//Find Last Packet(Loop Count ³¡³µÀ» ¶§)
		unTotalSize += ptPacketItem->tPacketHeader.len;
		if (unTotalSize == m_unFileSize && bFinish == true)
		{
			ptPacketItem->bFinishPacket = true;
		}

		//Push Item to Buffer
		bResult = m_pcBuffer->Push(ptPacketItem);
		while (bResult == false)
		{
			SwitchToThread();
			bResult = m_pcBuffer->Push(ptPacketItem);
		}
	}

	return true;
}
//==============================================================================

void CACapReader::PushPool(PTACAPQueueItem ptPacketItem)
{
	bool bResult;

	bResult = m_pcPool->Push(ptPacketItem);
	while (bResult == false)
	{
		SwitchToThread();
		bResult = m_pcPool->Push(ptPacketItem);
	}

	return;
}
//==============================================================================
