// ApplicationViewManager.cpp: implementation of the CApplicationViewManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ApplicationViewManager.h"

#include "views/main/MyFormView.h"
#include "views/main/MyScrollView.h"
#include "views/main/MyView.h"
#include "views/main/NodesFormView.h"
#include "views/main/MyFrameWnd.h"
#include "views/main/MyAddFlowSheet.h"


#include "MyWindowMessages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
CApplicationViewManager* CApplicationViewManager::GetInstance()
{
	static CApplicationViewManager instance;
	return (&instance);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CApplicationViewManager::CApplicationViewManager()
{
	pScrollView = NULL;
	pMyFrameWnd = NULL;
	pMyTreeView = NULL;
}

CApplicationViewManager::~CApplicationViewManager()
{

}



BOOL CApplicationViewManager::IsBindingViewActive(void) 
{
	if (pMyFrameWnd != NULL)
	{
		if (pMyFrameWnd->GetCurrentActiveView() == ACTIVE_VIEW_BINDINGS) 
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CApplicationViewManager::IsNodesViewActive(void) 
{
	if (pMyFrameWnd != NULL)
	{
		if (pMyFrameWnd->GetCurrentActiveView() == ACTIVE_VIEW_NODES) 
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CApplicationViewManager::IsUnitsViewActive(void) 
{
	if (pMyFrameWnd != NULL)
	{
		if (pMyFrameWnd->GetCurrentActiveView() == ACTIVE_VIEW_UNITS) 
		{
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CApplicationViewManager::IsTreeNodesActive(void) 
{
	ASSERT(pMyTreeView);

	if (pMyTreeView != NULL)
	{
		return (pMyTreeView->IsTreeNodesActive());
	}
	return FALSE;
}

BOOL CApplicationViewManager::IsTreeBindingsActive(void)
{
	ASSERT(pMyTreeView);

	if (pMyTreeView != NULL)
	{
		return (pMyTreeView->IsTreeBindingsActive());
	}
	return FALSE;
}

BOOL CApplicationViewManager::IsTreeTemplatesActive(void)
{
	ASSERT(pMyTreeView);

	if (pMyTreeView != NULL)
	{
		return (pMyTreeView->IsTreeTemplatesActive());
	}
	return FALSE;
}


BOOL CApplicationViewManager::CreateMainSplitter(CWnd *pMainWnd,CCreateContext* pContext)
{
	BOOL fResult = TRUE;
	CRect rect;

	// max 2 rows, 1 col
	fResult =  m_wndSplitter.CreateStatic(pMainWnd,2,1,WS_CHILD | WS_VISIBLE); //was this

    pMainWnd->GetWindowRect( &rect );
	
	// row 1, col 0 = MyView
	m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CMyView), CSize(800,0), pContext);	
	// row 2 , col 0 = MyScrollView
	m_wndSplitter.CreateView(1,0,RUNTIME_CLASS(CMyScrollView), CSize(0,0), pContext);
	
	m_wndSplitter.SetRowInfo(0,rect.Height()-10,100);
	// m_wndSplitter.SetColumnInfo(0,300,300);						// <----------------------------------- TESTEN.
	
	m_wndSplitter.RecalcLayout();
	
	// SET SCROLLBARS FOR SCROLLVIEW instead of using SplitterWnd ... !!!
	CWnd *myWnd = m_wndSplitter.GetPane(1,0);
	myWnd->ShowScrollBar(SB_BOTH,TRUE);

	CMyView *SplitView = static_cast <CMyView*> (m_wndSplitter.GetPane(0,0));	
	if (!SplitView->CreateViews(pContext))
		return FALSE;	

	pScrollView = static_cast <CMyScrollView*> (m_wndSplitter.GetPane(1,0));		
	pMyFrameWnd = SplitView->pMyFrameWnd;
	pMyTreeView = SplitView->pMyTreeView;	
	return fResult;
}

LONG CApplicationViewManager::OnNodeNameChanged(WPARAM wParam,LPARAM lParam)
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_NODENAME_CHANGED,
												 wParam,
												 lParam);
	}

	if (pMyFrameWnd != NULL)
	{
		pMyFrameWnd->SendMessage(WM_NODENAME_CHANGED,
								 wParam,
								 lParam);		
	}

	return TRUE;
}

LONG CApplicationViewManager::OnUnitNameChanged(WPARAM wParam,LPARAM lParam)
{
	if (pMyTreeView != NULL) {
		pMyTreeView->GetNodesTree()->SendMessage(WM_UNITNAME_CHANGED,wParam,lParam);
	}

	if (pMyFrameWnd != NULL) {
		pMyFrameWnd->SendMessage(WM_UNITNAME_CHANGED,wParam,lParam);
	}

	return TRUE;
}

LONG CApplicationViewManager::OnDisplayMessage(WPARAM wParam,LPARAM lParam)
{
	if (pScrollView != NULL)
	{
		return (pScrollView->SendMessage(WM_DISPLAY_MESSAGE,wParam,lParam));
	}
	return TRUE;
}

LONG CApplicationViewManager::OnClearMessages(WPARAM ,LPARAM )
{
	if (pScrollView != NULL)
	{
		return (pScrollView->SendMessage(WM_CLEAR_MESSAGES,0,0));
	}
	return FALSE;
}

LONG CApplicationViewManager::OnUnitStatusChanged(WPARAM wParam,LPARAM lParam)
{
	if (pMyFrameWnd != NULL && pMyTreeView!= NULL)
	{
		pMyFrameWnd->SendMessage(WM_UNIT_STATUS_CHANGED,wParam,lParam);

		pMyTreeView->GetNodesTree()->SendMessage(WM_UNIT_STATUS_CHANGED,wParam,lParam);

		return TRUE;
	}
	return TRUE;
}

LONG CApplicationViewManager::OnNodeStatusChanged(WPARAM wParam,LPARAM lParam)
{
	if (pMyFrameWnd != NULL)
	{
		return (pMyFrameWnd->SendMessage(WM_NODE_STATUS_CHANGED,wParam,lParam));
	}
	return TRUE;
}

// sedert V16.03
LONG CApplicationViewManager::onNodeEthernetNwkStatus(WPARAM wParam,LPARAM lParam) {
	if (pMyFrameWnd != NULL) {
		return (pMyFrameWnd->SendMessage(WM_NODE_ETHERNETNWK_STATUS,wParam,lParam));
	}
	return TRUE;
}

LONG CApplicationViewManager::OnTreeNodeCollapsed(WPARAM wParam,LPARAM lParam)
{
	if (pMyFrameWnd != NULL)
	{
		return (pMyFrameWnd->SendMessage(WM_TREE_NODE_COLLAPSED,wParam,lParam));
	}
	return TRUE;
}

LONG CApplicationViewManager::OnTreeNodeExpanded(WPARAM wParam,LPARAM lParam)
{
	if (pMyFrameWnd != NULL)
	{
		return (pMyFrameWnd->SendMessage(WM_TREE_NODE_EXPANDED,wParam,lParam));
	}
	return TRUE;
}


LONG CApplicationViewManager::OnAddFlowAddUnit(WPARAM wParam,LPARAM lParam)
{
	if (pMyFrameWnd != NULL)
	{
		if (IsBindingViewActive()) 
		{
			return (pMyFrameWnd->pAddFormView->SendMessage(WM_ADDFLOW_ADDUNIT,wParam,lParam));
		}
		return FALSE;
	}
	return TRUE;
}


LONG CApplicationViewManager::OnAddFlowDraggingUnit(WPARAM wParam,LPARAM lParam)
{
	if (pMyFrameWnd != NULL)
	{
		if (IsBindingViewActive()) 
		{
			return (pMyFrameWnd->pAddFormView->SendMessage(WM_ADDFLOW_DRAGGING_UNIT,wParam,lParam));
		}
		return FALSE;
	}
	return TRUE;
}

LONG CApplicationViewManager::OnAddFlowDropUnit(WPARAM wParam,LPARAM lParam)
{
	if (pMyFrameWnd != NULL)
	{
		if (IsBindingViewActive()) 
		{
			return(pMyFrameWnd->pAddFormView->SendMessage(WM_ADDFLOW_DROP_UNIT,wParam,lParam));												  
		}
		return FALSE;
	}
	return TRUE;
}

LONG CApplicationViewManager::OnAddFlowRemoveControl(WPARAM wParam,LPARAM lParam)
{
	if (pMyFrameWnd != NULL)
	{
		return (pMyFrameWnd->pAddFormView->SendMessage(WM_ADDFLOW_REMOVE_CONTROL,wParam,lParam));
	}
	return TRUE;
}

LONG CApplicationViewManager::OnBindingEntryAdd(WPARAM wParam,LPARAM )
{
	if ( pMyFrameWnd!= NULL && pMyTreeView!=NULL )
	{
		if (IsBindingViewActive()) 
		{
			pMyFrameWnd->pAddFormView->SendMessage(WM_BINDINGENTRY_NEW);
			pMyTreeView->GetBindingsTree()->SendMessage(WM_BINDINGDATABASE_ADDENTRY,wParam);
			pMyTreeView->GetNodesTree()->SendMessage(WM_BINDINGDATABASE_ADDENTRY,wParam);
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

LONG CApplicationViewManager::OnBindingEntryDelete(WPARAM wParam,LPARAM )
{
	if ( pMyFrameWnd!= NULL && pMyTreeView!=NULL )
	{
		if (IsBindingViewActive())  
		{
			pMyFrameWnd->pAddFormView->SendMessage(WM_BINDINGENTRY_NEW);							
			pMyTreeView->GetBindingsTree()->SendMessage(WM_BINDINGDATABASE_REMOVEENTRY,wParam);
			pMyTreeView->GetNodesTree()->SendMessage(WM_BINDINGDATABASE_REMOVEENTRY,wParam);		
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

LONG CApplicationViewManager::OnBindingEntryEdit(WPARAM wParam,LPARAM )
{
	if (pMyFrameWnd != NULL)
	{
		if (IsBindingViewActive()) 
		{
			return(pMyFrameWnd->pAddFormView->SendMessage(WM_BINDINGENTRY_EDIT,wParam));
		}
		return FALSE;
	}
	return TRUE;
}

LONG CApplicationViewManager::OnBindingEntryNew(WPARAM ,LPARAM )
{
	if (pMyFrameWnd != NULL)
	{
		if (IsBindingViewActive()) 
		{
			return (pMyFrameWnd->pAddFormView->SendMessage(WM_BINDINGENTRY_NEW));
		}
		return FALSE;
	}
	return TRUE;
}


LONG CApplicationViewManager::OnBindingDatabaseNameChanged(WPARAM wParam,LPARAM lParam) 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetBindingsTree()->SendMessage(WM_BINDINGDATABASE_NAMECHANGED,wParam,lParam);
		pMyTreeView->GetNodesTree()->SendMessage(WM_BINDINGDATABASE_NAMECHANGED,wParam,lParam);

		return TRUE;
	}
	return TRUE;
}

LONG CApplicationViewManager::OnBindingdatabaseUpdate(WPARAM ,LPARAM ) 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetBindingsTree()->SendMessage(WM_BINDINGDATABASE_UPDATE);
		pMyTreeView->GetNodesTree()->SendMessage(WM_BINDINGDATABASE_UPDATE);
		return TRUE;
	}
	return TRUE;
}



void CApplicationViewManager::OnButtonAnd()
{
	if (pMyFrameWnd != NULL && pMyFrameWnd->pAddFormView != NULL)
	{
		if (IsBindingViewActive()) 
		{
			pMyFrameWnd->pAddFormView->SendMessage(WM_ADDFLOW_ADDCONTROL,PM_OPERATOR_AND);
		}
	}	
}

void CApplicationViewManager::OnButtonOr()
{
	if (pMyFrameWnd != NULL)
	{
		if (IsBindingViewActive()) 
		{
			pMyFrameWnd->pAddFormView->SendMessage(WM_ADDFLOW_ADDCONTROL,PM_OPERATOR_OR);
		}
	}
}

void CApplicationViewManager::OnButtonTimer()
{
	if (pMyFrameWnd != NULL)
	{
		if (IsBindingViewActive()) 
		{
			pMyFrameWnd->pAddFormView->SendMessage(WM_ADDFLOW_ADDCONTROL,PM_OPERATOR_TIMER);
		}
	}
}

void CApplicationViewManager::OnButtonXor()
{
	if (pMyFrameWnd != NULL)
	{	
		if (IsBindingViewActive()) 
		{
			pMyFrameWnd->pAddFormView->SendMessage(WM_ADDFLOW_ADDCONTROL,PM_OPERATOR_XOR);
		}
	}
}

void CApplicationViewManager::ShowNodesForm() 
{
	if (pMyFrameWnd != NULL)
	{
		pMyFrameWnd->ShowNodesForm(TRUE);
	}
}

void CApplicationViewManager::ShowBindingsForm(void) 
{
	if (pMyFrameWnd != NULL)
	{
		pMyFrameWnd->ShowAddFlowForm(TRUE);
	}
}

void CApplicationViewManager::ShowUnitsForm(void) 
{
	if (pMyFrameWnd != NULL)
	{
		pMyFrameWnd->ShowUnitsForm(0xFF,TRUE);
	}
}

void CApplicationViewManager::ClearViews(void)
{
	if (pMyFrameWnd != NULL) {
		pMyFrameWnd->SendMessage(WM_APPLICATION_CLEARVIEWS);
	}

	if (pMyTreeView != NULL) {
		pMyTreeView->SendMessage(WM_APPLICATION_CLEARVIEWS);
	}	
}

void CApplicationViewManager::UpdateViews(int nodeAddress)
{
	if (pMyTreeView != NULL)	
		pMyTreeView->SendMessage(WM_APPLICATION_UPDATEVIEWS,nodeAddress);

	if (pMyFrameWnd != 0)
		pMyFrameWnd->SendMessage(WM_APPLICATION_UPDATEVIEWS,nodeAddress);

}

BOOL CApplicationViewManager::InitViews(void)
{
	if (pMyFrameWnd != NULL) 
	{
		pMyFrameWnd->InitViews();	
		return TRUE;
	}	
	return FALSE;
}

WPARAM CApplicationViewManager::GetBindingEntry(void) 
{
	if (pMyFrameWnd != NULL)
	{
		return ((WPARAM) (pMyFrameWnd->pAddFormView->GetBindingView()));
	}
	return 0;
}

CString CApplicationViewManager::GetEditControlBindingName(void)
{
	CString szName;

	if (pMyFrameWnd != NULL)
	{	
		// BUG-0120: 
		// Een tijdelijk buffer gebruiken en dit buffer copieren naar een CString.
		// Vroeger werd CString::GetBuffer(0) gebruikt.

		enum { MAX_TEMP_NAME_LENGTH = 20 };
		char szTempBuffer[ MAX_TEMP_NAME_LENGTH ];

		if (pMyFrameWnd->pAddFormView->m_EditBindingName.GetWindowText(szTempBuffer,MAX_TEMP_NAME_LENGTH-1) > 0)
		{
			szName = szTempBuffer;
		}
	}
	
	return szName;
}

void CApplicationViewManager::OnViewSortnodesbyname()
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_SORT_NODESBYNAME);
	}
}
void CApplicationViewManager::OnViewSortnodesbyphaddress()
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_SORT_NODESBYPHADDRESS);
	}
}

