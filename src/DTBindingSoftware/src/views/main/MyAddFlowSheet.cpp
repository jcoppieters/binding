// MyAddFlowSheet.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyAddFlowSheet.h"
#include "MyWindowMessages.h"

#include "CanNode.h"
#include "BindingGuiComponent.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"
#include "MainFrm.h"
#include "addflow\AddflowConst.h"

#include "utils/win32/NotifyError.h"

// ------------------------------------------
#include "TMGlobals.h"
using namespace TMBindingDatabase;
// ------------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
////////////////////////////////////////////////////////////////////////////

#define LBUTTON_DOWN			 1
#define RBUTTON_DOWN			 2		


extern float XPixelsToTwips(int x);
extern float YPixelsToTwips(int y);


#define CHANGES_2004
/////////////////////////////////////////////////////////////////////////////
// CMyAddFlowSheet

IMPLEMENT_DYNCREATE(CMyAddFlowSheet, CFormView)

CMyAddFlowSheet::CMyAddFlowSheet()
	: CFormView(CMyAddFlowSheet::IDD)
{
	//{{AFX_DATA_INIT(CMyAddFlowSheet)
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
	m_pStringEntries = 0;
}


CMyAddFlowSheet::~CMyAddFlowSheet()
{
}

void CMyAddFlowSheet::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyAddFlowSheet)
	DDX_Control(pDX, IDC_EDIT_BINDINGNAME, m_EditBindingName);
	DDX_Control(pDX, IDC_STATIC_BINDINGNR, m_StaticNr);
	DDX_Control(pDX, IDC_SPIN1, m_Spin1);
	DDX_Control(pDX, IDC_EDIT1_SPIN, m_EditSpin);
	DDX_Control(pDX, IDC_STATIC_BINDINGNAME, m_StaticName);
	DDX_Control(pDX, IDC_ADDFLOW_COMPONENT, m_AddFlow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyAddFlowSheet, CFormView)
	//{{AFX_MSG_MAP(CMyAddFlowSheet)
	ON_WM_SIZE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_EN_KILLFOCUS(IDC_EDIT_BINDINGNAME, OnKillfocusEditBindingname)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOW_ADDFLOW_FORM,OnShowAddflowForm)
	ON_MESSAGE(WM_ADDFLOW_ADDUNIT,OnAddFlowAddUnit)
	ON_MESSAGE(WM_ADDFLOW_ADDCONTROL,OnAddFlowAddControl)
	ON_MESSAGE(WM_ADDFLOW_DRAGGING_UNIT,OnAddFlowDraggingUnit)
	ON_MESSAGE(WM_ADDFLOW_DROP_UNIT	,OnAddFlowDropUnit)
	ON_MESSAGE(WM_ADDFLOW_REMOVE_CONTROL,OnAddFlowRemoveControl)
	ON_MESSAGE(WM_BINDINGENTRY_EDIT,OnBindingEntryEdit)
	ON_MESSAGE(WM_BINDINGENTRY_NEW,OnBindingEntryNew)
	ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
	ON_MESSAGE(WM_APPLICATION_CLEARVIEWS,OnApplicationClearViews)
	ON_MESSAGE(WM_NOTIFYPARENT_KEYENTER,OnNotifyParentKeyEnter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyAddFlowSheet diagnostics

#ifdef _DEBUG
void CMyAddFlowSheet::AssertValid() const
{
	CFormView::AssertValid();
}

void CMyAddFlowSheet::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyAddFlowSheet message handlers


LONG CMyAddFlowSheet::OnShowAddflowForm(WPARAM wParam,LPARAM lParam)
{	
	ShowWindow(TRUE);

	if (fControlsCreated == TRUE) 
	{
		
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
void CMyAddFlowSheet::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	InitAddFlowDiagram();	

	m_EditBindingName.SetReadOnly(FALSE);
}

LONG CMyAddFlowSheet::OnUnitStatusChanged(WPARAM wParam,LPARAM lParam)
{
	if (fControlsCreated == TRUE) {
		const BYTE bNodeAddress = (BYTE) (wParam & 0xFF);
		const BYTE bUnitAddress = (BYTE) (lParam & 0xFF);
		BindingGuiComponent.OnUnitStatusChanged(bNodeAddress,bUnitAddress);
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void CMyAddFlowSheet::InitAddFlowDiagram(void)
{
	if (fControlsCreated == FALSE) 
	{
		if (m_AddFlow.GetSafeHwnd() == 0)
		{		
			CNotifyError_Simple notify;
			notify("An unexpected error occured\n"
				   "Probably caused by wrong installation procedure.\n");

			exit(-1);
		}

		BindingGuiComponent.Init(&m_AddFlow,this);
		fControlsCreated = TRUE;
		fDropping = FALSE;	
		CreateNewBindingEntry();	
	}
}

void CMyAddFlowSheet::InitAddFlowDiagram2(void)
{
	if (fControlsCreated == TRUE) 
	{
		BindingGuiComponent.CreateBitmaps();
	}
}

#define BINDING_GROUP_YSIZE		20

void CMyAddFlowSheet::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect rcClient,rcAddFlow,rcGroup,rcControls;

	GetClientRect(&rcClient);
	rcAddFlow = rcClient;
	rcAddFlow.top += BINDING_GROUP_YSIZE;
	rcGroup = rcClient;
	rcGroup.bottom = BINDING_GROUP_YSIZE;
	rcControls = rcGroup;
	rcControls.right = rcClient.Width()/4;
	
	if (fControlsCreated == TRUE)
	{
		m_AddFlow.MoveWindow(rcAddFlow);		
		m_StaticName.MoveWindow(rcControls);
		rcControls.left += 100;
		rcControls.right += 100;
		m_EditBindingName.MoveWindow(rcControls);
		rcControls = rcGroup;
		rcGroup.left = (rcGroup.Width()/2) + (rcGroup.Width()/3);
		rcControls = rcGroup;
		rcControls.left += 15;
		m_EditSpin.MoveWindow(rcControls);
		rcControls = rcGroup;
		rcControls.right = rcGroup.left + 15;
		m_Spin1.MoveWindow(rcControls);
		rcControls = rcGroup;
		rcControls.left = rcClient.Width()/2;
		rcControls.right = rcClient.Width()/2 + rcClient.Width()/4;
		m_StaticNr.MoveWindow(rcControls);
	}	
}

///////////////////////////////////////////////////////////////////
void CMyAddFlowSheet::ShowProperties()
{	
}

void CMyAddFlowSheet::FreeProperties()
{
}


////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// EVENTS ////////////////////////////////////////////

BEGIN_EVENTSINK_MAP(CMyAddFlowSheet, CFormView)
    //{{AFX_EVENTSINK_MAP(CMyAddFlowSheet)
	ON_EVENT(CMyAddFlowSheet, IDC_ADDFLOW_COMPONENT, -605 /* MouseDown */, OnMouseDownAddflowComponent, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CMyAddFlowSheet, IDC_ADDFLOW_COMPONENT, -606 /* MouseMove */, OnMouseMoveAddflowComponent, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CMyAddFlowSheet, IDC_ADDFLOW_COMPONENT, -607 /* MouseUp */, OnMouseUpAddflowComponent, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CMyAddFlowSheet, IDC_ADDFLOW_COMPONENT, -601 /* DblClick */, OnDblClickAddflowComponent, VTS_NONE)
	ON_EVENT(CMyAddFlowSheet, IDC_ADDFLOW_COMPONENT, -602 /* KeyDown */, OnKeyDownAddflowComponent, VTS_PI2 VTS_I2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMyAddFlowSheet::OnMouseDownAddflowComponent(short Button, short Shift, long x, long y) 
{
	//float xPos,yPos;

	if (Button == LBUTTON_DOWN) 
	{
		BindingGuiComponent.SetNodeSelected(TRUE);
	}
	else if (Button == RBUTTON_DOWN) 
	{
		BindingGuiComponent.SetNodeSelected(FALSE);
		
		if (!BindingGuiComponent.SetLinkSelected(FALSE)) 
		{		
			BindingGuiComponent.PrepareToMove(TRUE,XPixelsToTwips(x),YPixelsToTwips(y));
		}
	}
}

void CMyAddFlowSheet::OnMouseMoveAddflowComponent(short Button, short Shift, long x, long y) 
{
	if ((Button == RBUTTON_DOWN) || (fDropping == TRUE))
	{	
		BindingGuiComponent.MoveSelectedNode(XPixelsToTwips(x),YPixelsToTwips(y));
	}
}

void CMyAddFlowSheet::OnMouseUpAddflowComponent(short Button, short Shift, long x, long y) 
{
	if ((Button == RBUTTON_DOWN) || (fDropping == TRUE)) 
	{
		BindingGuiComponent.PrepareToMove(FALSE);
	}
	else 
	{
		switch(m_AddFlow.GetLastUserAction())
		{
			case afNoUserAction :	/* 0 */										  
				BindingGuiComponent.ShowPropDlg(x,y);
			break;	

			case afNodeCreation : break;

			case afLinkCreation :			
				BindingGuiComponent.DestroyPropDlg();
				BindingGuiComponent.AddLink();
			break;
			
			case afNodeDragging: break;

			case afNodeResizing: break;

			case afLinkStretching: break;
		}
	}
}

void CMyAddFlowSheet::OnDblClickAddflowComponent() 
{	
	BindingGuiComponent.SetToggleLink();
}


LONG CMyAddFlowSheet::OnAddFlowDraggingUnit(WPARAM wParam,LPARAM lParam)
{	
	STRUCT_UNIT* const pUnit = (STRUCT_UNIT*) wParam;
	
	const int xpos = (int) (lParam & 0xFFFF0000) >> 16;
	const int ypos = (int) (lParam & 0x0000FFFF);

	BindingGuiComponent.DragControl(pUnit,xpos,ypos);
	
	return TRUE;
}

LONG CMyAddFlowSheet::OnAddFlowDropUnit(WPARAM wParam,LPARAM lParam)
{
	STRUCT_UNIT* const pUnit = (STRUCT_UNIT*) wParam;
	const int xpos = (int) (lParam & 0xFFFF0000) >> 16;
	const int ypos = (int) (lParam & 0x0000FFFF);
	
	BindingGuiComponent.DropControl();
	return TRUE;
}


LONG CMyAddFlowSheet::OnAddFlowAddUnit(WPARAM wParam,LPARAM lParam)
{			
	BindingGuiComponent.AddControl((STRUCT_UNIT*) lParam);
	return TRUE;
}

LONG CMyAddFlowSheet::OnAddFlowAddControl(WPARAM wParam,LPARAM lParam)
{	
	BindingGuiComponent.AddControl( (OperatorType) wParam);
	return TRUE;
}

LONG CMyAddFlowSheet::OnAddFlowRemoveControl(WPARAM wParam,LPARAM lParam)
{
	BindingGuiComponent.DeleteControl();
	return TRUE;
}



////////////////////////////////////////////////////////////////////

BOOL CMyAddFlowSheet::CreateNewBindingEntry(void)
{
	BindingView.Clear();
	BindingView.SetID(0);
	BindingGuiComponent.SetPtrToBindingEntry(&BindingView);
	BindingGuiComponent.GetLayoutManager()->ClearScreen();

	UpdateControls();
	return TRUE;
}


LONG CMyAddFlowSheet::OnBindingEntryEdit(WPARAM wParam,LPARAM lParam)
{
	if (TMGetBindingEntries() != NULL)
	{
		ASSERT(wParam);

		CreateNewBindingEntry();

		BindingView = *((CBindingEntry*) wParam);	

		BindingGuiComponent.SetPtrToBindingEntry(&BindingView);

		CBindingConversionToGui2 BindingConverterToGui(&BindingView);

		if (BindingConverterToGui.Process() == TRUE) 
		{	
			BindingGuiComponent.GetLayoutManager()->ClearScreen();
			BindingGuiComponent.GetLayoutManager()->UpdateScreen(TRUE);	// False,True				

			UpdateControls();
		}
		return TRUE;
	}

	return FALSE;
}


LONG CMyAddFlowSheet::OnBindingEntryNew(WPARAM wParam,LPARAM lParam)
{	
	CreateNewBindingEntry();	

	return TRUE;
}


void CMyAddFlowSheet::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ASSERT(m_pStringEntries);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;	
	const int iStep = ((pNMUpDown->iDelta)*(-1));

	CBindingFlagControl_Helper Helper(m_pStringEntries);
	BYTE Flags = BindingView.GetBindingReturnFlags();

	Helper.Adjust(Flags,((pNMUpDown->iDelta)*(-1)));

	BindingView.SetBindingReturnFlags(Flags);

	*pResult = 0;
	UpdateControls();
}

void CMyAddFlowSheet::UpdateControls(void)
{
	CBindingStringEntries_Col stringCol;
	m_pStringEntries = stringCol.Get(&BindingView,CBindingStringEntries_Col::BINDINGFLAGS); 

	CBindingFlagControl_Helper Helper(m_pStringEntries);
	Helper.Update(m_EditSpin,BindingView.GetBindingReturnFlags());

	m_EditBindingName.SetWindowText(BindingView.GetName());
	
	if (BindingView.GetBindingNr() == 0x0000) 
	{
		m_StaticNr.ShowWindow(FALSE);	
	}
	else 
	{
		CString szText;
		m_StaticNr.ShowWindow(TRUE);
		szText.Format("Binding nr:0x%04x",BindingView.GetBindingNr());
		m_StaticNr.SetWindowText(szText);
	}
}

void CMyAddFlowSheet::OnKillfocusEditBindingname() 
{	

}

LONG CMyAddFlowSheet::OnApplicationClearViews(WPARAM wParam,LPARAM lParam)
{
	CreateNewBindingEntry();
	return TRUE;
}

void CMyAddFlowSheet::OnKeyDownAddflowComponent(short FAR* KeyCode, short Shift) 
{
	if (*KeyCode == VK_DELETE) 
	{			
		BindingGuiComponent.DeleteLink();
		BindingGuiComponent.DeleteControl();
	}
}

LONG CMyAddFlowSheet::OnNotifyParentKeyEnter(WPARAM wParam, LPARAM lParam)
{
	CMainFrame *const pMainWnd = ((CMainFrame*) AfxGetMainWnd());
	pMainWnd->SendMessage(WM_COMMAND,ID_BUTTON_ADD);
	return TRUE;
}