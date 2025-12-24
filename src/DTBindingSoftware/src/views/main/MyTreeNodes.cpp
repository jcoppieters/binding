// MyTreeNodes.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyTreeNodes.h"

#include "MainFrm.h"
#include "BindingSoftwareDoc.h"
#include "TMFCControlsHelper.h"
#include "MyDefinitions.h"							// Bevat window messages / timers + enums.
#include "nodeScreenmapInfo.h"		
#include "app\appCfg\AppCfginfo\cfginfonode.h"
#include "app/appudpcfgserver/appudpservercfg.h"	// Sedert V16.27: UDP Lan Configuratie.


// ------------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
using namespace TMNodeDatabase;
using namespace TMBindingDatabase;
using namespace TMApplicationViews;
using namespace TMCommunication;
// ------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
#define DRAWING_DISABLE_CTRL		TShowWindow<CWnd> RTTI(this)		
////////////////////////////////////////////////////////////////////////////
// CMyTreeNodes
//////////////////////////////////////////////////////////////////////////////

namespace {

	// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
	// Sedert V17.03: Zie BUG-0178 - SendMessage() ipv PostMessage()
	static void _showNodeProperties(CMyTreeNodes& parent, BYTE bNodeLogicalAddress, int activePage) {
		if (0xff == bNodeLogicalAddress) {
			return;
		}					
		//AfxGetMainWnd()->PostMessage(WM_SHOW_NODE_PROPERTIES,(WPARAM)(bNodeLogicalAddress),(LPARAM)(activePage));			
		AfxGetMainWnd()->SendMessage(WM_SHOW_NODE_PROPERTIES, (WPARAM)(bNodeLogicalAddress), (LPARAM)(activePage));
	}

};

//////////////////////////////////////////////////////////////////////////////
CDraggingItem::CDraggingItem() : 
	m_state(STATE_IDLE), m_pImageList(NULL), m_point() { }

CDraggingItem::~CDraggingItem() {
	destroyImageList();
}

void CDraggingItem::setIdle( ) {
	destroyImageList();
	m_point = CPoint();
	m_state = STATE_IDLE;
}

void CDraggingItem::setWait(CPoint& pos) {
	m_point = pos;
	m_state = STATE_WAIT;
}

void CDraggingItem::setBusy(CImageList* pImageList) {
	if ((NULL != pImageList) && (m_pImageList != pImageList)) {
		ASSERT(NULL == m_pImageList);	
		destroyImageList();
	}	
	m_pImageList = pImageList;
	m_state = STATE_BUSY;
}

void CDraggingItem::destroyImageList() {
	if (NULL != m_pImageList) {
		delete m_pImageList;
		m_pImageList=NULL;
	}
}

CImageList* CDraggingItem::getImageList( ) {
	ASSERT(NULL != m_pImageList);
	return m_pImageList;
}

//////////////////////////////////////////////////////////////////////////////
CMyTreeNodes::CMyTreeNodes() : m_draggingItem(), m_hCurrentItemSelected(NULL), m_popupSelectedNodeAddress(0xff),
	m_popupSelectedItemDataValue(0) {
	pTreefont = NULL;
}

CMyTreeNodes::~CMyTreeNodes() {
	if (pTreefont != NULL) {
		delete 	pTreefont;
	}
}
///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMyTreeNodes, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTreeNodes)
	ON_WM_CREATE()
	// ON_WM_RBUTTONDBLCLK()
	ON_WM_KEYUP()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)	
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	
	ON_MESSAGE(WM_NODENAME_CHANGED,OnNodeNameChanged)
	ON_MESSAGE(WM_UNITNAME_CHANGED,OnUnitNameChanged)
	ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
	ON_MESSAGE(WM_BINDINGDATABASE_ADDENTRY,OnBindingDatabaseAddEntry)
	ON_MESSAGE(WM_BINDINGDATABASE_REMOVEENTRY,OnBindingDatabaseRemoveEntry)
	ON_MESSAGE(WM_BINDINGDATABASE_NAMECHANGED,OnBindingDatabaseNameChanged)
	ON_MESSAGE(WM_BINDINGDATABASE_UPDATE,OnBindingdatabaseUpdate)
	ON_MESSAGE(WM_APPLICATION_CLEARVIEWS,OnApplicationClearViews)
	ON_MESSAGE(WM_APPLICATION_UPDATEVIEWS,OnApplicationUpdateViews)
	ON_MESSAGE(WM_TREEVIEW_SORT_NODESBYNAME,OnTreeViewSortNodesByName)
	ON_MESSAGE(WM_TREEVIEW_SORT_NODESBYPHADDRESS,OnViewSortnodesbyphaddress)
	ON_MESSAGE(WM_TREEVIEW_SORT_NODESBYADDRESS,OnViewSortnodesbylogicaladdress)
	ON_MESSAGE(WM_TREEVIEW_SORT_NODES,OnButtonSortNodes)
	ON_MESSAGE(WM_TREEVIEW_SORT_NODESONLYNAME,OnViewSortNodesonlyname)
	ON_MESSAGE(WM_TREEVIEW_COLLAPSE,OnButtonCollapse)
	ON_MESSAGE(WM_TREEVIEW_EXPAND,OnButtonExpand)
	ON_MESSAGE(WM_TREEVIEW_SORT_UNITS,OnButtonSortUnits)
	ON_MESSAGE(WM_TREEVIEW_UNIT_BYNAME,OnViewUnitsbyname)
	ON_MESSAGE(WM_TREEVIEW_UNIT_BYADDRESS,OnViewUnitsbyaddress)
	ON_MESSAGE(WM_TREEVIEW_UNIT_ONLYNAME,OnViewUnitsonlyname)
	ON_COMMAND(ID_TREEUNITS_RENAMEUNIT, &CMyTreeNodes::OnContextmenuUnitRename)
	ON_COMMAND(ID_TREEUNITS_SHOWUNITPROPERTIES, &CMyTreeNodes::OnContextmenuShowUnitProperties)
	ON_COMMAND(ID_CONTEXTMENUNODE_RENAME, &CMyTreeNodes::OnContextmenuNodeRename)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWPROPERTIES, &CMyTreeNodes::OnContextmenuNodeShowProperties)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWCONTROL, &CMyTreeNodes::OnContextmenuNodeShowNodeControl)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWSCREENMAPPING, &CMyTreeNodes::OnContextmenuNodeShowNodeSmap)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWCONFIGURATION, &CMyTreeNodes::OnContextmenuNodeShowNodeConfig)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWMASTERTASKS, &CMyTreeNodes::OnContextmenuNodeShowNodeMasterTasks)	
	ON_COMMAND(ID_TREENODEBINDINGS_VIEWBINDING, &CMyTreeNodes::OnContextmenuBindingsShow)
	ON_COMMAND(ID_TREENODEBINDINGS_RENAMEBINDING, &CMyTreeNodes::OnContextmenuBindingsRename)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWLANCFG, &CMyTreeNodes::OnContextmenuShowLanCfg)	
	ON_COMMAND(ID_CONTEXTMENUNODE_BROWSE, &CMyTreeNodes::OnContextmenuNodeBrowse)
	ON_NOTIFY_REFLECT(TVN_GETINFOTIP, OnMyTreeGetInfoTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
LONG CMyTreeNodes::OnApplicationClearViews(WPARAM wParam,LPARAM lParam)
{
	DeleteAllItems();
	return TRUE;
}

// sedert V16.49:
// Mogelijkheid om de boomstructuur up te daten voor een afzonderlijke node.
// Het node address wordt meegeveven aan WPARAM.
// Standaard is deze 0xFFFFFFFF (=-1).
LONG CMyTreeNodes::OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam) {
	BOOL retValue = TRUE;
	if (wParam >= FIRST_VALID_LOGICAL_ADDRESS && wParam <= NODE_LCD_MASTER_ADDRESS) {
		this->updateSingleNode((BYTE) wParam);
	}
	else {
		retValue = this->Update();
	}
	return retValue;
}


//////////////////////////////////////////////////////////////////////////////
// Drag & Drop 
// To be sure all values are positive !
void CalculateCoordinates(CPoint point,unsigned long *ulPos)
{
	if (point.x < 0) point.x = 0;
	if (point.y < 0) point.y = 0;
	*ulPos = (point.x & 0x1FFF) << 16;
	*ulPos |= (point.y & 0x1FFF);
}

