
// DSTPPlayerView.cpp : implementation of the CDSTPPlayerView class
//

#include "pch.h"
//==============================================================================

#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DSTPPlayer.h"
#endif

#include "DSTPPlayerDoc.h"
#include "DSTPPlayerView.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

#define IDC_STATIC_MAIN_SELECT					1001
#define IDC_STATIC_MAIN_NICLIST					1002
#define IDC_COMBO_MAIN_NICLIST					1003
#define IDC_STATIC_MAIN_PACKETFILE				1004
#define IDC_EDIT_MAIN_FILELIST					1005
#define IDC_STATIC_MAIN_OPENSTATUS				1006
#define IDC_STATIC_MAIN_CHANGE_OPENSTATUS		1007
#define IDC_PROGRESS_MAIN_OPENSTATUS			1008
#define IDC_BUTTON_MAIN_OPEN					1009
#define IDC_STATIC_MAIN_FILEINFORMATION			1010
#define IDC_STATIC_MAIN_FILEPATH				1011
#define IDC_STATIC_MAIN_CHANGE_FILEPATH			1012
#define IDC_STATIC_MAIN_FILENAME				1013
#define IDC_STATIC_MAIN_CHANGE_FILENAME			1014
#define IDC_STATIC_MAIN_FILESIZE				1015
#define IDC_STATIC_MAIN_CHANGE_FILESIZE			1016
#define IDC_STATIC_MAIN_TOTALPACKETS			1017
#define IDC_BUTTON_MAIN_CHANGE_TOTALPACKETS		1018
#define IDC_BUTTON_MAIN_PACKETTIME				1019
#define IDC_BUTTON_MAIN_CHANGE_PACKETTIME		1020
#define IDC_BUTTON_MAIN_DURATION				1021
#define IDC_BUTTON_MAIN_CHANGE_DURATION			1022
#define IDC_BUTTON_MAIN_AVGBITRATE				1023
#define IDC_BUTTON_MAIN_CHANGE_AVGBITRATE		1024
#define IDC_STATIC_MAIN_OPTION					1025
#define IDC_STATIC_MAIN_LOOPSENDING				1026
#define IDC_EDIT_MAIN_LOOP						1027
#define IDC_SPIN_MAIN_LOOP						1028
#define IDC_STATIC_MAIN_ZERO					1029
#define IDC_STATIC_MAIN_DELAYLOOPS				1030
#define IDC_EDIT_MAIN_DELAY						1031
#define IDC_SPIN_MAIN_DELAY						1032
#define IDC_STATIC_MAIN_MILLISECONDS			1033
#define IDC_STATIC_MAIN_SENDINGINFORMATION		1034
#define IDC_STATIC_MAIN_PacketSent				1035
#define IDC_STATIC_MAIN_Change_PacketSent		1036
#define IDC_STATIC_MAIN_LoopCount				1037
#define IDC_STATIC_MAIN_Change_LoopCount		1038
#define IDC_STATIC_MAIN_SendingTime				1039
#define IDC_STATIC_MAIN_Change_SendingTime		1040
#define IDC_STATIC_MAIN_Bitrate					1041
#define IDC_STATIC_MAIN_Change_Bitrate			1042
#define IDC_STATIC_MAIN_Status					1043
#define IDC_STATIC_MAIN_Change_Status			1044
#define IDC_STATIC_MAIN_Progress				1045
#define IDC_PROGRESS_MAIN_Sending				1046
#define IDC_BUTTON_MAIN_PLAY					1047
#define IDC_BUTTON_MAIN_STOP					1048
#define IDC_BUTTON_MAIN_CLOSE					1049
//==============================================================================

#define Timer_ID_SendStatus						1
//==============================================================================
// CDSTPPlayerView

IMPLEMENT_DYNCREATE(CDSTPPlayerView, CView)

BEGIN_MESSAGE_MAP(CDSTPPlayerView, CView)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_MAIN_OPEN, &CDSTPPlayerView::OnBnClickedButtonMainOpen)
	ON_MESSAGE(WM_DSTP_OPENFILE, &CDSTPPlayerView::OnOpenFile)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_PLAY, &CDSTPPlayerView::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_STOP, &CDSTPPlayerView::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_CLOSE, &CDSTPPlayerView::OnBnClickedButtonClose)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAIN_DELAY, &CDSTPPlayerView::OnDeltaposSpinMainDelay)
	ON_WM_TIMER()
