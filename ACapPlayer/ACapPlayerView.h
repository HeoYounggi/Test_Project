// ACapPlayerView.h : interface of the CACapPlayerView class
//

#pragma once
//==============================================================================

class CACapManager;
//==============================================================================

#include "ACapManager.h"

#include "../Framework/xUtils.h"
#include "../Framework/xTrace.h"
//==============================================================================

class CACapPlayerView : public CFormView
{
protected: // create from serialization only
	CACapPlayerView() noexcept;
	DECLARE_DYNCREATE(CACapPlayerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_ACAPPLAYER_FORM };
#endif

// Attributes
public:
	CACapPlayerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	
// Implementation
public:
	virtual ~CACapPlayerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CACapManager* m_pcManager;
	unsigned int m_unTotalPacket;
	bool m_bStart;
	double m_dbAvgBitrate;
	double m_dbBitrate;
	double m_dbCount;
	unsigned long long m_unBitrateCount;
	CString m_strUnit;
	CXTPChartSeries* m_pSeries;
	
private:
	CFont m_EditFont;
	CFont m_BoxFont;
	CToolTipCtrl m_Tooltip_Ctrl;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	void SettingScreen();
	afx_msg void OnBnClickedButtonMainPlay();
	afx_msg void OnBnClickedButtonMainPause();
	afx_msg void OnBnClickedButtonMainStop();
	afx_msg void OnBnClickedButtonMainClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedButtonMainAddfile();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnAddFile(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStnClickedStaticMainPic();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnChangeEditMainLoop();
	afx_msg void OnChangeEditMainDelay();
	afx_msg void OnDeltaposSpinMainDelay(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnClose(WPARAM wParam, LPARAM lParam);
	void AddPoint();
	void SetAxisTitle(CXTPChartDiagram2D* pDiagram, CString strAxisX, CString strAxisY);
	void CreateChart();

	CStatic m_Select_Groupbox;
	CStatic m_Adapter_Static;
	CComboBox m_AdapterList_Combo;
	CStatic m_PacketFile_Static;
	CListBox m_FileList_List;
	CButton m_AddFile_Button;
	CStatic m_FileInfo_Groupbox;
	CStatic m_TotalPacket_Static;
	CStatic m_SelectTotalPacket_Static;
	CStatic m_Options_Groupbox;
	CEdit m_Loop_Edit;
	CSpinButtonCtrl m_Loop_Spin;
	CStatic m_Loops_Static;
	CStatic m_Delay_Static;
	CEdit m_Delay_Edit;
	CSpinButtonCtrl m_Delay_Spin;
	CStatic m_Millisecond_Static;
	CStatic m_Sending_Groupbox;
	CStatic m_Current_Static;
	CStatic m_SendFileName_Static;
	CStatic m_PacketSent_Static;
	CStatic m_Sending_Static;
	CStatic m_SendingTime_Static;
	CStatic m_ChangeTime_Static;
	CStatic m_Status_Static;
	CStatic m_ChangeStatus_Static;
	CStatic m_Progress_Static;
	CStatic m_Picture_Pic;
	CButton m_Play_Button;
	CButton m_Pause_Button;
	CButton m_Stop_Button;
	CButton m_Close_Button;
	CStatic m_ArrivalTime_Static;
	CStatic m_SelectArrivalTime_Static;
	CStatic m_FileSize_Static;
	CStatic m_SelectFileSize_Static;
	CStatic m_CaptureTime_Static;
	CStatic m_SelectCaptureTime_Static;
	CStatic m_Bitrate_Static;
	CStatic m_SelectBitrate_Static;
	CStatic m_FileName_Static;
	CStatic m_SendFilePath_Static;
	CStatic m_FileOpen_Static;
	CStatic m_FileOpen_Status_Static;
	CStatic m_CurrentIter_Static;
	CStatic m_ChangeIter_Static;
	CXTPChartControl m_wndChartControl_Custom;
	CStatic m_Loop_Static;
	CStatic m_NowBitrate_Static;
	CStatic m_NowChangeBitrate_Static;
	CXTPProgressCtrl m_AddFile_Progressbar;
	CXTPProgressCtrl m_Progress_ProgressBar;
};

#ifndef _DEBUG  // debug version in ACapPlayerView.cpp
inline CACapPlayerDoc* CACapPlayerView::GetDocument() const
   { return reinterpret_cast<CACapPlayerDoc*>(m_pDocument); }
#endif