int CMyTreeNodes::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;


	// Vragen om een tooltip weer te geven.
	this->ModifyStyle(0, TVS_INFOTIP);


	CImageList* const pImageList = TMGetGraphics()->GetSmallNodeUnitImageList();
	if (pImageList != NULL) {
		SetImageList(pImageList,TVSIL_NORMAL);		
	}	

	SetTextColor(RGB(0,0,255));

	NodesViewMode = NODES_VIEWMODE_PHADDRESS;
	UnitsViewMode = UNITS_VIEWMODE_ONLYNAME;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CMyTreeNodes::UpdateUnitNames(void)
{	
	int iNROfNodes = 0;
	
	if (TMGetDocumentNodes()->NROfNodes(&iNROfNodes) == FALSE) {
		AfxMessageBox("ERROR !!",MB_ICONSTOP);
		return FALSE;
	}
	
	if (iNROfNodes != 0) 
	{		
		for (int i=0;i<iNROfNodes;i++) 
		{
			CCanNode *m_pNode;
			if (TMGetDocumentNodes()->GetNode(i, &m_pNode))
			{
				const HTREEITEM hParentNode = GetNode(m_pNode->bLogicalAddress);

				const int iNROfUnits = m_pNode->bNROfUnits;

				for (int j=0;j<iNROfUnits;j++) 
				{
					STRUCT_UNIT *m_pUnit;
					if (TMGetDocumentNodes()->GetUnit(m_pNode,j,&m_pUnit))
					{								
						const HTREEITEM hItem = GetUnit(m_pNode->bLogicalAddress,
												        m_pUnit->bUnitAddress,
														hParentNode);

						if (hItem != NULL) 
						{
							DisplayUnitName(hItem, m_pUnit);
						}
						else 
						{
							ASSERT(0);
							return FALSE;
						}
					}
					else
					{
						ASSERT(0);
					}
				} // end for (int j=0;j<iNROfUnits;j++) 
			}
			else
			{
				ASSERT(0);
			}
		}
	}	
	return TRUE;
}

BOOL CMyTreeNodes::UpdateNodeNames()
{		
	int iNROfNodes = 0;
	if (TMGetDocumentNodes()->NROfNodes(&iNROfNodes) == FALSE) {
		AfxMessageBox("ERROR !!",MB_ICONSTOP);
		return FALSE;
	}		
	if (iNROfNodes != 0) {
		for (int i=0;i<iNROfNodes;i++) {
			CCanNode *m_pNode;
			if (TMGetDocumentNodes()->GetNode(i, &m_pNode)) {
				const HTREEITEM ParentNode = GetNode(m_pNode->bLogicalAddress);
				DisplayNodeName(ParentNode,m_pNode);
			}
		}		
		return TRUE;
	}
	return FALSE;
}

// TM 20081223: Gebruik van iterators.
BOOL CMyTreeNodes::Update()
{
	m_draggingItem.setIdle();

/*
	pDragImageList = NULL;
	fDragging = FALSE;	
*/	

	CWaitCursor wait;			// Sedert V16.46

	if (TMGetBindingEntries() != NULL) 
	{
		DeleteAllItems();
			
		if (1) {
			CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
			for (it.Begin();!it.IsDone();it.Next()) {
				CCanNode* const pNode = it.CurrentItem( );				
				HTREEITEM ParentNode;
				AddNode(TVI_ROOT,&ParentNode,pNode);
				CUnitIterator it2(TMGetDocumentNodes()->CreateUnitIterator(pNode));
				for (it2.Begin();!it2.IsDone();it2.Next()) {
					HTREEITEM ParentUnit;
					AddUnit(ParentNode,&ParentUnit,it2.CurrentItem());	
				}				
			}			
		}
										
		// TM 20081222: Gebruik van de CBindingEntries Iterator.
		CBindingEntries::Iterator it(TMGetBindingEntries()->CreateIterator());
		for (it.Begin();!it.IsDone();it.Next())
		{
			CBindingEntry* const pEntry = it.CurrentItem( );
			ASSERT( pEntry );

			CBindingUnits::Iterator it2(pEntry->GetColUnit()->CreateIterator());
			for (it2.Begin();!it2.IsDone();it2.Next())
			{
				CBindingUnit* const pUnit = it2.CurrentItem();
				ASSERT( pUnit );

				if (AddBindingEntry(pUnit->GetNodeAddress(),
								    pUnit->GetUnitAddress(),
									pEntry) == NULL) 
				{
					AfxMessageBox("BindingEntry could not be added to nodeTree");
					return FALSE;
				}
			}			
		}

		SortChildren(TVI_ROOT);		// CAN I SORT HERE ?
		return TRUE;
	}
	return FALSE;
}

void CMyTreeNodes::AddNode(HTREEITEM parent,HTREEITEM *pNewParent,CCanNode *pNode)
{
	TVITEM tvItem;
	UnitIconType iImage;

	CString szName(pNode->szNodeName);

	tvItem.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvItem.pszText = LPTSTR((LPCTSTR) szName);		// CHANGES_MVS_2008_CSTRING
	tvItem.cchTextMax = szName.GetLength();
	
	TMGetGuiUnit()->GetIcon(pNode->bLogicalAddress,&iImage);
	tvItem.iImage = tvItem.iSelectedImage = iImage;

	const unsigned long ulIdentifier =
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_NODES) << 24)) +
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_NODE_CTRL) << 16)) +
				   ( (unsigned long) ( (BYTE) pNode->bLogicalAddress) << 8) +
				   ( (unsigned long) ( 0xFF));

	tvItem.lParam = ulIdentifier;

	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = parent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item= tvItem;

	*pNewParent = InsertItem(&tvInsert);

	DisplayNodeName(*pNewParent,pNode);
}
void CMyTreeNodes::AddUnit(HTREEITEM parent,HTREEITEM *pNewParent,STRUCT_UNIT* pDocUnit)
{
	TVITEM tvItem;	
	UnitIconType iImage;	

	CString szName(pDocUnit->szUnitName);

	tvItem.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;	
	tvItem.pszText = LPTSTR((LPCTSTR) szName);	// CHANGES_MVS_2008_CSTRING
	tvItem.cchTextMax = szName.GetLength();

	CApplicationGuiUnit::AppUiIconsExtraInfo_t appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_NONE;
	if ((NULL != pDocUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pDocUnit->m_pAppUnitData->getUnitDataType())) {				
		appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_DALI;
	}

	TMGetGuiUnit()->GetIcon(pDocUnit->bNodeLogicalAddress, pDocUnit->bUnitAddress, appUiExtraInfo, &iImage);

	tvItem.iImage = tvItem.iSelectedImage = iImage;
	
	const unsigned long ulIdentifier = 
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_NODES) << 24)) +
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_UNIT_CTRL) << 16)) +
				   ( (unsigned long) ( (BYTE) pDocUnit->bNodeLogicalAddress) << 8) +
				   ( (unsigned long) ( pDocUnit->bUnitAddress));

	tvItem.lParam = ulIdentifier;

	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = parent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item= tvItem;	
	*pNewParent = InsertItem(&tvInsert);
	DisplayUnitName(*pNewParent,pDocUnit);
}


BOOL CMyTreeNodes::SetBindingEntryIcon(HTREEITEM parent,HTREEITEM hCurrent,CBindingEntry *pEntry)
{
	STRUCT_UNIT* const pUnit = GetUnit(parent);
	if (pUnit == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}

	BYTE bEvent = 0xFF;
	CBindingUnits::Iterator it(pEntry->GetColUnit()->CreateIterator());
	for (it.Begin(); !it.IsDone(); it.Next())
	{
		CBindingUnit* const pBindingUnit = it.CurrentItem();
		ASSERT( pBindingUnit );

		if (pBindingUnit->GetNodeAddress() == pUnit->bNodeLogicalAddress &&
			pBindingUnit->GetUnitAddress() == pUnit->bUnitAddress)
		{
			bEvent = pBindingUnit->GetEvent();

			// Is dit een bug en moet de switch case niet hier geplaatst worden ??
		}
	}
	switch(bEvent) 
	{
		case 0x01: 
			if (pEntry->IsSend() == TRUE) 
				SetItemImage(hCurrent,ICON_BINDINGS_L_SEND,ICON_BINDINGS_L_SEND);
			else 
				SetItemImage(hCurrent,ICON_BINDINGS_L_NOTSEND,ICON_BINDINGS_L_NOTSEND);
			
		break; 
		case 0x02: 
			if (pEntry->IsSend() == TRUE) 
				SetItemImage(hCurrent,ICON_BINDINGS_K_SEND,ICON_BINDINGS_K_SEND);
			else 
				SetItemImage(hCurrent,ICON_BINDINGS_K_NOTSEND,ICON_BINDINGS_K_NOTSEND);
		break; 
		case 0x03: 
			if (pEntry->IsSend() == TRUE) 
				SetItemImage(hCurrent,ICON_BINDINGS_P_SEND,ICON_BINDINGS_P_SEND);
			else 
				SetItemImage(hCurrent,ICON_BINDINGS_P_NOTSEND,ICON_BINDINGS_P_NOTSEND);			
		break; 

		default: 		
		{
			switch(pEntry->GetBindingReturnFlags())
			{
				case BINDINGFLAGS_STATUS_PRE:
				case BINDINGFLAGS_STATUS_PREPULS:
				{
					if (pEntry->IsSend() == TRUE) 
						SetItemImage(hCurrent,ICON_BINDINGS_PRE_SEND,ICON_BINDINGS_PRE_SEND);
					else
						SetItemImage(hCurrent,ICON_BINDINGS_PRE_NOTSEND,ICON_BINDINGS_PRE_NOTSEND);				
				} break;

				case BINDINGFLAGS_STATUS_POST:
				case BINDINGFLAGS_STATUS_POSTPULS:
				{
					if (pEntry->IsSend() == TRUE) 
						SetItemImage(hCurrent,ICON_BINDINGS_POST_SEND,ICON_BINDINGS_POST_SEND);
					else
						SetItemImage(hCurrent,ICON_BINDINGS_POST_NOTSEND,ICON_BINDINGS_POST_NOTSEND);
				} break;
				
				case BINDINGFLAGS_STATUS_NOCHANGE:
				default:
				{
					if (pEntry->IsSend() == TRUE) 
						SetItemImage(hCurrent,ICON_BINDINGS_NC_SEND,ICON_BINDINGS_NC_SEND);
					else
						SetItemImage(hCurrent,ICON_BINDINGS_NC_NOTSEND,ICON_BINDINGS_NC_NOTSEND);
				} break;			
			}
		} break;
	}

	return TRUE;
}

