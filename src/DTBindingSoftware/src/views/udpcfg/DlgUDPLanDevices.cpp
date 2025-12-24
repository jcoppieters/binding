// DlgUDPLanDevices.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgUDPLanDevices.h"
#include "MyWindowMessages.h"
#include "app/appudpcfgserver/appudpservercfg.h"
#include "uicontrols/utils/dlghelper.h"				// GUI fonts

/******************************************************************************/
using namespace DUOTECNO;
/******************************************************************************/
CDlgUDPLanDevices* CDlgUDPLanDevices::m_pInstance = NULL;
/******************************************************************************/
BOOL CDlgUDPLanDevices::Show(CWnd* pWndParent,AppUDPServerCfg& refWorker) {
	if (NULL != m_pInstance) {
		if (NULL != m_pInstance->GetSafeHwnd()) { 
			m_pInstance->SetActiveWindow();
			return TRUE;
		}
		delete m_pInstance;
		m_pInstance = NULL;
	}

	m_pInstance = new CDlgUDPLanDevices(pWndParent,refWorker);	
	if (NULL == m_pInstance) {
		ASSERT(NULL != m_pInstance);
		return FALSE;
	}
	if (m_pInstance->Create() == TRUE) {
		m_pInstance->ShowWindow(TRUE);
		return TRUE;
	}
	return FALSE;
}
CDlgUDPLanDevices* CDlgUDPLanDevices::getInstance() {
	return m_pInstance;
}
BOOL CDlgUDPLanDevices::IsActive() {
	if (NULL == m_pInstance) {
		return FALSE;
	}
	return (NULL != m_pInstance->GetSafeHwnd());
}

/******************************************************************************/

// CDlgUDPLanDevices dialog

IMPLEMENT_DYNAMIC(CDlgUDPLanDevices, CDialog)

CDlgUDPLanDevices::CDlgUDPLanDevices(CWnd* pParent,AppUDPServerCfg& refWorker)
	: CDialog(CDlgUDPLanDevices::IDD, pParent), m_refWorker(refWorker), m_pDlgFontHelper(NULL),
	  m_fAscendingOrder(FALSE), m_sortColumnIdx(0) { }

CDlgUDPLanDevices::~CDlgUDPLanDevices() { 
	if (NULL != m_pDlgFontHelper) {
		delete m_pDlgFontHelper;
		m_pDlgFontHelper = NULL;
	}
}

void CDlgUDPLanDevices::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICES, m_listDevices);
}


BEGIN_MESSAGE_MAP(CDlgUDPLanDevices, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgUDPLanDevices::OnBnClickedOk)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DEVICES, &CDlgUDPLanDevices::OnLvnColumnclickListdevices)
	ON_COMMAND(ID_CONTEXTMENU_UDPLANCFG, &CDlgUDPLanDevices::OnContextmenuUdplancfg)	
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgUDPLanDevices message handlers
BOOL CDlgUDPLanDevices::Create(){ 
	return CDialog::Create(CDlgUDPLanDevices::IDD,AfxGetMainWnd()); 
}

void CDlgUDPLanDevices::PostNcDestroy() {
	ASSERT(NULL != m_pInstance);
	delete this;
	m_pInstance = NULL;
}


BOOL CDlgUDPLanDevices::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (NULL == m_pDlgFontHelper) {
		m_pDlgFontHelper = new CDlgHelperFont(*this);	
		ASSERT(NULL != m_pDlgFontHelper);
		if (NULL != m_pDlgFontHelper) {
			m_pDlgFontHelper->initDialog();	
		}
	}

	this->CenterWindow();
	m_fAscendingOrder = FALSE;
	m_sortColumnIdx = 0;

	{		
		int nCol = 0;
		nCol = listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_NAME,"Name",120);
		ASSERT(COLUMN_DEVICE_NAME == nCol);
		nCol = listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_IPADDRESS,"IP-Address",100);
		ASSERT(COLUMN_DEVICE_IPADDRESS == nCol);
		nCol = listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_NETMASK,"Netmask",100);
		ASSERT(COLUMN_DEVICE_NETMASK == nCol);
		nCol = listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_GATEWAY,"Gateway",100);	
		ASSERT(COLUMN_DEVICE_GATEWAY == nCol);
		nCol = listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_DHCP,"DHCP",50);	
		ASSERT(COLUMN_DEVICE_DHCP == nCol);
		nCol = listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_MAC,"MAC-address",130);
		ASSERT(COLUMN_DEVICE_MAC == nCol);
		nCol = listCtrlInitAddColumn(&m_listDevices,COLUMN_DEVICE_FOUND,"Found",50);
		ASSERT(COLUMN_DEVICE_FOUND == nCol);
	}
	
	PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgUDPLanDevices::listCtrlInitAddColumn(CListCtrl* pCtrl,int columnNR,const char* szName,int sizeX) {
	int nCol = -1;
	if (pCtrl) {
		LVCOLUMN lvColumn;
		lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.cx = sizeX;
		lvColumn.pszText = (LPSTR) szName;
		nCol = pCtrl->InsertColumn(columnNR, &lvColumn);
	}
	return nCol;
}

