// DlgConversionFromNodes.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgConversionFromNodes.h"

#include "QHSMBindingConversionFromNodes.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMGlobalHSM;
// ------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CDlgConversionFromNodes* CDlgConversionFromNodes::m_pDlgThis = NULL;
/////////////////////////////////////////////////////////////////////////////

void CDlgConversionFromNodes::Show(void)
{
	if (m_pDlgThis == NULL) 
	{
		m_pDlgThis = new CDlgConversionFromNodes();
	
		if (m_pDlgThis->Create() == TRUE)
		{
			m_pDlgThis->ShowWindow(TRUE);
		}
	}
	else 
	{
		m_pDlgThis->SetActiveWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConversionFromNodes dialog


CDlgConversionFromNodes::CDlgConversionFromNodes(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConversionFromNodes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConversionFromNodes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nID = CDlgConversionFromNodes::IDD;
}


void CDlgConversionFromNodes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConversionFromNodes)
	DDX_Control(pDX, IDC_STATIC_INFO, m_StaticInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConversionFromNodes, CDialog)
	//{{AFX_MSG_MAP(CDlgConversionFromNodes)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConversionFromNodes message handlers

void CDlgConversionFromNodes::OnButtonCancel() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CQHSMBindingConversionFromNodes::GetInstance()->EvOperationCancel();
	}
	else if (CAppState::APP_TYPE_ENGINEERING==appType)
	{			
		TMGetGlobalHSM()->EvConvertToBindingDatabase(EV_OPERATION_CANCEL);
	}
}

void CDlgConversionFromNodes::OnButtonStart() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CQHSMBindingConversionFromNodes::GetInstance()->EvOperationStart();
	}
	else if (CAppState::APP_TYPE_ENGINEERING==appType)
	{				
		TMGetGlobalHSM()->EvConvertToBindingDatabase(EV_OPERATION_START);
	}
}

void CDlgConversionFromNodes::OnOK() 
{
	DestroyWindow();
}

void CDlgConversionFromNodes::PostNcDestroy() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_ENGINEERING==appType)
	{		
		TMGetGlobalHSM()->EvConvertToBindingDatabase(EV_SIGNAL_DESTROY);
	}

	delete this;
	m_pDlgThis = NULL;
}


void CDlgConversionFromNodes::AddInfo(CString s)
{
	if (GetSafeHwnd() != NULL)
	{
		m_StaticInfo.SetWindowText(s);	
	}
}

BOOL CDlgConversionFromNodes::OnInitDialog() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	CDialog::OnInitDialog();
	
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CString szMessage("Installation Mode : Debug\n"
						  "Press Start");

		m_StaticInfo.SetWindowText("Press Start...");
	}
	else if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		if (TMIsConnected() == TRUE)
		{
			CString szMessage("Tool: Conversion from binding files to binding database\n"
							  "Steps to perform:\n"
							  "1) Physical browse for nodes. (Done)\n"
							  "2) Read all bindings. (Done?)\n"
							  "3) Save project file. (Done?)\n"
							  "4) 'Start' Conversion to binding database.\n"
							  "5) Save New project file.\n\n"
							  "Information about the conversion can be found in "
							  "the logfiles saved at (3).\n"
							  "The files saved at (5) can be used by the Duotecno "
							  "installation software...");

			m_StaticInfo.SetWindowText(szMessage);
		}
		else
		{
			CString szMessage("Tool: Conversion from binding files to binding database\n"
							  "Steps to perform:\n"
							  "1) Import *.duo file + *.txt bindingfiles. (Done)\n"							 
							  "2) Save project file under new directory. (Done?) \n"
							  "3) 'Start' Conversion to binding database.\n"
							  "4) Save New project file.\n\n"
							  "Information about the conversion can be found in "
							  "the logfiles saved at (2).\n"
							  "The files saved at (4) can be used by the Duotecno "
							  "installation software...");

			m_StaticInfo.SetWindowText(szMessage);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConversionFromNodes::OnButtonClear() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CQHSMBindingConversionFromNodes::GetInstance()->EvOperationClear();	
	}
	else if (CAppState::APP_TYPE_ENGINEERING==appType)
	{			
		TMGetGlobalHSM()->EvConvertToBindingDatabase(EV_OPERATION_CLEAR);
	}
}

void CDlgConversionFromNodes::OnClose() 
{
	DestroyWindow();
}
