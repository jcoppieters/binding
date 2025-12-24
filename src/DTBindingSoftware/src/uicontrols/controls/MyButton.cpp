// MyButton.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyButton.h"

#include "MyWindowMessages.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyButton

CMyButton::CMyButton() :
fButtonState(FALSE)
{
	ASSERT(fButtonState == FALSE);
}

CMyButton::~CMyButton()
{

}


BEGIN_MESSAGE_MAP(CMyButton, CButton)
	//{{AFX_MSG_MAP(CMyButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyButton message handlers

CBrush CMyButton::m_brush(RGB(198,198,198));
CBrush CMyButton::m_brush2(RGB(0,255,0));


void CMyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* const pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	UINT uStyle = DFCS_BUTTONPUSH;

	 // This code only works with buttons.
   ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

   // If drawing selected, add the pushed style to DrawFrameControl.
   if (lpDrawItemStruct->itemState & ODS_SELECTED)
      uStyle |= DFCS_PUSHED;
  

   // Draw the button frame.
   ::DrawFrameControl(lpDrawItemStruct->hDC, 
					  &lpDrawItemStruct->rcItem, 
					  DFC_BUTTON, 
					  uStyle);

   // Fill Color to show the state ...
    //rcItem = lpDrawItemStruct->rcItem;

	CRect rcItem(lpDrawItemStruct->rcItem);
	rcItem.DeflateRect(CSize(2,2));	
	if (fButtonState == FALSE) 
	{
		pDC->FillRect(rcItem,&m_brush);
	}
	else 
	{
		pDC->FillRect(rcItem,&m_brush2);
	}


   // Get the button's text.
   CString strText;
   GetWindowText(strText);

   // Draw the button text using the text color red.
   const COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(0,0,0));

    ::SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);
   
	CFont* const pOldFont = pDC->SelectObject(TMGetGraphics()->GetFont(FONT_TIMES_NEW_ROMAN_12));

	::DrawText(lpDrawItemStruct->hDC, 
			   strText, 
			   strText.GetLength(), 
			   &lpDrawItemStruct->rcItem, 
			   DT_SINGLELINE | DT_VCENTER| DT_CENTER);

    ::SetTextColor(lpDrawItemStruct->hDC, crOldColor);	

   	if (pOldFont != NULL) 
	{
		pDC->SelectObject(pOldFont);	
	}
}


void CMyButton::OnClicked() 
{
	CWnd* const pWnd = GetOwner();
	if (pWnd != NULL) 
	{	
		pWnd->SendMessage(WM_MYBUTTON_CLICKED,iButtonIndex,0);
	}
}

void CMyButton::SetButtonState(BOOL flag,BOOL fRedraw)
{
	fButtonState = flag; 

	if (fRedraw) 
	{
		Invalidate();
	}
}