// MyFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyFrameWnd.h"

#include "NodesFormView.h"
#include "MyAddFlowSheet.h"
#include "UnitsFormView.h"

#include "MyWindowMessages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyFrameWnd

IMPLEMENT_DYNCREATE(CMyFrameWnd, CFrameWnd)

CMyFrameWnd::CMyFrameWnd()
{
	pNodesFormView = NULL;
	pAddFormView = NULL;
	pUnitsFormView = NULL;
}

CMyFrameWnd::~CMyFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CMyFrameWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CMyFrameWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NODENAME_CHANGED,OnNodeNameChanged)
	ON_MESSAGE(WM_UNITNAME_CHANGED,OnUnitNameChanged)
	ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
	ON_MESSAGE(WM_NODE_STATUS_CHANGED,OnNodeStatusChanged)
	ON_MESSAGE(WM_NODE_ETHERNETNWK_STATUS,onNodeEthernetNwkStatus)
	ON_MESSAGE(WM_TREE_NODE_COLLAPSED,OnTreeNodeCollapsed)
	ON_MESSAGE(WM_TREE_NODE_EXPANDED,OnTreeNodeExpanded)
	ON_MESSAGE(WM_APPLICATION_CLEARVIEWS,OnApplicationClearViews)
	ON_MESSAGE(WM_TREEVIEW_COLLAPSE,OnTreeviewCollapse)
	ON_MESSAGE(WM_APPLICATION_UPDATEVIEWS,OnApplicationUpdateViews)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyFrameWnd message handlers

BOOL CMyFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{		
	CCreateContext context;
	pContext = &context;

	pContext->m_pNewViewClass = RUNTIME_CLASS(CMyAddFlowSheet);
	pAddFormView = (CMyAddFlowSheet*) CreateView(pContext,IDD_MYADDFLOWFORM_FORM);
	if (pAddFormView == NULL)
		return FALSE;
	
	pContext->m_pNewViewClass = RUNTIME_CLASS(CNodesFormView);
	pNodesFormView = (CNodesFormView*) CreateView(pContext,IDD_NODESFORMVIEW_FORM);
	if (pNodesFormView == NULL)
		return FALSE;

	pContext->m_pNewViewClass = RUNTIME_CLASS(CUnitsFormView);
	pUnitsFormView = (CUnitsFormView*) CreateView(pContext,IDD_UNITSFORMVIEW_FORM);
	if (pUnitsFormView == NULL)
		return FALSE;

	pNodesFormView->OnInitialUpdate();
	pAddFormView->OnInitialUpdate();
	pUnitsFormView->OnInitialUpdate();

	ShowAddFlowForm(TRUE);
	
	return TRUE;

	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}

////////////////////////////////////////////////////////////////////////////////

void CMyFrameWnd::ShowUnitsForm(BYTE bNodeAddress, BOOL fBool)
{
	CRect rect;
	GetClientRect(&rect);

	ActiveView = ACTIVE_VIEW_UNITS;

	pAddFormView->ShowWindow(FALSE);	
	pNodesFormView->ShowWindow(FALSE);
	SetActiveView(pUnitsFormView,FALSE);

	pUnitsFormView->SendMessage(WM_SHOW_UNITS_FORM,(WPARAM) bNodeAddress,0);
}

void CMyFrameWnd::ShowNodesForm(BOOL fBool)
{
	ActiveView = ACTIVE_VIEW_NODES;

	pAddFormView->ShowWindow(FALSE);
	pUnitsFormView->ShowWindow(FALSE);
	SetActiveView(pNodesFormView,FALSE);

	pNodesFormView->SendMessage(WM_SHOW_NODES_FORM,0,0);
}

void CMyFrameWnd::ShowAddFlowForm(BOOL fBool)
{

	ActiveView = ACTIVE_VIEW_BINDINGS; 

	pNodesFormView->ShowWindow(FALSE);
	pUnitsFormView->ShowWindow(FALSE);
	SetActiveView(pAddFormView,FALSE);

	pAddFormView->SendMessage(WM_SHOW_ADDFLOW_FORM,0,0);
}

