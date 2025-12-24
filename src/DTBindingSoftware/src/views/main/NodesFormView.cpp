// NodesFormView.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodesFormView.h"
#include "BindingSoftwareDoc.h"
#include "MyWindowMessages.h"
#include "app\appCfg\appCfgInfo\cfgInfoNode.h"
#include "app/appudpcfgserver/appudpservercfg.h"
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

enum {
	LIST_ITEM_NAME = 0,
	LIST_ITEM_NODEADDRESS = 1,
	LIST_ITEM_UNITS = 2,
	LIST_ITEM_PHYSICALADDRESS = 3,
	LIST_ITEM_BINDINGS = 4,
	LIST_ITEM_VERSION = 5,
	LIST_ITEM_IPADDRESS = 6,
	LIST_ITEM_GATEWAY = 7,
	LIST_ITEM_NETMASK = 8,
	LIST_ITEM_DHCP = 9,
	LIST_ITEM_MAC = 10,
	LIST_ITEM_FOUND = 11,
	LIST_ITEM_MAX_ITEMS
};

/////////////////////////////////////////////////////////////////////////////
static const int m_maxSortItems = LIST_ITEM_MAX_ITEMS;			
static BOOL m_fSortMethod[m_maxSortItems] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
											  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
											};

/////////////////////////////////////////////////////////////////////////////
// CNodesFormView

IMPLEMENT_DYNCREATE(CNodesFormView, CFormView)

CNodesFormView::CNodesFormView()
	: CFormView(CNodesFormView::IDD), m_fControlsCreated(FALSE),m_pMainWnd(NULL),
		m_pListView(NULL)
{
	//{{AFX_DATA_INIT(CNodesFormView)
	//}}AFX_DATA_INIT
#if(0)
	m_CurrentFormState = STATE_VIEW_DISABLED;	
#endif 
}

CNodesFormView::~CNodesFormView()
{
	if (m_pListView != NULL) {
		delete m_pListView;
		m_pListView = NULL;
	}
}

void CNodesFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNodesFormView)
	DDX_Control(pDX, IDC_STATIC_STATS, m_StaticCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNodesFormView, CFormView)
	//{{AFX_MSG_MAP(CNodesFormView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOW_NODES_FORM,OnShowNodesForm)
	ON_MESSAGE(WM_UPDATE_NODES_FORM,OnUpdateNodesForm)
	ON_MESSAGE(WM_NODENAME_CHANGED,OnNodeNameChanged)
	ON_MESSAGE(WM_NODE_STATUS_CHANGED,OnNodeStatusChanged)
	ON_MESSAGE(WM_APPLICATION_CLEARVIEWS,OnApplicationClearViews)
	ON_MESSAGE(WM_NODE_ETHERNETNWK_STATUS,onNodeEthernetNwkStatus)		// since V16.03
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodesFormView diagnostics

#ifdef _DEBUG
void CNodesFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CNodesFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNodesFormView message handlers

void CNodesFormView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	m_pMainWnd = ((CMainFrame*) AfxGetMainWnd());
}


void CNodesFormView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
}


void CNodesFormView::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;
	GetClientRect(&rect);	
	CFormView::OnSize(nType, cx, cy);	

	this->ShowScrollBar(SB_BOTH,FALSE);

	if ((m_fControlsCreated == TRUE) && (NULL != m_pListView)) {
		rect.bottom -= 50;
		m_pListView->MoveWindow(rect);	
		rect.top = rect.bottom + 5;
		rect.bottom = rect.bottom + 50;
		rect.left += 10;
		m_StaticCtrl.MoveWindow(rect);	
	}
}

void CNodesFormView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFormView::OnLButtonDown(nFlags, point);
}



int CNodesFormView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

