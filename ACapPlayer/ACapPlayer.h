
// ACapPlayer.h : main header file for the ACapPlayer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CACapPlayerApp:
// See ACapPlayer.cpp for the implementation of this class
//

class CACapPlayerApp : public CWinApp
{
public:
	CACapPlayerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CACapPlayerApp theApp;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	CString m_strTitle;
	CFont m_AboutFont;

	virtual BOOL OnInitDialog();

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	CStatic m_Version_Static;
	CStatic m_Copyright_Static;
	CStatic m_Homepage_Static;
	CButton m_Idok_Button;
	CStatic m_Icon_Icon;
	CStatic m_Logo_Pic;
};