
// ACapPlayerView.cpp : implementation of the CACapPlayerView class
//

#include "pch.h"
//==============================================================================

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ACapPlayer.h"
#endif

#include "ACapPlayerDoc.h"
#include "ACapPlayerView.h"
//==============================================================================

#define Timer_ID_SendStatus		1
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

// CACapPlayerView

IMPLEMENT_DYNCREATE(CACapPlayerView, CFormView)

BEGIN_MESSAGE_MAP(CACapPlayerView, CFormView)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_MAIN_PLAY, &CACapPlayerView::OnBnClickedButtonMainPlay)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_PAUSE, &CACapPlayerView::OnBnClickedButtonMainPause)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_STOP, &CACapPlayerView::OnBnClickedButtonMainStop)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_CLOSE, &CACapPlayerView::OnBnClickedButtonMainClose)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_ADDFILE, &CACapPlayerView::OnBnClickedButtonMainAddfile)
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_STATIC_MAIN_PIC, &CACapPlayerView::OnStnClickedStaticMainPic)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_ACAP_ADDFILE, &CACapPlayerView::OnAddFile)
	ON_MESSAGE(WM_ACAP_CLOSE, &CACapPlayerView::OnClose)
	ON_EN_CHANGE(IDC_EDIT_MAIN_LOOP, &CACapPlayerView::OnChangeEditMainLoop)
	ON_EN_CHANGE(IDC_EDIT_MAIN_DELAY, &CACapPlayerView::OnChangeEditMainDelay)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAIN_DELAY, &CACapPlayerView::OnDeltaposSpinMainDelay)
END_MESSAGE_MAP()
//==============================================================================
// CACapPlayerView construction/destruction

CACapPlayerView::CACapPlayerView() noexcept
	: CFormView(IDD_ACAPPLAYER_FORM)
{
	// TODO: add construction code here
	m_pcManager = NULL;

	m_bStart = false;

	m_unTotalPacket = 0;

	m_dbAvgBitrate = 0.0;
	m_dbBitrate = 0.0;
	m_dbCount = 0.0;

	m_unBitrateCount = 0;

	m_strUnit = TEXT("Bitrate(Mbps)");
}
//==============================================================================

CACapPlayerView::~CACapPlayerView()
{

}
//==============================================================================

void CACapPlayerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MAIN_SELECT, m_Select_Groupbox);
	DDX_Control(pDX, IDC_STATIC_MAIN_ADAPTER, m_Adapter_Static);
	DDX_Control(pDX, IDC_COMBO_MAIN_ADAPTER, m_AdapterList_Combo);
	DDX_Control(pDX, IDC_STATIC_MAIN_PACKETFILE, m_PacketFile_Static);
	DDX_Control(pDX, IDC_LIST_MAIN_FILELIST, m_FileList_List);
	DDX_Control(pDX, IDC_BUTTON_MAIN_ADDFILE, m_AddFile_Button);
	DDX_Control(pDX, IDC_STATIC_MAIN_FILEINFO, m_FileInfo_Groupbox);
	DDX_Control(pDX, IDC_STATIC_MAIN_TOTALPACKET, m_TotalPacket_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_OPTIONS, m_Options_Groupbox);
	DDX_Control(pDX, IDC_EDIT_MAIN_LOOP, m_Loop_Edit);
	DDX_Control(pDX, IDC_SPIN_MAIN_LOOP, m_Loop_Spin);
	DDX_Control(pDX, IDC_STATIC_MAIN_LOOPS, m_Loops_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_DELAY, m_Delay_Static);
	DDX_Control(pDX, IDC_EDIT_MAIN_DELAY, m_Delay_Edit);
	DDX_Control(pDX, IDC_SPIN_MAIN_DELAY, m_Delay_Spin);
	DDX_Control(pDX, IDC_STATIC_MAIN_MILLISECONDS, m_Millisecond_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_SENDING, m_Sending_Groupbox);
	DDX_Control(pDX, IDC_STATIC_MAIN_CURRENT, m_Current_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_SENDFILENAME, m_SendFileName_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_SENT, m_PacketSent_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_NOWSENDING, m_Sending_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_SENDINGTIME, m_SendingTime_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_CHANGETIME, m_ChangeTime_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_STATUS, m_Status_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_CHANGESTATUS, m_ChangeStatus_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_PROGRESS, m_Progress_Static);
	DDX_Control(pDX, IDC_PROGRESS_MAIN_PROGRESS, m_Progress_ProgressBar);
	DDX_Control(pDX, IDC_STATIC_MAIN_PIC, m_Picture_Pic);
	DDX_Control(pDX, IDC_BUTTON_MAIN_PLAY, m_Play_Button);
	DDX_Control(pDX, IDC_BUTTON_MAIN_PAUSE, m_Pause_Button);
	DDX_Control(pDX, IDC_BUTTON_MAIN_STOP, m_Stop_Button);
	DDX_Control(pDX, IDC_BUTTON_MAIN_CLOSE, m_Close_Button);
	DDX_Control(pDX, IDC_STATIC_MAIN_SELECTTOTALPACKET, m_SelectTotalPacket_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_ARRIVALTIME, m_ArrivalTime_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_SELECTARRIVALTIME, m_SelectArrivalTime_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_FILESIZE, m_FileSize_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_SELECTFILESIZE, m_SelectFileSize_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_CAPTURETIME, m_CaptureTime_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_SELECTCAPTURETIME, m_SelectCaptureTime_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_BITRATE, m_Bitrate_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_SELECTBITRATE, m_SelectBitrate_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_FILENAME, m_FileName_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_FILEPATH, m_SendFilePath_Static);
	DDX_Control(pDX, IDC_PROGRESS_MAIN_FILEOPEN, m_AddFile_Progressbar);
	DDX_Control(pDX, IDC_STATIC_MAIN_FILEOPEN, m_FileOpen_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_FILEOPEN_STATUS, m_FileOpen_Status_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_CURRENTITER, m_CurrentIter_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_CHANGEITER, m_ChangeIter_Static);
	DDX_Control(pDX, IDC_CUSTOM_MAIN_CHART, m_wndChartControl_Custom);
	DDX_Control(pDX, IDC_STATIC_MAIN_LOOP, m_Loop_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_NOWBITRATE, m_NowBitrate_Static);
	DDX_Control(pDX, IDC_STATIC_MAIN_NOWCHANGEBITRATE, m_NowChangeBitrate_Static);
}
//==============================================================================

