
// ACapPlayer.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ACapPlayer.h"
#include "MainFrm.h"

#include "ACapPlayerDoc.h"
#include "ACapPlayerView.h"

#include "Version.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

// CACapPlayerApp

BEGIN_MESSAGE_MAP(CACapPlayerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CACapPlayerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CACapPlayerApp construction

CACapPlayerApp::CACapPlayerApp() noexcept
{
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("ACapPlayer.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CACapPlayerApp object

CACapPlayerApp theApp;


// CACapPlayerApp initialization

BOOL CACapPlayerApp::InitInstance()
{
	VERIFY(AfxOleInit());

	AfxEnableControlContainer();

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(TEXT("ATBiS"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CACapPlayerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CACapPlayerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

// CACapPlayerApp message handlers

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ABOUT_VERSION, m_Version_Static);
	DDX_Control(pDX, IDC_STATIC_ABOUT_COPYRIGHT, m_Copyright_Static);
	DDX_Control(pDX, IDC_STATIC_ABOUT_HOMEPAGE, m_Homepage_Static);
	DDX_Control(pDX, IDOK, m_Idok_Button);
	DDX_Control(pDX, IDC_STATIC_ABOUT_ICON, m_Icon_Icon);
	DDX_Control(pDX, IDC_STATIC_ABOUT_LOGO, m_Logo_Pic);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CACapPlayerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAboutDlg dialog used for App About

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_AboutFont.CreatePointFont(105, TEXT("맑은 고딕"));

	m_Version_Static.SetFont(&m_AboutFont);
	m_Copyright_Static.SetFont(&m_AboutFont);
	m_Homepage_Static.SetFont(&m_AboutFont);
	m_Idok_Button.SetFont(&m_AboutFont);

	CString strFormat;

	SetWindowText(m_strTitle);

	strFormat.Format(TEXT("%s, Version %s"), m_strTitle, STRING_VERSION);
	GetDlgItem(IDC_STATIC_ABOUT_VERSION)->SetWindowText(strFormat);

	strFormat.Format(TEXT("Copyright (C) 2024 ATBIS.co.,LTD. All Right Reserved."));
	GetDlgItem(IDC_STATIC_ABOUT_COPYRIGHT)->SetWindowText(strFormat);

	strFormat.Format(TEXT("Homepage : www.atbiss.com"));
	GetDlgItem(IDC_STATIC_ABOUT_HOMEPAGE)->SetWindowText(strFormat);

	//모든 요소 위치 재조정
	MoveWindow(0, 0, 530, 170);
	m_Icon_Icon.MoveWindow(15, 39, 37, 37);
	m_Version_Static.MoveWindow(65, 22, 390, 20);
	m_Copyright_Static.MoveWindow(65, 49, 335, 20);
	m_Homepage_Static.MoveWindow(65,78,300,20);
	m_Idok_Button.MoveWindow(427,92,80,30);
	m_Logo_Pic.MoveWindow(427,10,80,28);

	//About Dialog창 가운데에 생성
	CenterWindow();

	return TRUE;
}

// CACapPlayerApp message handlers



