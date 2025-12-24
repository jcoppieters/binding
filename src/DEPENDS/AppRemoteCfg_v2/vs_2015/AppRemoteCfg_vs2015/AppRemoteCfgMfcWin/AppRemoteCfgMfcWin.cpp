
// AppRemoteCfgMfcWin.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AppRemoteCfgMfcWin.h"
#include "AppRemoteCfgMfcWinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEBUG_MEMORY_LEAK		0

// CAppRemoteCfgMfcWinApp

BEGIN_MESSAGE_MAP(CAppRemoteCfgMfcWinApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAppRemoteCfgMfcWinApp construction

CAppRemoteCfgMfcWinApp::CAppRemoteCfgMfcWinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAppRemoteCfgMfcWinApp object

CAppRemoteCfgMfcWinApp theApp;


#if (DEBUG_MEMORY_LEAK == 1)
int MyAllocHook(int allocType, void* userData, size_t size, int blockType, long requestNumber,
	const unsigned char* filename, int lineNumber)
{
/*
	if (332 == size) {
		ASSERT(0);
	}
*/
	return 1;
}
#endif 


// CAppRemoteCfgMfcWinApp initialization

BOOL CAppRemoteCfgMfcWinApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

#if(DEBUG_MEMORY_LEAK == 1)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetAllocHook(MyAllocHook);
	// _CrtSetBreakAlloc(4796);
#endif 

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CAppRemoteCfgMfcWinDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
