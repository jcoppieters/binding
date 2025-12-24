// DlgChildAppTaskStatus.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildAppTaskStatus.h"

// -------------------------------
#include "TMGlobals.h"
using namespace TMGlobalHSM;
// -------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChildAppTaskStatus dialog


CDlgChildAppTaskStatus::CDlgChildAppTaskStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChildAppTaskStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChildAppTaskStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgChildAppTaskStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChildAppTaskStatus)
	DDX_Control(pDX, IDC_STATIC_INFO, m_StaticInfo);
	DDX_Control(pDX, IDOK, m_ButtonUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChildAppTaskStatus, CDialog)
	//{{AFX_MSG_MAP(CDlgChildAppTaskStatus)	
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChildAppTaskStatus message handlers

BOOL CDlgChildAppTaskStatus::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{	
	const BOOL fResult = CDialog::Create(IDD, pParentWnd);
	if (fResult == TRUE)
	{
		m_MyWindowRect = rect;	
	}
	return fResult;
}


void CDlgChildAppTaskStatus::ShowWindow(BOOL fShow)
{
	if (fShow == TRUE)
	{
		MoveWindow(m_MyWindowRect);
		CWnd::ShowWindow(TRUE);
		PostMessage(WM_UPDATE_CONTROLS);
	}
	else
	{
		CWnd::ShowWindow(FALSE);
	}
}

void CDlgChildAppTaskStatus::SetProperties(CApplicationDlgAppearance* pAppearance)
{
	ASSERT( pAppearance );
	m_Appearance = *pAppearance;
}

LRESULT CDlgChildAppTaskStatus::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	m_StaticInfo.SetWindowText(m_Appearance.GetMessage());

	if (m_Appearance.GetButtonProperties() == TASK_BUTTON_INVISIBLE)
	{
		m_ButtonUser.ShowWindow(FALSE);
	}
	else
	{	
		if (m_Appearance.GetButtonProperties() == TASK_BUTTON_CANCEL)
			m_ButtonUser.SetWindowText("Cancel");
		else if (m_Appearance.GetButtonProperties() == TASK_BUTTON_DONE)
			m_ButtonUser.SetWindowText("Done");
		if (m_Appearance.GetButtonProperties() == TASK_BUTTON_ERROR)
			m_ButtonUser.SetWindowText("Error");

		m_ButtonUser.ShowWindow(TRUE);
	}

	if (m_Appearance.GetProgressProperties() == TASK_PROGRESSBAR_INVISIBLE)
	{
		m_ProgressCtrl.ShowWindow(FALSE);
	}
	else if (m_Appearance.GetProgressProperties() == TASK_PROGRESSBAR_ERROR)
	{
		m_ProgressCtrl.SetError();
		m_ProgressCtrl.ShowWindow(TRUE);
	}
	else if (m_Appearance.GetProgressProperties() == TASK_PROGRESSBAR_DONE)
	{
		m_ProgressCtrl.SetComplete();
		m_ProgressCtrl.ShowWindow(TRUE);
	}
	else 
	{
		m_ProgressCtrl.ShowWindow(TRUE);
	}

	return TRUE;
}

void CDlgChildAppTaskStatus::OnOK() 
{
	TMGetGlobalHSM()->EvOperationTaskDialogUserButton();
}

BOOL CDlgChildAppTaskStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rcRect(10,55,340,72);

	m_ProgressCtrl.SetRange(0,30);
	m_ProgressCtrl.Create(NULL,NULL,WS_CHILD,rcRect,this,0);
	
	ASSERT(m_ProgressCtrl.GetSafeHwnd() != 0); 

	return TRUE;  
}


void CDlgChildAppTaskStatus::ShowProgressStepUp()
{
	if (m_ProgressCtrl.GetSafeHwnd() != 0)
		m_ProgressCtrl.StepUp();
}
