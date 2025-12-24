// MyComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyComboBox

CMyComboBox::CMyComboBox()
{
}

CMyComboBox::~CMyComboBox()
{
}


BEGIN_MESSAGE_MAP(CMyComboBox, CComboBox)
	//{{AFX_MSG_MAP(CMyComboBox)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyComboBox message handlers

void CMyComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CComboBox::OnSetFocus(pOldWnd);	
}
