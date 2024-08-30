#include "pch.h"
//==============================================================================

#include "ACapManager.h"

#include "ACapQueue.h"
#include "ACapReader.h"
#include "ACapSender.h"

#pragma comment(lib, "IPHLPAPI.lib")
//==============================================================================

#define ACAP_MAX_SIZE			(1000)
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

CACapManager::CACapManager(HWND hOwner)
{
	//멤버 변수 초기화
	m_hOwner = hOwner;

	m_ptQueue = new TACAPQueueItem[ACAP_MAX_SIZE];
	m_pcBuffer = new CACapQueue(ACAP_MAX_SIZE);
	m_pcPool = new CACapQueue(ACAP_MAX_SIZE);

	m_pcReader = new CACapReader(m_pcBuffer, m_pcPool);
	m_pcSender = new CACapSender(m_pcBuffer, m_pcPool);

	memset(&m_tFileInfo, 0x00, sizeof(TFileInfo));
	memset(&m_tSendingInfo, 0x00, sizeof(TSendingInfo));
	memset(m_szErrBuf, 0x00, sizeof(m_szErrBuf));

	m_bPause = false;

	m_nLoops = 0;
	m_nDelay = 0;

	//NicList구하기
	TNICInfo tNICInfo;
	memset(&tNICInfo, 0x00, sizeof(TNICInfo));

	DWORD AdapterSize = 1;

	PIP_ADAPTER_ADDRESSES Adapters = NULL; //376 Bytes
	PIP_ADAPTER_ADDRESSES AdapterItem;
	errno_t eError;

	int nCurrentIndex = 0;

	eError = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, Adapters, &AdapterSize);
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
			sprintf_s(tNICInfo.szName, MAX_PATH, "\\Device\\NPF_%s", AdapterItem->AdapterName);
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

CACapManager::~CACapManager()
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

	std::vector<TNICInfo>().swap(m_vecNICList);
}
//==============================================================================

EACAP_ERROR CACapManager::GetNICCount(int* pnNICCount)
{
	*pnNICCount = static_cast<int>(m_vecNICList.size());

	return eACAP_SUCCESS;
}
//==============================================================================

EACAP_ERROR CACapManager::GetNICInfo(int nIndex, TNICInfo* ptNICInfo)
{
	if(nIndex >= m_vecNICList.size())
	{
		return eACAP_FAIL;
	}

	memcpy(ptNICInfo, &m_vecNICList[nIndex], sizeof(TNICInfo));

	return eACAP_SUCCESS;
}
//==============================================================================

EACAP_ERROR CACapManager::SetNIC(int nSelectedIndex)
{
	pcap_t* pNetDevice = pcap_open_live(m_vecNICList[nSelectedIndex].szName, 65536, 1, 1000, m_szErrBuf);
	if (pNetDevice == nullptr)
	{
		return eACAP_FAIL_NIC_OPEN;
	}

	m_pcSender->SetPcapHandle(pNetDevice);

	return eACAP_SUCCESS;
}
//==============================================================================