BOOL CNodesFormView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CNodesFormView::InitControls()
{
	// CREATE ALL CONTROLS !!!!	
	if ((m_pListView = new CMyNodeUnitListCtrl(CMyNodeUnitListCtrl::LISTCTRL_STATE_VIEW_NODES)) != NULL)
	{
		CRect rect,btnRect;
		GetClientRect(&rect);
		rect.bottom -= 50;

		m_pListView->Create(WS_VISIBLE | 
						  WS_CHILD | 
						  WS_BORDER | 
						  LVS_REPORT | 
						  LVS_SINGLESEL |
						  LVS_SHAREIMAGELISTS | 
						  LVS_EDITLABELS ,					 
						  rect,
						  this,
						  IDC_LISTVIEW);
	
		CImageList* const pImageList = TMGetGraphics()->GetLargeNodeUnitImageList();
		if (pImageList != NULL) {
			m_pListView->SetImageList(pImageList,LVSIL_SMALL);		
		}		
		m_pListView->SetFont(TMGetGraphics()->GetFont(FONT_TREEVIEW_FIXED));			

	
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_CENTER;

		lvColumn.cx = 125;
		lvColumn.iSubItem = LIST_ITEM_NAME;
		lvColumn.pszText = "Name";
		m_pListView->InsertColumn(LIST_ITEM_NAME,&lvColumn);

		lvColumn.cx = 75;
		lvColumn.iSubItem = LIST_ITEM_NODEADDRESS;
		lvColumn.pszText = "LA. Address";
		m_pListView->InsertColumn(LIST_ITEM_NODEADDRESS,&lvColumn);

		lvColumn.cx = 75;
		lvColumn.iSubItem = LIST_ITEM_UNITS;
		lvColumn.pszText = "Nr of Units";
		m_pListView->InsertColumn(LIST_ITEM_UNITS,&lvColumn);

		lvColumn.cx = 75;
		lvColumn.iSubItem = LIST_ITEM_PHYSICALADDRESS;
		lvColumn.pszText = "PH. address";
		m_pListView->InsertColumn(LIST_ITEM_PHYSICALADDRESS,&lvColumn);
	
		// new
		lvColumn.cx = 75;
		lvColumn.iSubItem = LIST_ITEM_BINDINGS;
		lvColumn.pszText = "Nr of Bindings";
		m_pListView->InsertColumn(LIST_ITEM_BINDINGS,&lvColumn);
	
		// new 2
		lvColumn.cx = 50;
		lvColumn.iSubItem = LIST_ITEM_VERSION;
		lvColumn.pszText = "SW. Version";
		m_pListView->InsertColumn(LIST_ITEM_VERSION,&lvColumn);
		
		lvColumn.cx = 100;
		lvColumn.iSubItem = LIST_ITEM_IPADDRESS;
		lvColumn.pszText = "IP-Address";
		m_pListView->InsertColumn(LIST_ITEM_IPADDRESS,&lvColumn);

		lvColumn.cx = 100;
		lvColumn.iSubItem = LIST_ITEM_GATEWAY;
		lvColumn.pszText = "Gateway";
		m_pListView->InsertColumn(LIST_ITEM_GATEWAY,&lvColumn);

		lvColumn.cx = 100;
		lvColumn.iSubItem = LIST_ITEM_NETMASK;
		lvColumn.pszText = "Netmask";
		m_pListView->InsertColumn(LIST_ITEM_NETMASK,&lvColumn);

		lvColumn.cx = 50;
		lvColumn.iSubItem = LIST_ITEM_DHCP;
		lvColumn.pszText = "DHCP";
		m_pListView->InsertColumn(LIST_ITEM_DHCP,&lvColumn);

		lvColumn.cx = 150;
		lvColumn.iSubItem = LIST_ITEM_MAC;
		lvColumn.pszText = "MAC-Address";
		m_pListView->InsertColumn(LIST_ITEM_MAC,&lvColumn);

		lvColumn.cx = 50;
		lvColumn.iSubItem = LIST_ITEM_FOUND;
		lvColumn.pszText = "Found";
		m_pListView->InsertColumn(LIST_ITEM_FOUND,&lvColumn);
		
		rect.top = rect.bottom + 5;
		rect.bottom = rect.bottom + 50;
		rect.left += 10;

		m_StaticCtrl.MoveWindow(rect);
		m_fControlsCreated = TRUE;
	}
}

static bool findDevice(AppUDPDevices& ref,CCanNode* pNode, const AppUDPDevice** ppDevice) 
{
	if (NULL == pNode || NULL == ppDevice) {
		return false;
	}
	return ref.find(pNode->getPhysicalAddress(),ppDevice);
}