void CMyTreeNodes::AddBindingEntry(HTREEITEM parent,HTREEITEM *pNewParent,CBindingEntry *pEntry)
{
	ASSERT(pEntry);
	TVITEM tvItem;
	
	CString& szName = pEntry->GetName();

	tvItem.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvItem.pszText = LPTSTR((LPCTSTR) szName);		// CHANGES_MVS_2008_CSTRING
	tvItem.cchTextMax = szName.GetLength();	
	tvItem.iImage = tvItem.iSelectedImage = ICON_BINDINGS_NC_NOTSEND;

	unsigned long ulIdentifier;
	ulIdentifier = ( (unsigned long) ( ((BYTE) TYPE_TREE_NODES) << 24)) +
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_BINDING_CTRL) << 16));

	ulIdentifier |= (pEntry->GetID() & 0xFFFF);
	tvItem.lParam = ulIdentifier;

	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = parent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item= tvItem;
	
	*pNewParent = InsertItem(&tvInsert);

	if(!SetBindingEntryIcon(parent,*pNewParent,pEntry)) ASSERT(0);

}
///////////////////////////////////////////////////////////////////////////////////
HTREEITEM CMyTreeNodes::GetNode(BYTE bLogicalAddress)
{	
	const unsigned long ulIdentifier =
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_NODES) << 24)) +
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_NODE_CTRL) << 16)) +
				   ( (unsigned long) ( (BYTE) bLogicalAddress) << 8) +
				   ( (unsigned long) ( 0xFF));

	HTREEITEM hCurrent = GetNextItem(TVI_ROOT, TVGN_ROOT);

	while (hCurrent != NULL) 
	{		
		const unsigned long ulItemData = GetItemData(hCurrent);					
		if (ulItemData == ulIdentifier) 
			break;			

		hCurrent = GetNextItem(hCurrent, TVGN_NEXT);
	}
	return hCurrent;
}

HTREEITEM CMyTreeNodes::GetUnit(BYTE bNodeAddress, BYTE bUnitAddress,HTREEITEM parent)
{
	const unsigned long ulIdentifier = 
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_NODES) << 24)) +
				   ( (unsigned long) ( ((BYTE) TYPE_TREE_UNIT_CTRL) << 16)) +
				   ( (unsigned long) ( (BYTE) bNodeAddress) << 8) +
				   ( (unsigned long) ( bUnitAddress));

	HTREEITEM hCurrent = GetChildItem(parent);
	while (hCurrent != NULL)  {		
		const unsigned long ulItemData = GetItemData(hCurrent);					
		if (ulItemData == ulIdentifier) 
			break;			
		hCurrent = GetNextItem(hCurrent, TVGN_NEXT);
	}
	return hCurrent;
}

////////////////////////////////////////////////////////////////////////////////////:
BOOL CMyTreeNodes::DisplayUnitName(HTREEITEM hItem,STRUCT_UNIT *pUnit)	/* TM 20040716*/
{
	CString szText,szLogical;
	//CString szTemp;

	if (hItem == NULL) { ASSERT(0); return FALSE; }
	if (pUnit == NULL) { ASSERT(0); return FALSE; }

	// TODO : Display Names ifv UnitsViewMode
	szLogical.Format("#%02x#",pUnit->bUnitAddress);

	switch (UnitsViewMode) 
	{
		case UNITS_VIEWMODE_NAME :
			szText.Format("%-15s %s",pUnit->szUnitName,szLogical);
		break;

		case UNITS_VIEWMODE_LOGADDRESS :
			szText.Format("%s %s",szLogical,pUnit->szUnitName);
		break;

		case UNITS_VIEWMODE_ONLYNAME :
		default:
			szText.Format("%s",pUnit->szUnitName);
		break;	
	}	
	return(SetItemText(hItem,szText));
}

BOOL CMyTreeNodes::DisplayNodeName(HTREEITEM hItem,CCanNode *pNode)
{
	CString szText,szPhysical;
	CString szTemp;

	if (hItem == NULL) { ASSERT(0); return FALSE; }
	if (pNode == NULL) { ASSERT(0); return FALSE; }


	switch (NodesViewMode) 
	{	
		case NODES_VIEWMODE_LOGADDRESS:
			szPhysical.Format(_T("#%02x,%06x#"),pNode->bLogicalAddress,pNode->ulPhysicalAddress);
			szText.Format("%-14s %s",szPhysical,pNode->szNodeName);
		break;

		case NODES_VIEWMODE_NAME:
			szPhysical.Format(_T("#%06x,%02x#"),pNode->ulPhysicalAddress,pNode->bLogicalAddress);
			szText.Format("%-14s %s",pNode->szNodeName,szPhysical);
		break;

		default:
		case NODES_VIEWMODE_PHADDRESS:
			szPhysical.Format(_T("#%06x,%02x#"),pNode->ulPhysicalAddress,pNode->bLogicalAddress);
			szText.Format("%-14s %s",szPhysical,pNode->szNodeName);
		break;

		case NODES_VIEWMODE_ONLYNAME:		/* TM 20040716 */
			szText.Format("%-14s",pNode->szNodeName);
		break;

	}
	return(SetItemText(hItem,szText));
}

LONG CMyTreeNodes::OnNodeNameChanged(WPARAM wParam,LPARAM lParam)
{
	int i;

	const BYTE bLogicalAddress = (BYTE) ((wParam & 0xFF00) >> 8);
	const HTREEITEM hItem = GetNode(bLogicalAddress);

	CCanNode* pNode;
	if (TMGetDocumentNodes()->FindNode(bLogicalAddress, &i, &pNode)) 
	{
		if (hItem != NULL)
		{
			return (DisplayNodeName(hItem,pNode));				
		}
	}
	ASSERT(0);
	return FALSE;	
}

LONG CMyTreeNodes::OnUnitNameChanged(WPARAM wParam,LPARAM lParam) {
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());
	const BYTE bNodeAddress = (BYTE) ((wParam & 0xFF00) >> 8);
	const BYTE bUnitAddress = (BYTE) (wParam & 0xFF);	
	HTREEITEM hItem = GetNode(bNodeAddress);
	if (hItem != NULL)  {			
		if ((hItem = GetUnit(bNodeAddress,bUnitAddress,hItem)) != NULL) {						
			STRUCT_UNIT *pUnit;
			if (NodeDatabaseOperations.Find(bNodeAddress,bUnitAddress,&pUnit) == TRUE) {
				return (DisplayUnitName(hItem,pUnit));
			}			
		}
		ASSERT(hItem);	
	}
	ASSERT(hItem);

	return -1;
}