void CDlgUDPLanDevices::OnBnClickedOk() {	
	DestroyWindow();
}

void CDlgUDPLanDevices::OnCancel() {
	DestroyWindow();
}


// Updaten van de devices.
// Controleren of een device reeds aanwezig is in de lijst.
// Wanneer nog niet aanwezig is: Entry aanmaken.
// Wanneer reeds aanwezig is: Entry updaten.
LRESULT CDlgUDPLanDevices::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	AppUDPDevices& refDevices = m_refWorker.getDeviceList();
	const int count = refDevices.getCount();
	for (int i=0;i<count;++i) {
		AppUDPDevice* pDevice = NULL;
		if (refDevices.getAt(i,&pDevice)) {
			ASSERT(NULL != pDevice);
			const int itemIndex = listCtrlFindDevice(&m_listDevices,*pDevice);
			if (itemIndex < 0) {
				listCtrlAddDevice(&m_listDevices,*pDevice);
			}
			else {
				listCtrlUpdateDevice(&m_listDevices,itemIndex,*pDevice);
			}
		}	
	}
	return TRUE;
}

int CDlgUDPLanDevices::listCtrlFindDevice(CListCtrl* pList,const AppUDPDevice& refDevice) {
	const DWORD dwMACAddress = AppUDPDevice_Utils::convertMACAddress2Uint32(refDevice); 
	const int count = pList->GetItemCount();
	for (int i=0;i<count;++i) {
		if (dwMACAddress == pList->GetItemData(i)) {
			return i;		
		}	
	}
	return -1;
}

void CDlgUDPLanDevices::listCtrlAddDevice(CListCtrl* pList,const AppUDPDevice& refDevice) {
	LVITEM lvItem;	
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	lvItem.pszText = "name"; 
	const int nItem = pList->InsertItem(&lvItem);
	ASSERT(nItem>=0);
	listCtrlHelper_setDevice(pList,nItem,refDevice);
}

void CDlgUDPLanDevices::listCtrlUpdateDevice(CListCtrl* pList,int itemIndex,const AppUDPDevice& refDevice) {
	listCtrlHelper_setDevice(pList, itemIndex, refDevice);
}

void CDlgUDPLanDevices::listCtrlHelper_setDevice(CListCtrl* pList,int nItem,const AppUDPDevice& refDevice) {
	pList->SetItemText(nItem, COLUMN_DEVICE_NAME, refDevice.getName());			
	pList->SetItemText(nItem, COLUMN_DEVICE_DHCP, (refDevice.getDhcpOnOff() == 0 ? "off" : "on"));
	pList->SetItemText(nItem, COLUMN_DEVICE_IPADDRESS, refDevice.getIpAddress());
	pList->SetItemText(nItem, COLUMN_DEVICE_NETMASK, refDevice.getNetmask());
	pList->SetItemText(nItem, COLUMN_DEVICE_GATEWAY, refDevice.getGateway());
	
	pList->SetItemText(nItem, COLUMN_DEVICE_MAC, refDevice.getMACAddress());
	pList->SetItemData(nItem, AppUDPDevice_Utils::convertMACAddress2Uint32(refDevice));	// Item data.

	pList->SetItemText(nItem, COLUMN_DEVICE_FOUND, refDevice.getLastFound() ? "yes" : "no");
}

bool CDlgUDPLanDevices::listCtrlFindSelectedDevice(CListCtrl& refCtrl,const AppUDPDevice** ppDevice) {
	bool result = false;
	POSITION p = refCtrl.GetFirstSelectedItemPosition();
	while (p) {
		const int nSelected = refCtrl.GetNextSelectedItem(p);
		if (nSelected != -1) {		
			const DWORD dwDataItem = refCtrl.GetItemData(nSelected);
			result = findDeviceByListCtrlItemData(dwDataItem,ppDevice);
			if (true == result) {
				break;	// break while loop
			}
		}			
	}		
	return result;
}

bool CDlgUDPLanDevices::findDeviceByListCtrlItemData(DWORD dwItemData,const AppUDPDevice** ppDevice) {
	bool result = false;
	AppUDPDevices& refDevices = m_refWorker.getDeviceList();
	const int count = refDevices.getCount();
	for (int i=0;i<count;++i) {
		AppUDPDevice* p = NULL;
		if (refDevices.getAt(i,&p)) {
			ASSERT(NULL != p);
			const DWORD dwMACAddress = AppUDPDevice_Utils::convertMACAddress2Uint32(*p);
			if (dwMACAddress == dwItemData) {
				*ppDevice = p;
				result = true;
				break;	// break for loop.
			}
		}
	}
	return result;
}