//////////////////////////////////////////////////////////////////////////////////


void CMyFrameWnd::InitViews()
{
	ASSERT(pAddFormView);
	pAddFormView->InitAddFlowDiagram2();
}

void CMyFrameWnd::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	CRect rect;
	GetClientRect(&rect);

	if (pAddFormView != NULL)
		pAddFormView->MoveWindow(rect);

	if (pNodesFormView != NULL)
		pNodesFormView->MoveWindow(rect);

	if (pUnitsFormView != NULL)
		pUnitsFormView->MoveWindow(rect);
}

void CMyFrameWnd::UpdateAllViews()
{
	if (pNodesFormView != NULL)
		pNodesFormView->SendMessage(WM_UPDATE_NODES_FORM,0,0);
	
	ASSERT(pNodesFormView);
}



LONG CMyFrameWnd::OnNodeNameChanged(WPARAM wParam,LPARAM lParam)
{
	pNodesFormView->SendMessage(WM_NODENAME_CHANGED,
								wParam,
								lParam);
	
	return TRUE;
}

LONG CMyFrameWnd::OnUnitNameChanged(WPARAM wParam,LPARAM lParam)
{
	pUnitsFormView->SendMessage(WM_UNITNAME_CHANGED,
								wParam,
								lParam);

	return TRUE;
}

LONG CMyFrameWnd::OnUnitStatusChanged(WPARAM wParam,LPARAM lParam)
{
	pUnitsFormView->SendMessage(WM_UNIT_STATUS_CHANGED,wParam,lParam);
	pAddFormView->SendMessage(WM_UNIT_STATUS_CHANGED,wParam,lParam);
	
	return TRUE;
}

LONG CMyFrameWnd::OnNodeStatusChanged(WPARAM wParam,LPARAM lParam)
{
	pNodesFormView->SendMessage(WM_NODE_STATUS_CHANGED,wParam,lParam);	
	return TRUE;
}


LONG CMyFrameWnd::onNodeEthernetNwkStatus(WPARAM wParam,LPARAM lParam) {
	pNodesFormView->SendMessage(WM_NODE_ETHERNETNWK_STATUS,wParam,lParam);
	return TRUE;
}	

///////////////////////////////////////////////////////////////////////////
LONG CMyFrameWnd::OnTreeNodeCollapsed(WPARAM wParam,LPARAM lParam)
{
	pUnitsFormView->SendMessage(WM_TREE_NODE_COLLAPSED,wParam,lParam);
	return TRUE;
}

LONG CMyFrameWnd::OnTreeNodeExpanded(WPARAM wParam,LPARAM lParam)
{
	pUnitsFormView->SendMessage(WM_TREE_NODE_EXPANDED,wParam,lParam);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////

int CMyFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


LONG CMyFrameWnd::OnApplicationClearViews(WPARAM wParam,LPARAM lParam)
{
	if (pAddFormView != NULL) pAddFormView->SendMessage(WM_APPLICATION_CLEARVIEWS);
	if (pNodesFormView != NULL) pNodesFormView->SendMessage(WM_APPLICATION_CLEARVIEWS);
	if (pUnitsFormView != NULL) pUnitsFormView->SendMessage(WM_APPLICATION_CLEARVIEWS);	
	return TRUE;
}

LONG CMyFrameWnd::OnTreeviewCollapse(WPARAM wParam,LPARAM lParam)
{
	if (pUnitsFormView != NULL) pUnitsFormView->SendMessage(WM_APPLICATION_CLEARVIEWS);	
	return TRUE;
}

LONG CMyFrameWnd::OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam)
{
	if (pAddFormView != NULL) pAddFormView->SendMessage(WM_APPLICATION_UPDATEVIEWS);
	if (pNodesFormView != NULL) pNodesFormView->SendMessage(WM_APPLICATION_UPDATEVIEWS);
	if (pUnitsFormView != NULL) pUnitsFormView->SendMessage(WM_APPLICATION_UPDATEVIEWS);	
	return TRUE;
}