LONG CMyTreeNodes::OnUnitStatusChanged(WPARAM wParam,LPARAM lParam) {		
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;
	const BYTE bUnitAddress = (BYTE) lParam & 0xFF;		
	HTREEITEM hItem = GetNode(bNodeAddress);	
	if ((hItem = GetUnit(bNodeAddress,bUnitAddress,hItem)) != NULL) {	

		CApplicationGuiUnit::AppUiIconsExtraInfo_t appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_NONE;
		CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());
		STRUCT_UNIT *pUnit = NULL;
		if (NodeDatabaseOperations.Find(bNodeAddress,bUnitAddress,&pUnit) == TRUE) {
			ASSERT(NULL != pUnit);
			if ((NULL != pUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pUnit->m_pAppUnitData->getUnitDataType())) {				
				appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_DALI;
			}
		}

		UnitIconType iImage;														
		TMGetGuiUnit()->GetIcon(bNodeAddress,bUnitAddress,appUiExtraInfo, &iImage);
		SetItemImage(hItem,iImage,iImage);
	}	
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////
// Edit Label ...
void CMyTreeNodes::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	#define VK_ENTER		13
	
	HTREEITEM hItem = GetSelectedItem();

	switch(nChar) 
	{
		case VK_ENTER:		
		{
			if (hItem != NULL) 
			{	
				hItem = GetNextVisibleItem(hItem);		
				
				if (hItem) 
				{
					SelectItem(hItem);						
					EditLabel(hItem);
				}
				else	 // end of Tree 
				{
					hItem = GetFirstVisibleItem( );
					
					if (hItem) 
					{
						SelectItem(hItem);					
						EditLabel(hItem);
					}
				}
			}

		} break;

		case VK_F2:
		{
			if (hItem != NULL) 
			{	
				EditLabel(hItem);
			}

		} break;

		default:
		{
			// TM 20071120
			if (hItem != NULL) 
			{
				const unsigned long ulIdentifier = GetItemData(hItem);		
		
				if (((ulIdentifier & 0x00FF0000) >> 16) == TYPE_TREE_BINDING_CTRL)
				{	
					AfxGetMainWnd()->SendMessage(WM_BINDINGENTRY_EDIT,
												 ulIdentifier & 0x0000FFFF);
			
				}
			}

		} break;
	}

	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMyTreeNodes::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
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
	
			if (((ulIdentifier & 0x00FF0000) >> 16	) == TYPE_TREE_NODE_CTRL) 
			{
				
				// TODO , IFV VIEWNODESSTATE !!
				switch(NodesViewMode) 
				{
					case NODES_VIEWMODE_NAME:
					case NODES_VIEWMODE_ONLYNAME:		/* TM 20040716 */
						if (szMessage.GetLength() > 16) szMessage.SetAt(16,'\0');
						szTemp = szMessage;
						i = szTemp.Find('#',0);
						if (i != -1) {
							szMessage = szTemp.Left(i);
						}	
					break;

					case NODES_VIEWMODE_PHADDRESS :
					case NODES_VIEWMODE_LOGADDRESS :
					default:
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
						
						if (szMessage.GetLength() > 15)		// TM 20081224
						{
							szMessage.SetAt(15,'\0');
						}

					break;				
				}
				

				AfxGetMainWnd()->SendMessage(WM_NODENAME_CHANGED,
										    (WPARAM) (ulIdentifier),
										    (LPARAM) ((LPCTSTR)szMessage));	// CHANGES_MVS_2008_CSTRING
			}
			else if (((ulIdentifier & 0x00FF0000) >> 16	) == TYPE_TREE_UNIT_CTRL) 
			{																			
				/* 
					TODO ifv viewmode --> Trim unit Name !!!!
				*/

				switch (UnitsViewMode)
				{					
					case UNITS_VIEWMODE_LOGADDRESS:
						szTemp = szMessage;
						j = szMessage.GetLength();
						i = szTemp.Find('#',0);
						i = szTemp.Find('#',i+1);
						if (i!= -1) {
							szMessage = szTemp.Right(j-(i+1));
						}
						szMessage.TrimLeft();
						szMessage.TrimRight();
						
						if (szMessage.GetLength() > 16) szMessage.SetAt(16,'\0');
					break;

					default:
					case UNITS_VIEWMODE_NAME :
					case UNITS_VIEWMODE_ONLYNAME:
						
						if (szMessage.GetLength() > 16) szMessage.SetAt(16,'\0');
						szTemp = szMessage;
						i = szTemp.Find('#',0);
						if (i != -1) {
							szMessage = szTemp.Left(i);
						}	

					break;				
				}
			
				AfxGetMainWnd()->SendMessage(WM_UNITNAME_CHANGED,
									  (WPARAM) (ulIdentifier),
									  (LPARAM) ((LPCTSTR)szMessage));		// CHANGES_MVS_2008_CSTRING
			}	
			else if (((ulIdentifier & 0x00FF0000) >> 16	) == TYPE_TREE_BINDING_CTRL) {								
				
				
				if (szMessage.GetLength() > 20) szMessage.SetAt(20,'\0');
				AfxGetMainWnd()->SendMessage(WM_BINDINGDATABASE_NAMECHANGED,
									  (WPARAM) (ulIdentifier),
									  (LPARAM) ((LPCTSTR) szMessage));		// CHANGES_MVS_2008_CSTRING
			}
		}
	}			
	*pResult = 0;
}

void CMyTreeNodes::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = 0;
}

/*************************************************
/* MUST BE HERE - Caused by error Microsoft : 
/* Otherwise Enter, ESC are not handled !!
/* Q167960
/*************************************************/
BOOL CMyTreeNodes::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
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
////////////////////////////////////////////////////////////////////////////////////

void CMyTreeNodes::OnRButtonDown(UINT nFlags, CPoint point) 
{		
	m_draggingItem.setIdle();
	KillTimer(WM_TIMER_DRAG);

	BOOL handled = FALSE;
	UINT uFlags = 0;
	const HTREEITEM hItem = HitTest(point, &uFlags);
	if ((hItem != NULL) && (uFlags & TVHT_ONITEM))  {			/* V17.03: controle TVHT_ONITEM - zie BUG-0178*/
		if (!Select(hItem, TVGN_CARET)) {
			ASSERT(0);
		}
		this->setCurrentSelectedItem(hItem);		
		this->SetFocus();										// Nodig anders wordt de selectie niet altijd getoond.
		handled = TRUE;
				
		// Enkel starten met dragging wanneer de bindingsview aktief is....		
		if (!TMGetViews()->IsBindingViewActive()) {
			return;
		}

		const TypeOfTreeCtrl type = this->getTypeCtrl(hItem);
		if (TYPE_TREE_UNIT_CTRL == type) {						
			if (CDraggingItem::STATE_IDLE == m_draggingItem.getState()) {
				m_draggingItem.setWait(point);
				SetTimer(WM_TIMER_DRAG,1000,NULL);
				SetCapture();														// ???????????????????				
			}
		}	
	}		

	if (!handled) {
		CTreeCtrl::OnRButtonDown(nFlags, point);	
	}

	// Oppassen: Onderstaande mag niet uitgevoerd worden anders wordt de 'OnRButtonUp' niet aangeroepen.
 	// CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CMyTreeNodes::OnMouseMove(UINT nFlags, CPoint point) 
{	
	// Enkel starten met dragging wanneer de bindingsview aktief is....		
	if (TMGetViews()->IsBindingViewActive()) {			
		if (nFlags & MK_LBUTTON ) {
		
		}
		else if (nFlags & MK_RBUTTON ) 
		{
			CRect rcClient;
			GetClientRect(&rcClient);
			
			const CDraggingItem::State_t state = m_draggingItem.getState();
			if (CDraggingItem::STATE_BUSY == state || CDraggingItem::STATE_WAIT == state) {	

				KillTimer(WM_TIMER_DRAG);

				if (CDraggingItem::STATE_WAIT == state) {			
					const CPoint& pt = m_draggingItem.getPoint();
					UINT uFlags;
					HTREEITEM hItem = HitTest(pt, &uFlags);
					if (hItem != NULL)  {
						Select(hItem, TVGN_CARET);						
						const unsigned long ulIdentifier = GetItemData(hItem);
						if (((ulIdentifier & 0x00FF0000) >> 16) == TYPE_TREE_UNIT_CTRL) {
							CImageList* const pNewImageList = CreateDragImage(hItem);	
							m_draggingItem.setBusy(pNewImageList);
							pNewImageList->BeginDrag(0,CPoint(0,0));
							pNewImageList->DragEnter(this,point);		
							pNewImageList->DragMove(point);								
							SetCapture();			
						}	
					}						
				}

				CImageList* const pImageListDrag = m_draggingItem.getImageList();
				ASSERT(NULL != pImageListDrag);
				if (NULL != pImageListDrag) {			
					if (point.x <= rcClient.right) 
					{
						pImageListDrag->DragMove(point);							
					}
					else
					{							
						const HTREEITEM hItem = GetSelectedItem();				
						if (hItem != NULL) 
						{				
							unsigned long ulPos;
							const unsigned long ulIdentifier = GetItemData(hItem);					
							
							CalculateCoordinates(point,&ulPos);
							
							AfxGetMainWnd()->SendMessage(WM_ADDFLOW_DRAGGING_UNIT,
												  (WPARAM) (ulIdentifier & 0xFFFF),
												  (LPARAM) ulPos);					
							
							pImageListDrag->DragLeave(this);
							pImageListDrag->EndDrag();	

							//m_draggingItem.setIdle();
							//ReleaseCapture();
						}			
					}
				}
			}
		}
		else {
			m_draggingItem.setIdle();
		}
	}
	CTreeCtrl::OnMouseMove(nFlags, point);
}

// Zie WORKAROUND_LABEL_EDIT
// Via een timer het label voor editeren van de naam.
void CMyTreeNodes::OnTimer(UINT nIDEvent) {
	
	if (WM_TIMER_EDITNAME == nIDEvent) {
		const HTREEITEM hItem = GetSelectedItem();
		if (hItem != NULL) {
			EditLabel(hItem);
		}
	}
	else if (WM_TIMER_DRAG == nIDEvent) {
		if (CDraggingItem::STATE_WAIT == m_draggingItem.getState()) {
			bool drag = FALSE;
			const CPoint& point = m_draggingItem.getPoint();
			UINT uFlags;
			HTREEITEM hItem = HitTest(point, &uFlags);
			if (hItem != NULL)  {
				Select(hItem, TVGN_CARET);						
				const unsigned long ulIdentifier = GetItemData(hItem);
				if (((ulIdentifier & 0x00FF0000) >> 16) == TYPE_TREE_UNIT_CTRL) {
					CImageList* const pNewImageList = CreateDragImage(hItem);	
					ASSERT(NULL != pNewImageList);
					if (NULL != pNewImageList) {
						m_draggingItem.setBusy(pNewImageList);
						pNewImageList->BeginDrag(0,CPoint(0,0));
						pNewImageList->DragEnter(this,point);		
						pNewImageList->DragMove(point);		
						drag = TRUE;
						SetCapture();			
					}
				}	
			}
			if (!drag) {
				m_draggingItem.setIdle();
			}
		}
	}
	KillTimer(nIDEvent);
}

void CMyTreeNodes::OnRButtonUp(UINT nFlags, CPoint point) 
{
	const CDraggingItem::State_t state = m_draggingItem.getState();
	if (CDraggingItem::STATE_WAIT == state || CDraggingItem::STATE_IDLE == state) {
		m_draggingItem.setIdle();
		KillTimer(WM_TIMER_DRAG);				
		openContextMenu(this,point,GetSelectedItem());	
	}
	else if (CDraggingItem::STATE_BUSY == state) {
		CImageList* const pImageListDrag = m_draggingItem.getImageList();
		ASSERT(NULL != pImageListDrag);
		pImageListDrag->DragLeave(this);
		pImageListDrag->EndDrag();	
		m_draggingItem.setIdle();

		ReleaseCapture();

		const HTREEITEM hItem = GetSelectedItem();
		if (hItem != NULL) {				
			const unsigned long ulIdentifier = GetItemData(hItem);
			const unsigned long ulPos = ((point.x & 0xFFFF) << 16) | (point.y & 0xFFFF);					
			AfxGetMainWnd()->SendMessage(WM_ADDFLOW_DROP_UNIT,(WPARAM)(ulIdentifier & 0xFFFF),(LPARAM) ulPos);					
		}
	}
	CTreeCtrl::OnRButtonUp(nFlags, point);
}


// Oppassen: WORKAROUND_LABEL_EDIT !
// Sedert V16.05: Edit van de naam werkt niet meer op de standaard manier.
// Zelf in software geimplementeerd.
// Wanneer Linker muisknop op hetzelfde elemenet dan een timer starten.
// In deze timer expiry het label aanpassen.
void CMyTreeNodes::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	BOOL handled = FALSE;
	UINT uFlags = 0;
	static HTREEITEM hItemPrevious = NULL;
	const HTREEITEM hItem = HitTest(point, &uFlags);
	if (hItem != NULL) {
		if (uFlags & LVHT_ONITEMLABEL) {
			Select(hItem, TVGN_CARET);			
			if (hItemPrevious == hItem) {
				if (uFlags & LVHT_ONITEMLABEL) {
					SetTimer(WM_TIMER_EDITNAME,1,NULL);
					handled = TRUE;
				}
			}
			hItemPrevious = hItem;
		}
	}
	if (!handled) {
		CTreeCtrl::OnLButtonDown(nFlags, point);		
	}
}