END_MESSAGE_MAP()
//==============================================================================
// CDSTPPlayerView construction/destruction

CDSTPPlayerView::CDSTPPlayerView() noexcept
{
	// TODO: add construction code here
	m_pcManager = NULL;

	m_bStart = false;

	m_unTotalPacket = 0;
	m_unBitrateCount = 0;
	m_dbBitrate = 0.0;
	m_dbCount = 0.0;
}
//==============================================================================

CDSTPPlayerView::~CDSTPPlayerView()
{
}
//==============================================================================

void CDSTPPlayerView::OnDestroy()
{
	CView::OnDestroy();

	delete m_pcManager;
	m_pcManager = NULL;
	// TODO: Add your message handler code here
}
//==============================================================================

BOOL CDSTPPlayerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}
//==============================================================================

void CDSTPPlayerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	DSTP_ERROR eResult;
	int nNICCount;
	TNICInfo tNICInfo;
	CString strMacAddress;

	AfxGetMainWnd()->MoveWindow(0, 0, 800, 705);
	AfxGetMainWnd()->CenterWindow();

	CreateControls();

	m_WhiteBrush.CreateSolidBrush(RGB(255, 255, 255));

	m_Font.CreatePointFont(105, TEXT("맑은 고딕"));
	m_NICFont.CreatePointFont(95, TEXT("굴림체"));

	SendMessageToDescendants(WM_SETFONT, (WPARAM)m_Font.GetSafeHandle(), FALSE, TRUE, TRUE);
	m_ComboBox_NIC.SetFont(&m_NICFont);

	m_Spin_Loop.SetBuddy(&m_Edit_Loop);
	m_Spin_Loop.SetRange(0, 10000);
	m_Spin_Loop.SetPos(0);

	m_Spin_Delay.SetBuddy(&m_Edit_Delay);
	m_Spin_Delay.SetRange(0, 32767);
	m_Spin_Delay.SetPos(500);

	m_Progress_OpenStatus.SetRange(0, 100);
	m_Progress_OpenStatus.SetPos(0);
	m_Progress_OpenStatus.SetTheme(xtpControlThemeFlat);
	m_Progress_OpenStatus.SetBarColor(RGB(6, 176, 37));
	m_Progress_OpenStatus.SetBorderColor(RGB(0, 0, 0));

	m_Progress_Sending.SetRange(0, 100);
	m_Progress_Sending.SetPos(0);
	m_Progress_Sending.SetTheme(xtpControlThemeFlat);
	m_Progress_Sending.SetBarColor(RGB(6, 176, 37));
	m_Progress_Sending.SetBorderColor(RGB(0, 0, 0));

	m_pcManager = new CDSTPManager(this->GetSafeHwnd());

	eResult = m_pcManager->GetNICCount(&nNICCount);
	if (eResult != eDSTP_SUCCESS)
	{
		AfxMessageBox(TEXT("[DSTP Player ERROR]\nFailed to find NIC. Please try again."));
	}

	for (int i = 0; i < nNICCount; i++)
	{
		eResult = m_pcManager->GetNICInfo(i, &tNICInfo);

		if (eResult == eDSTP_SUCCESS)
		{
			strMacAddress.Format(TEXT("[%02X:%02X:%02X:%02X:%02X:%02X] ")
				, tNICInfo.szMACAddress[0], tNICInfo.szMACAddress[1], tNICInfo.szMACAddress[2]
				, tNICInfo.szMACAddress[3], tNICInfo.szMACAddress[4], tNICInfo.szMACAddress[5]);

			m_ComboBox_NIC.AddString(strMacAddress + tNICInfo.szDescription);
		}

		else
		{
			AfxMessageBox(TEXT("[DSTP Player ERROR]\nFailed to find NIC. Please try again."));
		}
	}

	m_ComboBox_NIC.SetCurSel(0);
}
//==============================================================================
// CDSTPPlayerView drawing

void CDSTPPlayerView::OnDraw(CDC* /*pDC*/)
{
	CDSTPPlayerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}
//==============================================================================