BOOL CACapPlayerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}
//==============================================================================

void CACapPlayerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	//3D 효과 제거
	//ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	
	//화면 세팅
	SettingScreen();

	//Font 세팅
	m_EditFont.CreatePointFont(105, TEXT("맑은 고딕"));
	m_BoxFont.CreatePointFont(95, TEXT("굴림체"));

	//IPARAM은 다시 그릴거냐 (redraw 할거냐? 를 묻는것)
	SendMessageToDescendants(WM_SETFONT, (WPARAM)m_EditFont.GetSafeHandle(), FALSE, TRUE, TRUE);
	m_AdapterList_Combo.SetFont(&m_BoxFont);

	//차트 그려주기
	//SubTitle 생성
	CXTPChartTitle* pTitle = m_wndChartControl_Custom.GetContent()->GetTitles()->Add(new CXTPChartTitle());
	//SubTitle
	pTitle->SetText(TEXT("Bitrate"));
	pTitle->SetDocking(xtpChartDockTop);
	pTitle->SetAlignment(xtpChartAlignCenter);

	//Font 설정
	pTitle->SetFont(CXTPChartFont::GetTahoma12());
	pTitle->SetTextColor(CXTPChartColor::Black);

	CreateChart();

	//Tooltip
	m_Tooltip_Ctrl.Create(this);
	m_Tooltip_Ctrl.AddTool(&m_Picture_Pic, TEXT("Click Here To Check the Information about ATBiS Packet Player"));
	m_Tooltip_Ctrl.SetDelayTime(TTDT_AUTOPOP, 20000);

	//스크롤바 제거
	SetScrollSizes(MM_TEXT, CSize(0, 0));

	//범위 지정
	m_Loop_Spin.SetRange(0, 10000);
	m_Loop_Spin.SetPos(0);

	m_Delay_Spin.SetRange(0, 32767);
	m_Delay_Spin.SetPos(500);

	m_AddFile_Progressbar.ShowWindow(FALSE);

	m_AddFile_Progressbar.SetRange(0, 100);
	m_AddFile_Progressbar.SetPos(0);

	m_Progress_ProgressBar.SetRange(0, 100);
	m_Progress_ProgressBar.SetPos(0);

	m_AddFile_Progressbar.SetTheme(xtpControlThemeFlat);
	m_AddFile_Progressbar.SetBarColor(RGB(6, 176, 37));
	m_AddFile_Progressbar.SetBkColor(RGB(230, 230, 230));
	m_AddFile_Progressbar.SetBorderColor(RGB(188, 188, 188));

	m_Progress_ProgressBar.SetTheme(xtpControlThemeFlat);
	m_Progress_ProgressBar.SetBarColor(RGB(6, 176, 37));
	m_Progress_ProgressBar.SetBkColor(RGB(230, 230, 230));
	m_Progress_ProgressBar.SetBorderColor(RGB(188, 188, 188));

	TNICInfo tNICInfo;
	EACAP_ERROR eResult;
	CString strMacAddress;
	int nNICCount;

	m_pcManager = new CACapManager(this->GetSafeHwnd());
	
	eResult = m_pcManager->GetNICCount(&nNICCount);

	if (eResult != eACAP_SUCCESS)
	{
		AfxMessageBox(TEXT("Failed to read NIC. Please try again."));
	}

	for (int i = 0; i < nNICCount; i++)
	{
		eResult = m_pcManager->GetNICInfo(i, &tNICInfo);

		if (eResult == eACAP_SUCCESS)
		{
			strMacAddress.Format(TEXT("[%02X:%02X:%02X:%02X:%02X:%02X] ")
				, tNICInfo.szMACAddress[0], tNICInfo.szMACAddress[1], tNICInfo.szMACAddress[2]
				, tNICInfo.szMACAddress[3], tNICInfo.szMACAddress[4], tNICInfo.szMACAddress[5]);
			m_AdapterList_Combo.AddString(strMacAddress + tNICInfo.szDescription);
		}
		else
		{
			AfxMessageBox(TEXT("Failed to read NIC. Please try again."));
		}
	}

	m_AdapterList_Combo.SetCurSel(0);
}
//==============================================================================

