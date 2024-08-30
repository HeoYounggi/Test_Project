#include "pch.h"
//==============================================================================

#include "DSTPManager.h"

#include "DSTPQueue.h"
#include "DSTPReader.h"
#include "DSTPSender.h"

#include "DSTPPcap.h"
#include "DSTPLayerHeader.h"

#pragma comment(lib, "IPHLPAPI.lib")
//==============================================================================

#define DSTP_MAX_SIZE			(1000)
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

CDSTPManager::CDSTPManager(HWND hOwner)
{
	m_hOwner = hOwner;

	m_ptQueue = new TDSTPQueueItem[DSTP_MAX_SIZE];
	m_pcBuffer = new CDSTPQueue(DSTP_MAX_SIZE);
	m_pcPool = new CDSTPQueue(DSTP_MAX_SIZE);

	m_pcReader = new CDSTPReader(m_pcBuffer, m_pcPool);
	m_pcSender = new CDSTPSender(m_pcBuffer, m_pcPool);

	TNICInfo tNICInfo;
	memset(&tNICInfo, 0x00, sizeof(TNICInfo));

	DWORD AdapterSize = 1;

	PIP_ADAPTER_ADDRESSES Adapters = NULL; //376 Bytes
	PIP_ADAPTER_ADDRESSES AdapterItem;
	errno_t eError;

	m_nLoop = 0;
	m_nDelay = 0;

	eError = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, Adapters, &AdapterSize);
	//동적 크기 할당
	if (eError == ERROR_BUFFER_OVERFLOW)
	{
		Adapters = (PIP_ADAPTER_ADDRESSES)malloc(AdapterSize);
		if (Adapters == NULL)
		{
			free(Adapters);

			return;
		}
	}

	eError = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, Adapters, &AdapterSize);
	if (eError != ERROR_SUCCESS)
	{
		free(Adapters);

		return;
	}

	AdapterItem = Adapters;
	while (AdapterItem)
	{
		if (AdapterItem->IfType == IF_TYPE_ETHERNET_CSMACD || AdapterItem->IfType & IF_TYPE_IEEE80211)
		{
			//sprintf_s(tNICInfo.szName, MAX_PATH, "\\Device\\NPF_%s", AdapterItem->AdapterName);
			sprintf_s(tNICInfo.szName, MAX_PATH, "%s", AdapterItem->AdapterName);
			_stprintf_s(tNICInfo.szDescription, MAX_PATH, TEXT("%s"), AdapterItem->Description);

			if (AdapterItem->PhysicalAddressLength != 0)
			{
				memcpy(tNICInfo.szMACAddress, AdapterItem->PhysicalAddress, 6);
			}

			m_vecNICList.push_back(tNICInfo);
		}

		AdapterItem = AdapterItem->Next;
	}

	free(Adapters);

	return;
}
//==============================================================================

CDSTPManager::~CDSTPManager()
{
	delete m_pcReader;
	m_pcReader = NULL;

	delete m_pcSender;
	m_pcSender = NULL;

	delete m_pcPool;
	m_pcPool = NULL;

	delete m_pcBuffer;
	m_pcBuffer = NULL;

	delete[] m_ptQueue;
	m_ptQueue = NULL;

	vector<TNICInfo>().swap(m_vecNICList);
}
//==============================================================================

DSTP_ERROR CDSTPManager::GetNICCount(int* pnNICCount)
{
	*pnNICCount = static_cast<int>(m_vecNICList.size());

	return eDSTP_SUCCESS;
}
//==============================================================================

DSTP_ERROR CDSTPManager::GetNICInfo(int nIndex, TNICInfo* ptNICInfo)
{
	if (nIndex >= static_cast<int>(m_vecNICList.size()))
	{
		return eDSTP_FAIL;
	}
	
	memcpy(ptNICInfo, &m_vecNICList[nIndex], sizeof(TNICInfo));

	return eDSTP_SUCCESS;
}
//==============================================================================