////////////////////////////////////////////////////////////////////////////////////
// L DbClk Button Click -> Show Unit / Node Properties ...
void CMyTreeNodes::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UINT uFlags;

	const HTREEITEM hItem = HitTest(point, &uFlags);
	if ((hItem != NULL) && (uFlags & TVHT_ONITEM) )
	{		
		unsigned long ulIdentifier = GetItemData(hItem);	
		// ONLY WHEN UnitCtrl
		
		switch((ulIdentifier & 0x00FF0000) >> 16)
		{
			case TYPE_TREE_UNIT_CTRL:
			{
				/* TM20120830 */
				((CMainFrame*) AfxGetMainWnd())->showBindingsForm( );


				const BYTE bNodeAddress = (BYTE) ((ulIdentifier & 0xFF00) >> 8);	
				const BYTE bUnitAddress = (BYTE) ((ulIdentifier & 0xFF));

				AfxGetMainWnd()->SendMessage(WM_ADDFLOW_ADDUNIT,
											 (WPARAM) bNodeAddress,
											 (LPARAM) bUnitAddress);
				
			} break;

			case TYPE_TREE_BINDING_CTRL:
			{
				/* TM20120830 */
				((CMainFrame*) AfxGetMainWnd())->showBindingsForm( );


				AfxGetMainWnd()->SendMessage(WM_BINDINGENTRY_EDIT,
											 ulIdentifier & 0x0000FFFF);

			} break;

			default:
			break;
		}
	}
	//CTreeCtrl::OnLButtonDblClk(nFlags, point);
}
////////////////////////////////////////////////////////////////////////////////////
// Item expanding --> show units ...
void CMyTreeNodes::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TVITEM tvItem = pNMTreeView->itemNew;
	LPARAM lParam = tvItem.lParam;
	TypeOfTreeCtrl CtrlType = (TypeOfTreeCtrl) ((lParam & 0x00FF0000) >> 16);

	if (CtrlType == TYPE_TREE_NODE_CTRL) 
	{
		const BYTE bNodeAddress = (lParam & 0xFF00) >> 8;	

		if (pNMTreeView->action == TVE_COLLAPSE) 
		{
			AfxGetMainWnd()->SendMessage(WM_TREE_NODE_COLLAPSED,
										 (WPARAM) bNodeAddress);		
		}
		else if (pNMTreeView->action == TVE_EXPAND)				
		{
			AfxGetMainWnd()->SendMessage(WM_TREE_NODE_EXPANDED,
										 (WPARAM) bNodeAddress);
		}
	}
	*pResult = 0;	// was 0
}

////////////////////////////////////////////////////////////////////////////////////
BOOL CMyTreeNodes::AddBindingEntry(BYTE bNodeAddress,BYTE bUnitAddress,CBindingEntry *pEntry)
{
	ASSERT(pEntry);
	if (pEntry->GetID() == 0) return FALSE;

	HTREEITEM hNewParent;
	HTREEITEM hParentItem = GetNode(bNodeAddress);

	if (hParentItem != NULL) 
	{
		hParentItem = GetUnit(bNodeAddress,bUnitAddress,hParentItem);
		if (hParentItem != NULL) 
		{						
			AddBindingEntry(hParentItem,&hNewParent,pEntry);
		}
	}
	RedrawWindow();
	return TRUE;
}

