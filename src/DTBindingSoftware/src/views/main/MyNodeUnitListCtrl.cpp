// MyNodeUnitListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyNodeUnitListCtrl.h"
#include "MainFrm.h"
#include "MyDefinitions.h"								// Bevat window messages / timers + enums.
#include "nodeScreenmapInfo.h"					
#include "app\appCfg\AppCfginfo\cfginfonode.h"
#include "app/appudpcfgserver/appudpservercfg.h"		// Sedert V16.27: UDP Lan Configuratie.
// ------------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
using namespace TMNodeDatabase;
using namespace TMCommunication;
// ---------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMyNodeUnitListCtrl


// Sedert V16.17: BUG_0169_CHANGES_PROPERTY_NODESSHEET_POSTMESSAGE
// PostMessage( ) aanroepen ipv SendMessage( )
static void showNodeProperties(CMyNodeUnitListCtrl& parent, int selectedItem, int activePage) {
	if (selectedItem<0) {
		return;
	}
	const unsigned long ulIdentifier = parent.GetItemData(selectedItem);				
	const BYTE bNodeLogicalAddress = (BYTE) ((unsigned long)(ulIdentifier >> 8) & 0x000000FF);			
	AfxGetMainWnd()->PostMessage(WM_SHOW_NODE_PROPERTIES,(LPARAM)(bNodeLogicalAddress),activePage);			
}


CMyNodeUnitListCtrl::CMyNodeUnitListCtrl(ListCtrlStates myState) : 
	m_CurrentState(myState) { }

CMyNodeUnitListCtrl::~CMyNodeUnitListCtrl() { }


BEGIN_MESSAGE_MAP(CMyNodeUnitListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyNodeUnitListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	// ON_WM_RBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_KEYUP()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
  	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_CONTEXTMENUNODE_RENAME, &CMyNodeUnitListCtrl::OnContextmenunodeRename)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWPROPERTIES, &CMyNodeUnitListCtrl::OnContextmenuShowProperties)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWCONTROL, &CMyNodeUnitListCtrl::OnContextmenuShowNodeControl)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWSCREENMAPPING, &CMyNodeUnitListCtrl::OnContextmenuShowNodeSmap)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWCONFIGURATION, &CMyNodeUnitListCtrl::OnContextmenuShowNodeConfig)
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWMASTERTASKS, &CMyNodeUnitListCtrl::OnContextmenuShowNodeMasterTasks)	
	ON_COMMAND(ID_CONTEXTMENUNODE_SHOWLANCFG, &CMyNodeUnitListCtrl::OnContextmenuShowLanCfg)	
	ON_COMMAND(ID_CONTEXTMENUNODE_BROWSE, &CMyNodeUnitListCtrl::OnContextmenuNodeBrowse)
	ON_COMMAND(ID_VIEWUNITS_SHOWUNITPROPERTIES, &CMyNodeUnitListCtrl::OnContextMenuUnitShowProperties)
	ON_COMMAND(ID_VIEWUNITS_RENAMEUNIT, &CMyNodeUnitListCtrl::OnContextMenuUnitRename)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyNodeUnitListCtrl message handlers

void CMyNodeUnitListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	switch(m_CurrentState)
	{
		case LISTCTRL_STATE_VIEW_NODES: {					
			UINT uFlags = LVHT_ONITEMICON;
			const int nItem = HitTest(point,&uFlags);
			if (nItem != -1) {
				const unsigned long ulIdentifier = GetItemData(nItem);				
				const BYTE bNodeLogicalAddress = (BYTE) ((unsigned long)(ulIdentifier >> 8) & 0x000000FF);						
			}		
		}break;		
		case LISTCTRL_STATE_VIEW_UNITS:
		break;
	}
	
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CMyNodeUnitListCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	CListCtrl::OnRButtonUp(nFlags, point);
}

void CMyNodeUnitListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CListCtrl::OnRButtonDown(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////////

void CMyNodeUnitListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	BOOL fHandled = FALSE;	
	switch(m_CurrentState)
	{
		case LISTCTRL_STATE_VIEW_NODES:			
		break;

		case LISTCTRL_STATE_VIEW_UNITS:
		{					
			UINT uFlags;
			const int nItem = HitTest(point,&uFlags);
			if ((uFlags & LVHT_ONITEMICON) && (nItem != -1)) {			
				const unsigned long ulIdentifier = GetItemData(nItem);				
				const BYTE bNodeLogicalAddress = (BYTE) ((unsigned long)(ulIdentifier >> 8) & 0x000000FF);
				const BYTE bUnitAddress = (BYTE) ( ulIdentifier & 0xFF);
				TMGetGuiUnit()->ClickOn(bNodeLogicalAddress,bUnitAddress);
				fHandled = TRUE;
			}			
		} break;
	}

	if (fHandled == FALSE) {
		CListCtrl::OnLButtonDown(nFlags, point);
	}
}
void CMyNodeUnitListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	BOOL fHandled = FALSE;
	switch(m_CurrentState)
	{
		case LISTCTRL_STATE_VIEW_NODES:
		{		
		}break;

		case LISTCTRL_STATE_VIEW_UNITS:
		{				
			UINT uFlags;			
			const int nItem = HitTest(point,&uFlags);		
			if ((uFlags & LVHT_ONITEMICON) && (nItem != -1)) {
				KillTimer(TIMERID_PULSTOGGLE);
				const unsigned long ulIdentifier = GetItemData(nItem);				
				const BYTE bNodeLogicalAddress = (BYTE) ((unsigned long)(ulIdentifier >> 8) & 0x000000FF);
				const BYTE bUnitAddress = (BYTE) ( ulIdentifier & 0xFF);
				TMGetGuiUnit()->ClickOff(bNodeLogicalAddress,bUnitAddress);								
				SetItem(nItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED,LVIS_SELECTED, 0);
				fHandled = TRUE;
			}				
		}break;
	}
	
	if (fHandled == FALSE) {
		CListCtrl::OnLButtonUp(nFlags, point);
	}
}

void CMyNodeUnitListCtrl::OnTimer(UINT nIDEvent) 
{	
	CListCtrl::OnTimer(nIDEvent);
}

unsigned long CMyNodeUnitListCtrl::SetParamData(ListTypes x, TypeOfListCtrl y, BYTE bNodeAddress, BYTE bUnitAddress)
{
	const unsigned long ulIdentifier = ((unsigned long)(((BYTE) x)<< 24)) +
									   ((unsigned long)(((BYTE) y)<< 16)) +
									   ((unsigned long)((BYTE) bNodeAddress)<<8) +
									   ((unsigned long)(bUnitAddress));	
	return ulIdentifier;
}

void CMyNodeUnitListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
		case VK_F2: {
			const int nSel = GetSelectionMark();
			if (nSel != -1) {
				EditLabel(nSel);
			}
		} break;	
	}	
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMyNodeUnitListCtrl::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Send WM_CONTEXTMENU to self
  	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
  	// Mark message as handled and suppress default handling
	*pResult = 1;
}

void CMyNodeUnitListCtrl::OnContextMenu(CWnd* pWnd, CPoint ptMousePos) 
{
	if (LISTCTRL_STATE_VIEW_UNITS == m_CurrentState) {	
		openUnitContextMenu(pWnd,ptMousePos);
	}
	else {
		openNodeContextMenu(pWnd,ptMousePos);
	}
}


BOOL CMyNodeUnitListCtrl::getSelectedNode(int item,CCanNode** ppNode) {
	BYTE bNodeAddress = 0xff;
	BYTE bUnitAddress = 0xff;
	if (!getSelItemAddress(item,&bNodeAddress,&bUnitAddress)) {
		return FALSE;
	}

	int iEntryNr = -1;
	return TMGetDocumentNodes()->FindNode(bNodeAddress,&iEntryNr,ppNode);
}

BOOL CMyNodeUnitListCtrl::getSelItemAddress(int item,BYTE* pbNodeAddress,BYTE* pbUnitAddress) {
	const unsigned long ulIdentifier = GetItemData(item);		
	*pbNodeAddress = (BYTE) ((unsigned long)(ulIdentifier >> 8) & 0x000000FF);
	*pbUnitAddress = (BYTE) ( ulIdentifier & 0xFF);	
	return TRUE;
}


void CMyNodeUnitListCtrl::openNodeContextMenu(CWnd* pWnd, CPoint ptMousePos) {

	// if Shift-F10
	if (ptMousePos.x == -1 && ptMousePos.y == -1)
		ptMousePos = (CPoint) GetMessagePos();

	ScreenToClient(&ptMousePos);

	UINT uFlags;	
	const int nItem = HitTest( ptMousePos, &uFlags );	
	if( nItem < 0 )
		return;


	CCanNode* pNode = NULL;
	if (!getSelectedNode(nItem,&pNode)) {
		return;
	}

	CMenu menu;
	// the font popup is stored in a resource
	menu.LoadMenu(IDR_CONTEXTMENU_NODE);
	CMenu* const pPopup = menu.GetSubMenu(0);
	if (NULL == pPopup) {
		return;
	}

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

	m_selectedItem = nItem;
}

