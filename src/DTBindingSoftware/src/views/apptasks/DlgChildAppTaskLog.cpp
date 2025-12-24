// DlgChildAppTaskLog.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildAppTaskLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChildAppTaskLog dialog


CDlgChildAppTaskLog::CDlgChildAppTaskLog(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChildAppTaskLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChildAppTaskLog)
	//}}AFX_DATA_INIT

	m_szErrorLog.Empty();
}


void CDlgChildAppTaskLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChildAppTaskLog)
	DDX_Control(pDX, IDC_EDIT_ERRORLOG, m_EditErrorLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChildAppTaskLog, CDialog)
	//{{AFX_MSG_MAP(CDlgChildAppTaskLog)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChildAppTaskLog message handlers

BOOL CDlgChildAppTaskLog::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	const BOOL fResult = CDialog::Create(IDD, pParentWnd);

	if (fResult == TRUE)
	{
		m_MyWindowRect = rect;	
	}	
	return fResult;
}


void CDlgChildAppTaskLog::ShowWindow(BOOL fShow)
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

BOOL CDlgChildAppTaskLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CDlgChildAppTaskLog::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	m_EditErrorLog.SetWindowText(m_szErrorLog);
	return TRUE;
}

void CDlgChildAppTaskLog::Set(const CString& szErrors)
{
	m_szErrorLog = szErrors;
}

void CDlgChildAppTaskLog::OnButtonOk() 
{
	if (GetParent() != 0)
	{
		GetParent()->DestroyWindow();
	}
}