LONG CMyTreeNodes::OnBindingDatabaseAddEntry(WPARAM wParam,LPARAM lParam)
{	
	const unsigned short nID = (wParam & 0xFFFF);	

	if (nID == 0) 
	{
		return FALSE;
	}

	if (TMGetBindingEntries() != NULL)
	{
		CBindingEntry* const pEntry = TMGetBindingEntries()->Get(nID);					
		if (pEntry)
		{
			if (pEntry->GetColUnit()->Count() == 0) 
			{
				return FALSE;
			}

			// First we must delete all current entries ...
			DeleteBindingEntry(nID);

			CBindingUnits::Iterator it(pEntry->GetColUnit()->CreateIterator());
			for (it.Begin();!it.IsDone();it.Next())
			{
				CBindingUnit* const pUnit = it.CurrentItem( );
				ASSERT( pUnit );

				if (AddBindingEntry(pUnit->GetNodeAddress(),
									pUnit->GetUnitAddress(),
									pEntry) == FALSE)		
				{
					return FALSE;		
				}
			}		
		}
		else
		{
			ASSERT(0);
			return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}

// Mogelijkheid om een individuele node up te daten.
void CMyTreeNodes::updateSingleNode(BYTE bNodeAddress) {

	HTREEITEM hItem = this->GetNode(bNodeAddress);
	if (NULL == hItem) {
		return;
	}
	if (NULL == TMGetBindingEntries() || NULL == TMGetDocumentNodes()) {
		ASSERT(0);
		return;
	}

	this->m_draggingItem.setIdle();

	// Zorgt ervoor dat ook alle children verwijderd worden.
	this->DeleteItem(hItem);

	// Updaten
	CCanNode* pNode = NULL;
	int dummy = 0;
	if (FALSE == TMGetDocumentNodes()->FindNode(bNodeAddress,&dummy,&pNode)) {
		return;
	}

	// Updaten node + units.
	HTREEITEM ParentNode;
	this->AddNode(TVI_ROOT,&ParentNode,pNode);
	CUnitIterator it2(TMGetDocumentNodes()->CreateUnitIterator(pNode));
	for (it2.Begin();!it2.IsDone();it2.Next()) {
		HTREEITEM ParentUnit;
		this->AddUnit(ParentNode,&ParentUnit,it2.CurrentItem());	
	}				


	// Binding entries enkel voor deze node.
	CBindingEntries::Iterator it(TMGetBindingEntries()->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CBindingEntry* const pEntry = it.CurrentItem( );
		ASSERT( pEntry );
		CBindingUnits::Iterator it2(pEntry->GetColUnit()->CreateIterator());
		for (it2.Begin();!it2.IsDone();it2.Next()) {
			CBindingUnit* const pUnit = it2.CurrentItem();
			ASSERT( pUnit );
			if ((NULL != pUnit) && (pUnit->GetNodeAddress() == bNodeAddress)) {
				if (AddBindingEntry(pUnit->GetNodeAddress(), pUnit->GetUnitAddress(), pEntry) == NULL) {
					ASSERT(0);
					AfxMessageBox("BindingEntry could not be added to nodeTree");				
					return;
				}
			}
		}			
	}

	// Opnieuw sorteren...
	this->SortChildren(TVI_ROOT);
}

/////////////////////////////////////////////////////////////////////////////////
void CMyTreeNodes::IterateBindingEntries(_CALLBACK pfc)
{
	HTREEITEM hCurrentUnit = 0;
	HTREEITEM hPrevious,hCurrent;
	HTREEITEM hCurrentNode = GetNextItem(TVI_ROOT, TVGN_ROOT);

	while (hCurrentNode != NULL) 
	{
		hCurrentUnit = GetNextItem(hCurrentNode, TVGN_CHILD);
		while(hCurrentUnit != NULL) 
		{		
			if (hCurrentUnit == NULL) break;
			
			if (ItemHasChildren(hCurrentUnit)) 
			{
				hCurrent = GetNextItem(hCurrentUnit, TVGN_CHILD);
				hPrevious = hCurrent;
				while (hPrevious != NULL)
				{
					hCurrent = GetNextItem(hCurrent, TVGN_NEXT);
					
					(this->*pfc)( &hPrevious );	// Dit is de callBack

					hPrevious = hCurrent;								
				}			
			}
			hCurrentUnit = GetNextItem(hCurrentUnit, TVGN_NEXT);		
		}
		hCurrentNode = GetNextItem(hCurrentNode, TVGN_NEXT);
	}
}
////////////////////////////////////////////////////////////////////////////////


HTREEITEM CMyTreeNodes::GetBindingEntry(unsigned short nBindingID,int nCnt)
{
	if (nBindingID == 0) return NULL;
	int i = 0;

	unsigned long ulIdentifier = ( (unsigned long) ( ((BYTE) TYPE_TREE_NODES) << 24)) +
						           ((unsigned long) ( ((BYTE) TYPE_TREE_BINDING_CTRL) << 16));
	ulIdentifier |= nBindingID;

	HTREEITEM hCurrentUnit = 0;
	HTREEITEM hPrevious,hCurrent;
	HTREEITEM hCurrentNode = GetNextItem(TVI_ROOT, TVGN_ROOT);

	while (hCurrentNode != NULL) 
	{
		hCurrentUnit = GetNextItem(hCurrentNode, TVGN_CHILD);
		while(hCurrentUnit != NULL) 
		{		
			if (hCurrentUnit == NULL) break;
			
			if (ItemHasChildren(hCurrentUnit)) 
			{
				hCurrent = GetNextItem(hCurrentUnit, TVGN_CHILD);
				hPrevious = hCurrent;
				while (hPrevious != NULL)
				{
					hCurrent = GetNextItem(hCurrent, TVGN_NEXT);
					
					if (GetItemData(hPrevious) == ulIdentifier)
					{
						if (i == nCnt) 
						{
							return hPrevious;
						}
						i++;
					}
					hPrevious = hCurrent;								
				}			
			}
			hCurrentUnit = GetNextItem(hCurrentUnit, TVGN_NEXT);		
		}
		hCurrentNode = GetNextItem(hCurrentNode, TVGN_NEXT);
	}
	return NULL;	
}

void CMyTreeNodes::UpdateBindingEntry(HTREEITEM* hBindingEntry)
{
	ASSERT( hBindingEntry );

	const unsigned short usIdentifier = (unsigned short) GetItemData(*hBindingEntry) & 0xFFFF;
	
	HTREEITEM hParent = GetParentItem(*hBindingEntry);
	if (hParent)
	{	
		CBindingEntry* const pEntry = TMGetBindingEntries()->Get(usIdentifier); 

		if (SetBindingEntryIcon(hParent,*hBindingEntry,pEntry) == FALSE) 
		{
			ASSERT(0);
		}	
	}
}


/**
 * Date 20060322 : Changed function
 */
BOOL CMyTreeNodes::UpdateBindingEntry(CBindingEntry* const pEntry)
{
	ASSERT( pEntry );

	HTREEITEM hCurrent = 0;
	HTREEITEM hParent = 0;
	
	int nCnt = 0;
	const int nBindingID = pEntry->GetID( );

	while(1) 
	{
		if ((hCurrent = GetBindingEntry(nBindingID,nCnt)) == NULL)
		{
			return TRUE;
		}
			
		if ((hParent = GetParentItem(hCurrent)) == NULL) 
		{	
			ASSERT(0); 
			return FALSE;
		}

		if (SetBindingEntryIcon(hParent,hCurrent,pEntry) == FALSE) 
		{
			ASSERT(0);
		}

		nCnt++;
	}		
	return TRUE;
}

BOOL CMyTreeNodes::DeleteBindingEntry(unsigned short nBindingID)
{
	if (nBindingID == 0) return FALSE;
	
	while (1) 
	{		
		const HTREEITEM hCurrent = GetBindingEntry(nBindingID,0);

		if (hCurrent == NULL) return TRUE;

		if (DeleteItem(hCurrent) == FALSE) 
		{
			ASSERT(0);		
		}	
	}
	return TRUE;
}

LONG CMyTreeNodes::OnBindingDatabaseNameChanged(WPARAM wParam,LPARAM lParam)
{	
	CString szName;

	szName.Format("%s",lParam);
	
	if (szName.IsEmpty() == TRUE) 
	{
		return FALSE;
	}

	int nCnt = 0;
	const unsigned short usIdentifier = wParam &0x0000FFFF;

	while(1) 
	{
		const HTREEITEM hCurrent = GetBindingEntry(usIdentifier,nCnt);

		if (hCurrent == NULL)
		{
			return TRUE;
		}	

		SetItemText(hCurrent,szName);

		nCnt++;
	}
	return TRUE;
}

LONG CMyTreeNodes::OnBindingDatabaseRemoveEntry(WPARAM wParam,LPARAM lParam)
{	
	return (DeleteBindingEntry(wParam & 0xFFFF));
}

/**
 * Date 20060322 : Changed function
 */
#ifdef SKIP
LONG CMyTreeNodes::OnBindingdatabaseUpdate(WPARAM wParam,LPARAM lParam)
{	
	if (TMGetBindingEntries())
	{
		CBindingEntries::Iterator it(TMGetBindingEntries()->CreateIterator());
		for (it.Begin();!it.IsDone();it.Next())
		{
			UpdateBindingEntry(it.CurrentItem()); 
		}
		return TRUE;
	}
	return FALSE;
}
#endif


// TM 20081224: Optimalisatie via een callback functie !!!
LONG CMyTreeNodes::OnBindingdatabaseUpdate(WPARAM wParam,LPARAM lParam)
{	
	IterateBindingEntries( &CMyTreeNodes::UpdateBindingEntry );		// CHANGES_MVS_2008
	return TRUE;
}


STRUCT_UNIT* CMyTreeNodes::GetUnit(HTREEITEM hItem)
{
	const unsigned long ulIdentifier = GetItemData(hItem);
		
	if (((ulIdentifier >> 16) & 0xFF) != TYPE_TREE_UNIT_CTRL) {
		return NULL;
	}
			
	const BYTE bNodeAddress = (BYTE) (ulIdentifier>>8) & 0xFF;
	const BYTE bUnitAddress = (BYTE) ulIdentifier & 0xFF;

	STRUCT_UNIT* pUnit;
	CNodeDatabaseOperations Operations(TMGetDocumentNodes());
	if (Operations.Find(bNodeAddress,bUnitAddress,&pUnit) == TRUE) 
	{
		return pUnit;
	}	
	ASSERT(0);
	return NULL;
}
LONG CMyTreeNodes::OnTreeViewSortNodesByName(WPARAM wParam,LPARAM lParam)
{
	CWaitCursor wait;			// Sedert V16.46
	DRAWING_DISABLE_CTRL;
	if (NodesViewMode != NODES_VIEWMODE_NAME) {
		NodesViewMode = NODES_VIEWMODE_NAME;	// Display all Nodes by name ...
		UpdateNodeNames();		
	}

	SortChildren(TVI_ROOT);
	return TRUE;
}

LONG CMyTreeNodes::OnViewSortnodesbyphaddress(WPARAM wParam,LPARAM lParam)
{
	DRAWING_DISABLE_CTRL;
	CWaitCursor wait;			// Sedert V16.46
	if (NodesViewMode != NODES_VIEWMODE_PHADDRESS) {
		NodesViewMode = NODES_VIEWMODE_PHADDRESS; // display all nodes by Ph Address
		UpdateNodeNames();
	}
	SortChildren(TVI_ROOT);
	return TRUE;
}

LONG CMyTreeNodes::OnViewSortnodesbylogicaladdress(WPARAM wParam,LPARAM lParam)
{	
	DRAWING_DISABLE_CTRL;	
	CWaitCursor wait;			// Sedert V16.46
	if (NodesViewMode != NODES_VIEWMODE_LOGADDRESS) {
		NodesViewMode = NODES_VIEWMODE_LOGADDRESS; // display all nodes by Ph Address
		UpdateNodeNames();
	}
	SortChildren(TVI_ROOT);
	return TRUE;
}
LONG CMyTreeNodes::OnViewSortNodesonlyname(WPARAM wParam,LPARAM lParam)
{
	DRAWING_DISABLE_CTRL;
	if (NodesViewMode != NODES_VIEWMODE_ONLYNAME)  {	
		CWaitCursor wait;			// Sedert V16.46
		NodesViewMode = NODES_VIEWMODE_ONLYNAME; // display all nodes by Ph Address
		UpdateNodeNames();
	}
	SortChildren(TVI_ROOT);
	return TRUE;
}

LONG CMyTreeNodes::OnViewUnitsbyname(WPARAM wParam,LPARAM lParam)
{
	if(UnitsViewMode != UNITS_VIEWMODE_NAME) {
		CWaitCursor wait;			// Sedert V16.46
		DRAWING_DISABLE_CTRL;
		UnitsViewMode = UNITS_VIEWMODE_NAME;
		UpdateUnitNames();
	}
	return TRUE;
}

LONG CMyTreeNodes::OnViewUnitsbyaddress(WPARAM wParam,LPARAM lParam)
{
	if(UnitsViewMode != UNITS_VIEWMODE_LOGADDRESS) {
		CWaitCursor wait;			// Sedert V16.46
		DRAWING_DISABLE_CTRL;
		UnitsViewMode = UNITS_VIEWMODE_LOGADDRESS;
		UpdateUnitNames();
	}
	return TRUE;
}

LONG CMyTreeNodes::OnViewUnitsonlyname(WPARAM wParam,LPARAM lParam)
{
	if(UnitsViewMode != UNITS_VIEWMODE_ONLYNAME)  {
		CWaitCursor wait;			// Sedert V16.46
		DRAWING_DISABLE_CTRL;
		UnitsViewMode = UNITS_VIEWMODE_ONLYNAME;
		UpdateUnitNames();
	}
	return TRUE;
}

LONG CMyTreeNodes::OnButtonSortNodes(WPARAM wParam,LPARAM lParam)
{
	DRAWING_DISABLE_CTRL;

	CWaitCursor wait;			// Sedert V16.46
	switch(NodesViewMode) {
		case NODES_VIEWMODE_LOGADDRESS:
			NodesViewMode = NODES_VIEWMODE_PHADDRESS;
		break;
		case NODES_VIEWMODE_PHADDRESS:
			NodesViewMode = NODES_VIEWMODE_ONLYNAME;	
		break;
		case NODES_VIEWMODE_ONLYNAME:
			NodesViewMode = NODES_VIEWMODE_NAME;
		break;
		default:
		case NODES_VIEWMODE_NAME:
			NodesViewMode = NODES_VIEWMODE_LOGADDRESS;
		break;	
	}	
	UpdateNodeNames();
	SortChildren(TVI_ROOT);
	return TRUE;
}

LONG CMyTreeNodes::OnButtonSortUnits(WPARAM wParam,LPARAM lParam)
{
	DRAWING_DISABLE_CTRL;
	CWaitCursor wait;			// Sedert V16.46
	switch (UnitsViewMode) {
		case UNITS_VIEWMODE_NAME:
			UnitsViewMode = UNITS_VIEWMODE_LOGADDRESS;
		break;
		case UNITS_VIEWMODE_LOGADDRESS:
			UnitsViewMode = UNITS_VIEWMODE_ONLYNAME;
		break;
		case UNITS_VIEWMODE_ONLYNAME:
		default:
			UnitsViewMode = UNITS_VIEWMODE_NAME;
		break;	
	}
	UpdateUnitNames();
	return TRUE;
}
LONG CMyTreeNodes::OnButtonCollapse(WPARAM wParam,LPARAM lParam)
{	
	DRAWING_DISABLE_CTRL;
	CWaitCursor wait;			// Sedert V16.46
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		const HTREEITEM ParentNode = GetNode(pNode->bLogicalAddress);
		if (ParentNode) {									
			Expand(ParentNode,TVE_COLLAPSE);
			CUnitIterator it2(TMGetDocumentNodes()->CreateUnitIterator( pNode ));
			for (it2.Begin(); !it2.IsDone(); it2.Next()) {
				STRUCT_UNIT* const pUnit = it2.CurrentItem( );
				const HTREEITEM ParentUnit = GetUnit(pNode->bLogicalAddress, pUnit->bUnitAddress, ParentNode);
				if (ParentUnit) {																
					Expand(ParentUnit,TVE_COLLAPSE);									
				}
			}
		}
	}
	return true;
}
LONG CMyTreeNodes::OnButtonExpand(WPARAM wParam,LPARAM lParam)
{	
	DRAWING_DISABLE_CTRL;
	CWaitCursor wait;			// Sedert V16.46
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );		
		const HTREEITEM ParentNode = GetNode(pNode->bLogicalAddress);
		if (ParentNode)	{
			Expand(ParentNode,TVE_EXPAND);	
		}
	}
	return TRUE;
}

