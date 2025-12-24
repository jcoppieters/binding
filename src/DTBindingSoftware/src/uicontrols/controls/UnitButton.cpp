// UnitButton.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "UnitButton.h"
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

/////////////////////////////////////////////////////////////////////////////
// CUnitButton

CUnitButton::CUnitButton()
{
	bSelNodeAddress = 0xFF;
	bSelUnitAddress = 0xFF;
}

CUnitButton::~CUnitButton()
{
}


BEGIN_MESSAGE_MAP(CUnitButton, CButton)
	//{{AFX_MSG_MAP(CUnitButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitButton message handlers

void CUnitButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT(bSelNodeAddress != 0xFF);	

	if (bSelUnitAddress != 0xFF) 
	{
		TMGetGuiUnit()->ClickOn(bSelNodeAddress,bSelUnitAddress);	
	}

	CButton::OnLButtonDown(nFlags, point);
}

void CUnitButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (bSelUnitAddress != 0xFF) 
	{
		TMGetGuiUnit()->ClickOff(bSelNodeAddress,bSelUnitAddress);
	}

	CButton::OnLButtonUp(nFlags, point);
}

void CUnitButton::Init(BYTE bNodeAddress,BYTE bUnitAddress)
{
	bSelNodeAddress = bNodeAddress;
	bSelUnitAddress = bUnitAddress;
}





