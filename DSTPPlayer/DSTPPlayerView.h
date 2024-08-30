// DSTPPlayerView.h : interface of the CDSTPPlayerView class
//

#pragma once
//==============================================================================

class CDSTPManager;
//==============================================================================

#include "DSTPManager.h"

#include "../Framework/xUtils.h"
#include "../Framework/xTrace.h"
//==============================================================================

class CDSTPPlayerView : public CView
{
protected: // create from serialization only
	CDSTPPlayerView() noexcept;
	DECLARE_DYNCREATE(CDSTPPlayerView)

// Attributes
public:
	CDSTPPlayerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
// Implementation
public:
	virtual ~CDSTPPlayerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CDSTPManager* m_pcManager;

private:
	CFont m_Font;
	CFont m_NICFont;
	CBrush m_WhiteBrush;

protected:
	unsigned int m_unTotalPacket;
	double m_dbAvgBitrate;
	double m_dbBitrate;
	double m_dbCount;
	CString m_strUnit;
	bool m_bStart;
	unsigned long long m_unBitrateCount;

public:
	afx_msg void OnDestroy();
	void CreateControls();
	afx_msg void OnBnClickedButtonMainOpen();
	afx_msg LRESULT OnOpenFile(WPARAM wParam, LPARAM lParam);
	void OnBnClickedButtonPlay();
	void OnBnClickedButtonStop();
	void OnBnClickedButtonClose();
	afx_msg void OnDeltaposSpinMainDelay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CButton m_GroupBox_Select;
	CStatic m_Static_NICList;
	CComboBox m_ComboBox_NIC;
	CStatic m_Static_PacketFile;
	CEdit m_Edit_FileList;
	CButton m_Button_Open;
	CStatic m_Static_OpenStatus;
	CStatic m_Static_Change_OpenStatus;
	CXTPProgressCtrl m_Progress_OpenStatus;

	CButton m_GroupBox_FileInformation;
	CStatic m_Static_FilePath;
	CStatic m_Static_Change_FilePath;
	CStatic m_Static_FileName;
	CStatic m_Static_Change_FileName;
	CStatic m_Static_FileSize;
	CStatic m_Static_Change_FileSize;
	CStatic m_Static_TotalPackets;
	CStatic m_Static_Change_TotalPackets;
	CStatic m_Static_PacketTime;
	CStatic m_Static_Change_PacketTime;
	CStatic m_Static_Duration;
	CStatic m_Static_Change_Duration;
	CStatic m_Static_AvgBitrate;
	CStatic m_Static_Change_AvgBitrate;

	CButton m_GroupBox_Option;
	CStatic m_Static_LoopSending;
	CEdit m_Edit_Loop;
	CSpinButtonCtrl m_Spin_Loop;
	CStatic m_Static_Zero;
	CStatic m_Static_DelayLoops;
	CEdit m_Edit_Delay;
	CSpinButtonCtrl m_Spin_Delay;
	CStatic m_Static_Miliseconds;

	CButton m_GroupBox_SendingInformation;
	CStatic m_Static_PacketsSent;
	CStatic m_Static_Change_PacketsSent;
	CStatic m_Static_LoopCount;
	CStatic m_Static_Change_LoopCount;
	CStatic m_Static_SendingTime;
	CStatic m_Static_Change_SendingTime;
	CStatic m_Static_Bitrate;
	CStatic m_Static_Change_Bitrate;
	CStatic m_Static_Status;
	CStatic m_Static_Change_Status;
	CStatic m_Static_Progress;
	CXTPProgressCtrl m_Progress_Sending;

	CButton m_Button_Play;
	CButton m_Button_Stop;
	CButton m_Button_Close;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
//==============================================================================

#ifndef _DEBUG  // debug version in DSTPPlayerView.cpp
inline CDSTPPlayerDoc* CDSTPPlayerView::GetDocument() const
   { return reinterpret_cast<CDSTPPlayerDoc*>(m_pDocument); }
#endif
//==============================================================================

