#ifndef _DSRPPlayer_H_
#define _DSTPPlayer_H_
//==============================================================================

struct TDSTPQueueItem;
class CDSTPQueue;
class CDSTPReader;
class CDSTPSender;
//==============================================================================

#define WM_DSTP_OPENFILE			(WM_USER + 1)
//==============================================================================

#include "../Framework/xThread.h"
//==============================================================================

typedef enum DSTP_ERROR
{
	eDSTP_SUCCESS					= 0X00,
	eDSTP_FAIL						= 0X01,

	eDSTP_FAIL_FILE_OPEN			= 0X11,
	eDSTP_FAIL_FILE_READ			= 0X12,

	eDSTP_FAIL_FILE_MATCH_TYPE		= 0x21,

	eDSTP_FAIL_PROGRAM				= 0XFF,
}DSTP_ERROR, *PDSTP_ERROR;
//==============================================================================

typedef struct TNICInfo
{
	char szName[MAX_PATH];
	unsigned char szMACAddress[6];
	TCHAR szDescription[MAX_PATH];
} TNICInfo, *PTNICInfo;
//==============================================================================

typedef struct TFileInfo
{
	TCHAR szFilePath[MAX_PATH];
	unsigned long long unFileSize;
	unsigned int unTotalPacket;
	long long nFirstPacketTime;
	unsigned long long unTransferTime;
	unsigned long long unAvgBitrate;
}TFileInfo, *PTFileInfo;
//==============================================================================

typedef struct TSendingInfo
{
	unsigned int unFileSendCount;
	int nLoopCount;
	unsigned long long unSendingTime;
	unsigned int unSendBytes;
	unsigned long long unBitrateCount;
	bool bFinish;
}TSendingInfo, *PTSendingInfo;
//==============================================================================

class CDSTPManager : public CxThread
{
public:
	CDSTPManager(HWND hOwner);
	virtual ~CDSTPManager();

	DSTP_ERROR GetNICCount(int* pnNICCount);
	DSTP_ERROR GetNICInfo(int nIndex, TNICInfo* ptNICInfo);

	DSTP_ERROR OpenFile(TCHAR* pszFilePath, TFileInfo* ptFileInfo);

	DSTP_ERROR Start(int nLoop, int nDelay);
	DSTP_ERROR Stop();

	DSTP_ERROR GetLoopDelay(int* pnLoops, int* pnDelay);

	DSTP_ERROR GetSendingInfo(TSendingInfo* ptSendingInfo);

protected:
	HWND m_hOwner;

	TDSTPQueueItem* m_ptQueue;

	CDSTPQueue* m_pcBuffer;
	CDSTPQueue* m_pcPool;

	CDSTPReader* m_pcReader;
	CDSTPSender* m_pcSender;

	vector<TNICInfo> m_vecNICList;

	TFileInfo m_tFileInfo;
	TSendingInfo m_tSendingInfo;

	unsigned long long m_unTotalSize;

	int m_nLoop;
	int m_nDelay;

	virtual DWORD Execute();
};
//==============================================================================
#endif // _DSRPPlayer_H_



