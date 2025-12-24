// BindingControlUnit.cpp : implementation file
//

#include "stdafx.h"
#include "Timer.h"
#include "BindingControlUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBindingControlUnit dialog


CBindingControlUnit::CBindingControlUnit(CWnd* pParent /*=NULL*/)
	: CDialog(CBindingControlUnit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBindingControlUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBindingControlUnit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBindingControlUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBindingControlUnit, CDialog)
	//{{AFX_MSG_MAP(CBindingControlUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBindingControlUnit message handlers
