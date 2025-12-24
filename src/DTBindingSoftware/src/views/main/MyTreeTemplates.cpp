// MyTreeTemplates.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyTreeTemplates.h"

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
// CMyTreeTemplates

CMyTreeTemplates::CMyTreeTemplates()
{
}

CMyTreeTemplates::~CMyTreeTemplates()
{
}


BEGIN_MESSAGE_MAP(CMyTreeTemplates, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTreeTemplates)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APPLICATION_CLEARVIEWS,OnApplicationClearViews)
	ON_MESSAGE(WM_APPLICATION_UPDATEVIEWS,OnApplicationUpdateViews)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
LONG CMyTreeTemplates::OnApplicationClearViews(WPARAM wParam,LPARAM lParam)
{
	DeleteAllItems();
	return TRUE;
}

LONG CMyTreeTemplates::OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam)
{
	return (Update());
}

/////////////////////////////////////////////////////////////////////////////
// CMyTreeTemplates message handlers
int CMyTreeTemplates::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CImageList* const pImageList = TMGetGraphics()->GetSmallBindingsImageList();
	if (pImageList != NULL)
	{
		SetImageList(pImageList,TVSIL_NORMAL);	
	}	

	SetTextColor(RGB(0,0,255));
	
	return 0;
}

////////////////////
BOOL CMyTreeTemplates::Update()
{
	CString szName("Template Database");

	TVITEM tvItem;
	tvItem.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvItem.pszText = (LPTSTR) ((LPCTSTR)szName);		// CHANGES_MVS_2008_CSTRING
	tvItem.cchTextMax = szName.GetLength();
	tvItem.iImage = tvItem.iSelectedImage = 1;

	const unsigned long ulIdentifier = ( ((unsigned long) ((BYTE) 0) << 24) +
										 ((unsigned long) ((BYTE) 0) << 16) +
										 ((unsigned long) ((BYTE) 0) << 8) +
										 ((unsigned long) ((BYTE) 0)));
	
	tvItem.lParam = ulIdentifier;
	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = TVI_ROOT;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item= tvItem;
	InsertItem(&tvInsert);

	return TRUE;
}