void CMyNodeUnitListCtrl::openUnitContextMenu(CWnd* pWnd, CPoint ptMousePos) {
	// if Shift-F10
	if (ptMousePos.x == -1 && ptMousePos.y == -1)
		ptMousePos = (CPoint) GetMessagePos();

	ScreenToClient(&ptMousePos);

	UINT uFlags;	
	const int nItem = HitTest( ptMousePos, &uFlags );	
	if( nItem < 0 )
		return;

	CMenu menu;
	// the font popup is stored in a resource
	menu.LoadMenu(IDR_CONTEXTMENU_VIEWUNITS);
	CMenu* const pPopup = menu.GetSubMenu(0);
	if (NULL == pPopup) {
		return;
	}
	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this );

	m_selectedItem = nItem;
}

void CMyNodeUnitListCtrl::OnContextmenunodeRename()
{
	if (m_selectedItem<0) {
		return;
	}
	EditLabel(m_selectedItem);
}

void CMyNodeUnitListCtrl::OnContextmenuShowProperties() {
	if (LISTCTRL_STATE_VIEW_UNITS == m_CurrentState) {	
		return;			
	}
	showNodeProperties(*this,m_selectedItem,NODES_PAGE_RESOURCES);
}

void CMyNodeUnitListCtrl::OnContextmenuShowNodeControl() {
	if (LISTCTRL_STATE_VIEW_UNITS == m_CurrentState) {	
		return;			
	}
	showNodeProperties(*this,m_selectedItem,NODES_PAGE_CONTROL);
}

void CMyNodeUnitListCtrl::OnContextmenuShowNodeSmap() {
	if (LISTCTRL_STATE_VIEW_UNITS == m_CurrentState) {	
		return;			
	}
	showNodeProperties(*this,m_selectedItem,NODES_PAGE_SMAP);
}

void CMyNodeUnitListCtrl::OnContextmenuShowNodeConfig() {
	if (LISTCTRL_STATE_VIEW_UNITS == m_CurrentState) {	
		return;			
	}
	showNodeProperties(*this,m_selectedItem,NODES_PAGE_CONFIG);
}

void CMyNodeUnitListCtrl::OnContextmenuShowNodeMasterTasks() {
	if (LISTCTRL_STATE_VIEW_UNITS == m_CurrentState) {	
		return;			
	}
	showNodeProperties(*this,m_selectedItem,NODES_PAGE_MASTERTASKS);
}

// Sedert V16.27: UDP Lan Configuratie.
void CMyNodeUnitListCtrl::OnContextmenuShowLanCfg() {
	if (LISTCTRL_STATE_VIEW_UNITS == m_CurrentState) {	
		return;			
	}
	if (m_selectedItem<0) {
		return;
	}
	const unsigned long ulIdentifier = this->GetItemData(m_selectedItem);				
	const BYTE bNodeLogicalAddress = (BYTE) ((unsigned long)(ulIdentifier >> 8) & 0x000000FF);
	AfxGetMainWnd()->PostMessage(WM_SHOW_NODE_UDPLANCFG,(LPARAM)(bNodeLogicalAddress),(WPARAM)0xffffffff);		
}

void CMyNodeUnitListCtrl::OnContextmenuNodeBrowse() {
	if (LISTCTRL_STATE_VIEW_UNITS == m_CurrentState) {	
		return;			
	}
	if (m_selectedItem<0) {
		return;
	}
	const unsigned long ulIdentifier = this->GetItemData(m_selectedItem);				
	const BYTE bNodeLogicalAddress = (BYTE) ((unsigned long)(ulIdentifier >> 8) & 0x000000FF);
	AfxGetMainWnd()->PostMessage(WM_APPLICATION_BROWSE_SINGLE_NODE,(WPARAM)bNodeLogicalAddress,TRUE);			
}

// Sedert V16.17: BUG_0169_CHANGES_PROPERTY_NODESSHEET_POSTMESSAGE
// PostMessage( ) aanroepen ipv SendMessage( )
// Aanpassingen ook doorgevoerd voor het openen van de dialogen van de unit properties.
void CMyNodeUnitListCtrl::OnContextMenuUnitShowProperties() {
	if (LISTCTRL_STATE_VIEW_NODES == m_CurrentState) {	
		return;			
	}
	if (m_selectedItem<0) {
		return;
	}
	const unsigned long ulIdentifier = GetItemData(m_selectedItem);	
	AfxGetMainWnd()->PostMessage(WM_SHOW_DIALOG_UNITPROPERTIES,(WPARAM) (ulIdentifier & 0x0000FFFF));
}

void CMyNodeUnitListCtrl::OnContextMenuUnitRename() {
	if (m_selectedItem<0) {
		return;
	}
	EditLabel(m_selectedItem);
}