void CDlgUDPLanDevices::OnLvnColumnclickListdevices(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (0!=pNMLV) {	
		if (this->m_sortColumnIdx == pNMLV->iSubItem){
			if (this->m_fAscendingOrder==TRUE)
				this->m_fAscendingOrder=FALSE;
			else
				this->m_fAscendingOrder=TRUE;
		}
		else {
			this->m_sortColumnIdx=pNMLV->iSubItem;	
			this->m_fAscendingOrder=TRUE;
		}
		m_listDevices.SortItems(&CDlgUDPLanDevices::SortFunc,(LPARAM)this);
	}
	*pResult = 0;
}

int CALLBACK CDlgUDPLanDevices::SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	int retVal = 0;
	CDlgUDPLanDevices* const pThisDlg = (CDlgUDPLanDevices*) lParamSort;	
	const AppUDPDevice* pDevice1=NULL;	
	const DWORD dwItemData1=(DWORD)lParam1;
	if (pThisDlg->findDeviceByListCtrlItemData(dwItemData1,&pDevice1)) {
		const DWORD dwItemData2=(DWORD)lParam2;
		const AppUDPDevice* pDevice2=NULL;
		if (pThisDlg->findDeviceByListCtrlItemData(dwItemData2,&pDevice2)) {
			const int sortColumnIdx = pThisDlg->m_sortColumnIdx;
			switch(sortColumnIdx) {
				case COLUMN_DEVICE_NAME: {
					const CString& s1 = pDevice1->getName();
					const CString& s2 = pDevice2->getName();					
					retVal=s1.CollateNoCase(s2);					
				} break;
				case COLUMN_DEVICE_IPADDRESS: {
					const CString& s1=pDevice1->getIpAddress();
					const CString& s2=pDevice2->getIpAddress();
					retVal=s1.CollateNoCase(s2);									
				} break;
				case COLUMN_DEVICE_NETMASK: {
					const CString& s1=pDevice1->getNetmask();
					const CString& s2=pDevice2->getNetmask();
					retVal=s1.CollateNoCase(s2);													
				} break;
				case COLUMN_DEVICE_GATEWAY: {
					const CString& s1=pDevice1->getGateway();
					const CString& s2=pDevice2->getGateway();
					retVal=s1.CollateNoCase(s2);				
				} break;
				case COLUMN_DEVICE_DHCP: {
					const int v1 = pDevice1->getDhcpOnOff() == true ? 1 : 0;
					const int v2 = pDevice2->getDhcpOnOff() == true ? 1 : 0;
					if (v1 == v2) {
						retVal = 0;
					}
					else {
						retVal = (v1<v2 ? -1 : 1);
					}				
				} break;
				case COLUMN_DEVICE_MAC: {
					ASSERT(dwItemData1 != dwItemData2);
					retVal = (dwItemData1<dwItemData2 ? -1 : 1);				
				} break;
				case COLUMN_DEVICE_FOUND: {
					const int v1 = pDevice1->getLastFound() == true ? 1 : 0;
					const int v2 = pDevice2->getLastFound() == true ? 1 : 0;
					if (v1 == v2) {
						retVal = 0;
					}
					else {
						retVal = (v1<v2 ? -1 : 1);
					}							  
				} break;
			}
		}
	}	
	return (pThisDlg->m_fAscendingOrder==TRUE ? retVal : (retVal*-1));
}

void CDlgUDPLanDevices::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd == &m_listDevices)
	{	
		// Select the item the user clicked on.
		UINT uFlags;
		CPoint clPoint(point);
		m_listDevices.ScreenToClient(&clPoint);

		const int nItem = m_listDevices.HitTest(clPoint, &uFlags);		
		if (nItem != -1)
		{
			if (uFlags & LVHT_ONITEMLABEL)
			{
				CMenu mnuPopupSubmit;
				mnuPopupSubmit.LoadMenu(IDR_CONTEXTMENU_UDP);

				// Get a pointer to the first item of the menu
				CMenu *mnuPopupMenu = mnuPopupSubmit.GetSubMenu(0);
				ASSERT(mnuPopupMenu);
			
				mnuPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
	}	
}

void CDlgUDPLanDevices::OnContextmenuUdplancfg() {
	const AppUDPDevice* pDevice = NULL;
	if (listCtrlFindSelectedDevice(m_listDevices,&pDevice)) {
		ASSERT(NULL != pDevice);
		DWORD dwItemData = AppUDPDevice_Utils::convertMACAddress2Uint32(*pDevice);	
		AfxGetMainWnd()->PostMessage(WM_SHOW_NODE_UDPLANCFG,(LPARAM)(0xff),(WPARAM)dwItemData);		
	}
}