EACAP_ERROR CACapManager::AddFile(TCHAR* pszFilePath, TFileInfo* ptFileInfo)
{
	FILE* pFile = NULL;
	errno_t eError;
	size_t sReadSize;

	pcap_file_header tGlobalHeader;
	pcap_pkthdr tPacketHeader;

	unsigned long long unFileSize;

	unsigned long long unFirstPacketTime;
	unsigned long long unLastPacketTime;
	unsigned long long unTransferTime;

	bool bFirstPacket = true;

	int nFileReadPos = 0;
	int nPrevValue = 0;
	int nCheckCount = 0;

	m_unTotalSize = 0;

	unsigned int unTotalPacket;

	eError = _tfopen_s(&pFile, pszFilePath, TEXT("rb"));
	if (eError != 0)
	{
		return eACAP_FAIL_NIC_OPEN;
	}

	//파일의 크기 알아내기
	//파일의 끝에 포인터를 두고 해당 위치값(바이트 단위)를 반환
	//ftell은 2기가 이상부터는 정삭작동 X _ftelli64는 더 큰 용량도 가능
	fseek(pFile, 0, SEEK_END);
	unFileSize = _ftelli64(pFile);

	ptFileInfo->unFileSize = unFileSize;

	//사용했으니 파일을 읽어야해서 파일 포인터를 다시 맨 앞으로 변경
	fseek(pFile, 0, SEEK_SET);

	//Global Header 읽기
	sReadSize = fread(&tGlobalHeader, 1, sizeof(pcap_file_header), pFile); //24 Bytes
	if (sReadSize != sizeof(pcap_file_header))
	{
		fclose(pFile);
		pFile = NULL;

		return eACAP_FAIL_FILE_READ;
	}

	//magic 번호를 확인 (0xa1b2c3d4는 pcap파일 구조)
	if (tGlobalHeader.magic == 0xa1b2c3d4)
	{
		_tcscpy_s(ptFileInfo->szFilePath, pszFilePath);
	}
	else
	{
		return eACAP_FAIL_FILE_MATCH_TYPE;
	}

	//첫번째 패킷 값을 계산하기 위해 먼저 한번 읽기
	sReadSize = fread(&tPacketHeader, 1, sizeof(pcap_pkthdr), pFile); //16 Bytes
	if (sReadSize != sizeof(pcap_pkthdr))
	{
		return eACAP_FAIL_FILE_READ;
	}

	ptFileInfo->nFirstPacketTime = tPacketHeader.ts.tv_sec;

	unFirstPacketTime = (tPacketHeader.ts.tv_sec * 1000000) + tPacketHeader.ts.tv_usec;

	//파일의 포인터를 글로벌 헤더 다음으로 이동
	fseek(pFile, 24, SEEK_SET);

	unTotalPacket = 0;

	//파일 읽기
	while (true)
	{
		sReadSize = fread(&tPacketHeader, 1, sizeof(pcap_pkthdr), pFile); //16 Bytes
		if (sReadSize != sizeof(pcap_pkthdr))
		{
			break;
		}

		unLastPacketTime = (tPacketHeader.ts.tv_sec * 1000000) + tPacketHeader.ts.tv_usec;

		fseek(pFile, tPacketHeader.caplen, SEEK_CUR);

		if (nCheckCount == 10000)
		{
			nCheckCount = 0;

			nFileReadPos = static_cast<int>(_ftelli64(pFile) * 100 / unFileSize);

			if (nPrevValue != nFileReadPos)
			{
				::SendMessage(m_hOwner, WM_ACAP_ADDFILE, nFileReadPos, NULL);
				nPrevValue = nFileReadPos;
			}
		}

		unTotalPacket++;
		nCheckCount++;
		m_unTotalSize += tPacketHeader.len;
	}

	nFileReadPos = static_cast<int>(_ftelli64(pFile) * 100 / unFileSize);

	if (nPrevValue != nFileReadPos)
	{
		::SendMessage(m_hOwner, WM_ACAP_ADDFILE, nFileReadPos, NULL);
	}

	//총 패킷의 개수
	ptFileInfo->unTotalPacket = unTotalPacket;

	//총 전송시간
	unTransferTime = unLastPacketTime - unFirstPacketTime;
	ptFileInfo->unTransferTime = unTransferTime;

	//평균 비트레이트 구하기
	ptFileInfo->unAvgBitrate = m_unTotalSize / (unTransferTime / 1000000);
	
	memcpy(&m_tFileInfo, ptFileInfo, sizeof(TFileInfo));
		
	fclose(pFile);
	pFile = NULL;

	return eACAP_SUCCESS;
}
//==============================================================================

EACAP_ERROR CACapManager::Start(int nLoops, int nDelay)
{
	StartThread();

	m_pcReader->SetFilePath(m_tFileInfo.szFilePath);
	m_pcReader->SetLoopDelayCount(nLoops, nDelay);
	m_pcReader->SetFileSize(m_unTotalSize);

	m_nLoops = nLoops;
	m_nDelay = nDelay;

	m_pcBuffer->Clear();
	m_pcPool->Clear();

	for (unsigned int i = 0; i < ACAP_MAX_SIZE; i++)
	{
		m_pcPool->Push(m_ptQueue + i);
	}

	m_pcSender->Start();
	m_pcReader->Start();

	return eACAP_SUCCESS;
}
//==============================================================================

EACAP_ERROR CACapManager::Pause()
{
	m_pcReader->Pause();

	return eACAP_SUCCESS;
}
//==============================================================================

EACAP_ERROR CACapManager::Continue()
{
	m_pcReader->Continue();

	return eACAP_SUCCESS;
}
//==============================================================================

EACAP_ERROR CACapManager::Stop()
{
	m_pcReader->Stop();
	m_pcSender->Stop();

	StopThread();
	WaitFor();

	return eACAP_SUCCESS;
}
//==============================================================================

EACAP_ERROR CACapManager::GetLoopDelayCount(int* pnLoops, int* pnDelay)
{
	*pnLoops = m_nLoops;
	*pnDelay = m_nDelay;

	return eACAP_SUCCESS;
}
//==============================================================================

DWORD CACapManager::Execute()
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

EACAP_ERROR CACapManager::GetSendingInfo(TSendingInfo* ptSendingInfo)
{
	memcpy(ptSendingInfo, &m_tSendingInfo, sizeof(TSendingInfo));

	return eACAP_SUCCESS;
}
//==============================================================================