void CACapPlayerView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
	delete m_pcManager;
	m_pcManager = NULL;
}
//==============================================================================
// CACapPlayerView diagnostics

#ifdef _DEBUG
void CACapPlayerView::AssertValid() const
{
	CFormView::AssertValid();
}
//==============================================================================

void CACapPlayerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
//==============================================================================

CACapPlayerDoc* CACapPlayerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CACapPlayerDoc)));
	return (CACapPlayerDoc*)m_pDocument;
}
//==============================================================================
#endif //_DEBUG


// CACapPlayerView message handlers
//==============================================================================

void CACapPlayerView::SettingScreen()
{
	AfxGetMainWnd()->MoveWindow(0, 0, 800, 705);
	AfxGetMainWnd()->CenterWindow();

	//위치 조정
	//Select
	m_Select_Groupbox.MoveWindow(10, 1, 761, 115);
	m_Adapter_Static.MoveWindow(25, 26, 110, 30);
	m_AdapterList_Combo.MoveWindow(145, 25, 520, 30);
	m_PacketFile_Static.MoveWindow(25, 55, 110, 30);
	m_FileList_List.MoveWindow(145, 52, 520, 25);
	m_AddFile_Button.MoveWindow(673, 52, 88, 57);
	m_FileOpen_Static.MoveWindow(25, 84, 110, 30);
	m_AddFile_Progressbar.MoveWindow(145, 83, 520, 25);
	m_FileOpen_Status_Static.MoveWindow(145, 84, 520, 30);

	//FileInfo
	m_FileInfo_Groupbox.MoveWindow(10, 116, 761, 172);
	m_Current_Static.MoveWindow(25, 143, 114, 30);
	m_SendFilePath_Static.MoveWindow(145, 143, 440, 30);
	m_FileName_Static.MoveWindow(25, 171, 114, 30);
	m_SendFileName_Static.MoveWindow(145, 171, 440, 30);
	m_FileSize_Static.MoveWindow(25, 199, 114, 30);
	m_SelectFileSize_Static.MoveWindow(145, 199, 250, 30);
	m_TotalPacket_Static.MoveWindow(427, 199, 130, 30);
	m_SelectTotalPacket_Static.MoveWindow(557, 199, 114, 30);
	m_ArrivalTime_Static.MoveWindow(25, 227, 130, 30);
	m_SelectArrivalTime_Static.MoveWindow(145, 227, 195, 30);
	m_CaptureTime_Static.MoveWindow(427, 227, 122, 30);
	m_SelectCaptureTime_Static.MoveWindow(557, 227, 150, 30);
	m_Bitrate_Static.MoveWindow(25, 255, 114, 30);
	m_SelectBitrate_Static.MoveWindow(145, 255, 190, 30);

	//Options
	m_Options_Groupbox.MoveWindow(10, 288, 761, 67);
	m_Loop_Static.MoveWindow(25, 315, 120, 30);
	m_Loop_Edit.MoveWindow(150, 313, 80, 26);
	m_Loops_Static.MoveWindow(240, 315, 200, 30);
	m_Delay_Static.MoveWindow(427, 315, 150, 30);
	m_Delay_Edit.MoveWindow(537, 313, 80, 26);
	m_Millisecond_Static.MoveWindow(625, 315, 100, 30);

	//Sending
	m_Sending_Groupbox.MoveWindow(10, 355, 761, 260);
	m_PacketSent_Static.MoveWindow(25, 390, 110, 30);
	m_Sending_Static.MoveWindow(145, 390, 170, 30);
	m_wndChartControl_Custom.MoveWindow(412, 367, 358, 246);
	m_CurrentIter_Static.MoveWindow(25, 419, 110, 30);
	m_ChangeIter_Static.MoveWindow(145, 419, 170, 30);
	m_SendingTime_Static.MoveWindow(25, 448, 120, 30);
	m_ChangeTime_Static.MoveWindow(145, 448, 170, 30);
	m_NowBitrate_Static.MoveWindow(25, 477, 170, 30);
	m_NowChangeBitrate_Static.MoveWindow(145, 477, 170, 30);
	m_Status_Static.MoveWindow(25, 506, 110, 30);
	m_ChangeStatus_Static.MoveWindow(145, 506, 190, 30);
	m_Progress_Static.MoveWindow(25, 540, 110, 30);
	m_Progress_ProgressBar.MoveWindow(145, 537, 257, 26);

	//Control Button
	m_Play_Button.MoveWindow(381, 623, 90, 33);
	m_Pause_Button.MoveWindow(481, 623, 90, 33);
	m_Stop_Button.MoveWindow(581, 623, 90, 33);
	m_Close_Button.MoveWindow(681, 623, 90, 33);
	m_Picture_Pic.MoveWindow(10, 621, 108, 38);

	//Buddy 지정
	m_Loop_Spin.SetBuddy(&m_Loop_Edit);
	m_Delay_Spin.SetBuddy(&m_Delay_Edit);

	return;
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainPlay()
{
	// TODO: Add your control notification handler code here
	if (m_FileList_List.GetCount() != 0)
	{
		AfxGetMainWnd()->EnableWindow(FALSE);

		int nSelectedIndex=0;

		m_dbBitrate = 0.0;
		m_dbCount = 0.0;

		m_unBitrateCount = 0;

		//Control Button
		m_Stop_Button.EnableWindow(true);
		//m_Pause_Button.EnableWindow(true);
		m_Play_Button.EnableWindow(false);
		m_Close_Button.EnableWindow(false);

		//Select
		m_Select_Groupbox.EnableWindow(false);
		m_Adapter_Static.EnableWindow(false);
		m_PacketFile_Static.EnableWindow(false);
		m_AddFile_Button.EnableWindow(false);
		m_AdapterList_Combo.EnableWindow(false);
		m_FileList_List.EnableWindow(false);
		m_FileOpen_Static.EnableWindow(false);
		m_FileOpen_Status_Static.EnableWindow(false);

		//Options
		m_Options_Groupbox.EnableWindow(false);
		m_Loop_Static.EnableWindow(false);
		m_Loop_Edit.EnableWindow(false);
		m_Loop_Spin.EnableWindow(false);
		m_Loops_Static.EnableWindow(false);
		m_Delay_Static.EnableWindow(false);
		m_Delay_Spin.EnableWindow(false);
		m_Delay_Edit.EnableWindow(false);
		m_Millisecond_Static.EnableWindow(false);

		m_ChangeStatus_Static.SetWindowText(TEXT("Sending Packet..."));

		m_Progress_ProgressBar.SetBarColor(RGB(6, 176, 37));

		nSelectedIndex = m_AdapterList_Combo.GetCurSel();

		EACAP_ERROR eResult = m_pcManager->SetNIC(nSelectedIndex);
		if (eResult == eACAP_FAIL_NIC_OPEN)
		{
			AfxMessageBox(TEXT("Error open network device"));
			return;
		}

		m_bStart = true;

		CreateChart();

		m_pcManager->Start(m_Loop_Spin.GetPos(), m_Delay_Spin.GetPos());

		SetTimer(Timer_ID_SendStatus, 100, NULL);

		AfxGetMainWnd()->EnableWindow(TRUE);
	}
	else
	{
		AfxMessageBox(TEXT("File not found. Please add the file."), MB_ICONSTOP);
	}
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainPause()
{
	// TODO: Add your control notification handler code here
	CString strPauseButton;

	m_Pause_Button.GetWindowText(strPauseButton);

	if (strPauseButton == TEXT("Pause"))
	{
		m_Pause_Button.SetWindowText(TEXT("Continue"));
		m_ChangeStatus_Static.SetWindowText(TEXT("Pause Playback..."));

		m_pcManager->Pause();
	}
	else
	{
		m_Pause_Button.SetWindowText(TEXT("Pause"));
		m_ChangeStatus_Static.SetWindowText(TEXT("Sending Packet..."));

		m_pcManager->Continue();
	}
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainStop()
{
	// TODO: Add your control notification handler code here
	AfxGetMainWnd()->EnableWindow(FALSE);

	m_Stop_Button.EnableWindow(false);
	m_Pause_Button.EnableWindow(false);
	m_Play_Button.EnableWindow(true);
	m_Close_Button.EnableWindow(true);

	//Select
	m_Select_Groupbox.EnableWindow(true);
	m_Adapter_Static.EnableWindow(true);
	m_PacketFile_Static.EnableWindow(true);
	m_AddFile_Button.EnableWindow(true);
	//m_Clear_Button.EnableWindow(true);
	m_AdapterList_Combo.EnableWindow(true);
	m_FileList_List.EnableWindow(true);
	m_FileOpen_Static.EnableWindow(true);
	m_FileOpen_Status_Static.EnableWindow(true);

	//Options
	m_Options_Groupbox.EnableWindow(true);
	m_Loop_Static.EnableWindow(true);
	m_Loop_Spin.EnableWindow(true);
	m_Loops_Static.EnableWindow(true);
	m_Delay_Static.EnableWindow(true);
	m_Delay_Spin.EnableWindow(true);
	m_Millisecond_Static.EnableWindow(true);

	m_Loop_Edit.EnableWindow(TRUE);
	m_Delay_Edit.EnableWindow(TRUE);

	m_ChangeStatus_Static.SetWindowText(TEXT("Packet File Playback Stopped."));
	m_Progress_ProgressBar.SetBarColor(RGB(128, 128, 128));

	if (m_pSeries)
	{
		CXTPChartAreaSeriesStyle* pStyle = DYNAMIC_DOWNCAST(CXTPChartAreaSeriesStyle, m_pSeries->GetStyle());
		if (pStyle)
		{
			pStyle->SetColor(CXTPChartColor::MakeARGB(255, 128, 128, 128)); // 멈춤 상태 색상 (회색)
		}
	}

	KillTimer(Timer_ID_SendStatus);

	m_bStart = false;

	m_pcManager->Stop();

	AfxGetMainWnd()->EnableWindow(TRUE);
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainClose()
{
	// TODO: Add your control notification handler code here
	m_pcManager->Stop();

	AfxGetMainWnd()->PostMessageW(WM_CLOSE);
}
//==============================================================================
//X버튼 클릭 시 스레드 종료

void CACapPlayerView::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if (nID == SC_CLOSE)
	{
		m_pcManager->Stop();
	}

	CFormView::OnSysCommand(nID, lParam);
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainAddfile()
{
	// TODO: Add your control notification handler code here
	EACAP_ERROR eResult;
	TFileInfo tInfo;
	
	int nIndex;

	CString strFileType = TEXT("Pcap Files(*.pcap)|*.pcap|STL Files(*.stl)|*.stl|All Files(*.*)|*.*|");
	CFileDialog fdDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFileType);

	if (fdDlg.DoModal() != IDOK)
	{
		return;
	}
	
	eResult = m_pcManager->AddFile(const_cast<TCHAR*>(fdDlg.GetPathName().GetString()), &tInfo);

	if (eResult != eACAP_SUCCESS)
	{
		switch (eResult)
		{
		case eACAP_FAIL_FILE_OPEN:
			AfxMessageBox(TEXT("Failed to open the file. Please check the file."));
			return;

		case eACAP_FAIL_FILE_READ:
			AfxMessageBox(TEXT("Failed to read the file. Please try again."));
			return;

		case eACAP_FAIL_FILE_MATCH_TYPE:
			AfxMessageBox(TEXT("This is not a Pcap file. Please provide a Pcap file only."));
			return;
		}
	}

	//FileList에 추가
	if (tInfo.szFilePath != NULL)
	{
		m_FileList_List.ResetContent();
		ASSERT(m_FileList_List.GetCount() == 0);

		nIndex = m_FileList_List.AddString(tInfo.szFilePath);

		m_SendFilePath_Static.SetWindowText(ExtractFilePath(tInfo.szFilePath));
		m_SendFileName_Static.SetWindowText(ExtractFileName(tInfo.szFilePath));

		//파일의 총 패킷 개수 구하기
		m_unTotalPacket = tInfo.unTotalPacket;

		m_SelectTotalPacket_Static.SetWindowText(GetNumberString(m_unTotalPacket, 0));

		//File Size 구하기
		CString strFileInfo;
		int nUnitResult;

		//TB,GB,MB,KB,BYTE로 나눠서 표현
		//만약 자릿수가 3자리 이상(100 이상)이면 소수점 제거
		//자리수가 3자리 이하(99 이하)면 소수점 2자리까지 표현
		if (tInfo.unFileSize >= (1ULL << 40)) //TB
		{
			nUnitResult = static_cast<int>(tInfo.unFileSize >> 40);
			if (nUnitResult >= 100)
			{
				strFileInfo.Format(TEXT("%d TB"), nUnitResult);
			}
			else
			{
				strFileInfo.Format(TEXT("%d.%d TB"), nUnitResult
					, ((tInfo.unFileSize % (1ULL << 40)) * 100) >> 40);
			}
		}
		else if (tInfo.unFileSize >= (1 << 30)) //GB
		{
			nUnitResult = static_cast<int>(tInfo.unFileSize >> 30);
			if (nUnitResult >= 100)
			{
				strFileInfo.Format(TEXT("%d GB"), nUnitResult);
			}
			else
			{
				strFileInfo.Format(TEXT("%d.%d GB"), nUnitResult
					, ((tInfo.unFileSize % (1 << 30)) * 100) >> 30);
			}
		}
		else if (tInfo.unFileSize >= (1 << 20)) //MB
		{
			nUnitResult = static_cast<int>(tInfo.unFileSize >> 20);
			if (nUnitResult >= 100)
			{
				strFileInfo.Format(TEXT("%d MB"), nUnitResult);
			}
			else
			{
				strFileInfo.Format(TEXT("%d.%d MB"), nUnitResult
					, ((tInfo.unFileSize % (1 << 20)) * 100) >> 20);
			}
		}
		else if (tInfo.unFileSize >= (1<<10)) //KB
		{
			nUnitResult = static_cast<int>(tInfo.unFileSize >> 10);
			if (nUnitResult >= 100)
			{
				strFileInfo.Format(TEXT("%d KB"), nUnitResult);
			}
			else
			{
				strFileInfo.Format(TEXT("%d.%d KB"), nUnitResult
					, ((tInfo.unFileSize % (1 << 10)) * 100) >> 10);
			}
		}
		else
		{
			strFileInfo.Format(TEXT("%llu Bytes"), tInfo.unFileSize);
		}

		strFileInfo += (TEXT(" ( ") + GetNumberString64(tInfo.unFileSize, 0) + TEXT(" Bytes )"));
		m_SelectFileSize_Static.SetWindowText(strFileInfo);

		//첫번째 패킷의 시간
		//tm형식으로 변경 (년,월,일 등등)
		struct tm ArrivalTM;
		localtime_s(&ArrivalTM, &tInfo.nFirstPacketTime);

		strFileInfo.Format(TEXT("%d-%02d-%02d  %02d : %02d : %02d")
			, ArrivalTM.tm_year + 1900, ArrivalTM.tm_mon+1 , ArrivalTM.tm_mday
			, ArrivalTM.tm_hour, ArrivalTM.tm_min, ArrivalTM.tm_sec);

		//출력
		m_SelectArrivalTime_Static.SetWindowText(strFileInfo);

		//총 전송시간 구하기
		int nHour;
		int nMin;
		int unSec;

		int unTransSec = static_cast<int>(tInfo.unTransferTime / 1000000);

		if ((tInfo.unTransferTime / 1000000) > 0)
		{
			nHour = unTransSec / 3600;
			unTransSec -= nHour * 3600;
			nMin = unTransSec / 60;
			unTransSec -= nMin * 60;
			unSec = unTransSec;

			strFileInfo.Format(TEXT("%02d : %02d : %02d.%d"), nHour, nMin, unSec, (tInfo.unTransferTime % 1000000) / 1000);
		}
		m_SelectCaptureTime_Static.SetWindowText(strFileInfo);

		m_dbAvgBitrate = (tInfo.unAvgBitrate * 8) / 1000000.0;

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

		m_SelectBitrate_Static.SetWindowText(strFileInfo);
	}
}
//==============================================================================

void CACapPlayerView::OnTimer(UINT_PTR nIDEvent)
{
	CString strSending;
	TSendingInfo tSendingInfo;

	if (m_pcManager->GetSendingInfo(&tSendingInfo) == eACAP_SUCCESS)
	{
		switch (nIDEvent)
		{
		case Timer_ID_SendStatus:
			m_Sending_Static.SetWindowText(GetNumberString(tSendingInfo.unFileSendCount, 0));

			m_ChangeIter_Static.SetWindowText(GetNumberString(tSendingInfo.nLoopCount, 0));

			m_Progress_ProgressBar.SetPos((tSendingInfo.unFileSendCount * 100) / m_unTotalPacket);
			strSending.Format(TEXT("%d%%"), m_Progress_ProgressBar.GetPos());
			m_Progress_ProgressBar.SetWindowText(strSending);

			strSending.Format(TEXT("%llu.%llu Seconds"), tSendingInfo.unSendingTime / 1000000
				, (tSendingInfo.unSendingTime % 1000000) / 100000);
			m_ChangeTime_Static.SetWindowText(strSending);

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

				m_NowChangeBitrate_Static.SetWindowText(strSending);

				m_dbCount += 0.5;

				AddPoint();
			}

			if (tSendingInfo.bFinish)
			{
				OnBnClickedButtonMainStop();
			}

			break;

		default:
			break;
		}
	}
	
	CFormView::OnTimer(nIDEvent);
}
//==============================================================================

afx_msg LRESULT CACapPlayerView::OnAddFile(WPARAM wParam, LPARAM lParam)
{
	CString strProgress;

	//파일 읽어오는동안 다른 일 못하게 전체 UI 비활성화
	AfxGetMainWnd()->EnableWindow(FALSE);

	m_FileOpen_Status_Static.ShowWindow(FALSE);
	m_AddFile_Progressbar.ShowWindow(TRUE);

	m_AddFile_Progressbar.SetPos(static_cast<int>(wParam));
	strProgress.Format(TEXT("%d%%"), m_AddFile_Progressbar.GetPos());
	m_AddFile_Progressbar.SetWindowText(strProgress);

	//컴퓨터 사양 차이로 오래 걸리는 작업 시 응답없음뜨는거 안뜨게 해주는 것
	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	if (wParam == 100)
	{
		AfxGetMainWnd()->EnableWindow(TRUE);

		m_FileOpen_Status_Static.SetWindowText(TEXT("Complete File Open"));

		m_AddFile_Progressbar.ShowWindow(FALSE);
		m_AddFile_Progressbar.SetPos(0);

		m_FileOpen_Status_Static.ShowWindow(TRUE);
	}

	return 0;
}
//==============================================================================

void CACapPlayerView::OnStnClickedStaticMainPic()
{
	// TODO: Add your control notification handler code here
	CAboutDlg dlg;

	dlg.m_strTitle = TEXT("ATBiS Packet Player");

	dlg.DoModal();
}
//==============================================================================

BOOL CACapPlayerView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	m_Tooltip_Ctrl.RelayEvent(pMsg);

	return CFormView::PreTranslateMessage(pMsg);
}
//==============================================================================

void CACapPlayerView::OnChangeEditMainLoop()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int nLoop = GetDlgItemInt(IDC_EDIT_MAIN_LOOP);

	if (nLoop > 10000)
	{
		AfxMessageBox(TEXT("Loop count can't be set to more than 10,000."));
		nLoop = 10000;
		SetDlgItemInt(IDC_EDIT_MAIN_LOOP, nLoop);
	}
	else if (nLoop < 0)
	{
		AfxMessageBox(TEXT("Loop count can't be set to less than 0."));
		nLoop = 0;
		SetDlgItemInt(IDC_EDIT_MAIN_LOOP, nLoop);
	}
}
//==============================================================================

