
// DSTPPlayer.h : main header file for the DSTPPlayer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CDSTPPlayerApp:
// See DSTPPlayer.cpp for the implementation of this class
//

class CDSTPPlayerApp : public CWinApp
{
public:
	CDSTPPlayerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDSTPPlayerApp theApp;
