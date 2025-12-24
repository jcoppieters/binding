// MyTreeBindings.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyTreeBindings.h"

#include "MainFrm.h"
#include "MyWindowMessages.h"
#include "TMFCControlsHelper.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
using namespace TMBindingDatabase;
// ------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
#define DRAWING_DISABLE_CTRL		TShowWindow<CWnd> RTTI(this)		
////////////////////////////////////////////////////////////////////////////
// CMyTreeBindings

CMyTreeBindings::CMyTreeBindings() : m_pMainWnd(NULL), m_bindingViewMode(BINDINGS_VIEWMODE_NAME),
	m_hActiveItem(NULL) { }

CMyTreeBindings::~CMyTreeBindings() { }

BEGIN_MESSAGE_MAP(CMyTreeBindings, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTreeBindings)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
  	ON_WM_CONTEXTMENU()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BINDINGDATABASE_UPDATE,OnBindingdatabaseUpdate)	
	ON_MESSAGE(WM_BINDINGDATABASE_ADDENTRY,OnBindingDatabaseAdd)
	ON_MESSAGE(WM_BINDINGDATABASE_REMOVEENTRY,OnBindingDatabaseRemove)
	ON_MESSAGE(WM_BINDINGDATABASE_NAMECHANGED,OnBindingDatabaseNameChanged)
	ON_MESSAGE(WM_APPLICATION_CLEARVIEWS,OnApplicationClearViews)
	ON_MESSAGE(WM_APPLICATION_UPDATEVIEWS,OnApplicationUpdateViews)
	ON_MESSAGE(WM_TREEVIEW_SORT_BINDINGS,OnViewSortBindings)
	ON_MESSAGE(WM_TREEVIEW_SORTBINDINGSONLYNAME,OnTreeViewSortBindingsOnlyName)
	ON_MESSAGE(WM_TREEVIEW_SORTBINDINGSNAME,OnTreeViewSortBindingsName)
	ON_MESSAGE(WM_TREEVIEW_SORTBINDINGSNUMBER,OnTreeViewSortBindingsNumber)
	ON_COMMAND(ID_TREENODEBINDINGS_VIEWBINDING, &CMyTreeBindings::OnContextmenuBindingsShow)
	ON_COMMAND(ID_TREENODEBINDINGS_RENAMEBINDING, &CMyTreeBindings::OnContextmenuBindingsRename)
	// ON_COMMAND(ID_TREEBINDINGS_DELETEBINDING, &CMyTreeBindings::OnContextmenuBindingsDelete)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

LONG CMyTreeBindings::OnApplicationClearViews(WPARAM wParam,LPARAM lParam)
{
	DeleteAllItems();
	return TRUE;
}

LONG CMyTreeBindings::OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam)
{
	return (Update());
}
/////////////////////////////////////////////////////////////////////////////
// CMyTreeBindings message handlers

int CMyTreeBindings::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pMainWnd = ((CMainFrame*)AfxGetMainWnd());
	ASSERT(m_pMainWnd);

	CImageList* const pImageList = TMGetGraphics()->GetSmallBindingsImageList();
	if (pImageList != NULL)
	{
		SetImageList(pImageList,TVSIL_NORMAL);			
	}

	SetTextColor(RGB(0,0,255));
	return 0;
}

BOOL CMyTreeBindings::Update()
{	
	if (TMGetBindingEntries() != NULL) {
		DRAWING_DISABLE_CTRL;
		CWaitCursor wait;			// Sedert V16.46
		HTREEITEM ParentNode;		
		DeleteAllItems();
		CBindingEntries::Iterator it(TMGetBindingEntries()->CreateIterator());
		for (it.Begin();!it.IsDone();it.Next()) {
			AddBindingEntry(TVI_ROOT,&ParentNode,it.CurrentItem());	
		}		
		SortChildren(TVI_ROOT);		// CAN I SORT HERE ?		
		return TRUE;
	}
	return FALSE;
}

void CMyTreeBindings::AddBindingEntry(HTREEITEM parent,HTREEITEM *pNewParent,CBindingEntry *pEntry)
{		
	CString& szName = pEntry->GetName();			// CHANGES_MVS_2008_CSTRING

	TVITEM tvItem;
	tvItem.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvItem.pszText = (LPTSTR)((LPCTSTR) szName);	// CHANGES_MVS_2008_CSTRING
	tvItem.cchTextMax = szName.GetLength();	

	if (pEntry->IsSend() == TRUE) tvItem.iImage = tvItem.iSelectedImage = 1;
	else tvItem.iImage = tvItem.iSelectedImage = 0;

	const unsigned long ulIdentifier = pEntry->GetID();
	tvItem.lParam = ulIdentifier;
	
	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = parent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item= tvItem;

	*pNewParent = InsertItem(&tvInsert);

	DisplayBindingName(*pNewParent,pEntry);
}