void CACapPlayerView::OnChangeEditMainDelay()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int nDelay = GetDlgItemInt(IDC_EDIT_MAIN_DELAY);

	if (nDelay > 32767)
	{
		AfxMessageBox(TEXT("Delay time can't be set to more than 32,767 milliseconds."));
		nDelay = 32767;
		SetDlgItemInt(IDC_EDIT_MAIN_DELAY, nDelay);
	}
	else if (nDelay < 0)
	{
		AfxMessageBox(TEXT("Delay time can't be set to less than 0 milliseconds."));
		nDelay = 0;
		SetDlgItemInt(IDC_EDIT_MAIN_DELAY, nDelay);
	}
}
//==============================================================================

void CACapPlayerView::OnDeltaposSpinMainDelay(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	pNMUpDown->iDelta *= 100;
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

LRESULT CACapPlayerView::OnClose(WPARAM wParam, LPARAM lParam)
{
	return m_bStart;
}
//==============================================================================

void CACapPlayerView::AddPoint()
{
	CXTPChartContent* pContent = m_wndChartControl_Custom.GetContent();

	CXTPChartSeriesCollection* pCollection = pContent->GetSeries();

	int nCount = 0;
	int nMaxPoint = 120;

	//값 설정
	if (pCollection)
	{
		for (int s = 0; s < pCollection->GetCount(); s++)
		{
			CXTPChartSeries* pSeries = pCollection->GetAt(s);
			if (pSeries)
			{
				if (pSeries->GetPoints()->GetCount() > nMaxPoint)
				{
					pSeries->GetPoints()->RemoveAt(0);
				}
				for (int i = 0; i < pSeries->GetPoints()->GetCount(); i++)
				{
					pSeries->GetPoints()->GetAt(i)->SetArgumentValue(pSeries->GetPoints()->GetAt(i)->GetArgumentValue() + 0.5);
				}

				pSeries->GetPoints()->Add(new CXTPChartSeriesPoint(0, m_dbBitrate));
			}
		}
	}
	/*if (pCollection)
	{
		for (int s = 0; s < pCollection->GetCount(); s++)
		{
			CXTPChartSeries* pSeries = pCollection->GetAt(s);
			if (pSeries)
			{
				if (pSeries->GetPoints()->GetCount() > nMaxPoint)
				{
					pSeries->GetPoints()->RemoveAt(0);

					for (int i = 0; i < pSeries->GetPoints()->GetCount(); i++)
					{
						pSeries->GetPoints()->GetAt(i)->SetArgumentValue(pSeries->GetPoints()->GetAt(i)->GetArgumentValue() - 0.5);
					}

					pSeries->GetPoints()->Add(new CXTPChartSeriesPoint(60, m_dbBitrate));
				}
				else
				{
					pSeries->GetPoints()->Add(new CXTPChartSeriesPoint(m_dbCount, m_dbBitrate));
				}
			}
		}
	}*/
}
//==============================================================================

void CACapPlayerView::SetAxisTitle(CXTPChartDiagram2D* pDiagram, CString strAxisX, CString strAxisY)
{
	if (pDiagram)
	{
		CXTPChartAxis* pAxisX = pDiagram->GetAxisX();
		if (pAxisX)
		{
			CXTPChartAxisTitle* pTitle = pAxisX->GetTitle();
			if (pTitle)
			{
				pTitle->SetText(strAxisX);
				pTitle->SetAlignment(xtpChartAlignCenter);
				pTitle->SetFont(CXTPChartFont::GetTahoma8());
				pTitle->SetTextColor(CXTPChartColor::Black);
				pTitle->SetVisible(TRUE);
			}
		}

		CXTPChartAxis* pAxisY = pDiagram->GetAxisY();
		if (pAxisY)
		{
			CXTPChartAxisTitle* pTitle = pAxisY->GetTitle();
			if (pTitle)
			{
				pTitle->SetText(strAxisY);
				pTitle->SetAlignment(xtpChartAlignCenter);
				pTitle->SetFont(CXTPChartFont::GetTahoma8());
				pTitle->SetTextColor(CXTPChartColor::Black);
				pTitle->SetVisible(TRUE);
			}
		}
	}
}
//==============================================================================

void CACapPlayerView::CreateChart()
{
	CXTPChartContent* pContent = m_wndChartControl_Custom.GetContent();

	//배경 색과 Border 색 변경
	pContent->SetBackgroundColor(CXTPChartColor::MakeARGB(255, 240, 240, 240));
	pContent->GetBorder()->SetColor(CXTPChartColor::MakeARGB(255, 240, 240, 240));

	CXTPChartSeriesCollection* pCollection = pContent->GetSeries();
	pCollection->RemoveAll();

	if (pCollection)
	{
		//Series(선) 추가
		m_pSeries = pCollection->Add(new CXTPChartSeries());
		if (m_pSeries)
		{
			m_pSeries->SetName(TEXT("Series"));

			//Area으로 스타일 설정
			//CXTPChartFastLineSeriesStyle* pStyle = new CXTPChartFastLineSeriesStyle();
			CXTPChartAreaSeriesStyle* pStyle = new CXTPChartAreaSeriesStyle();

			pStyle->SetLabel(FALSE);
			pStyle->GetMarker()->SetVisible(FALSE);
			pStyle->SetColor(CXTPChartColor::MakeARGB(255, 0, 225, 0));

			m_pSeries->SetStyle(pStyle);

			//Antialiasing 사용 (매끈하게 보여주기)
			//pStyle->SetAntialiasing(TRUE);

			// 시리즈의 인수 스케일 유형을 수치형으로 설정
			m_pSeries->SetArgumentScaleType(xtpChartScaleNumerical);
		}
	}

	// Set the X and Y Axis title for the series.
	CXTPChartDiagram2D* pDiagram = DYNAMIC_DOWNCAST(CXTPChartDiagram2D, pCollection->GetAt(0)->GetDiagram());
	ASSERT(pDiagram);

	//X축, Y축 이름 설정
	SetAxisTitle(pDiagram, TEXT("Time (Seconds)"), m_strUnit);

	//Zoom 기능 허용
	pDiagram->SetAllowZoom(FALSE);

	// Y축 최대값을 평균 비트레이트의 60프로로 설정하고 자동 범위 설정을 비활성화
	if (m_dbAvgBitrate == 0)
	{
		pDiagram->GetAxisY()->GetRange()->SetMaxValue(5.1);
	}
	else
	{
		pDiagram->GetAxisY()->GetRange()->SetMaxValue(m_dbAvgBitrate + (m_dbAvgBitrate * 0.6));
	}

	pDiagram->GetAxisY()->GetRange()->SetAutoRange(FALSE);
	// Y축 줌을 허용하지 않도록 설정
	//pDiagram->GetAxisY()->SetAllowZoom(FALSE);

	//Y축의 선 색
	//pDiagram->GetAxisY()->SetColor(CXTPChartColor::MakeARGB(255, 240, 240, 240));

	//Y축 글씨 색 변경
	//pDiagram->GetAxisY()->GetLabel()->SetTextColor(CXTPChartColor::MakeARGB(255, 240, 240, 240));

	// X축 최대값을 60.1로 설정하고 자동 범위 설정을 비활성화
	pDiagram->GetAxisX()->GetRange()->SetMaxValue(60.1);
	pDiagram->GetAxisX()->GetRange()->SetAutoRange(FALSE);

	//x축 좌우 반전
	pDiagram->GetAxisX()->SetReversed(TRUE);


	// X축 줌 제한을 10으로 설정
	//pDiagram->GetAxisX()->GetRange()->SetZoomLimit(10);

	//보조선 (interlaced lines)을 비활성화
	pDiagram->GetAxisX()->SetInterlaced(FALSE);
	pDiagram->GetAxisY()->SetInterlaced(FALSE);

	// 차트 패널의 채우기 스타일을 단색으로 설정
	pDiagram->GetPane()->GetFillStyle()->SetFillMode(xtpChartFillSolid);
	pDiagram->GetPane()->SetBackgroundColor(CXTPChartColor::MakeARGB(255, 255, 255, 255));
	pDiagram->GetPane()->SetBorderColor(CXTPChartColor::MakeARGB(255, 240, 240, 240));
}
//==============================================================================
