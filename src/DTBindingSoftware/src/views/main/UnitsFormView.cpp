// UnitsFormView.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "UnitsFormView.h"

#include "CanNode.h"
#include "MyWindowMessages.h"

#include "app\appnodedbase\appnodedbase.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
using namespace TMNodeDatabase;
// ------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitsFormView

IMPLEMENT_DYNCREATE(CUnitsFormView, CFormView)

CUnitsFormView::CUnitsFormView()
	: CFormView(CUnitsFormView::IDD)
{
	//{{AFX_DATA_INIT(CUnitsFormView)
	//}}AFX_DATA_INIT

	fControlsCreated = FALSE;
	pListView = NULL;
	NodeList.Init();	
}

CUnitsFormView::~CUnitsFormView()
{
	if (pListView != NULL)	
	{
		delete pListView;
	}

}

void CUnitsFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnitsFormView)
	DDX_Control(pDX, IDC_STATIC_STATS, m_StaticCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnitsFormView, CFormView)
	//{{AFX_MSG_MAP(CUnitsFormView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOW_UNITS_FORM,OnShowUnitsForm)
	ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
	ON_MESSAGE(WM_UNITNAME_CHANGED,OnUnitNameChanged)
	ON_MESSAGE(WM_TREE_NODE_COLLAPSED,OnTreeNodeCollapsed)
	ON_MESSAGE(WM_TREE_NODE_EXPANDED,OnTreeNodeExpanded)
	ON_MESSAGE(WM_APPLICATION_CLEARVIEWS,OnApplicationClearViews)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitsFormView diagnostics

#ifdef _DEBUG
void CUnitsFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CUnitsFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUnitsFormView message handlers


void CUnitsFormView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
}

BOOL CUnitsFormView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	static BOOL fEditTextEnable = TRUE;

	const LV_DISPINFO* lv_dispinfo = (LV_DISPINFO*) lParam;
	const LV_ITEM* plvItem = &lv_dispinfo->item;
			
	if (lv_dispinfo->hdr.code == LVN_ENDLABELEDIT ) 
	{
		if (plvItem->pszText != NULL) 
		{
			if (strcmp(plvItem->pszText,"") != 0) 
			{
				CString szMessage(plvItem->pszText);
			
				if (szMessage.GetLength() > 16) 
				{
					szMessage.SetAt(16,'\0');			
				}
				
				const unsigned long ulIdentifier = plvItem->lParam;

				AfxGetMainWnd()->SendMessage(WM_UNITNAME_CHANGED,
										  (LPARAM) (ulIdentifier & 0x0000FFFF),
										  (WPARAM) ((LPCTSTR)szMessage));			// CHANGES_MVS_2008_CSTRING
								  								
				pListView->SetItemText(plvItem->iItem, plvItem->iSubItem, szMessage);
			}
		}
		return TRUE;
	}	
	
	return CFormView::OnNotify(wParam, lParam, pResult);
}

void CUnitsFormView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	
	this->ShowScrollBar(SB_BOTH,FALSE);

	if (fControlsCreated == TRUE)
	{
		rect.bottom -= 50;
		pListView->MoveWindow(rect);
		
		rect.top = rect.bottom + 5;
		rect.bottom = rect.bottom + 50;
		rect.left += 10;

		m_StaticCtrl.MoveWindow(rect);	
	}
}

void CUnitsFormView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	
}

void CUnitsFormView::InitControls()
{
	if (fControlsCreated != TRUE)
	{
		// CREATE ALL CONTROLS !!!!
		if ((pListView = new CMyNodeUnitListCtrl(CMyNodeUnitListCtrl::LISTCTRL_STATE_VIEW_UNITS)) != NULL)
		{

			CRect rect,btnRect;
			GetClientRect(&rect);
			rect.bottom -= 50;

			pListView->Create(WS_VISIBLE | 
							  WS_CHILD | 					
							  LVS_ALIGNTOP |   
							  LVS_ICON |					
							  LVS_SINGLESEL |
							  LVS_SHAREIMAGELISTS | 
							  LVS_EDITLABELS ,					 
							  rect,
							  this,
							  IDC_LISTVIEW_UNITS);

		
			CImageList* const pImageList = TMGetGraphics()->GetLargeNodeUnitImageList();

			if (pImageList != NULL)
			{
				pListView->SetImageList(pImageList,LVSIL_NORMAL);
			}			


			rect.top = rect.bottom + 5;
			rect.bottom = rect.bottom + 50;
			rect.left += 10;
			m_StaticCtrl.MoveWindow(rect);	

			fControlsCreated = TRUE;
		}		
	}
}


void CUnitsFormView::CreateUnitListView(BYTE bLogicalAddress)
{	
	if (fControlsCreated == TRUE)
	{
		pListView->DeleteAllItems();
		
		int iEntryNr;
		CCanNode *m_pNode;
			
		if (TMGetDocumentNodes()->FindNode(bLogicalAddress,&iEntryNr,&m_pNode))
		{
			AddUnitsToListView(m_pNode);
		}		
	}
}