template <class T>
int Sort(T t,T s)
{
	if (t > s) return 1;
	else if (t < s) return -1;
	return 0;
}

int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int nRetVal = 0;
	int iEntryNR;	
	const BYTE bAddress1 = (lParam1 & 0x0000FF00) >> 8;
	const BYTE bAddress2 = (lParam2 & 0x0000FF00) >> 8;
	CCanNode *pNode1, *pNode2;
	if (TMGetDocumentNodes()->FindNode(bAddress1,&iEntryNR,&pNode1)) {
		if (TMGetDocumentNodes()->FindNode(bAddress2,&iEntryNR,&pNode2)) {						
			switch(lParamSort)
			{
				case LIST_ITEM_NAME:	/* Name */
				{									
					nRetVal = strcmp(pNode1->szNodeName, pNode2->szNodeName);																		
				} break;

				case LIST_ITEM_NODEADDRESS: /* Address */
				{
					nRetVal = Sort<BYTE>(pNode1->bLogicalAddress,pNode2->bLogicalAddress);				
				} break;

				case LIST_ITEM_UNITS:	/* NR of Units */
				{
					nRetVal = Sort<BYTE>(pNode1->bNROfUnits,pNode2->bNROfUnits);
				} break;

				case LIST_ITEM_PHYSICALADDRESS:	/* Physical Address */
				{
					nRetVal = Sort<unsigned long>(pNode1->ulPhysicalAddress,pNode2->ulPhysicalAddress);
				} break;

				case LIST_ITEM_BINDINGS:	/* Nr Of Bindings */
				{
					nRetVal = Sort<unsigned short>(pNode1->getNodeBindingsNrEntries(),pNode2->getNodeBindingsNrEntries());
				} break;

				case LIST_ITEM_VERSION: /* Software version */
				{
					nRetVal = Sort<int>
						(((pNode1->GetSoftwareVersion()<<8)+(pNode1->GetSoftwareVersionRev())),
						 ((pNode2->GetSoftwareVersion()<<8)+(pNode2->GetSoftwareVersionRev())));
				} break;
			
				case LIST_ITEM_IPADDRESS: 
				case LIST_ITEM_GATEWAY:
				case LIST_ITEM_NETMASK:
				case LIST_ITEM_DHCP:
				case LIST_ITEM_MAC:
				case LIST_ITEM_FOUND:
				{		
					CMainFrame* const pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
					if (NULL != pMainFrame) {
						const AppUDPDevice* pDevice1 = NULL;
						const AppUDPDevice* pDevice2 = NULL;
						CString valueDevice1;
						CString valueDevice2;
						AppUDPDevices& ref = pMainFrame->getAppUDPServerCfg().getDeviceList();
						if (findDevice(ref,pNode1,&pDevice1)) {
							if (LIST_ITEM_IPADDRESS == lParamSort) {
								valueDevice1 = pDevice1->getIpAddress();
							}
							else if (LIST_ITEM_GATEWAY == lParamSort) {
								valueDevice1 = pDevice1->getGateway();
							}
							else if (LIST_ITEM_NETMASK == lParamSort) {
								valueDevice1 = pDevice1->getNetmask();
							}
							else if (LIST_ITEM_MAC == lParamSort) {
								valueDevice1 = pDevice1->getMACAddress();
							}
							else if (LIST_ITEM_DHCP == lParamSort) {
								valueDevice1 = pDevice1->getDhcpOnOff() ? "on" : "off";
							}	
							else if (LIST_ITEM_FOUND == lParamSort) {
								valueDevice1 = pDevice1->getLastFound() ? "yes" : "no";
							}	
						}					
						if (findDevice(ref,pNode2,&pDevice2)) {
							if (LIST_ITEM_IPADDRESS == lParamSort) {
								valueDevice2 = pDevice2->getIpAddress();
							}
							else if (LIST_ITEM_GATEWAY == lParamSort) {
								valueDevice2 = pDevice2->getGateway();
							}
							else if (LIST_ITEM_NETMASK == lParamSort) {
								valueDevice2 = pDevice2->getNetmask();
							}
							else if (LIST_ITEM_MAC == lParamSort) {
								valueDevice2 = pDevice2->getMACAddress();
							}
							else if (LIST_ITEM_DHCP == lParamSort) {
								valueDevice2 = pDevice2->getDhcpOnOff() ? "on" : "off";
							}	
							else if (LIST_ITEM_FOUND == lParamSort) {
								valueDevice2 = pDevice2->getLastFound() ? "yes" : "no";
							}	
						}
						nRetVal = valueDevice1.Compare(valueDevice2);						
					}										  
				} break;
			}
		}
	}

	if (lParamSort < m_maxSortItems) {
		if (m_fSortMethod[lParamSort] == FALSE) {
			if (nRetVal == 1) nRetVal = -1;
			else if (nRetVal == -1) nRetVal = 1;
		}
	}
	return nRetVal;
}


