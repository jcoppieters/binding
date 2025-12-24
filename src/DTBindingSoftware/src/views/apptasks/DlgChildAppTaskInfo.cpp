// DlgChildAppTaskInfo.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildAppTaskInfo.h"

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
// CDlgChildAppTaskInfo dialog


CDlgChildAppTaskInfo::CDlgChildAppTaskInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChildAppTaskInfo::IDD, pParent),
	m_fUserButtonPushed(FALSE)
{
	//{{AFX_DATA_INIT(CDlgChildAppTaskInfo)
	//}}AFX_DATA_INIT
}


void CDlgChildAppTaskInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChildAppTaskInfo)
	DDX_Control(pDX, IDCANCEL, m_BtnCancel);
	DDX_Control(pDX, IDOK, m_BtnOk);
	DDX_Control(pDX, IDC_STATIC_INFO, m_StaticInfo);
	DDX_Control(pDX, IDC_STATIC_ICON, m_StaticIcon);
	DDX_Control(pDX, IDC_STATIC_GROUP, m_StaticGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChildAppTaskInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgChildAppTaskInfo)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_UPDATE_CONTROLS, OnUpdateControls)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChildAppTaskInfo message handlers

void CDlgChildAppTaskInfo::OnOK() 
{
	m_fUserButtonPushed = TRUE;
	TMGetGlobalHSM()->EvOperationMessageBox(EV_OPERATION_USER_YES);

	GetParent()->DestroyWindow();	// ???
}

void CDlgChildAppTaskInfo::OnCancel() 
{
	m_fUserButtonPushed = TRUE;
	TMGetGlobalHSM()->EvOperationMessageBox(EV_OPERATION_USER_NO);	
	
	GetParent()->DestroyWindow();	// ???
}

void CDlgChildAppTaskInfo::OnClose() 
{
	if (m_fUserButtonPushed == FALSE)
	{
		TMGetGlobalHSM()->EvOperationMessageBox(EV_OPERATION_USER_NO);
	}
}

BOOL CDlgChildAppTaskInfo::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	const BOOL fResult = CDialog::Create(IDD, pParentWnd);

	if (fResult == TRUE)
	{
		m_MyWindowRect = rect;	
	}	
	return fResult;
}


void CDlgChildAppTaskInfo::ShowWindow(BOOL fShow)
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

void CDlgChildAppTaskInfo::SetProperties(CApplicationDlgInfoAppearance* pAppearance)
{
	ASSERT( pAppearance );
	m_Appearance = *pAppearance;
}

LRESULT CDlgChildAppTaskInfo::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	switch(m_Appearance.GetMode())
	{
		case ENUM_DIALOG_MODE_RETRY:
		{
			m_StaticIcon.SetIcon(AfxGetApp()->LoadStandardIcon(IDI_QUESTION));

			CString szMessage(m_Appearance.GetMessage());

			if (szMessage.IsEmpty() == TRUE)
			{
				szMessage = "Try again?";				
			}

			m_BtnOk.SetWindowText("Yes");
			m_BtnCancel.SetWindowText("No");
			m_StaticInfo.SetWindowText(szMessage);
								
		} break;
	
		case ENUM_DIALOG_MODE_CONFIRMATION:
		{
			m_StaticIcon.SetIcon(AfxGetApp()->LoadStandardIcon(IDI_QUESTION));

			CString szMessage(m_Appearance.GetMessage());

			if (szMessage.IsEmpty() == TRUE)
			{
				szMessage = "Are you sure?";			
			}

			m_BtnOk.SetWindowText("Yes");
			m_BtnCancel.SetWindowText("No");								
			m_StaticInfo.SetWindowText(szMessage);			

		} break;

		case ENUM_DIALOG_MODE_INFORMATION:
		{
			m_BtnOk.CenterWindow(&m_StaticGroup);
			m_BtnOk.SetWindowText("Ok");
			m_BtnCancel.ShowWindow(FALSE);
			
			m_StaticIcon.SetIcon(AfxGetApp()->LoadStandardIcon(IDI_WARNING));
			
			m_StaticInfo.SetWindowText(m_Appearance.GetMessage());

		} break;
	}
	return TRUE;
}