void CUnitsFormView::AddUnitsToListView(CCanNode *pNode)
{		
	for (int i=0;i<pNode->iNROfUnits;i++)
	{
		STRUCT_UNIT *pUnit;
		if (TMGetDocumentNodes()->GetUnit(pNode,i,&pUnit))
		{				
			CString szName(pUnit->szUnitName);
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE | LVIF_IMAGE; 
			lvItem.state = 0;
			lvItem.stateMask = 0;		
			lvItem.iItem = i;
			lvItem.iSubItem = 0;
			lvItem.pszText = (LPTSTR)((LPCTSTR)szName);		// CHANGES_MVS_2008_CSTRING
			lvItem.lParam = pListView->SetParamData(CMyNodeUnitListCtrl::TYPE_LIST_UNITS,
													CMyNodeUnitListCtrl::TYPE_LIST_UNIT_CTRL,
													pUnit->bNodeLogicalAddress,
													pUnit->bUnitAddress);
		



			CApplicationGuiUnit::AppUiIconsExtraInfo_t appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_NONE;
			if ((NULL != pUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pUnit->m_pAppUnitData->getUnitDataType())) {				
				appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_DALI;
			}

			UnitIconType iImage;
			TMGetGuiUnit()->GetIcon(pUnit->bNodeLogicalAddress, pUnit->bUnitAddress, appUiExtraInfo, &iImage);

			lvItem.iImage = iImage;	
			pListView->InsertItem(&lvItem);	
		}
		else {
			CString szDebug("Debug Fatal error : Can't retrieve Unit from Database");
			AfxMessageBox(szDebug,MB_ICONSTOP);				
			break; // break for loop ...
		}
	}
}

/////////////////

LONG CUnitsFormView::OnUnitStatusChanged(WPARAM wParam,LPARAM lParam)
{
	if (fControlsCreated == TRUE) {		
		int iEntryNr;			
		const BYTE bNodeLogicalAddress = (BYTE) (wParam & 0xFF);
		const BYTE bUnitNr = (BYTE) (lParam & 0xFF);
		if (GetUnitItem(bNodeLogicalAddress,bUnitNr,&iEntryNr))
		{		

			// Opvragen van de info in de node database...
			CApplicationGuiUnit::AppUiIconsExtraInfo_t appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_NONE;
			CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());
			STRUCT_UNIT *pUnit = NULL;
			if (NodeDatabaseOperations.Find(bNodeLogicalAddress,bUnitNr,&pUnit) == TRUE) {
				ASSERT(NULL != pUnit);
				if ((NULL != pUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pUnit->m_pAppUnitData->getUnitDataType())) {				
					appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_DALI;
				}
			}

			UnitIconType iImage;
			TMGetGuiUnit()->GetIcon(bNodeLogicalAddress, bUnitNr, appUiExtraInfo, &iImage);	

			if ((pListView->SetItem(iEntryNr,
									0,
									LVIF_IMAGE,NULL,
									iImage,
									0,
									0,
									0)) == FALSE)
			{
				AfxMessageBox("Debug Fatal error : Unable to set Image",MB_ICONSTOP);
				return FALSE;
			}			
		}

		return TRUE;
	}

	return FALSE;
}

LONG CUnitsFormView::OnShowUnitsForm(WPARAM wParam,LPARAM lParam)
{
	ShowWindow(TRUE);
	InitControls();

	const BYTE bNodeAddress = BYTE (wParam & 0xFF);
	
	if (bNodeAddress == 0xFF)
	{
		DisplayUnits();
	}
	else
	{
		CreateUnitListView(bNodeAddress);
	}

	return TRUE;
}