BOOL CNodesFormView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	CString szMessage;
	static BOOL fEditTextEnable = TRUE;


	LV_DISPINFO *lv_dispinfo = (LV_DISPINFO*) lParam;
	LV_ITEM      *plvItem = &lv_dispinfo->item;
		
	if (lv_dispinfo->hdr.code == LVN_COLUMNCLICK) 
	{				
		/** 
		 * Sort Function : Flags specify direction ...
		 */			
		if (plvItem->iItem < m_maxSortItems)
		{
			if (m_fSortMethod[plvItem->iItem] == FALSE)
			{
				m_fSortMethod[plvItem->iItem] = TRUE;
			}
			else
			{
				m_fSortMethod[plvItem->iItem] = FALSE;
			}

			m_pListView->SortItems(SortFunc,plvItem->iItem);
		}
		return TRUE;
	}
	else if (lv_dispinfo->hdr.code == LVN_BEGINLABELEDIT ) 
	{		
		return TRUE;
	}
	else if (lv_dispinfo->hdr.code == LVN_ENDLABELEDIT ) 
	{
		if (plvItem->pszText != NULL) 
		{
			if (strcmp(plvItem->pszText,"") != 0) 
			{
				szMessage.Format("%s",plvItem->pszText);
				if (szMessage.GetLength() > 15) 
				{
					szMessage.SetAt(15,'\0');			// TM 20081224
				}

				unsigned long ulIdentifier = plvItem->lParam;

				m_pMainWnd->SendMessage(WM_NODENAME_CHANGED,
			  					     (LPARAM) (ulIdentifier & 0x0000FFFF),
									 (WPARAM) ((LPCTSTR)szMessage));		

				m_pListView->SetItemText(plvItem->iItem, plvItem->iSubItem, szMessage);
			}
		}
		return TRUE;
	}	
	return CFormView::OnNotify(wParam, lParam, pResult);
}


//////////////////////////////////////////////////////////////////////////////////////
void CNodesFormView::CreateNodeListView()
{
	if (m_pListView != NULL)
	{		
		m_pListView->DeleteAllItems();

		int i = 0;
		CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
		for (it.Begin();!it.IsDone();it.Next()) {
			AddNodeToListView(it.CurrentItem(), i++);		
		}	
	}
}

static void updateNode(CMyNodeUnitListCtrl* pListView, int entryNr, AppUDPDevices& refNodes, int physicalNodeAddress) {

	const AppUDPDevice* pDevice = NULL;
	if (refNodes.find(physicalNodeAddress,&pDevice)) {

		const CString& strIPAddress = pDevice->getIpAddress();
		pListView->SetItemText(entryNr, LIST_ITEM_IPADDRESS, strIPAddress);

		const CString& strGateway = pDevice->getGateway();
		pListView->SetItemText(entryNr, LIST_ITEM_GATEWAY, strGateway);

		const CString& strNetmask = pDevice->getNetmask();
		pListView->SetItemText(entryNr, LIST_ITEM_NETMASK, strNetmask);

		const CString& macAddress = pDevice->getMACAddress();
		pListView->SetItemText(entryNr, LIST_ITEM_MAC, macAddress);

		const CString strDHCP = ((true == pDevice->getDhcpOnOff()) ? "on" : "off");
		pListView->SetItemText(entryNr, LIST_ITEM_DHCP, strDHCP);

		const CString strFound = ((true == pDevice->getLastFound()) ? "yes" : "no");
		pListView->SetItemText(entryNr, LIST_ITEM_FOUND, strFound);
	}
}