DSTP_ERROR CDSTPManager::OpenFile(TCHAR* pszFilePath, TFileInfo* ptFileInfo)
{
	FILE* pFile = NULL;
	errno_t eError;

	pcap_hdr_t tGlobalHeader;
	pcaprec_hdr_t tPacketHeader;

	EthernetHeader tEthernetHeader;
	IPHeader tIPHeader;

	unsigned long long unFileSize;
	unsigned int unTotalPacket;

	unsigned long long unFirstPacketTime;
	unsigned long long unLastPacketTime;
	unsigned long long unTransferTime;

	char szDesIP[16]; // 숫자 3개 * 4 = 12 + 구분자 3 + null값 1 = 16

	int nFileReadPos = 0;
	int nPrevValue = 0;
	int nCheckCount = 0;

	m_unTotalSize = 0;

	eError = _tfopen_s(&pFile, pszFilePath, TEXT("rb"));
	if (eError != 0)
	{
		return eDSTP_FAIL_FILE_OPEN;
	}

	fseek(pFile, 0, SEEK_END);
	unFileSize = _ftelli64(pFile);

	ptFileInfo->unFileSize = unFileSize;

	fseek(pFile, 0, SEEK_SET);

	if (fread(&tGlobalHeader, 1, sizeof(pcap_hdr_t), pFile) != sizeof(pcap_hdr_t))
	{
		fclose(pFile);
		pFile = NULL;

		return eDSTP_FAIL_FILE_READ;
	}

	if (tGlobalHeader.magic_number == 0xa1b2c3d4)
	{
		_tcscpy_s(ptFileInfo->szFilePath, pszFilePath);
	}
	else
	{
		return eDSTP_FAIL_FILE_MATCH_TYPE;
	}

	if (fread(&tPacketHeader, 1, sizeof(pcaprec_hdr_t), pFile) != sizeof(pcaprec_hdr_t))
	{
		fclose(pFile);
		pFile = NULL;

		return eDSTP_FAIL_FILE_READ;
	}

	ptFileInfo->nFirstPacketTime = tPacketHeader.ts_sec;

	unFirstPacketTime = (tPacketHeader.ts_sec * 1000000) + tPacketHeader.ts_usec;

	fseek(pFile, 24, SEEK_SET);

	unTotalPacket = 0;

	while (true)
	{
		if (fread(&tPacketHeader, 1, sizeof(pcaprec_hdr_t), pFile) != sizeof(pcaprec_hdr_t))
		{
			break;
		}

		if (fread(&tEthernetHeader, 1, sizeof(EthernetHeader), pFile) != sizeof(EthernetHeader))
		{
			break;
		}
		
		if (fread(&tIPHeader, 1, sizeof(IPHeader), pFile) != sizeof(IPHeader))
		{
			break;
		}

		unLastPacketTime = (tPacketHeader.ts_sec * 1000000) + tPacketHeader.ts_usec;

		sprintf_s(szDesIP, 16, "%d.%d.%d.%d", tIPHeader.dst_addr[0], tIPHeader.dst_addr[1], tIPHeader.dst_addr[2], tIPHeader.dst_addr[3]);

		fseek(pFile, tPacketHeader.incl_len - sizeof(EthernetHeader) - sizeof(IPHeader), SEEK_CUR);

		if (nCheckCount == 10000)
		{
			nCheckCount = 0;

			nFileReadPos = static_cast<int>(_ftelli64(pFile) * 100 / unFileSize);

			if (nPrevValue != nFileReadPos)
			{
				::SendMessage(m_hOwner, WM_DSTP_OPENFILE, nFileReadPos, NULL);
				nPrevValue = nFileReadPos;
			}
		}

		nCheckCount++;
		unTotalPacket++;
		m_unTotalSize += tPacketHeader.orig_len;
	}
	
	nFileReadPos = static_cast<int>(_ftelli64(pFile) * 100 / unFileSize);

	if (nPrevValue != nFileReadPos)
	{
		::SendMessage(m_hOwner, WM_DSTP_OPENFILE, nFileReadPos, NULL);
	}

	ptFileInfo->unTotalPacket = unTotalPacket;

	unTransferTime = unLastPacketTime - unFirstPacketTime;
	ptFileInfo->unTransferTime = unTransferTime;

	ptFileInfo->unAvgBitrate = m_unTotalSize / (unTransferTime / 1000000);

	memcpy(&m_tFileInfo, ptFileInfo, sizeof(TFileInfo));

	fclose(pFile);
	pFile = NULL;

	return eDSTP_SUCCESS;
}
//==============================================================================

DSTP_ERROR CDSTPManager::Start(int nLoop, int nDelay)
{
	StartThread();

	m_nLoop = nLoop;
	m_nDelay = nDelay;

	m_pcBuffer->Clear();
	m_pcPool->Clear();
	
	for (int i = 0; i < DSTP_MAX_SIZE; i++)
	{
		m_pcPool->Push(m_ptQueue + i);
	}

	m_pcReader->SetFilePath(m_tFileInfo.szFilePath);
	m_pcReader->SetLoopDelay(nLoop, nDelay);
	m_pcReader->SetFileSize(m_unTotalSize);

	m_pcSender->Start();
	m_pcReader->Start();

	return eDSTP_SUCCESS;
}
//==============================================================================

DSTP_ERROR CDSTPManager::Stop()
{
	m_pcReader->Stop();
	m_pcSender->Stop();

	StopThread();
	WaitFor();

	return eDSTP_SUCCESS;
}
//==============================================================================

DSTP_ERROR CDSTPManager::GetLoopDelay(int* pnLoops, int* pnDelay)
{
	*pnLoops = m_nLoop;
	*pnDelay = m_nDelay;

	return eDSTP_SUCCESS;
}
//==============================================================================

DWORD CDSTPManager::Execute()
{
	AxTRACE(TEXT("Manager Thread Start!\n"));

	TSenderInfo tSenderInfo;

	while (m_bTerminated == false)
	{
		if (m_pcSender->GetSenderInfo(&tSenderInfo))
		{
			memcpy(&m_tSendingInfo, &tSenderInfo, sizeof(TSendingInfo));
		}

		Sleep(10);
	}

	AxTRACE(TEXT("Manager Thread Stop!\n"));

	return 0;
}
//==============================================================================

DSTP_ERROR CDSTPManager::GetSendingInfo(TSendingInfo* ptSendingInfo)
{
	memcpy(ptSendingInfo, &m_tSendingInfo, sizeof(TSendingInfo));

	return eDSTP_SUCCESS;
}
//==============================================================================