void CApplicationViewManager::OnViewSortnodesbylogicaladdress()
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_SORT_NODESBYADDRESS);
	}
}

void CApplicationViewManager::OnViewSortnodesonlyname() 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_SORT_NODESONLYNAME);
	}
}

void CApplicationViewManager::OnViewSortBindings()
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetBindingsTree()->SendMessage(WM_TREEVIEW_SORT_BINDINGS);
	}
}

void CApplicationViewManager::OnButtonSortNodes()
{
	if (pMyTreeView != NULL)
	{
		// f(Current Tree View ...)
		if (IsTreeNodesActive()) 
		{
			pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_SORT_NODES);
		}
		else if (IsTreeBindingsActive()) 
		{
			pMyTreeView->GetBindingsTree()->SendMessage(WM_TREEVIEW_SORT_BINDINGS);
		}
	}
}

void CApplicationViewManager::OnButtonCollapse()
{
	if (pMyTreeView != NULL && pMyFrameWnd != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_COLLAPSE);
		pMyFrameWnd->SendMessage(WM_TREEVIEW_COLLAPSE);
	}
}

void CApplicationViewManager::OnButtonExpand()
{
	if (pMyTreeView != NULL && pMyFrameWnd != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_EXPAND);
		pMyFrameWnd->SendMessage(WM_TREEVIEW_EXPAND);	//TODO!
	}
}
void CApplicationViewManager::OnButtonSortUnits() 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_SORT_UNITS);
	}
}

void CApplicationViewManager::OnViewUnitsbyaddress() 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_UNIT_BYADDRESS);	
	}
}

void CApplicationViewManager::OnViewUnitsbyname() 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_UNIT_BYNAME);		
	}
}

void CApplicationViewManager::OnViewUnitsonlyname() 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetNodesTree()->SendMessage(WM_TREEVIEW_UNIT_ONLYNAME);		
	}
}

void CApplicationViewManager::OnTreeviewBindingsNameandnumber() 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetBindingsTree()->SendMessage(WM_TREEVIEW_SORTBINDINGSNAME);
	}
}
void CApplicationViewManager::OnTreeviewBindingsNumberandname() 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetBindingsTree()->SendMessage(WM_TREEVIEW_SORTBINDINGSNUMBER);
	}
}
void CApplicationViewManager::OnTreeviewBindingsOnlyname() 
{
	if (pMyTreeView != NULL)
	{
		pMyTreeView->GetBindingsTree()->SendMessage(WM_TREEVIEW_SORTBINDINGSONLYNAME);
	}
}
