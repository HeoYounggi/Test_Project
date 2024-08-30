#ifndef _ACAPMANAGER_H_
#define _ACAPMANAGER_H_
//==============================================================================

#include "../Framework/xThread.h"
#include "../Framework/xUtils.h"
//==============================================================================

#define WM_ACAP_ADDFILE				(WM_USER + 1)
//==============================================================================

typedef struct TNICInfo
{
	char szName[MAX_PATH];
	unsigned char szMACAddress[6];
	TCHAR szDescription[MAX_PATH];
} TNICInfo, * PTNICInfo;
//==============================================================================

typedef struct TFileInfo
{
	TCHAR szFilePath[MAX_PATH];
	unsigned long long unFileSize;
	unsigned int unTotalPacket;
	long long nFirstPacketTime;
	unsigned long long unTransferTime;
	unsigned long long unAvgBitrate;
} TFileInfo, * PTFileInfo;
//==============================================================================

typedef struct TSendingInfo
{
	unsigned int unFileSendCount;
	int nLoopCount;
	unsigned long long unSendingTime;
	unsigned int unSendBytes;
	unsigned long long unBitrateCount;
	bool bFinish;
}TSendingInfo, * PTSendingInfo;
//==============================================================================

typedef enum EACAP_ERROR
{
	//Success or Fail
	eACAP_SUCCESS						= 0x00,
	eACAP_FAIL							= 0x01,

	//File Error
	eACAP_FAIL_FILE_OPEN				= 0x11,
	eACAP_FAIL_FILE_READ				= 0x12,
	eACAP_FAIL_FILE_MATCH_TYPE			= 0x13,

	//NIC Error
	eACAP_FAIL_NIC_FIND					= 0x21,
	eACAP_FAIL_NIC_OPEN					= 0x22,
	eACAP_FAIL_NIC_MEMORY				= 0x23,

	eACAP_FAIL_PROGRAM					= 0xFF,
}EACAP_ERROR, *PEACAP_ERROR;
//==============================================================================

struct TACAPQueueItem;
class CACapQueue;
class CACapReader;
class CACapSender;
//==============================================================================

class CACapManager : public CxThread
{
public:
	CACapManager(HWND hOwner);
	virtual ~CACapManager();

	//반복 횟수 및 루프 간 딜레이 시간 가져오기
	EACAP_ERROR GetLoopDelayCount(int* pnLoops, int* pnDelay);

	//NIC리스트 가져오기 및 선택한 디바이스 정보 가져오기
	EACAP_ERROR GetNICCount(int* pnNICCount);
	EACAP_ERROR GetNICInfo(int nIndex, TNICInfo* ptNICInfo);
	EACAP_ERROR SetNIC(int SelectedIndex);

	EACAP_ERROR Start(int nLoops, int nDelay);
	EACAP_ERROR Pause();
	EACAP_ERROR Continue();
	EACAP_ERROR Stop();

	EACAP_ERROR AddFile(TCHAR* pszFilePath, TFileInfo* ptFileInfo);

	EACAP_ERROR GetSendingInfo(TSendingInfo* ptSendingInfo);

protected:
	HWND m_hOwner;

	TACAPQueueItem* m_ptQueue;

	CACapQueue* m_pcBuffer;
	CACapQueue* m_pcPool;

	CACapReader* m_pcReader;
	CACapSender* m_pcSender;

	std::vector<TNICInfo> m_vecNICList;

	char m_szErrBuf[PCAP_ERRBUF_SIZE];

	bool m_bPause;

	int m_nLoops;
	int m_nDelay;

	unsigned long long m_unTotalSize;

	TFileInfo m_tFileInfo;
	TSendingInfo m_tSendingInfo;

	virtual DWORD Execute();
};
//==============================================================================

#endif // _ACAPMANAGER_H_