/////////////////////////////////////////////////////////////////////////////
// View binding in AddFlow Sheet
void CMyTreeBindings::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	const HTREEITEM hItem = GetSelectedItem();
	if (hItem != NULL) {	
		showViewBindings();
		RequestEditBinding(hItem);
	}
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}


void CMyTreeBindings::showViewBindings() {
	if (NULL == m_pMainWnd) {
		return;
	}
	m_pMainWnd->showBindingsForm();
}

/////////////////////////////////////////////////////////////////////////////
// Description message in ScrollView
void CMyTreeBindings::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
HTREEITEM CMyTreeBindings::GetBindingEntry(unsigned short nID)
{	
	const unsigned long ulIdentifier = (nID & 0xFFFF);

	HTREEITEM hCurrent = GetNextItem(TVI_ROOT, TVGN_ROOT);

	while (hCurrent != NULL) 
	{		
		const unsigned long ulItemData = GetItemData(hCurrent);					

		if (ulItemData == ulIdentifier) break;			
		hCurrent = GetNextItem(hCurrent, TVGN_NEXT);
	}
	
	return hCurrent;	// Not Found : hCurrent = NULL !
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMyTreeBindings::AddBindingEntry(unsigned short nID)
{
	if (nID == 0) 
	{
		return FALSE;
	}
	
	if (TMGetBindingEntries() != NULL)
	{
		CBindingEntry *pEntry = NULL;
		
		if ((pEntry = TMGetBindingEntries()->Get(nID)) == NULL) 
		{			
			CString szError;
			szError.Format("Fatal error : Unknow Entry Id %i",nID);
			AfxMessageBox(szError);
			return FALSE;
		}
	
		HTREEITEM ParentNode;

		// Only if ID is not in List Add
		HTREEITEM Node = GetBindingEntry(nID);
	
	
		if (Node == NULL)  
		{
			AddBindingEntry(TVI_ROOT,&ParentNode,pEntry);	
		}
		else 
		{				
			// Node is in TREE --> Update Name ...
			
			if (DisplayBindingName(Node,pEntry) == FALSE) 
			{
				ASSERT(0);
				return FALSE;
			}
		
			// new Update Image ...
			if (pEntry->IsSend() == TRUE) 
			{
				SetItemImage(Node,1,1);
			}
			else 
			{
				SetItemImage(Node,0,0);
			}		
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CMyTreeBindings::DeleteEntry(unsigned short nID)
{
	if (nID == 0) return FALSE;

	const HTREEITEM Node = GetBindingEntry(nID);
	if (Node == NULL) 
	{
		CString szError;
		szError.Format("Fatal error : Can't find Entry Id %i in BindingTree",nID);
		AfxMessageBox(szError);
		
		return FALSE;
	}
	return (DeleteItem(Node));
}
/////////////////////////////////////////////////////////////////////////////
// Update Full Database Tree
LONG CMyTreeBindings::OnBindingdatabaseUpdate(WPARAM wParam,LPARAM lParam)
{
	Update();
	return TRUE;
}

LONG CMyTreeBindings::OnBindingDatabaseAdd(WPARAM wParam,LPARAM lParam)
{
	return (AddBindingEntry(wParam & 0xFFFF));
}

LONG CMyTreeBindings::OnBindingDatabaseRemove(WPARAM wParam,LPARAM lParam)
{
	return (DeleteEntry(wParam & 0xFFFF));
}

/**
 * Date 20060322 : Changed function
 */
LONG CMyTreeBindings::OnBindingDatabaseNameChanged(WPARAM wParam,LPARAM lParam)
{
	const HTREEITEM node = GetBindingEntry(wParam & 0xFFFF);

	if (node == 0) 
	{
		CString szError("Error : Couldn't find entry in Binding Tree ");
		AfxMessageBox(szError);
		return FALSE;
	}

	if (TMGetBindingEntries() != NULL)
	{
		CBindingEntry *pEntry;
			
		if ((pEntry = TMGetBindingEntries()->Get(wParam & 0xFFFF)) != NULL) 
		{
			return (DisplayBindingName(node,pEntry));
		}
		else
		{
			CString szError;
			szError.Format("Fatal error : Unknow Entry Id %i",wParam & 0xFFFF);
			AfxMessageBox(szError);			
			return FALSE;
		}	
	}	
	ASSERT(0);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
void CMyTreeBindings::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString szMessage,szTemp;
	int i,j;
	unsigned long ulIdentifier;

	TV_DISPINFO *tv_dispinfo = (TV_DISPINFO*) pNMHDR;
	TVITEM *ptvItem = &tv_dispinfo->item; 
   
	if (ptvItem->pszText != NULL) 
	{		
		if (strcmp(ptvItem->pszText,"") != 0) 
		{
			szMessage.Format("%s",ptvItem->pszText);		
			ulIdentifier = ptvItem->lParam;
	
			switch (m_bindingViewMode) 
			{
				case BINDINGS_VIEWMODE_BINDINGNR :

					szTemp = szMessage;
					j = szMessage.GetLength();
					i = szTemp.Find('#',0);
					i = szTemp.Find('#',i+1);
					if (i!= -1) 
					{
						szMessage = szTemp.Right(j-(i+1));
					}
					szMessage.TrimLeft();
					szMessage.TrimRight();
						
					if (szMessage.GetLength() > 20) szMessage.SetAt(20,'\0');

				break;

				case BINDINGS_VIEWMODE_ONLYNAME :
				case BINDINGS_VIEWMODE_NAME :
				default :
					
					if (szMessage.GetLength() > 20) szMessage.SetAt(20,'\0');

					szTemp = szMessage;
					i = szTemp.Find('#',0);
					if (i != -1) {
						szMessage = szTemp.Left(i);
					}	
				
				break;
			}

			m_pMainWnd->SendMessage(WM_BINDINGDATABASE_NAMECHANGED,
							     (WPARAM) (ulIdentifier),
								 (LPARAM) (LPCTSTR) (szMessage));	// CHANGES_MVS_2008_CSTRING
		}
	}		
	*pResult = 0;
}

BOOL CMyTreeBindings::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && 
		pMsg->wParam == VK_RETURN || 
		pMsg->wParam == VK_ESCAPE)
	{	
         CEdit* const edit = GetEditControl();	
         if (edit) 
		 {
            edit->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
            return TRUE;
         }  
	}
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void CMyTreeBindings::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	#define VK_ENTER		13
	
	HTREEITEM hItem = GetSelectedItem();

	switch(nChar) 
	{
		case VK_ENTER:
			if (hItem != NULL) {	
				hItem = GetNextVisibleItem(hItem);		
				if (hItem) {
					RequestEditBinding(hItem);
// Show Binding ??				
					SelectItem(hItem);						
					EditLabel(hItem);
				}
				else  { // end of Tree 
					hItem = GetFirstVisibleItem( );
					if (hItem) {
// Show Binding ??
						//AfxMessageBox("Show Binding ...");
						RequestEditBinding(hItem);
						SelectItem(hItem);					
						EditLabel(hItem);
					}
				}
			}
		break;

		case VK_F2:
			if (hItem != NULL) {	
				EditLabel(hItem);
			}
		break;	

		default:
			if (hItem != NULL) {
				RequestEditBinding(hItem);
			}
		break;
	}

	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}


LONG CMyTreeBindings::OnViewSortBindings(WPARAM wParam,LPARAM lParam)
{
	switch (m_bindingViewMode) {
		case BINDINGS_VIEWMODE_NAME :
			m_bindingViewMode = BINDINGS_VIEWMODE_BINDINGNR;
		break;
		case BINDINGS_VIEWMODE_BINDINGNR :
			m_bindingViewMode = BINDINGS_VIEWMODE_ONLYNAME;
		break;
		case BINDINGS_VIEWMODE_ONLYNAME :
		default:
			m_bindingViewMode = BINDINGS_VIEWMODE_NAME;
		break;
	}	
	Update();		
	return TRUE;
}

/********/
BOOL CMyTreeBindings::DisplayBindingName(HTREEITEM hItem,CBindingEntry *pEntry)
{
	CString szName,szText,szBindingNr;
	
	if (hItem == NULL) { ASSERT(0); return FALSE; }
	if (pEntry == NULL) { ASSERT(0); return FALSE; }
	
	szName.Format(_T("%s"),pEntry->GetName());
	szBindingNr.Format(_T("%04x"),pEntry->GetBindingNr());

	switch (m_bindingViewMode) {
		case BINDINGS_VIEWMODE_NAME :
			szText.Format(_T("%-20s #%s#"),szName,szBindingNr);	
		break;		
		case BINDINGS_VIEWMODE_BINDINGNR :
			szText.Format(_T("#%s# %s"),szBindingNr,szName);
		break;
		case BINDINGS_VIEWMODE_ONLYNAME:
		default:
			szText.Format(_T("%s"),szName);
		break;
	}
	return(SetItemText(hItem,szText));
}


LONG CMyTreeBindings::OnTreeViewSortBindingsOnlyName(WPARAM wParam,LPARAM lParam)
{
	if (m_bindingViewMode != BINDINGS_VIEWMODE_ONLYNAME) 
	{
		m_bindingViewMode = BINDINGS_VIEWMODE_ONLYNAME;
		Update();
	}
	SortChildren(TVI_ROOT);
	return TRUE;
}

LONG CMyTreeBindings::OnTreeViewSortBindingsName(WPARAM wParam,LPARAM lParam)
{
	if (m_bindingViewMode != BINDINGS_VIEWMODE_NAME) 
	{
		m_bindingViewMode = BINDINGS_VIEWMODE_NAME;
		Update();
	}
	SortChildren(TVI_ROOT);
	return TRUE;
}

LONG CMyTreeBindings::OnTreeViewSortBindingsNumber(WPARAM wParam,LPARAM lParam)
{
	if (m_bindingViewMode != BINDINGS_VIEWMODE_BINDINGNR) 
	{
		m_bindingViewMode = BINDINGS_VIEWMODE_BINDINGNR;
		Update();
	}
	SortChildren(TVI_ROOT);
	return TRUE;
}

void CMyTreeBindings::RequestEditBinding(HTREEITEM hItem)
{
	if ((NULL == m_pMainWnd) || (NULL == hItem)) {
		return;
	}
	
	const unsigned long ulIdentifier = GetItemData(hItem);	
	m_pMainWnd->SendMessage(WM_BINDINGENTRY_EDIT,ulIdentifier & 0x0000FFFF);	
}

#if(0)
void CMyTreeBindings::RequestDeleteBinding(HTREEITEM hItem) {
	if ((NULL == m_pMainWnd) || (NULL == hItem)) {
		return;
	}
	const unsigned long ulIdentifier = GetItemData(hItem);	
	m_pMainWnd->SendMessage(WM_BINDINGENTRY_DELETE,ulIdentifier & 0x0000FFFF);	
}
#endif 

void CMyTreeBindings::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
  	TRACE0("CMyTreeCtrl::OnRClick()\n");
  	// Send WM_CONTEXTMENU to self
  	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
  	// Mark message as handled and suppress default handling
  	*pResult = 1;
}

void CMyTreeBindings::OnContextMenu(CWnd* pWnd, CPoint ptMousePos) 
{
	// if Shift-F10
	if (ptMousePos.x == -1 && ptMousePos.y == -1)
		ptMousePos = (CPoint) GetMessagePos();

	ScreenToClient(&ptMousePos);

	UINT uFlags;
	HTREEITEM htItem = HitTest( ptMousePos, &uFlags );
	if( htItem == NULL )
		return;

	// Sedert V16.35: Item selecteren.
	// Aanpassing zorgt ervoor dat het juiste item/binding geselecteerd is bij het openen van 
	// een binding via het context menu.
	if (TVHT_ONITEM & uFlags) {
		this->SelectItem(htItem);
	}

	m_hActiveItem = htItem;

	CMenu menu;
	CMenu* pPopup;

	// the font popup is stored in a resource
	menu.LoadMenu(IDR_CONTEXTMENU_TREEBINDINGS);
	pPopup = menu.GetSubMenu(0);
	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this );
}

void CMyTreeBindings::OnContextmenuBindingsRename() {
	if (NULL != m_hActiveItem) {
		EditLabel(m_hActiveItem);
	}
}
void CMyTreeBindings::OnContextmenuBindingsShow() {
	if (NULL == m_hActiveItem) {
		return;
	}
	showViewBindings();
	RequestEditBinding(m_hActiveItem);
}


#if(0)
void CMyTreeBindings::OnContextmenuBindingsDelete() {
	if (NULL == m_hActiveItem) {
		return;
	}
	RequestDeleteBinding(m_hActiveItem);
	m_hActiveItem = NULL;
}
#endif 