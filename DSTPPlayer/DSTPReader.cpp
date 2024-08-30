#include "pch.h"
//==============================================================================

#include "DSTPReader.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

CDSTPReader::CDSTPReader(CDSTPQueue* pcBuffer, CDSTPQueue* pcPool)
{
	m_pcBuffer = pcBuffer;
	m_pcPool = pcPool;

	memset(m_szFilePath, 0, sizeof(m_szFilePath));

	m_nLoopCount = 0;
	m_nDelay = 0;

	m_unFileSize = 0;
}
//==============================================================================

CDSTPReader::~CDSTPReader()
{
	m_pcBuffer = NULL;
	m_pcPool = NULL;
}
//==============================================================================

bool CDSTPReader::SetFilePath(TCHAR* pszFilePath)
{
	_tcscpy_s(m_szFilePath, MAX_PATH, pszFilePath);

	return true;
}
//==============================================================================

bool CDSTPReader::GetFilePath(TCHAR* pszGetFilePath)
{
	_tcscpy_s(pszGetFilePath, MAX_PATH, m_szFilePath);

	return true;
}
//==============================================================================

bool CDSTPReader::SetLoopDelay(int nLoopCount, int nDelay)
{
	m_nLoopCount = nLoopCount;
	m_nDelay = nDelay;

	return true;
}
//==============================================================================

bool CDSTPReader::GetLoopDelay(int* pnLoopCount, int* pnDelay)
{
	*pnLoopCount = m_nLoopCount;
	*pnDelay = m_nDelay;

	return true;
}
//==============================================================================

bool CDSTPReader::SetFileSize(unsigned long long unFileSize)
{
	m_unFileSize = unFileSize;

	return true;
}
//==============================================================================

bool CDSTPReader::GetFileSize(unsigned long long* punFileSize)
{
	*punFileSize = m_unFileSize;

	return true;
}
//==============================================================================

bool CDSTPReader::Start()
{
	StartThread();

	return true;
}
//==============================================================================

bool CDSTPReader::Stop()
{
	StopThread();
	WaitFor();

	return true;
}
//==============================================================================

DWORD CDSTPReader::Execute()
{
	AxTRACE(TEXT("Reader Thread Start!\n"));

	FILE* pFile = NULL;
	errno_t eError;

	pcap_hdr_t tGlobalHeader;

	int nLoopCount = 0;

	bool bFinish = false;

	eError = _tfopen_s(&pFile, m_szFilePath, TEXT("rb"));
	if (eError != 0)
	{
		return -1;
	}

	if (fread(&tGlobalHeader, 1, sizeof(pcap_hdr_t), pFile) != sizeof(pcap_hdr_t))
	{
		fclose(pFile);
		pFile = NULL;

		return -1;
	}

	while (m_bTerminated == false)
	{
		AxTRACE(TEXT("Read Loop : %d Buffer Size : %d, Pool Size : %d\n"), nLoopCount, m_pcBuffer->GetSize(), m_pcPool->GetSize());

		if (nLoopCount == (m_nLoopCount -1))
		{
			bFinish = true;
		}

		ReadPacketFile(pFile, bFinish);

		Sleep(m_nDelay);

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

bool CDSTPReader::ReadPacketFile(FILE* pFile, bool bFinish)
{
	bool bResult;
	bool bFirstPacket = true;
	unsigned int a = 0;
	unsigned int b = 0;
	unsigned long long unTotalSize = 0;

	PTDSTPQueueItem ptPacketItem;

	fseek(pFile, sizeof(pcap_hdr_t), SEEK_SET);

	while (m_bTerminated == false)
	{
		bResult = m_pcPool->Pop(&ptPacketItem);
		a = m_pcPool->GetSize();
		if (bResult == false)
		{
			Sleep(10);

			continue;
		}

		if (fread(&ptPacketItem->tPacketHeader, 1, sizeof(pcaprec_hdr_t), pFile) != sizeof(pcaprec_hdr_t))
		{
			PushPool(ptPacketItem);

			break;
		}

		if (fread(ptPacketItem->zbPacketData, 1, ptPacketItem->tPacketHeader.incl_len, pFile) != ptPacketItem->tPacketHeader.incl_len)
		{
			PushPool(ptPacketItem);

			break;
		}

		ptPacketItem->bFirstPacket = false;
		ptPacketItem->bFinishPacket = false;

		if (bFirstPacket)
		{
			bFirstPacket = false;

			ptPacketItem->bFirstPacket = true;
		}

		unTotalSize += ptPacketItem->tPacketHeader.orig_len;
		if (unTotalSize == unTotalSize && bFinish == true)
		{
			ptPacketItem->bFinishPacket = true;
		}

		bResult = m_pcBuffer->Push(ptPacketItem);
		b = m_pcBuffer->GetSize();
		while (bResult == false)
		{
			SwitchToThread();
			bResult = m_pcBuffer->Push(ptPacketItem);
		}
	}

	return true;
}
//==============================================================================

void CDSTPReader::PushPool(PTDSTPQueueItem ptPacketItem)
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
