// MyEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyEditCtrl

CMyEditCtrl::CMyEditCtrl()
{
	fReadOnly = TRUE;
}

CMyEditCtrl::~CMyEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CMyEditCtrl)
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEditCtrl message handlers

// OVERRIDES SETFOCUS !!!
void CMyEditCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	if (fReadOnly == FALSE) {
		CEdit::OnSetFocus(pOldWnd);		
	}
}

// To Handle the enter key -
// Must create with style = MULTILINE + WANT RETURN !!!

void CMyEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//int nTextLen;
	//char szBuffer[255];

	if (fReadOnly == FALSE) {

		if (nChar == VK_RETURN) {
			HandleKeyEnter();
		}
		else {					
			CEdit::OnChar(nChar, nRepCnt, nFlags);		
		}
	}
}

void CMyEditCtrl::HandleKeyEnter()
{
	CWnd *pWnd = NULL;
	pWnd = GetOwner();
	if (pWnd != NULL) pWnd->SendMessage(WM_NOTIFYPARENT_KEYENTER,GetDlgCtrlID());
}
/*
BOOL CMyEditCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
*/
