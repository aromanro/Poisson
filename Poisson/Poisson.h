
// Poisson.h : main header file for the Poisson application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPoissonApp:
// See Poisson.cpp for the implementation of this class
//

class CPoissonApp : public CWinAppEx
{
public:
	CPoissonApp();

// Overrides
	BOOL InitInstance() override;
	void PreLoadState() override;
	void LoadCustomState() override;
	void SaveCustomState() override;

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPoissonApp theApp;