void CNodesFormView::AddNodeToListView(CCanNode *pNode,int EntryNr)
{
	CString szName(pNode->szNodeName);	
	UnitIconType iImage;
		
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE | LVIF_IMAGE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	
	TMGetGuiUnit()->GetIcon(pNode->bLogicalAddress,&iImage);	

	lvItem.iImage = iImage;	
	lvItem.iItem = EntryNr;
	lvItem.iSubItem = 0;
	lvItem.pszText = (LPTSTR)((LPCTSTR)szName);
	lvItem.lParam = m_pListView->SetParamData(CMyNodeUnitListCtrl::TYPE_LIST_NODES,
											  CMyNodeUnitListCtrl::TYPE_LIST_NODE_CTRL,
											  pNode->bLogicalAddress,
											  0xFF);
	m_pListView->InsertItem(&lvItem);
	
	szName.Format("0x%02x",pNode->bLogicalAddress);
	m_pListView->SetItemText(EntryNr,LIST_ITEM_NODEADDRESS,szName);
	szName.Format("%d",pNode->bNROfUnits);
	m_pListView->SetItemText(EntryNr,LIST_ITEM_UNITS,szName);
	szName.Format("0x%06x",pNode->ulPhysicalAddress);
	m_pListView->SetItemText(EntryNr,LIST_ITEM_PHYSICALADDRESS,szName);
	szName.Format("%d",pNode->getNodeBindingsNrEntries());
	m_pListView->SetItemText(EntryNr,LIST_ITEM_BINDINGS,szName);
	CCanNodeInfo_Util::formatSoftwareVersion(*pNode,szName);
	m_pListView->SetItemText(EntryNr,LIST_ITEM_VERSION,szName);

	if (0 !=m_pMainWnd) {	
		updateNode(m_pListView, EntryNr, m_pMainWnd->getAppUDPServerCfg().getDeviceList(),pNode->ulPhysicalAddress);	
	}
}

void CNodesFormView::UpdateView(void)
{	
	if (FALSE == m_fControlsCreated) {
		InitControls();
	}
	CreateNodeListView();
	DisplayInstallationStats();
}

/////////////////
LONG CNodesFormView::OnShowNodesForm(WPARAM wParam,LPARAM lParam)
{	
	ShowWindow(TRUE);
	UpdateView();
	return TRUE;
}

LONG CNodesFormView::OnUpdateNodesForm(WPARAM wParam,LPARAM lParam)
{
	UpdateView();
	return TRUE;
}


LONG CNodesFormView::OnNodeStatusChanged(WPARAM wParam,LPARAM lParam)
{
	if (m_fControlsCreated == TRUE) 
	{		
		CCanNode *pNode;		
		int i;
		const BYTE bLogicalAddress = (BYTE) (wParam & 0xFF);
		if (TMGetDocumentNodes()->FindNode(bLogicalAddress,&i,&pNode)) 
		{	
			int iEntryNr;
			CString szName;

			if (GetNodeItem(bLogicalAddress,&iEntryNr))
			{
				szName.Format("%s",pNode->szNodeName);
				m_pListView->SetItemText(iEntryNr,LIST_ITEM_NAME,(LPCTSTR) szName);	// CHANGES_MVS_2008_CSTRING
				szName.Format("0x%02x",pNode->bLogicalAddress);
				m_pListView->SetItemText(iEntryNr,LIST_ITEM_NODEADDRESS,szName);
				szName.Format("%d",pNode->bNROfUnits);
				m_pListView->SetItemText(iEntryNr,LIST_ITEM_UNITS,szName);
				szName.Format("0x%06x",pNode->ulPhysicalAddress);
				m_pListView->SetItemText(iEntryNr,LIST_ITEM_PHYSICALADDRESS,szName);
				szName.Format("%d",pNode->getNodeBindingsNrEntries());
				m_pListView->SetItemText(iEntryNr,LIST_ITEM_BINDINGS,szName);				
				CCanNodeInfo_Util::formatSoftwareVersion(*pNode,szName);
				m_pListView->SetItemText(iEntryNr,LIST_ITEM_VERSION,szName);

				if (0 !=m_pMainWnd) {	
					updateNode(m_pListView, iEntryNr, m_pMainWnd->getAppUDPServerCfg().getDeviceList(),pNode->ulPhysicalAddress);	
				}
			}
		}
		return TRUE;
	}	
	return FALSE;
}

