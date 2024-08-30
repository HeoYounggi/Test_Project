
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
//==============================================================================

#include "ACapPlayer.h"

#include "MainFrm.h"

#include "Version.h"
//==============================================================================

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#define malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__ , __LINE__) 
#endif
//==============================================================================

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
//==============================================================================

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}
//==============================================================================

CMainFrame::~CMainFrame()
{
}
//==============================================================================

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	*/

	return 0;
}
//==============================================================================

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	static TCHAR szVersion[MAX_PATH];

	cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~WS_THICKFRAME;
	cs.style &= ~WS_MAXIMIZEBOX;

	_stprintf_s(szVersion, MAX_PATH, TEXT("ATBiS Packet Player - %s"), STRING_VERSION);

	cs.lpszName = szVersion;
	if (cs.hMenu != NULL)
	{
		::DestroyMenu(cs.hMenu);
		cs.hMenu = NULL;
	}
	return TRUE;
}
//==============================================================================

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}
//==============================================================================

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
//==============================================================================
#endif //_DEBUG


// CMainFrame message handlers
void CMainFrame::OnClose()
{
	m_pcView = GetActiveView();

	//m_pcView 초기화 필요
	if (m_pcView == NULL)
	{
		return CFrameWnd::OnClose();
	}

	LRESULT lResult = m_pcView->SendMessage(WM_ACAP_CLOSE);

	if (lResult == false)
	{
		return CFrameWnd::OnClose();
	}
	else
	{
		AfxMessageBox(TEXT("Still sending. Please Complete or Stop the transmission before exiting."));
	}

	return;
}
//==============================================================================