void CMyTreeNodes::OnContextmenuUnitRename() {
	if (NULL != m_hCurrentItemSelected) {
		EditLabel(m_hCurrentItemSelected);
	}
}

// Sedert V16.17: BUG_0169_CHANGES_PROPERTY_NODESSHEET_POSTMESSAGE
// PostMessage( ) aanroepen ipv SendMessage( )
// Aanpassingen ook doorgevoerd voor het openen van de dialogen van de unit properties.
//
// Sedert V16.35: Selected item data gebruiken.
void CMyTreeNodes::OnContextmenuShowUnitProperties() {
	ASSERT(0 != this->m_popupSelectedItemDataValue);
	// TM 20120829:
	// Listview: Rechter dubbele muisklik op de node toont ook de node properties.
	const int type = (this->m_popupSelectedItemDataValue & 0x00FF0000) >> 16; 
	switch(type) {
		case TYPE_TREE_UNIT_CTRL:
		{
			AfxGetMainWnd()->PostMessage(WM_SHOW_DIALOG_UNITPROPERTIES,(WPARAM) (this->m_popupSelectedItemDataValue & 0x0000FFFF));
		} break;
		case TYPE_TREE_NODE_CTRL:
		{
			const BYTE bNodeLogicalAddress = (BYTE) ((this->m_popupSelectedItemDataValue & 0x0000FF00)>>8);
			AfxGetMainWnd()->PostMessage(WM_SHOW_NODE_PROPERTIES,(WPARAM)(bNodeLogicalAddress),NODES_PAGE_DEFAULT);						
		} break;
	}
}


// NOTE: m_hItemSelected dient als het selected item (ipv deze van de CTreeCtrl) zie ook BUG-0176 
void CMyTreeNodes::OnContextmenuNodeRename() {
	if (NULL != m_hCurrentItemSelected) {
		EditLabel(m_hCurrentItemSelected);
	}
}

// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
void CMyTreeNodes::OnContextmenuNodeShowProperties() {
	ASSERT(0xff != this->m_popupSelectedNodeAddress);
	_showNodeProperties(*this,this->m_popupSelectedNodeAddress,NODES_PAGE_RESOURCES);
}

// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
void CMyTreeNodes::OnContextmenuNodeShowNodeControl() {
	ASSERT(0xff != this->m_popupSelectedNodeAddress);
	_showNodeProperties(*this,this->m_popupSelectedNodeAddress,NODES_PAGE_CONTROL);
}

// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
void CMyTreeNodes::OnContextmenuNodeShowNodeSmap() {
	ASSERT(0xff != this->m_popupSelectedNodeAddress);
	_showNodeProperties(*this,this->m_popupSelectedNodeAddress,NODES_PAGE_SMAP);
}

// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
void CMyTreeNodes::OnContextmenuNodeShowNodeConfig() {
	ASSERT(0xff != this->m_popupSelectedNodeAddress);
	_showNodeProperties(*this,this->m_popupSelectedNodeAddress,NODES_PAGE_CONFIG);
}

// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
void CMyTreeNodes::OnContextmenuNodeShowNodeMasterTasks() {
	ASSERT(0xff != this->m_popupSelectedNodeAddress);
	_showNodeProperties(*this,this->m_popupSelectedNodeAddress,NODES_PAGE_MASTERTASKS);
}

// NOTE: m_hItemSelected dient als het selected item (ipv deze van de CTreeCtrl) zie ook BUG-0176 
void CMyTreeNodes::OnContextmenuBindingsRename() {
	if (NULL != m_hCurrentItemSelected) {
		EditLabel(m_hCurrentItemSelected);
	}
}	

// Sedert V16.35: Selected item data gebruiken 
void CMyTreeNodes::OnContextmenuBindingsShow() {
	ASSERT(0 != this->m_popupSelectedItemDataValue);
	CMainFrame* const pWnd = ((CMainFrame*) AfxGetMainWnd());
	if (NULL == pWnd) {
		return;
	}	
	pWnd->showBindingsForm( );
	pWnd->SendMessage(WM_BINDINGENTRY_EDIT,(this->m_popupSelectedItemDataValue & 0x0000FFFF));
}

// Sedert V16.27: UDP Lan Configuratie.
// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
void CMyTreeNodes::OnContextmenuShowLanCfg() {
	ASSERT(0xff != this->m_popupSelectedNodeAddress);
	AfxGetMainWnd()->PostMessage(WM_SHOW_NODE_UDPLANCFG,(WPARAM)(this->m_popupSelectedNodeAddress),(WPARAM)0xffffffff);			
}

void CMyTreeNodes::OnContextmenuNodeBrowse() {
	ASSERT(0xff != this->m_popupSelectedNodeAddress);
	AfxGetMainWnd()->PostMessage(WM_APPLICATION_BROWSE_SINGLE_NODE,(WPARAM)(this->m_popupSelectedNodeAddress),TRUE);			
}

CMyTreeNodes::TypeOfTreeCtrl CMyTreeNodes::getTypeCtrl(HTREEITEM htreeItem) const {
	TypeOfTreeCtrl type = TYPE_TREE_NO_CTRL;
	if (NULL != htreeItem) {
		const unsigned long ulIdentifier = GetItemData(htreeItem);
		type = (TypeOfTreeCtrl) ((ulIdentifier & 0x00FF0000) >> 16); 
	}
	return type;
}

void CMyTreeNodes::setCurrentSelectedItem(HTREEITEM hItem) {
	this->m_hCurrentItemSelected = hItem;
}

BOOL CMyTreeNodes::getSelectedNode(HTREEITEM htreeItem,CCanNode** ppNode) {
	BYTE bNodeAddress = 0xff;
	BYTE bUnitAddress = 0xff;
	if (!getSelItemAddress(htreeItem,&bNodeAddress,&bUnitAddress)) {
		return FALSE;
	}

	int iEntryNr = -1;
	return TMGetDocumentNodes()->FindNode(bNodeAddress,&iEntryNr,ppNode);
}