LONG CUnitsFormView::OnUnitNameChanged(WPARAM wParam,LPARAM lParam)
{	
	if (fControlsCreated ) 
	{	
		const BYTE bLogicalAddress = (BYTE) ((wParam & 0xFF00) >> 8);
		const BYTE bUnitAddress = (BYTE) (wParam & 0xFF);
		int iEntryNr;	

		if (GetUnitItem(bLogicalAddress,bUnitAddress,&iEntryNr))
		{
			CString szUnitName(reinterpret_cast<const char*>(lParam));							
			if (pListView->SetItemText(iEntryNr,0,(LPCTSTR) szUnitName) == FALSE)		// CHANGES_MVS_2008_CSTRING
			{
				AfxMessageBox("ERROR - Failed to edit name , UnitListiew",MB_ICONSTOP);	
				ASSERT(0);
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CUnitsFormView::GetUnitItem(BYTE bLogicalAddress,BYTE bUnitAddress,int *iItem)
{
	const unsigned long ulIdentifier = pListView->SetParamData(CMyNodeUnitListCtrl::TYPE_LIST_UNITS,
															   CMyNodeUnitListCtrl::TYPE_LIST_UNIT_CTRL,
															   bLogicalAddress,
															   bUnitAddress);
	const int nCount = pListView->GetItemCount();	
	for (int i=0;i<nCount;i++) {
		if (ulIdentifier == pListView->GetItemData(i)) {
			*iItem = i;
			return TRUE;		
		}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////
LONG CUnitsFormView::OnTreeNodeCollapsed(WPARAM wParam,LPARAM)
{
	const BYTE bNodeAddress = (BYTE) wParam;	

	NodeList.RemoveFromList(bNodeAddress);
	DisplayUnits();

	return TRUE;
}

LONG CUnitsFormView::OnTreeNodeExpanded(WPARAM wParam,LPARAM)
{
	const BYTE bNodeAddress = (BYTE) wParam;

	if (NodeList.Count() < NODELIST_SIZE) 
	{
		NodeList.AddToList(bNodeAddress);
		DisplayUnits();
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
void CUnitsFormView::DisplayUnits(void)
{
	if (fControlsCreated == TRUE)
	{		
		BYTE bNodeAddress;

		if (pListView->GetItemCount())
		{
			pListView->DeleteAllItems();
		}


		for (BYTE i=0;i<NodeList.Count();i++)
		{
			if (NodeList.Get(i,&bNodeAddress) == FALSE) 
			{
				break;
			}

			ShowNodeInUnitListView(bNodeAddress);
		}
	
		DisplayStats();
	}
}

BOOL CUnitsFormView::ShowNodeInUnitListView(BYTE bLogicalAddress)
{
	if (fControlsCreated == TRUE)
	{	
		int iEntryNr;
		CCanNode *m_pNode;	
	
		if (TMGetDocumentNodes()->FindNode(bLogicalAddress, &iEntryNr, &m_pNode))
		{
			AddUnitsToListView(m_pNode);
			return TRUE;
		}			
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

DisplayNodesList::DisplayNodesList()
{
}

void DisplayNodesList::Init()
{
	for (int i=0;i<NODELIST_SIZE;i++)
	{
		NodeList[i] = 0xFF;
	}
	
	bCount = 0;
	bIndex = 0;
}

BOOL DisplayNodesList::AddToList(BYTE bNodeAddress)
{
	BYTE x;

	if (bCount <= NODELIST_SIZE)
	{	
		for (x=0;x<bCount;x++)
		{
			if(NodeList[x] == bNodeAddress) 
				break;
		}

		if (x == bCount)
		{								
			bIndex = bCount;

			bCount ++;
		
			NodeList[bIndex] = bNodeAddress;					

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL DisplayNodesList::RemoveFromList(BYTE bNodeAddress)
{
	for (BYTE x=0; x<bCount; x++)
	{
		if (NodeList[x] == bNodeAddress)
		{
			NodeList[x] = 0xFF;		
			FragmentList();
		}
	}
	return FALSE;
}

BYTE DisplayNodesList::Count()
{
	return (bCount);	
}


BOOL DisplayNodesList::Get(BYTE bItemIndex,BYTE *bAddress)
{
	if (bItemIndex < bCount)
	{
		if (NodeList[bItemIndex] != 0xFF)
		{
			*bAddress = NodeList[bItemIndex];
			return TRUE;
		}
		else
		{
			return FALSE;
		}

	}
	return FALSE;
}

void DisplayNodesList::FragmentList()
{
	BYTE x,y;
	
	BYTE Cpy[NODELIST_SIZE];


	for (x=0;x<NODELIST_SIZE;x++)
	{
		Cpy[x] = NodeList[x];
		NodeList[x] = 0xFF;
	}



	for (x=0,y=0;x<NODELIST_SIZE;x++)
	{	
		if (Cpy[x] != 0xFF)
		{
			NodeList[y] = Cpy[x];
			y++;
		}	
	}

	//if (y != bCount) ;//ASSERT(0);

	bCount = y;
}

///////////////////////////////////////////////////////////////////////////
void CUnitsFormView::DisplayStats()
{
	CString szStatic;	

	if (pListView != NULL) 
	{
		szStatic.Format("%d of max. 5 nodes are shown ...\n%d units are shown ...",
					    NodeList.Count(),
						pListView->GetItemCount());
	}
		
	m_StaticCtrl.SetWindowText(szStatic);
}

LONG CUnitsFormView::OnApplicationClearViews(WPARAM ,LPARAM )
{
	NodeList.Init();

	if (!fControlsCreated ) return TRUE;
	
	CString szStatic;
	ASSERT(pListView);

	szStatic.Empty();
	
	m_StaticCtrl.SetWindowText(szStatic);
	pListView->DeleteAllItems();
	return TRUE;
}