HBRUSH CDSTPPlayerView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		hbr = m_WhiteBrush;
	}

	//왜 안되는지 문제점 찾기!
	/*else if (nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		hbr = m_WhiteBrush;
	}*/

	if (pWnd->GetDlgCtrlID() == IDC_EDIT_MAIN_FILELIST)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		hbr = m_WhiteBrush;
	}

	return hbr;
}
//==============================================================================
// CDSTPPlayerView diagnostics

#ifdef _DEBUG
void CDSTPPlayerView::AssertValid() const
{
	CView::AssertValid();
}
//==============================================================================

void CDSTPPlayerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
//==============================================================================

CDSTPPlayerDoc* CDSTPPlayerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDSTPPlayerDoc)));
	return (CDSTPPlayerDoc*)m_pDocument;
}
//==============================================================================
#endif //_DEBUG


// CDSTPPlayerView message handlers
//==============================================================================
void CDSTPPlayerView::CreateControls()
{
	//800 705
	int GroupBoxXFront = 10;
	int GroupBoxXRear = 771;
	int StaticXFront = 25;
	int StaticXRear = 125;
	int ComponentXFront = 135;
	int ComponentXShortRear = 417;
	int ComponentXRear = 761;

	//Setting
	m_GroupBox_Select.Create(TEXT("Select"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX
		, CRect(GroupBoxXFront, 1, GroupBoxXRear, 120), this, IDC_STATIC_MAIN_SELECT);
	m_Static_NICList.Create(TEXT("Adapter :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 24, StaticXRear, 54), this, IDC_STATIC_MAIN_NICLIST);
	m_ComboBox_NIC.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST
		, CRect(ComponentXFront, 24, 663, 54), this, IDC_COMBO_MAIN_NICLIST);
	m_Static_PacketFile.Create(TEXT("Packet File :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 56, StaticXRear, 86), this, IDC_STATIC_MAIN_PACKETFILE);
	m_Edit_FileList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL
		, CRect(ComponentXFront, 55, 663, 80), this, IDC_EDIT_MAIN_FILELIST);
	m_Button_Open.Create(TEXT("Open File"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON
		, CRect(673, 52, ComponentXRear, 112), this, IDC_BUTTON_MAIN_OPEN);
	m_Static_OpenStatus.Create(TEXT("Open Status :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 87, StaticXRear, 117), this, IDC_STATIC_MAIN_OPENSTATUS);
	m_Static_Change_OpenStatus.Create(TEXT("Waiting for file open"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 87, 663, 117), this, IDC_STATIC_MAIN_CHANGE_OPENSTATUS);
	m_Progress_OpenStatus.Create(WS_CHILD | PBS_SMOOTH 
		, CRect(ComponentXFront, 86, 663, 111), this, IDC_PROGRESS_MAIN_OPENSTATUS);

	//FileInformation
	m_GroupBox_FileInformation.Create(TEXT("File Information"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX
		, CRect(GroupBoxXFront, 120, GroupBoxXRear, 300), this, IDC_STATIC_MAIN_FILEINFORMATION);
	m_Static_FilePath.Create(TEXT("File Path :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 143, 125, 173), this, IDC_STATIC_MAIN_FILEPATH);
	m_Static_Change_FilePath.Create(TEXT(""), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 143, ComponentXRear, 173), this, IDC_STATIC_MAIN_CHANGE_FILEPATH);
	m_Static_FileName.Create(TEXT("File Name :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 174, 125, 204), this, IDC_STATIC_MAIN_FILENAME);
	m_Static_Change_FileName.Create(TEXT(""), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 174, ComponentXRear, 204), this, IDC_STATIC_MAIN_CHANGE_FILENAME);
	m_Static_FileSize.Create(TEXT("File Size :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 205, 125, 235), this, IDC_STATIC_MAIN_FILESIZE);
	m_Static_Change_FileSize.Create(TEXT("0 Bytes"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 205, ComponentXShortRear, 235), this, IDC_STATIC_MAIN_CHANGE_FILESIZE);
	m_Static_TotalPackets.Create(TEXT("Total Packets :"), WS_CHILD | WS_VISIBLE
		, CRect(427, 205, 547, 235), this, IDC_STATIC_MAIN_TOTALPACKETS);
	m_Static_Change_TotalPackets.Create(TEXT("0"), WS_CHILD | WS_VISIBLE
		, CRect(558, 205, ComponentXRear, 235), this, IDC_BUTTON_MAIN_CHANGE_TOTALPACKETS);
	m_Static_PacketTime.Create(TEXT("Packet Time :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 236, 125, 266), this, IDC_BUTTON_MAIN_PACKETTIME);
	m_Static_Change_PacketTime.Create(TEXT("0"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 236, ComponentXShortRear, 266), this, IDC_BUTTON_MAIN_CHANGE_PACKETTIME);
	m_Static_Duration.Create(TEXT("Duration :"), WS_CHILD | WS_VISIBLE
		, CRect(427, 236, 547, 266), this, IDC_BUTTON_MAIN_DURATION);
	m_Static_Change_Duration.Create(TEXT("00 : 00 : 00"), WS_CHILD | WS_VISIBLE
		, CRect(558, 236, ComponentXRear, 266), this, IDC_BUTTON_MAIN_CHANGE_DURATION);
	m_Static_AvgBitrate.Create(TEXT("Avg Bitrate :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 267, 125, 297), this, IDC_BUTTON_MAIN_AVGBITRATE);
	m_Static_Change_AvgBitrate.Create(TEXT("0.0 Mbps"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 267, ComponentXRear, 297), this, IDC_BUTTON_MAIN_CHANGE_AVGBITRATE);

	//Option
	m_GroupBox_Option.Create(TEXT("Option"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX
		, CRect(GroupBoxXFront, 300, GroupBoxXRear, 356), this, IDC_STATIC_MAIN_OPTION);
	m_Static_LoopSending.Create(TEXT("Loop Sending :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 323, 125, 353), this, IDC_STATIC_MAIN_LOOPSENDING);
	m_Edit_Loop.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER
		, CRect(ComponentXFront, 322, 215, 347), this, IDC_EDIT_MAIN_FILELIST);
	//UDS_ALIGNRIGHT : Buddy로 연결된 애의 오른쪽에 붙음, UDS_ARROWKEYS : 방향키로 조정 가능
	//UDS_NOTHOUSANDS : 천단위 쉼표 표시 X, UDS_SETBUDDYINT :Buddy된 애한테 자동으로 Spin의 값 입력
	m_Spin_Loop.Create(WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_NOTHOUSANDS | UDS_SETBUDDYINT
		, CRect(220, 322, 230, 347), this, IDC_SPIN_MAIN_LOOP);	
	m_Static_Zero.Create(TEXT("(Zero for infinite)"), WS_CHILD | WS_VISIBLE
		, CRect(230, 323, 370, 353), this, IDC_STATIC_MAIN_ZERO);
	m_Static_DelayLoops.Create(TEXT("Delay Loops :"), WS_CHILD | WS_VISIBLE
		, CRect(427, 323, 547, 353), this, IDC_STATIC_MAIN_DELAYLOOPS);
	m_Edit_Delay.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER
		, CRect(558, 322, 638, 347), this, IDC_EDIT_MAIN_DELAY);
	m_Spin_Delay.Create(WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_NOTHOUSANDS | UDS_SETBUDDYINT
		, CRect(640, 322, 650, 347), this, IDC_SPIN_MAIN_DELAY);
	m_Static_Miliseconds.Create(TEXT("Miliseconds"), WS_CHILD | WS_VISIBLE
		, CRect(651, 323, ComponentXRear, 353), this, IDC_STATIC_MAIN_MILLISECONDS);

	//SendingInformation
	m_GroupBox_SendingInformation.Create(TEXT("Sending Information"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX
		, CRect(GroupBoxXFront, 356, GroupBoxXRear, 567), this, IDC_STATIC_MAIN_SENDINGINFORMATION);
	m_Static_PacketsSent.Create(TEXT("Packet Sent :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 379, 125, 409), this, IDC_STATIC_MAIN_PacketSent);
	m_Static_Change_PacketsSent.Create(TEXT("0"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 379, ComponentXShortRear, 409), this, IDC_STATIC_MAIN_Change_PacketSent);
	m_Static_LoopCount.Create(TEXT("Loop Count :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 410, 125, 440), this, IDC_STATIC_MAIN_LoopCount);
	m_Static_Change_LoopCount.Create(TEXT("0"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 410, ComponentXShortRear, 440), this, IDC_STATIC_MAIN_Change_LoopCount);
	m_Static_SendingTime.Create(TEXT("Sending Time :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 441, 125, 471), this, IDC_STATIC_MAIN_SendingTime);
	m_Static_Change_SendingTime.Create(TEXT("0 Seconds"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 441, ComponentXShortRear, 471), this, IDC_STATIC_MAIN_Change_SendingTime);
	m_Static_Bitrate.Create(TEXT("Bitrate :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 472, 125, 502), this, IDC_STATIC_MAIN_Bitrate);
	m_Static_Change_Bitrate.Create(TEXT("0.0 Mbps"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 472, ComponentXShortRear, 502), this, IDC_STATIC_MAIN_Change_Bitrate);
	m_Static_Status.Create(TEXT("Status :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 503, 125, 533), this, IDC_STATIC_MAIN_Status);
	m_Static_Change_Status.Create(TEXT("Waiting for play"), WS_CHILD | WS_VISIBLE
		, CRect(ComponentXFront, 503, ComponentXShortRear, 533), this, IDC_STATIC_MAIN_Change_Status);
	m_Static_Progress.Create(TEXT("Progress :"), WS_CHILD | WS_VISIBLE
		, CRect(StaticXFront, 534, 125, 564), this, IDC_STATIC_MAIN_Progress);
	m_Progress_Sending.Create(WS_CHILD | PBS_SMOOTH | WS_VISIBLE
		, CRect(ComponentXFront, 534, ComponentXShortRear, 559), this, IDC_PROGRESS_MAIN_Sending);


	//Control Button
	m_Button_Play.Create(TEXT("Play"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON
		, CRect(481, 623, 571, 656), this, IDC_BUTTON_MAIN_PLAY);
	m_Button_Stop.Create(TEXT("Stop"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON
		, CRect(581, 623, 671, 656), this, IDC_BUTTON_MAIN_STOP);
	m_Button_Close.Create(TEXT("Close"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON
		, CRect(681, 623, 771, 656), this, IDC_BUTTON_MAIN_CLOSE);
}
//==============================================================================

void CDSTPPlayerView::OnBnClickedButtonMainOpen()
{
	// TODO: Add your control notification handler code here
	DSTP_ERROR eResult;
	TFileInfo tFileInfo;

	CString strFileType = TEXT("Pcap Files(*.pcap)|*.pcap|STL Files(*.stl)|*.stl|All Files(*.*)|*.*|");
	CFileDialog fdDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFileType);

	if (fdDlg.DoModal() != IDOK)
	{
		return;
	}

	eResult = m_pcManager->OpenFile(const_cast<TCHAR*>(fdDlg.GetPathName().GetString()), &tFileInfo);
	if (eResult != eDSTP_SUCCESS)
	{
		switch (eResult)
		{
		case eDSTP_FAIL_FILE_OPEN:
			AfxMessageBox(TEXT("[DSTP Player ERROR]\nFailed to open the file. Please check the file."));
			break;

		case eDSTP_FAIL_FILE_READ:
			AfxMessageBox(TEXT("[DSTP Player ERROR]\nFailed to read the file. Please try again."));
			break;

		case eDSTP_FAIL_FILE_MATCH_TYPE:
			AfxMessageBox(TEXT("[DSTP Player ERROR]\nThis is not a Pcap file. Please provide a Pcap file only."));
			break;

		default:
			break;
		}
	}
	else
	{
		if (tFileInfo.szFilePath != NULL)
		{
			m_Edit_FileList.SetSel(0, -1, TRUE);
			m_Edit_FileList.Clear();

			//FilePath
			m_Edit_FileList.SetWindowText(tFileInfo.szFilePath);

			m_Static_Change_FilePath.SetWindowText(ExtractFilePath(tFileInfo.szFilePath));
			m_Static_Change_FileName.SetWindowText(ExtractFileName(tFileInfo.szFilePath));

			//FileSize
			CString strFileInfo;
			int nUnitResult;

			if (tFileInfo.unFileSize >= (1ULL << 40)) //TB
			{
				nUnitResult = static_cast<int>(tFileInfo.unFileSize >> 40);
				if (nUnitResult >= 100)
				{
					strFileInfo.Format(TEXT("%d TB"), nUnitResult);
				}
				else
				{
					strFileInfo.Format(TEXT("%d.%d TB"), nUnitResult
						, ((tFileInfo.unFileSize % (1ULL << 40)) * 100) >> 40);
				}
			}
			else if (tFileInfo.unFileSize >= (1 << 30)) //GB
			{
				nUnitResult = static_cast<int>(tFileInfo.unFileSize >> 30);
				if (nUnitResult >= 100)
				{
					strFileInfo.Format(TEXT("%d GB"), nUnitResult);
				}
				else
				{
					strFileInfo.Format(TEXT("%d.%d GB"), nUnitResult
						, ((tFileInfo.unFileSize % (1 << 30)) * 100) >> 30);
				}
			}
			else if (tFileInfo.unFileSize >= (1 << 20)) //MB
			{
				nUnitResult = static_cast<int>(tFileInfo.unFileSize >> 20);
				if (nUnitResult >= 100)
				{
					strFileInfo.Format(TEXT("%d MB"), nUnitResult);
				}
				else
				{
					strFileInfo.Format(TEXT("%d.%d MB"), nUnitResult
						, ((tFileInfo.unFileSize % (1 << 20)) * 100) >> 20);
				}
			}
			else if (tFileInfo.unFileSize >= (1 << 10)) //KB
			{
				nUnitResult = static_cast<int>(tFileInfo.unFileSize >> 10);
				if (nUnitResult >= 100)
				{
					strFileInfo.Format(TEXT("%d KB"), nUnitResult);
				}
				else
				{
					strFileInfo.Format(TEXT("%d.%d KB"), nUnitResult
						, ((tFileInfo.unFileSize % (1 << 10)) * 100) >> 10);
				}
			}
			else
			{
				strFileInfo.Format(TEXT("%llu Bytes"), tFileInfo.unFileSize);
			}

			strFileInfo += (TEXT(" ( ") + GetNumberString64(tFileInfo.unFileSize, 0) + TEXT(" Bytes )"));
			m_Static_Change_FileSize.SetWindowText(strFileInfo);

			//TotalPacket
			m_unTotalPacket = tFileInfo.unTotalPacket;
			m_Static_Change_TotalPackets.SetWindowText(GetNumberString(m_unTotalPacket, 0));

			//PacketTIme
			struct tm ArrivalTM;
			localtime_s(&ArrivalTM, &tFileInfo.nFirstPacketTime);

			strFileInfo.Format(TEXT("%d-%02d-%02d  %02d : %02d : %02d")
				, ArrivalTM.tm_year + 1900, ArrivalTM.tm_mon + 1, ArrivalTM.tm_mday
				, ArrivalTM.tm_hour, ArrivalTM.tm_min, ArrivalTM.tm_sec);

			m_Static_Change_PacketTime.SetWindowText(strFileInfo);

			//Duration
			int nHour;
			int nMin;
			int unSec;

			int unTransSec = static_cast<int>(tFileInfo.unTransferTime / 1000000);

			if ((tFileInfo.unTransferTime / 1000000) > 0)
			{
				nHour = unTransSec / 3600;
				unTransSec -= nHour * 3600;
				nMin = unTransSec / 60;
				unTransSec -= nMin * 60;
				unSec = unTransSec;

				strFileInfo.Format(TEXT("%02d : %02d : %02d.%d"), nHour, nMin, unSec, (tFileInfo.unTransferTime % 1000000) / 1000);
			}
			m_Static_Change_Duration.SetWindowText(strFileInfo);

			//Avg Bitrate
			m_dbAvgBitrate = (tFileInfo.unAvgBitrate * 8) / 1000000.0;

			//평균 비트레이트 구하기
			if (m_dbAvgBitrate < 0.1)
			{
				m_dbAvgBitrate *= 1000;
				strFileInfo.Format(TEXT("%.2f Kbps"), m_dbAvgBitrate);
				m_strUnit = TEXT("Bitrate(Kbps)");
			}
			else
			{
				strFileInfo.Format(TEXT("%.2f Mbps"), m_dbAvgBitrate);
				m_strUnit = TEXT("Bitrate(Mbps)");
			}

			m_Static_Change_AvgBitrate.SetWindowText(strFileInfo);
		}
	}
}
//==============================================================================

LRESULT CDSTPPlayerView::OnOpenFile(WPARAM wParam, LPARAM lParam)
{
	CString strProgress;

	AfxGetMainWnd()->EnableWindow(FALSE);

	m_Static_Change_OpenStatus.ShowWindow(FALSE);
	m_Progress_OpenStatus.ShowWindow(TRUE);

	m_Progress_OpenStatus.SetPos(static_cast<int>(wParam));
	strProgress.Format(TEXT("%d%%"), m_Progress_OpenStatus.GetPos());
	m_Progress_OpenStatus.SetWindowText(strProgress);

	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	if (wParam == 100)
	{
		AfxGetMainWnd()->EnableWindow(TRUE);

		m_Static_Change_OpenStatus.SetWindowText(TEXT("Complete File Open"));
		m_Static_Change_OpenStatus.ShowWindow(TRUE);

		m_Progress_OpenStatus.ShowWindow(FALSE);
		m_Progress_OpenStatus.SetPos(0);
	}

	return 0;
}
//==============================================================================

void CDSTPPlayerView::OnBnClickedButtonPlay()
{
	CString strFileList;
	int nSelectedIndex;

	AfxGetMainWnd()->EnableWindow(FALSE);
	
	m_Edit_FileList.GetWindowText(strFileList);

	if (!strFileList.IsEmpty())
	{
		m_Static_Change_Status.SetWindowText(TEXT("Sending Packet..."));
		nSelectedIndex = m_ComboBox_NIC.GetCurSel();

		m_pcManager->Start(m_Spin_Loop.GetPos(), m_Spin_Delay.GetPos());

		SetTimer(Timer_ID_SendStatus, 100, NULL);
	}
	else
	{
		AfxMessageBox(TEXT("[DSTP Player ERROR]\nFile not found. Please open the file."), MB_ICONSTOP);
	}

	AfxGetMainWnd()->EnableWindow(TRUE);
}
//==============================================================================

void CDSTPPlayerView::OnBnClickedButtonStop()
{
	AfxGetMainWnd()->EnableWindow(FALSE);

	m_Static_Change_Status.SetWindowText(TEXT("Packet file playback stopped."));

	KillTimer(Timer_ID_SendStatus);

	m_pcManager->Stop();

	AfxGetMainWnd()->EnableWindow(TRUE);
}
//==============================================================================

void CDSTPPlayerView::OnBnClickedButtonClose()
{
	m_pcManager->Stop();

	AfxGetMainWnd()->PostMessageW(WM_CLOSE);
}
//==============================================================================

void CDSTPPlayerView::OnDeltaposSpinMainDelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	pNMUpDown->iDelta *= 100;
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
//==============================================================================

void CDSTPPlayerView::OnTimer(UINT_PTR nIDEvent)
{
	TSendingInfo tSendingInfo;
	CString strSending;

	if (m_pcManager->GetSendingInfo(&tSendingInfo) == eDSTP_SUCCESS)
	{
		switch (nIDEvent)
		{
		case Timer_ID_SendStatus:
			m_Static_Change_PacketsSent.SetWindowText(GetNumberString(tSendingInfo.unFileSendCount, 0));
			m_Static_Change_LoopCount.SetWindowText(GetNumberString(tSendingInfo.nLoopCount, 0));

			strSending.Format(TEXT("%llu.%llu Seconds"), tSendingInfo.unSendingTime / 1000000
				, (tSendingInfo.unSendingTime % 1000000) / 100000);
			m_Static_Change_SendingTime.SetWindowText(strSending);

			if (m_unBitrateCount != tSendingInfo.unBitrateCount)
			{
				m_unBitrateCount = tSendingInfo.unBitrateCount;

				m_dbBitrate = tSendingInfo.unSendBytes * 8 / 500000.0;

				if (m_dbBitrate < 0.1)
				{
					m_dbBitrate *= 1000;
					strSending.Format(TEXT("%.2f Kbps"), m_dbBitrate);
				}
				else
				{
					strSending.Format(TEXT("%.2f Mbps"), m_dbBitrate);
				}

				m_Static_Change_Bitrate.SetWindowText(strSending);

				m_dbCount += 0.5;

				//AddPoint();
			}

			m_Progress_Sending.SetPos((tSendingInfo.unFileSendCount * 100) / m_unTotalPacket);
			strSending.Format(TEXT("%d%%"), m_Progress_Sending.GetPos());
			m_Progress_Sending.SetWindowText(strSending);

			if (tSendingInfo.bFinish)
			{
				OnBnClickedButtonStop();
			}

			break;

		default:
			break;
		}
	}

	CView::OnTimer(nIDEvent);
}
//==============================================================================