BOOL CMyTreeNodes::getSelItemAddress(HTREEITEM htreeItem,BYTE* pbNodeAddress,BYTE* pbUnitAddress) {
	const unsigned long ulIdentifier = GetItemData(htreeItem);		
	*pbNodeAddress = (BYTE) ((unsigned long)(ulIdentifier >> 8) & 0x000000FF);
	*pbUnitAddress = (BYTE) ( ulIdentifier & 0xFF);	
	return TRUE;
}

// Zie BUG-0176:
// Er wordt hier reeds een HTREEITEM van het geselecteerde item meegegeven.
// Bij het openenen van de context menu wordt deze opnieuw bepaald via hitTest.
// Mogelijk is dit nog een probleem ???
void CMyTreeNodes::openContextMenu(CWnd* pWnd, CPoint ptMousePos,HTREEITEM hSelectedItem) {
	const TypeOfTreeCtrl type = getTypeCtrl(hSelectedItem);
	switch (type) {
		case TYPE_TREE_UNIT_CTRL: {
			openUnitContextMenu(this,ptMousePos);
		} break;
		case TYPE_TREE_NODE_CTRL: {
			openNodeContextMenu(this,ptMousePos);
		} break;
		case TYPE_TREE_BINDING_CTRL: {
			openBindingContextMenu(this,ptMousePos);
		} break;
	}
}


// Sedert v17.03: Zie BUG-0178
// Controle doen op het geselecteerd item, indien deze NULL is dan een extra hit test uitvoeren...
// Probleem met de hit test is bij een mouse drag wordt er soms een ander item geopend dan deze die geselecteerd is.
static HTREEITEM findSelectedItem(CMyTreeNodes* pThis, CPoint ptMousePos) {
	HTREEITEM hItem = pThis->GetSelectedItem();
	ASSERT(NULL != hItem);
	if (NULL != hItem) {
		return hItem;
	}

	UINT uFlags = 0;
	hItem = pThis->HitTest(ptMousePos, &uFlags);
	ASSERT(NULL != hItem);
	if ((NULL != hItem) && (uFlags & TVHT_ONITEM)) {
		return hItem;
	}	
	return NULL;
}

// Sedert V17.03: findSelectedItem()
static CMenu* openContextPopupMenu(CMyTreeNodes* m_pThis,CWnd* pWnd, CPoint ptMousePos,unsigned int resourceID,unsigned long* pulSelectedItemData) {
	const HTREEITEM hItem = findSelectedItem(m_pThis, ptMousePos);
	if (NULL == hItem) 
		return NULL;

	*pulSelectedItemData = m_pThis->GetItemData(hItem);

	CMenu menu;
	menu.LoadMenu(resourceID);
	CMenu* const pPopup = menu.GetSubMenu(0);
	if (NULL == pPopup) {
		return NULL;
	}

	m_pThis->ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, m_pThis );
	return pPopup;
}

// Sedert V16.35: Selected item data bijhouden
void CMyTreeNodes::openUnitContextMenu(CWnd* pWnd, CPoint ptMousePos) {
#if(0)
	// if Shift-F10
	if (ptMousePos.x == -1 && ptMousePos.y == -1)
		ptMousePos = (CPoint) GetMessagePos();

	ScreenToClient(&ptMousePos);
#endif 

	unsigned long ulSelectedItemData = 0;
	if (NULL != openContextPopupMenu(this,pWnd,ptMousePos,IDR_CONTEXTMENU_TREEUNITS,&ulSelectedItemData)) {
		this->m_popupSelectedItemDataValue = ulSelectedItemData;
	}
}

// BUG-0178: Sedert v17.03: findSelectedItem() gebruiken
void CMyTreeNodes::openNodeContextMenu(CWnd* pWnd, CPoint ptMousePos) {
	HTREEITEM hItem = findSelectedItem(this, ptMousePos);
	if (NULL == hItem) {
		return;
	}
	CCanNode* pNode = NULL;
	if (!getSelectedNode(hItem,&pNode)) {
		return;
	}

	CMenu menu;
	// the font popup is stored in a resource
	menu.LoadMenu(IDR_CONTEXTMENU_TREENODE);
	CMenu* const pPopup = menu.GetSubMenu(0);
	if (NULL == pPopup) {
		return;
	}

	// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
	this->m_popupSelectedNodeAddress = pNode->getNodeAddress();

	// In functie van de geselecteerde node het menu opbouwen...
	if (CNodeExtConfigInfo_Util::isSupported(pNode)) {
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_SHOWCONFIGURATION, MF_ENABLED);
	}
	else {
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_SHOWCONFIGURATION, MF_GRAYED);
	}
	if ((CNodeScreenmapInfo_Util::isSupported(pNode)) || (CNodeScreenmapLiteInfo_Util::isSupported(pNode))) {
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_SHOWSCREENMAPPING, MF_ENABLED);
	}
	else {
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_SHOWSCREENMAPPING, MF_GRAYED);
	}

	if (TMGetCommunicationApp()->isSocketHomeClientMasterSupported(pNode)) {
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_SHOWMASTERTASKS, MF_ENABLED);
	}
	else {
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_SHOWMASTERTASKS, MF_GRAYED);
	}

	{
		// Sedert V16.27: UDP Lan Configuratie.
		// Mogelijk wanneer een device gevonden werd in de lijst.
		// Wanneer het device niet gevonden werd in de laatste scan wordt er door CMainFrame 
		// een messagebox getoond.
		bool enableItem = FALSE;
		CMainFrame* const pMainWnd = (CMainFrame*)AfxGetMainWnd();
		if (NULL != pMainWnd) {
			const AppUDPDevice* pDevice = NULL;
			AppUDPDevices& refDeviceNodes = pMainWnd->getAppUDPServerCfg().getDeviceList();			
			if (refDeviceNodes.find(pNode->getPhysicalAddress(),&pDevice)) {
				ASSERT(NULL != pDevice);
				enableItem = TRUE;
			}
		}
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_SHOWLANCFG, (TRUE == enableItem ? MF_ENABLED : MF_GRAYED));
	}
	// In functie van de geselecteerde node het menu opbouwen... TOT HIER


	// Updaten van de gui ifv de toestand van de applicatie.
	if ((TMGetCommunicationApp()->IsConnected()) && (CAppState::APP_STATE_IDLE == CAppState::getInstance().getAppState())) {
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_BROWSE, MF_ENABLED);
	}
	else {
		pPopup->EnableMenuItem(ID_CONTEXTMENUNODE_BROWSE, MF_GRAYED);
	}

	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this );
}

// Sedert V16.35: Selected item data bijhouden
void CMyTreeNodes::openBindingContextMenu(CWnd* pWnd, CPoint ptMousePos) {
	unsigned long ulSelectedItemData = 0;
	if (NULL != openContextPopupMenu(this,pWnd,ptMousePos,IDR_CONTEXTMENU_TREENODEBINDINGS,&ulSelectedItemData)) {
		this->m_popupSelectedItemDataValue = ulSelectedItemData;
	}
}



void CMyTreeNodes::OnMyTreeGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVGETINFOTIP* pGetInfoTip = reinterpret_cast<NMTVGETINFOTIP*>(pNMHDR);
    
    if (!pGetInfoTip || !pGetInfoTip->hItem)
        return;

	if (NULL == pGetInfoTip->hItem) 
		return;


	static const unsigned long VALUE_TREE_NODES = (TYPE_TREE_NODES << 24);
	static const unsigned long MASK_TREE_NODES = (0xFF << 24);

	const unsigned long dwData = this->GetItemData(pGetInfoTip->hItem);
	if (VALUE_TREE_NODES != (dwData & MASK_TREE_NODES)) {
		return;
	}

	static const unsigned long VALUE_TREE_ITEM_UNIT = (TYPE_TREE_UNIT_CTRL << 16);
	static const unsigned long MASK_TREE_ITEM_UNIT = (0xFF << 16);

	CString tooltipText;
	if (VALUE_TREE_ITEM_UNIT == (dwData & MASK_TREE_ITEM_UNIT)) {
		
		const BYTE bNodeAddress = (dwData & 0xFF00) >> 8;
		const BYTE bUnitAddress = (dwData & 0xFF);

		const CString strUnitName(TMGetDocumentNodes()->GetUnitName(bNodeAddress,bUnitAddress));
		if (!strUnitName.IsEmpty()) {
			CString strInfo;
			strInfo.Format("Name='%s' ",strUnitName);
			tooltipText += strInfo;
		}

		const CString strLabelText(TMGetNodeDatabaseApp()->getUnitTextLabel(bNodeAddress,bUnitAddress));	
		if (!strLabelText.IsEmpty()) {
			CString strInfo;
			strInfo.Format("Info='%s' ",strLabelText);
			tooltipText += strInfo;
		}
	
		CString strUnitAddress;
		strUnitAddress.Format("Address=0x%02x,0x%02x", bNodeAddress, bUnitAddress);
		tooltipText += strUnitAddress;
	}

	if (tooltipText.IsEmpty()) {
		return;
	}
		
	// Kopieer de tekst naar het buffer van de tooltip
	_tcsncpy_s(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, tooltipText, _TRUNCATE);	

    *pResult = 0;

}


