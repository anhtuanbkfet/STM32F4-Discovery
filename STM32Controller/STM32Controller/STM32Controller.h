
// STM32Controller.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSTM32ControllerApp:
// See STM32Controller.cpp for the implementation of this class
//

class CSTM32ControllerApp : public CWinApp
{
public:
	CSTM32ControllerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSTM32ControllerApp theApp;