// sedert V16.03
// wParam : Physical node address
LONG CNodesFormView::onNodeEthernetNwkStatus(WPARAM wParam,LPARAM lParam)		
{
	if ((m_fControlsCreated == FALSE) || (0 == m_pMainWnd)) {
		ASSERT(0 != m_pMainWnd); 
		return FALSE;
	}
	if (wParam>0 && wParam<0xffffff) {
		CCanNode *pNode;		
		int i;
		if (TMGetDocumentNodes()->FindPhysicalNode(wParam,&i,&pNode)) {
			int iEntryNr;		
			if (GetNodeItem(pNode->getNodeAddress(),&iEntryNr)) {
				if (0 !=m_pMainWnd) {	
					updateNode(m_pListView, iEntryNr, m_pMainWnd->getAppUDPServerCfg().getDeviceList(),pNode->ulPhysicalAddress);	
					return TRUE;
				}
			}
		}		
	}
	return FALSE;
}



LONG CNodesFormView::OnNodeNameChanged(WPARAM wParam,LPARAM lParam)
{	
	if (m_fControlsCreated == TRUE) 
	{
		int iEntryNr;
		const BYTE bLogicalAddress = (BYTE) ((wParam & 0xFF00) >> 8);
		if (GetNodeItem(bLogicalAddress,&iEntryNr)) 
		{
			CString szNodeName;
			szNodeName.Format("%s",lParam);	
			
			if (m_pListView->SetItemText(iEntryNr,0,(LPCTSTR) szNodeName)== FALSE) 
			{
				AfxMessageBox("ERROR - Failed to edit name , nodeListiew",MB_ICONSTOP);	
				ASSERT(0);
			}
		}
		return TRUE;
	}
	return FALSE;
}


BOOL CNodesFormView::GetNodeItem(BYTE bLogicalAddress,int *iItem)
{	
	const unsigned long ulIdentifier = m_pListView->SetParamData(CMyNodeUnitListCtrl::TYPE_LIST_NODES,
																 CMyNodeUnitListCtrl::TYPE_LIST_NODE_CTRL,
																 bLogicalAddress,
																 0xFF);

	const int nCount = m_pListView->GetItemCount();
	int i;	
	for (i=0;i<nCount;i++) {
		if (ulIdentifier == m_pListView->GetItemData(i)) {
			break;
		}
	}

	if (i<nCount) 
	{
		*iItem = i;
		return TRUE;
	}

	return FALSE;		
}

void CNodesFormView::DisplayInstallationStats()
{
	CString szMessage;		
	int iNROfNodes;

	if (TMGetDocumentNodes()->NROfNodes(&iNROfNodes))
	{
		int iNROfLcds,iNROfStdNodes,iNrOfGatewayNodes;

		CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());

		NodeDatabaseOperations.GetNROfNodesType(NODE_PIN_STD_UNITS,&iNROfStdNodes);
		NodeDatabaseOperations.GetNROfNodesType(NODE_LCD,&iNROfLcds);
		NodeDatabaseOperations.GetNROfNodesType(NODE_PIN_GATEWAY,&iNrOfGatewayNodes);
	
		if (iNrOfGatewayNodes > 0)
		{
			szMessage.Format("Installation contains %02d nodes\n"
							 "%02d standard nodes,%02d gateway nodes\n"
							 "%02d LCD nodes",
							 iNROfNodes,
							 iNROfStdNodes,
							 iNrOfGatewayNodes,
							 iNROfLcds);
		}
		else
		{
			szMessage.Format("Installation contains %02d nodes\n%02d standard nodes\n%02d LCD nodes",
							iNROfNodes,
							iNROfStdNodes,
							iNROfLcds);
		}
	}

	m_StaticCtrl.SetWindowText(szMessage);
}


LONG CNodesFormView::OnApplicationClearViews(WPARAM wParam,LPARAM lParam)
{
	if (m_fControlsCreated ) {				
		if (m_pListView != NULL) {
			m_pListView->DeleteAllItems();
		}
		m_StaticCtrl.SetWindowText("");
	}
	return TRUE;
}

