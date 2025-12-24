// DlgQHSMApplication.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgQHSMApplication.h"

// -------------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMGraphics;
// -------------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CDlgQHSMApplication* CDlgQHSMApplication::m_pThis = 0;
/////////////////////////////////////////////////////////////////////////////

BOOL CDlgQHSMApplication::Show(ENUM_APPLICATION_DIALOG_MODE mode,CString szMessage)
{
	try
	{
		if (m_pThis == 0) 
		{
			m_pThis = new CDlgQHSMApplication(AfxGetMainWnd());
			m_pThis->Create();
		}	
		
		m_pThis->Display(mode,szMessage);			
		m_pThis->ShowWindow(TRUE);
		return TRUE;
		
	}
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgQHSMApplication dialog


CDlgQHSMApplication::CDlgQHSMApplication(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQHSMApplication::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgQHSMApplication)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pParent = pParent;
	m_nID = CDlgQHSMApplication::IDD;
}


void CDlgQHSMApplication::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQHSMApplication)
	DDX_Control(pDX, IDC_STATIC_GROUP, m_StaticGroup);
	DDX_Control(pDX, IDC_STATIC_ICON, m_StaticIcon);
	DDX_Control(pDX, IDOK, m_BtnOk);
	DDX_Control(pDX, IDCANCEL, m_BtnCancel);
	DDX_Control(pDX, IDC_STATIC_INFO, m_StaticInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQHSMApplication, CDialog)
	//{{AFX_MSG_MAP(CDlgQHSMApplication)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQHSMApplication message handlers

void CDlgQHSMApplication::OnCancel() 
{
	m_fUserButtonPushed = TRUE;
	TMGetGlobalHSM()->EvOperationMessageBox(EV_OPERATION_USER_NO);
	DestroyWindow();	
}

void CDlgQHSMApplication::OnOK() 
{
	m_fUserButtonPushed = TRUE;
	TMGetGlobalHSM()->EvOperationMessageBox(EV_OPERATION_USER_YES);	
	DestroyWindow();
}

void CDlgQHSMApplication::PostNcDestroy() 
{
	if (m_pThis != 0)
	{
		delete m_pThis;
		m_pThis = 0;
	}	
}

void CDlgQHSMApplication::OnClose() 
{
	if (m_fUserButtonPushed == FALSE)
	{
		TMGetGlobalHSM()->EvOperationMessageBox(EV_OPERATION_USER_NO);
	}

	DestroyWindow();
}

BOOL CDlgQHSMApplication::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CenterWindow();

	m_fUserButtonPushed = FALSE;

	m_BtnOk.SetWindowText("Yes");
	m_BtnCancel.SetWindowText("No");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgQHSMApplication::Display(ENUM_APPLICATION_DIALOG_MODE mode,CString Message)
{
	m_Mode = mode;
	
	switch(m_Mode)
	{
		case ENUM_MODE_RETRY:
		{
			m_StaticIcon.SetIcon(AfxGetApp()->LoadStandardIcon(IDI_QUESTION));

			if (Message.IsEmpty() == TRUE)
			{
				CString szMessage("Try again?");
				m_StaticInfo.SetWindowText(szMessage);
			}
			else
			{				
				m_StaticInfo.SetWindowText(Message);
			}						
		} break;
	
		case ENUM_MODE_CONFIRMATION:
		{		
			m_StaticIcon.SetIcon(AfxGetApp()->LoadStandardIcon(IDI_QUESTION));

			if (Message.IsEmpty() == TRUE)
			{
				CString szMessage("Are you sure?");
				m_StaticInfo.SetWindowText(szMessage);
			}
			else
			{							
				m_StaticInfo.SetWindowText(Message);
			}	
		} break;

		case ENUM_MODE_INFORMATION:
		{		
				m_BtnOk.CenterWindow(&m_StaticGroup);
				m_BtnOk.SetWindowText("Ok");
				m_BtnCancel.ShowWindow(FALSE);
				m_StaticIcon.SetIcon(AfxGetApp()->LoadStandardIcon(IDI_WARNING));
				m_StaticInfo.SetWindowText(Message);
		} break;

		default:
		{			
		} break;	
	}
}


