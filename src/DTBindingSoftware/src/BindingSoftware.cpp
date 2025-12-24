// BindingSoftware.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MainFrm.h"
#include "BindingSoftwareDoc.h"
#include "AppCommandLineInfo.h"
#include "views/main/BindingSoftwareView.h"
#include "views/intro/Splash.h"
// ------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalHSM;
// ------------------------------------

/////////////////////////////////////////////////////////////////////////////
#define DUMP_MEMORY_LEAK_INFO		0
/////////////////////////////////////////////////////////////////////////////
#if(DUMP_MEMORY_LEAK_INFO == 1)
	#define DUMP_MEMORY_LEAKS( )		_CrtDumpMemoryLeaks()
#else
	#define DUMP_MEMORY_LEAKS( )
#endif 
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerApp

BEGIN_MESSAGE_MAP(CTimerApp, CWinApp)
	//{{AFX_MSG_MAP(CTimerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerApp construction

CTimerApp::CTimerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTimerApp object

CTimerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTimerApp initialization

BOOL CTimerApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.

	// sedert V16.16
	// setlocale(LC_ALL, "en-US");


	AfxInitRichEdit2( );

	//CApplicationGlobalState::Initialise();

	// CG: The following block was added by the Splash Screen component.
\
	{
\
		//CCommandLineInfo cmdInfo;

		CAppCommandLineInfo cmdInfo;
\
		ParseCommandLine(cmdInfo);
\

\
		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
\
	}

	CString szCompanyName;



/*
	{	// BLOCK: doc template registration
		// Register the document template.  Document templates serve
		// as the connection between documents, frame windows and views.
		// Attach this form to another document or frame window by changing
		// the document or frame class in the constructor below.
		CSingleDocTemplate* pNewDocTemplate = new CSingleDocTemplate(
			IDR_UNITSFORMVIEW_TMPL,
			RUNTIME_CLASS(CTimerDoc),		// document class
			RUNTIME_CLASS(CMyFrameWnd),		// frame class
			RUNTIME_CLASS(CUnitsFormView));		// view class
		AddDocTemplate(pNewDocTemplate);
	}
*/
/*

	{	// BLOCK: doc template registration
		// Register the document template.  Document templates serve
		// as the connection between documents, frame windows and views.
		// Attach this form to another document or frame window by changing
		// the document or frame class in the constructor below.
		CSingleDocTemplate* pNewDocTemplate = new CSingleDocTemplate(
			IDR_MYADDFLOWSHEET_TMPL,
			RUNTIME_CLASS(CTimerDoc),		// document class
			RUNTIME_CLASS(CMyFrameWnd),		// frame class
			RUNTIME_CLASS(CMyAddFlowSheet));		// view class
		AddDocTemplate(pNewDocTemplate);
	}
*/
/*
	{	// BLOCK: doc template registration
		// Register the document template.  Document templates serve
		// as the connection between documents, frame windows and views.
		// Attach this form to another document or frame window by changing
		// the document or frame class in the constructor below.
		CSingleDocTemplate* pNewDocTemplate = new CSingleDocTemplate(
			//IDR_NODESFORMVIEW_TMPL,
			IDR_MAINFRAME,
			RUNTIME_CLASS(CTimerDoc),		// document class
			RUNTIME_CLASS(CMainFrame),		// frame class
			RUNTIME_CLASS(CNodesFormView));		// view class
		AddDocTemplate(pNewDocTemplate);
	}

*/
/*
	{	// BLOCK: doc template registration
		// Register the document template.  Document templates serve
		// as the connection between documents, frame windows and views.
		// Attach this form to another document or frame window by changing
		// the document or frame class in the constructor below.
		CSingleDocTemplate* pNewDocTemplate = new CSingleDocTemplate(
			IDR_MYADDFLOWFORM_TMPL,
			RUNTIME_CLASS(CTimerDoc),		// document class
			RUNTIME_CLASS(CMainFrame),		// frame class
			RUNTIME_CLASS(CMyAddflowForm));		// view class
		AddDocTemplate(pNewDocTemplate);
	}
*/
/*
	{	// BLOCK: doc template registration
		// Register the document template.  Document templates serve
		// as the connection between documents, frame windows and views.
		// Attach this form to another document or frame window by changing
		// the document or frame class in the constructor below.
	
		CSingleDocTemplate* pNewDocTemplate = new CSingleDocTemplate(
			//IDR_MYFORMVIEW_TMPL,
			IDR_MAINFRAME,
			RUNTIME_CLASS(CTimerDoc),		// document class
			RUNTIME_CLASS(CMainFrame),		// frame class
			RUNTIME_CLASS(CMyFormView));		// view class
		AddDocTemplate(pNewDocTemplate);
	}
*/


	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	// SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	szCompanyName.LoadString(IDS_REGKEY_COMPANYNAME);
	SetRegistryKey(szCompanyName);

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION == appType)
	{
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME_INSTALLATION,
			RUNTIME_CLASS(CTimerDoc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CTimerView));
		AddDocTemplate(pDocTemplate);
	}
	else if (CAppState::APP_TYPE_ENGINEERING == appType)
	{
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME_ENGINEERING,
			RUNTIME_CLASS(CTimerDoc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CTimerView));
		AddDocTemplate(pDocTemplate);
	}
	else if (CAppState::APP_TYPE_SIMULATION == appType)
	{
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME_SIMULATION,
			RUNTIME_CLASS(CTimerDoc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CTimerView));
		AddDocTemplate(pDocTemplate);	
	}
	else
	{
		ASSERT(0);
		exit(-1);
	}

	OnFileNew();				
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_StaticSwVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_STATIC_SWVERSION, m_StaticSwVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CTimerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CTimerApp message handlers



BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str;
	
#ifdef SKIP
	const BYTE bVersion = (BYTE) ((CURRENT_SW_VERSION & 0xFF00) >> 8);
	const BYTE bRevision = (BYTE) CURRENT_SW_VERSION & 0x00FF;
	str.Format(_T("Software version %02x revision %02x"),bVersion,bRevision);
#endif

	const BYTE bMajor = (CURRENT_SW_VERSION>>8);
	const BYTE bMinor = (CURRENT_SW_VERSION&0xFF);

	str.Format(_T("Software version %02d.%02d"),bMajor,bMinor);
	m_StaticSwVersion.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/*
void CTimerApp::OnFileOpen() 
{
	// TODO: Add your command handler code here
	
}
*/

BOOL CTimerApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

int CTimerApp::ExitInstance() 
{
	//CApplicationGlobalState::Destroy();
		
	//DoCleanup();

	const int retVal = CWinApp::ExitInstance();
	DUMP_MEMORY_LEAKS();
	return retVal;
}
