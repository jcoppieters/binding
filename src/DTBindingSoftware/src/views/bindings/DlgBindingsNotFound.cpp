// DlgBindingsNotFound.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingsNotFound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingsNotFound dialog


CDlgBindingsNotFound::CDlgBindingsNotFound(CWnd* pParent /*=NULL*/,CStringArray *pStrings /*= NULL*/)
	: CDialog(CDlgBindingsNotFound::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingsNotFound)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	ASSERT(pStrings);
	pBindingsNotFound = pStrings;
}


void CDlgBindingsNotFound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingsNotFound)
	DDX_Control(pDX, IDC_LIST_BINDINGS, m_ListBindings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingsNotFound, CDialog)
	//{{AFX_MSG_MAP(CDlgBindingsNotFound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingsNotFound message handlers

BOOL CDlgBindingsNotFound::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int nCnt,i;
	CString str;

	m_ListBindings.ResetContent();
	nCnt = pBindingsNotFound->GetSize();
	for(i=0;i<nCnt;i++) {
		str = pBindingsNotFound->GetAt(i);
		m_ListBindings.AddString(str);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
