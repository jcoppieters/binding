// PropertyPageDaliNwk.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliDevices.h"
#include "appdlgdaliworker.h"	

#include "app/appdali/appdalidevices.h"

// Child dialogs.
#include "DlgChildDaliControlDevice.h"
#include "DlgChildDaliInstances.h"
#include "DlgChildDaliControlgearProperties.h"
#include "DlgChildDaliControlgearTest.h"
#include "DlgChildDaliControlgearGroups.h"
#include "DlgChildDaliControlgearScenes.h"
#include "DlgChildDaliAddressing.h"
#include "DlgChildDaliControlDeviceOverview.h"
#include "DlgChildDaliDeviceErrors.h"
#include "DlgChildDaliControlgearExtra.h"

// Application graphics.
#include "ApplicationGraphics.h"
/******************************************************************************************/
using namespace DUOTECNO::DALI;
/******************************************************************************************/


/******************************************************************************************/
// CPropertyPageDaliNwk dialog
IMPLEMENT_DYNAMIC(CPropertyPageDaliDevices, CPropertyPage)

CPropertyPageDaliDevices::CPropertyPageDaliDevices(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext)
	: CPropertyPageDaliBaseClass(CPropertyPageDaliDevices::IDD, refWorker, refContext), m_initDone(false), m_busyEditingName(false), 
	  m_viewMode(VIEWMODE_OFFLINE), m_identifyState(IDENTIFY_STATE_DISABLED), m_pSelectedDevice(NULL),
	  m_modeTabControl(TABCONTROL_VIEWMODE_INVISIBLE), m_strNormalizedName( ), m_previousChildDialog(-1) {
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;++i) {
		this->m_pArrDlgChild[i] = NULL;
	}		
}

CPropertyPageDaliDevices::~CPropertyPageDaliDevices()
{
	// TODO: deallocDialogs gebruiken.

	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;++i) {
		if (NULL != this->m_pArrDlgChild[i]) {		
			this->m_pArrDlgChild[i]->DestroyWindow();
			delete this->m_pArrDlgChild[i];
			this->m_pArrDlgChild[i] = NULL;
		}
	}
}

void CPropertyPageDaliDevices::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DEVICES, m_treeControl);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_GROUP, m_groupDeviceInfo);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_GTIN, m_staticDeviceInfoGTIN);
	DDX_Control(pDX, IDC_STATIC_ADDRESS, m_staticDeviceAddress);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_editDeviceAddress);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_SERIAL, m_staticDeviceInfoSerialNr);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_TYPE, m_staticDeviceInfoType);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_GTIN_EDIT, m_editDeviceInfoGTIN);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_SERIAL_EDIT, m_editDeviceInfoSerialNr);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_TYPE_EDIT, m_editDeviceInfoType);
	DDX_Control(pDX, IDC_CHECK_IDENTIFY, m_btnCheckIdentify);	
	DDX_Control(pDX, IDC_TAB_DIALOGS, m_tabDialogs);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_NAME, m_staticDeviceInfoName);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_EDIT_NAME, m_editDeviceInfoName);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_DEVICETYPE, m_staticDeviceType);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_FW, m_staticDeviceFirmwareVersion);
	DDX_Control(pDX, IDC_DALI_DEVICEINFO_DALIVERSION, m_staticDeviceDaliVersion);
	DDX_Control(pDX, IDC_EDIT_DALI_DEVICEINFO_DEVICETYPE, m_editDeviceType);
	DDX_Control(pDX, IDC_EDIT_DALI_DEVICEINFO_FW, m_editDeviceFirmwareVersion);
	DDX_Control(pDX, IDC_EDIT_DALI_DEVICEINFO_DALIVERSION, m_editDeviceDaliVersion);
}

BEGIN_MESSAGE_MAP(CPropertyPageDaliDevices, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DEVICES, &CPropertyPageDaliDevices::OnTvnSelchangedTreeDevices)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_DEVICES, &CPropertyPageDaliDevices::OnTvnSelchangingTreeDevices)
	ON_BN_CLICKED(IDC_CHECK_IDENTIFY, &CPropertyPageDaliDevices::OnBnClickedCheckIdentify)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_DIALOGS, &CPropertyPageDaliDevices::OnTabChanged)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_DIALOGS, &CPropertyPageDaliDevices::OnTabChanging)
	ON_WM_TIMER()
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_DEVICES, &CPropertyPageDaliDevices::OnTvnEndlabeleditTreeDevices)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE_DEVICES, &CPropertyPageDaliDevices::OnTvnBeginlabeleditTreeDevices)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_DEVICES, &CPropertyPageDaliDevices::OnTvnKeydownTreeDevices)
	ON_EN_CHANGE(IDC_DALI_DEVICEINFO_EDIT_NAME, &CPropertyPageDaliDevices::OnEnChangeDaliDeviceinfoEditName)
	ON_EN_KILLFOCUS(IDC_DALI_DEVICEINFO_EDIT_NAME, &CPropertyPageDaliDevices::OnEnKillfocusDaliDeviceinfoEditName)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CPropertyPageDaliDevices::OnToolTipNeedText)
END_MESSAGE_MAP()

// Nodig voor de 'Want Return' van de edit box voor het editeren van de naam.
// zie Q167960: Treecontrol Nodig voor de enter & escape.
BOOL CPropertyPageDaliDevices::PreTranslateMessage(MSG* pMsg) {	
	if (NULL != pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {	
		CWnd* pWnd = GetFocus();
		if (pWnd) {
			const DWORD identifier = pWnd->GetDlgCtrlID();													
			if (IDC_DALI_DEVICEINFO_EDIT_NAME == identifier) {
				// Nodig voor de 'Want Return'. Bij het verliezen van de focus wordt de tekst upgedate.
				m_treeControl.SetFocus();
				return TRUE;
			}		      
			else {
				// zie Q167960
				CEdit* edit = m_treeControl.GetEditControl();
				if (edit) {
					edit->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
					return TRUE;
				}
			}
		}
	}
    return CPropertyPage::PreTranslateMessage(pMsg);    
}

// CPropertyPageDaliDevices message handlers
BOOL CPropertyPageDaliDevices::OnInitDialog() {
	CPropertyPage::OnInitDialog();		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	this->initDialog();

	// Vorige tab...

	return TRUE;
}

#if(USE_DEALLOC_DIALOGS == 1)
void CPropertyPageDaliDevices::deallocDialogs() {
	for (int i = 0; i < DIALOG_CHILD_MAX_DIALOGS; ++i) {
		if (NULL != this->m_pArrDlgChild[i]) {
			this->m_pArrDlgChild[i]->DestroyWindow();
			delete this->m_pArrDlgChild[i];
			this->m_pArrDlgChild[i] = NULL;
		}
	}
}
#endif 

void CPropertyPageDaliDevices::setTabCtrlModeControlDevice(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	if (TABCONTROL_VIEWMODE_CONTROLDEVICE != this->m_modeTabControl) {
#if(USE_DEALLOC_DIALOGS == 1)
		this->deallocDialogs();
#endif
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL] =  new CDlgChildDaliControlDevice(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL]->Create(CDlgChildDaliControlDevice::IDD,&m_tabDialogs);
		}		
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLDEVICE_OVERVIEW]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLDEVICE_OVERVIEW] =  new CDlgChildDaliControlDeviceOverview(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLDEVICE_OVERVIEW]->Create(CDlgChildDaliControlDeviceOverview::IDD,&m_tabDialogs);
		}	
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_INSTANCES]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_INSTANCES] =  new CDlgChildDaliInstances(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_INSTANCES]->Create(CDlgChildDaliInstances::IDD,&m_tabDialogs);
		}
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING] =  new CDlgChildDaliAddressing(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING]->Create(CDlgChildDaliAddressing::IDD,&m_tabDialogs);
		}

		// 
		m_tabDialogs.DeleteAllItems();	
		m_tabDialogs.ShowWindow(TRUE);	
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,0,"General",0,DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL,0,0);
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,1,"Overview",0,DIALOG_CHILD_DALI_CONTROLDEVICE_OVERVIEW,0,0);
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,2,"Instances",0,DIALOG_CHILD_DALI_INSTANCES,0,0);
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,3,"Addressing",0,DIALOG_CHILD_DALI_ADDRESSING,0,0);		
		this->m_modeTabControl = TABCONTROL_VIEWMODE_CONTROLDEVICE;
		this->OnTabChanged(NULL, NULL);
		m_tabDialogs.SetCurSel(0);
	}

	// Dynamic properties.
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;++i) {
		if ((DIALOG_CHILD_DALI_CONTROLDEVICE_OVERVIEW == i) ||
			(DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL == i) ||
			(DIALOG_CHILD_DALI_INSTANCES == i) ||
			(DIALOG_CHILD_DALI_ADDRESSING == i)) 
		{
			CDlgChildDali* const pDlg = m_pArrDlgChild[i];
			if (NULL != pDlg) {
				pDlg->init(pDevice);
			}	
		}
	}
}

void CPropertyPageDaliDevices::setTabCtrlModeControlGear(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	if (TABCONTROL_VIEWMODE_CONTROLGEAR != this->m_modeTabControl) {	
#if(USE_DEALLOC_DIALOGS == 1)
		this->deallocDialogs();
#endif
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_TEST]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_TEST] =  new CDlgChildDaliControlgearTest(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_TEST]->Create(CDlgChildDaliControlgearTest::IDD,&m_tabDialogs);
		}		
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_PROPERTIES]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_PROPERTIES] =  new CDlgChildDaliControlgearProperties(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_PROPERTIES]->Create(CDlgChildDaliControlgearProperties::IDD,&m_tabDialogs);
		}	
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_GROUPS]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_GROUPS] =  new CDlgChildDaliControlgearGroups(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_GROUPS]->Create(CDlgChildDaliControlgearGroups::IDD,&m_tabDialogs);
		}
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_EXTRA]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_EXTRA] =  new CDlgChildDaliControlgearExtra(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_EXTRA]->Create(CDlgChildDaliControlgearExtra::IDD,&m_tabDialogs);
		}
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_SCENES]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_SCENES] =  new CDlgChildDaliControlgearScenes(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_CONTROLGEAR_SCENES]->Create(CDlgChildDaliControlgearScenes::IDD,&m_tabDialogs);
		}
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING] =  new CDlgChildDaliAddressing(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING]->Create(CDlgChildDaliAddressing::IDD,&m_tabDialogs);
		}

		m_tabDialogs.DeleteAllItems();	
		m_tabDialogs.ShowWindow(TRUE);	
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,0,"Quick test",0,DIALOG_CHILD_DALI_CONTROLGEAR_TEST,0,0);
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,1,"Properties",0,DIALOG_CHILD_DALI_CONTROLGEAR_PROPERTIES,0,0);		
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,2,"Groups",0,DIALOG_CHILD_DALI_CONTROLGEAR_GROUPS,0,0);
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,3,"DT6 Extra",0,DIALOG_CHILD_DALI_CONTROLGEAR_EXTRA,0,0);
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,4,"Scenes",0,DIALOG_CHILD_DALI_CONTROLGEAR_SCENES,0,0);
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,5,"Addressing",0,DIALOG_CHILD_DALI_ADDRESSING,0,0);
		this->m_modeTabControl = TABCONTROL_VIEWMODE_CONTROLGEAR;
		this->OnTabChanged(NULL, NULL);
		m_tabDialogs.SetCurSel(0);
	}

	// Dynamic properties.	

	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;++i) {
		if ((DIALOG_CHILD_DALI_CONTROLGEAR_TEST == i) ||
			(DIALOG_CHILD_DALI_CONTROLGEAR_PROPERTIES == i) ||			
			(DIALOG_CHILD_DALI_CONTROLGEAR_GROUPS == i) ||
			(DIALOG_CHILD_DALI_CONTROLGEAR_EXTRA == i) ||
			(DIALOG_CHILD_DALI_CONTROLGEAR_SCENES == i) ||
			(DIALOG_CHILD_DALI_ADDRESSING == i)) {
			CDlgChildDali* const pDlg = m_pArrDlgChild[i];
			if (NULL != pDlg) {
				pDlg->init(pDevice);
			}	
		}
	}
}

void CPropertyPageDaliDevices::setTabCtrlModeControlInvisible( ) {
	if (TABCONTROL_VIEWMODE_INVISIBLE == this->m_modeTabControl) {
		return;
	}
#if(USE_DEALLOC_DIALOGS == 1)
	this->deallocDialogs();
#endif 
	m_tabDialogs.DeleteAllItems();	
	m_tabDialogs.ShowWindow(FALSE);	
	this->m_modeTabControl = TABCONTROL_VIEWMODE_INVISIBLE;
	this->OnTabChanged(NULL, NULL);
}

void CPropertyPageDaliDevices::setTabCtrlModeControlStatusError(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	if (TABCONTROL_VIEWMODE_STATUS_ERROR != this->m_modeTabControl) {	
#if(USE_DEALLOC_DIALOGS == 1)
		this->deallocDialogs();
#endif 
		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_DEVICE_ERRORINFO]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_DEVICE_ERRORINFO] =  new CDlgChildDaliDeviceErrors(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_DEVICE_ERRORINFO]->Create(CDlgChildDaliDeviceErrors::IDD,&m_tabDialogs);
		}

		if (NULL == this->m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING]) {
			this->m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING] =  new CDlgChildDaliAddressing(this->getWorker(),this);
			m_pArrDlgChild[DIALOG_CHILD_DALI_ADDRESSING]->Create(CDlgChildDaliAddressing::IDD,&m_tabDialogs);
		}

		m_tabDialogs.DeleteAllItems();	
		m_tabDialogs.ShowWindow(TRUE);	
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,0,"Device error",0,DIALOG_CHILD_DALI_DEVICE_ERRORINFO,0,0);
		m_tabDialogs.InsertItem(TCIF_TEXT | TCIF_PARAM,1,"Addressing",0,DIALOG_CHILD_DALI_ADDRESSING,0,0);
		this->m_modeTabControl = TABCONTROL_VIEWMODE_STATUS_ERROR;
		this->OnTabChanged(NULL, NULL);
		m_tabDialogs.SetCurSel(0);
	}

	// Dynamic properties.	
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;++i) {
		if ((DIALOG_CHILD_DALI_DEVICE_ERRORINFO == i) || 
			(DIALOG_CHILD_DALI_ADDRESSING == i)) {
			CDlgChildDali* const pDlg = m_pArrDlgChild[i];
			if (NULL != pDlg) {
				pDlg->init(pDevice);
			}	
		}
	}
}

BOOL CPropertyPageDaliDevices::OnSetActive() {
	if (this->getContext().getFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_DEVICES)) {
		this->deallocDialogs();
		this->initDialog();		
		this->getContext().clearFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_DEVICES);
	}
	if (false == this->m_initDone) {
		this->initTree();
		this->updateTree( );
		this->m_initDone = true;
	}


	// Doorgeven aan de child dialogen dat de parent actief geworden is.
	// Zoeken naar het tabblad die momenteel aktief en melden...
	const int currentActiveDialog = tabDialogsGetCurrSelectedDialog(m_tabDialogs);	
	for (int i = 0; i < DIALOG_CHILD_MAX_DIALOGS; i++) {
		if (NULL != this->m_pArrDlgChild[i]) {
			const bool activeDialog = (currentActiveDialog == i ? true : false);		
			this->m_pArrDlgChild[i]->OnParentPropertyPageActive(this->m_pSelectedDevice,activeDialog);
		}
	}

	//m_dwWizardButtons = 0xFFFFFF;
	//PostMessage(WM_UPDATE_CONTROLS);
	return CPropertyPage::OnSetActive();
}

BOOL CPropertyPageDaliDevices::OnKillActive() {
	// Doorgeven aan de child dialogen dat de parent actief geworden is.
	for (int i = 0; i < DIALOG_CHILD_MAX_DIALOGS; i++) {
		if (NULL != this->m_pArrDlgChild[i]) {
			// Return waarde TRUE is aanpassingen gebeurd, vragen om up te daten (momenteel enkel namen).
			if (this->m_pArrDlgChild[i]->OnParentPropertyPageLeave()) {
				this->setNotifyGuiNamesChanged();
			}
		}
	}
	// m_dwWizardButtons = 0xFFFFFF;
	return CPropertyPage::OnKillActive();
}

void CPropertyPageDaliDevices::initDialog() {
	if (this->getWorker().isConnected()) {
		if (this->getWorker().getDaliCfgTaskBusy()) {
			this->m_viewMode = VIEWMODE_ONLINE_TASK_BUSY;
		}
		else {
			this->m_viewMode = VIEWMODE_ONLINE_IDLE;
		}
	}
	else {
		this->m_viewMode = VIEWMODE_OFFLINE;
	}

	this->m_initDone = false;
	this->m_busyEditingName = false;
	this->m_modeTabControl = TABCONTROL_VIEWMODE_CONTROLGEAR;		// Om zeker een update te hebben...
	this->showDeviceInfo(NULL);
	this->showTabCtrlDeviceInfo(NULL);
	this->m_pSelectedDevice = NULL;
	this->m_identifyState = IDENTIFY_STATE_DISABLED;
	this->EnableToolTips(TRUE);
	this->updateStateBtnIdentify();	
}

void CPropertyPageDaliDevices::initTree() {
	ASSERT(false == this->m_initDone);

	CApplicationGraphics* const pAppGraphics = CApplicationGraphics::GetInstance();
	if (NULL == pAppGraphics) {
		ASSERT(NULL != pAppGraphics);
		return;
	}

	this->m_treeControl.SetImageList(&pAppGraphics->GetSmallDaliDeviceStatusImageList(), TVSIL_NORMAL);
	this->m_treeControl.SetTextColor(RGB(0,0,255));
}

// Callback functie om te sorteren.
static int CALLBACK TreeCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to the tree control.
	// The lParam of an item is just its handle, 
	// as specified with SetItemData
	DWORD dwData[2] = { 0, 0 };
	CTreeCtrl* pmyTreeCtrl = (CTreeCtrl*)lParamSort;
	if ((NULL == pmyTreeCtrl) || (NULL == pmyTreeCtrl->GetSafeHwnd())) {
		return 0;
	}
	if (lParam1 < lParam2)
		return -1;
	if (lParam1 > lParam2)
		return 1;

	return 0;
}

void CPropertyPageDaliDevices::sortTree() {
	TVSORTCB tvs;
	tvs.hParent = TVI_ROOT;
	tvs.lpfnCompare = TreeCompareProc;
	tvs.lParam = (LPARAM)&this->m_treeControl;
	this->m_treeControl.SortChildrenCB(&tvs);
}

void CPropertyPageDaliDevices::updateTree() {
	if (this->m_viewMode == VIEWMODE_ONLINE_TASK_BUSY) {
		return;
	}

	// Alle devices wissen.
	m_treeControl.DeleteAllItems();

	// Alle devices updaten.
	CAppDaliDevices* pDevices = this->getWorker().getDaliDevices();
	if (NULL == pDevices) {
		ASSERT(NULL != pDevices);
		return;
	}

	const int count = pDevices->getCount();
	for (int i=0;i<count;++i) {
		CAppDaliDevice* pDevice = NULL;
		if (pDevices->get(i,&pDevice)) {
			HTREEITEM parentNode;
			ASSERT(NULL != pDevice);
			this->addDevice(TVI_ROOT,&parentNode,*pDevice);
		}	
	}	
	this->sortTree();

	// Sedert V17.05: Bij het aanmaken van de tree altijd het eerste item selecteren.
	HTREEITEM hItem = m_treeControl.GetRootItem(); 
	if (NULL != hItem) {
		m_treeControl.SelectItem(hItem);
	}	
}


// Bit 0...7:	= shortAddress.
// Bit 8: 0 = controlGear, 1= controldevice

static unsigned long convert2DataItem(DaliDeviceType_t type,int shortAddress) {
	unsigned long retValue = 0;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == type) {
		retValue = shortAddress;
	}
	else {
		retValue = shortAddress + (1<<8);
	}
	return retValue;
}

static unsigned long convert2DataItem(DUOTECNO::DALI::CAppDaliDevice& refDevice) {
	return convert2DataItem(refDevice.getType(),refDevice.getShortAddress());
}

static bool convertFromDataItem(unsigned long dataItem, int* type, int* shortAddress) {
	if (dataItem & (1<<8)) {
		*type = DALI_DEVICE_TYPE_INPUTCONTROL;
	}
	else {
		*type = DALI_DEVICE_TYPE_CONTROLGEAR;
	}
	*shortAddress = (dataItem & 0xff);
	return true;
}

bool CPropertyPageDaliDevices::formatTreeItemDeviceName(DUOTECNO::DALI::CAppDaliDevice& refDevice,CString* pStr) {
	REQUIRE(NULL != pStr);
	const int shortAddress = refDevice.getShortAddress();
	const DaliDeviceType_t type = refDevice.getType();
	return this->formatTreeItemDeviceName(type,shortAddress,pStr);
}

bool CPropertyPageDaliDevices::formatTreeItemDeviceName(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress,CString* pStr) {
	REQUIRE(NULL != pStr);

	CString strAddress;
	if (!this->getWorker().formatStringAddress(type,shortAddress,&strAddress)) {
		ASSERT(0);
		return false;
	}

	CString strNameDevice;
	if (!this->getWorker().getDeviceName(type,shortAddress,&strNameDevice)) {
		ASSERT(0);
		return false;	
	}

	ASSERT(!strAddress.IsEmpty());
	CString strLabel;
	if (strNameDevice.IsEmpty()) {
		strLabel = strAddress;
	}
	else {
		strLabel = strAddress + " - " + strNameDevice;
	}
	*pStr = strLabel;
	return true;
}

bool CPropertyPageDaliDevices::formatTreeItemDeviceName(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress,const CString& strNewName,CString* pStr) {
	REQUIRE(NULL != pStr);

	CString strAddress;
	if (!this->getWorker().formatStringAddress(type,shortAddress,&strAddress)) {
		ASSERT(0);
		return false;
	}

	ASSERT(!strAddress.IsEmpty());
	CString strLabel;
	if (strNewName.IsEmpty()) {
		strLabel = strAddress;
	}
	else {
		strLabel = strAddress + " - " + strNewName;
	}
	*pStr = strLabel;
	return true;
}

static bool parseAddress(const CString& str,CString* pStrAddress) {
	if (str.IsEmpty() || NULL == pStrAddress) {	
		return false;
	}

	if (str.GetAt(0) != 'A') {
		return false;
	}

	const int indexInputDevice = str.Find('²');		
	if (indexInputDevice >= 2 && indexInputDevice <=3) {	
		int tempValue = -1;
		if (sscanf((LPCTSTR) str,"A%d² -",&tempValue) >= 1) {
			if (tempValue >= 0 && tempValue <= 63) {
				pStrAddress->Format("A%d³",tempValue);				
			}
		}
	}
	else {
		int tempValue = -1;
		if (sscanf((LPCTSTR) str,"A%d -",&tempValue) >= 1) {
			if (tempValue >= 0 && tempValue <= 63) {
				pStrAddress->Format("A%d",tempValue);				
			}
		}
	}
	return (pStrAddress->IsEmpty() ? false : true);
}


// Src string strippen van Address string...
bool CPropertyPageDaliDevices::parseTreeItemDeviceLabelName(LPSTR srcString,CString* pStrAddress,CString* pStrDeviceName) {
	if (NULL == srcString || NULL == pStrAddress || NULL == pStrDeviceName) {
		REQUIRE(0);
		return false;
	}

	// 0123456789
	// A0³ -
	// A63² -	
	const CString src(srcString);
	if (false == parseAddress(src,pStrAddress)) {
		*pStrDeviceName = src;
		return true;
	}
	const int length = src.GetLength();
	int index = pStrAddress->GetLength();
	if (++index < length) {
		if (src.GetAt(index) == ' ') {
			++index;
		}
		if ((index < length) && (src.GetAt(index) == '-')) {
			++index;
		}
	}
	if (pStrAddress->IsEmpty()) {	
		*pStrDeviceName = src; 
		pStrDeviceName->TrimLeft();
		pStrDeviceName->TrimRight();
		return true;
	}
	if (index >= src.GetLength()) {
		*pStrDeviceName = src; 
		return true;
	}
	*pStrDeviceName = src.Mid(index,src.GetLength());
	pStrDeviceName->Trim();
	return true;
}

void CPropertyPageDaliDevices::setTreeItemLabelText(HTREEITEM hItem,const CString& strText) {
	if (NULL == hItem) {
		REQUIRE(0);
		return;
	}
	if (strText.IsEmpty()) {
		return;
	}
	this->m_treeControl.SetItemText(hItem,strText);
}

void CPropertyPageDaliDevices::addDevice(HTREEITEM parent,HTREEITEM *pNewParent,DUOTECNO::DALI::CAppDaliDevice& refDevice) {	

	CString strLabelText;
	if (!this->formatTreeItemDeviceName(refDevice,&strLabelText)) {
		ASSERT(0);
		return;
	}

	int imageIdentifier = ICON_DALIDEVICE_STATUS_NORMAL;
	DaliDeviceErrorStatus_t deviceStatus = refDevice.getErrorStatus();
	switch(deviceStatus) {
		case ERROR_STATUS_NONE: {
			imageIdentifier = ICON_DALIDEVICE_STATUS_NORMAL; 		
		} break;
		case ERROR_STATUS_PENDING: {
			imageIdentifier = ICON_DALIDEVICE_STATUS_ERROR;		   
		} break;
/*
		case ERROR_STATUS_UNKNOWN: {
			imageIdentifier = ICON_DALIDEVICE_STATUS_UNKNOWN;							   
		} break;
*/
	}

	TVITEM tvItem;	
	tvItem.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM; 
	tvItem.pszText = LPTSTR((LPCTSTR) strLabelText);		
	tvItem.cchTextMax = strLabelText.GetLength();
	tvItem.lParam = convert2DataItem(refDevice);
	tvItem.iImage = imageIdentifier;
	tvItem.iSelectedImage = imageIdentifier;

	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = parent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item= tvItem;

	*pNewParent = m_treeControl.InsertItem(&tvInsert);
}

void CPropertyPageDaliDevices::updateStateBtnIdentify( ) {
	bool stateEnabled = false;
	bool stateChecked = false;
	bool stateVisible = false;

	if ((NULL != this->m_pSelectedDevice) && (this->m_viewMode == VIEWMODE_ONLINE_IDLE)) {
		stateEnabled = true;
	}
	if (IDENTIFY_STATE_DISABLED != this->m_identifyState) {
		stateChecked = true;
	}
	if ((m_treeControl.GetCount() > 0) && (NULL != this->m_pSelectedDevice)) {
		stateVisible = true;
	}

	this->m_btnCheckIdentify.SetCheck(stateChecked == true ? 1 : 0);
	this->m_btnCheckIdentify.EnableWindow(stateEnabled == true ? 1 : 0);
	this->m_btnCheckIdentify.ShowWindow(stateVisible == true ? 1 : 0);
}

void CPropertyPageDaliDevices::showTabCtrlDeviceInfo(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	if (NULL == pDevice) {
		this->setTabCtrlModeControlInvisible( );
		return;
	}

	if (ERROR_STATUS_NONE != pDevice->getErrorStatus()) {
		this->setTabCtrlModeControlStatusError(pDevice);
	}
	else if (DALI_DEVICE_TYPE_CONTROLGEAR == pDevice->getType() ){
		this->setTabCtrlModeControlGear(pDevice);
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == pDevice->getType() ){
		this->setTabCtrlModeControlDevice(pDevice);
	}
}

void CPropertyPageDaliDevices::showDeviceInfo(DUOTECNO::DALI::CAppDaliDevice* pDevice) {

	this->m_busyEditingName = false;

	// ShowWindow visible....
	m_groupDeviceInfo.ShowWindow(NULL != pDevice);
	m_staticDeviceAddress.ShowWindow(NULL != pDevice);
	m_staticDeviceInfoGTIN.ShowWindow(NULL != pDevice);
	m_staticDeviceInfoSerialNr.ShowWindow(NULL != pDevice);
	m_staticDeviceInfoType.ShowWindow(NULL != pDevice);
	m_editDeviceAddress.ShowWindow(NULL != pDevice);
	m_editDeviceInfoGTIN.ShowWindow(NULL != pDevice);
	m_editDeviceInfoSerialNr.ShowWindow(NULL != pDevice);
	m_editDeviceInfoType.ShowWindow(NULL != pDevice);
	m_staticDeviceInfoName.ShowWindow(NULL != pDevice);
	m_editDeviceInfoName.ShowWindow(NULL != pDevice);
	m_staticDeviceType.ShowWindow(NULL != pDevice);
	m_staticDeviceFirmwareVersion.ShowWindow(NULL != pDevice);
	m_staticDeviceDaliVersion.ShowWindow(NULL != pDevice);
	m_editDeviceType.ShowWindow(NULL != pDevice);
	m_editDeviceFirmwareVersion.ShowWindow(NULL != pDevice);
	m_editDeviceDaliVersion.ShowWindow(NULL != pDevice);


	CString strGTIN;
	CString strSerialNumber;
	CString strType;
	CString strDeviceAddress;
	CString strDeviceName;
	CString strDeviceType;
	CString strDeviceFirmware;
	CString strDeviceDaliVersion;

	if (NULL != pDevice) {			
		{
			char strTemp[64];
			if (CAppDaliDevice::getStrGTIN(*pDevice,strTemp,sizeof(strTemp))) {
				strGTIN = strTemp;
			}
		}
		{
			char strTemp[64];
			if (CAppDaliDevice::getStrSerialCode(*pDevice,strTemp,sizeof(strTemp))) {
				strSerialNumber = strTemp;
			}
		}
		const DaliDeviceType_t type = pDevice->getType();
		if (DALI_DEVICE_TYPE_CONTROLGEAR == type) {
			strType = "Control gear";
			strDeviceAddress.Format("A%d",pDevice->getShortAddress());
		}
		else {
			strType = "Control device";
			strDeviceAddress.Format("A%d²",pDevice->getShortAddress());
		}			
		CString strName;
		if (this->getWorker().getDeviceName(type,pDevice->getShortAddress(),&strName)) {
			strDeviceName = strName;
		}

		// DALI Versie nummer V1 of V2 kan niet direct opgevraagd worden via het protocol.
		// Control input devices zijn altijd V2.
		// Control gear devices kunnen we enkel bepalen of deze antwoorden op een query die enkel beschikbaar is in DALI-2.
		INVARIANT(NULL != pDevice);
		const int versionNumber = CAppDaliDevice_Utils::supportDALIVersion2(*pDevice) ? 2 : 1;
		strDeviceDaliVersion.Format("%d",versionNumber);

		// Device type is enkel aanwezig bij de control gear...
		// Sedert V16.48: Devices kunnen meerdere types ondersteunen.
		if (DALI_DEVICE_TYPE_CONTROLGEAR == type) {
			CAppDaliDeviceProperties_ControlGear* const pControlGearProperties = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&pDevice->getProperties());
			if (NULL != pControlGearProperties) {
				int deviceType = pControlGearProperties->getDeviceType();
				if (deviceType >= 0 && deviceType<0xff) {
					strDeviceType.Format("%d",deviceType);
				}
				else if (deviceType == 0xff) {
					// Indien het deviceType==255 worden er soms meerdere types ondersteund.
					const CList<int>& listDeviceTypes = pControlGearProperties->getDeviceTypes();
					POSITION pos = listDeviceTypes.GetHeadPosition();
					while(pos!=NULL) {	
						int v = listDeviceTypes.GetNext(pos);
						if (v>=0 && v<0xff) {
							CString strTemp;
							strTemp.Format("%d ",v);
							strDeviceType += strTemp;						
						}
					}								
				}
			}
		}	

		int firmwareVersion = pDevice->getVersionFirmware();
		if (firmwareVersion >=0) {
			strDeviceFirmware.Format("V%x.%x",(firmwareVersion & 0xFF00)>>8, firmwareVersion & 0xFF);
		}		
	}


	if (strDeviceType.IsEmpty()) {
		strDeviceType = "N/A";
	}

	m_editDeviceInfoGTIN.SetWindowText(strGTIN);
	m_editDeviceInfoSerialNr.SetWindowText(strSerialNumber);
	m_editDeviceInfoType.SetWindowText(strType);
	m_editDeviceAddress.SetWindowText(strDeviceAddress);
	m_editDeviceInfoName.SetWindowText(strDeviceName);
	m_editDeviceType.SetWindowText(strDeviceType);
	m_editDeviceFirmwareVersion.SetWindowText(strDeviceFirmware);
	m_editDeviceDaliVersion.SetWindowText(strDeviceDaliVersion);
}

// Het geselecteerde item wordt getoond in bold in de treeview.
static void updateTreeControlSelection(CTreeCtrl* pCtrl,LPNMTREEVIEW pNMTreeView) {
	TVITEMA pNewTvItem = pNMTreeView->itemNew;	
	HTREEITEM const pTreeNewSelectedItem = pNewTvItem.hItem;
	if (NULL != pTreeNewSelectedItem) {
		 pCtrl->SetItemState(pTreeNewSelectedItem, TVIS_BOLD | TVIS_SELECTED, TVIS_BOLD | TVIS_SELECTED);
	}	

	TVITEMA pOldTvItem = pNMTreeView->itemOld;	
	HTREEITEM const pTreeOldSelectedItem = pOldTvItem.hItem;
	if (NULL != pTreeOldSelectedItem) {
		 pCtrl->SetItemState(pTreeOldSelectedItem, 0, TVIS_BOLD | TVIS_SELECTED);
	}	
}

BOOL CPropertyPageDaliDevices::requestChildDialogCanLeave( ) {
	BOOL continueLeave = TRUE;
	const int m_paramPageLeaving = tabDialogsGetCurrSelectedDialog(m_tabDialogs);
	if (m_paramPageLeaving >= 0) {
		ASSERT(m_paramPageLeaving < DIALOG_CHILD_MAX_DIALOGS);
		if (NULL != this->m_pArrDlgChild[m_paramPageLeaving]) {						
			if (FALSE == this->m_pArrDlgChild[m_paramPageLeaving]->onTabDialogCanLeaveDialog()) {
				continueLeave = FALSE;
			}
		}
	}
	return continueLeave;
}

// Deze handler wordt aangeroepen voor er een nieuw item geselecteerd wordt.
void CPropertyPageDaliDevices::OnTvnSelchangingTreeDevices(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = ((this->requestChildDialogCanLeave() == TRUE) ? 0 : 1);		// Result 1 wil zeggen dat er niet mag verlaten worden
}

void CPropertyPageDaliDevices::OnTvnSelchangedTreeDevices(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (NULL == pNMTreeView) {
		return;
	}

	if (this->m_busyEditingName) {
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
		this->GetEditBoxName();
	}

	// Bij de selectie van een ander device de Identify stoppen.
	this->handleIdentifyFsm(IDENTIFY_EVENT_SET_OFF);

	updateTreeControlSelection(&m_treeControl,pNMTreeView);
	
	CAppDaliDevice* pDevice = NULL;
	int t = -1;
	int shortAddress = -1;		
	if (convertFromDataItem(pNMTreeView->itemNew.lParam,&t,&shortAddress)) {		
		const DaliDeviceType_t type = (DaliDeviceType_t) t;		// TODO <------------	ANDERE MANIER
		CAppDaliDevices* pDevices = this->getWorker().getDaliDevices();
		if (NULL != pDevices) {				
			pDevices->find(type, shortAddress, &pDevice);								
		}
	}	
	this->m_pSelectedDevice = pDevice;
	this->showDeviceInfo(pDevice);
	this->showTabCtrlDeviceInfo(pDevice);
	this->updateStateBtnIdentify();	
	this->m_treeControl.SetFocus();		// Focus moet in de treeCtrl blijven.
	*pResult = 0;
}

void CPropertyPageDaliDevices::OnBnClickedCheckIdentify() {
	if (0 == m_btnCheckIdentify.GetCheck()) {
		this->handleIdentifyFsm(IDENTIFY_EVENT_SET_OFF);
	}
	else {
		this->handleIdentifyFsm(IDENTIFY_EVENT_SET_ON);
	}
}

void CPropertyPageDaliDevices::OnTimer(UINT nIDEvent) {
	if (TIMER_ID_IDENTIFY == nIDEvent) {
		this->handleIdentifyFsm(IDENTIFY_EVENT_TIMEREXPIRY);
	}
	else if (TIMER_ID_EDITBOX_NAME == nIDEvent) {
		this->GetEditBoxName( );
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
	}
	else if (TIMER_ID_EDITBOX_NAME_UPDATE == nIDEvent) {
		// De naam updaten van de edit box.
		this->m_editDeviceInfoName.SetWindowText(this->m_strNormalizedName);
		this->KillTimer(TIMER_ID_EDITBOX_NAME_UPDATE);
	}
}

// Sedert V16.41:
// Controleren of er een algoritme bezig is.
// Wanneer er een algoritme bezig is dan worden de identify berichten niet verstuurd omdat de toetand van de FSM/algoritmes
// kan gewijzigd worden door de TX-done events. (zie ook USE_WORKAROUND_IGNORE_TXDONE_EVENT_IDENTIFY_BUSY)
void CPropertyPageDaliDevices::handleIdentifyFsm(IdentifyEvent_t ev) {
	enum { IDENTIFY_TIMER_INTERVAL_MSEC = 1000 };

	switch(this->m_identifyState) {
		case IDENTIFY_STATE_DISABLED: {
			switch(ev) {
				case IDENTIFY_EVENT_SET_ON: {
					if (this->getWorker().getDaliCfgTaskBusy()) {
						this->SetTimer(TIMER_ID_IDENTIFY, IDENTIFY_TIMER_INTERVAL_MSEC, (TIMERPROC) NULL);
						this->m_identifyState = IDENTIFY_STATE_ENABLED_TOGGLE_STATE_OFF;
					}
					else {
						this->sendIdentify(true);
						this->SetTimer(TIMER_ID_IDENTIFY, IDENTIFY_TIMER_INTERVAL_MSEC, (TIMERPROC) NULL);
						this->m_identifyState = IDENTIFY_STATE_ENABLED_TOGGLE_STATE_ON;
					}
				} break;
			}	  
		} break;

		case IDENTIFY_STATE_ENABLED_TOGGLE_STATE_ON: {
			switch(ev) {										 
				case IDENTIFY_EVENT_TIMEREXPIRY: {
					if (!this->getWorker().getDaliCfgTaskBusy()) {
						this->sendIdentify(false);
						this->m_identifyState = IDENTIFY_STATE_ENABLED_TOGGLE_STATE_OFF;
					}
				} break;

				case IDENTIFY_EVENT_SET_OFF: {
					this->sendIdentify(false);
					this->KillTimer(TIMER_ID_IDENTIFY);
					this->m_identifyState = IDENTIFY_STATE_DISABLED;
				} break;
			}
		} break;

		case IDENTIFY_STATE_ENABLED_TOGGLE_STATE_OFF: {
			switch(ev) {										 
				case IDENTIFY_EVENT_TIMEREXPIRY: {
					if (!this->getWorker().getDaliCfgTaskBusy()) {
						this->sendIdentify(true);
						this->m_identifyState = IDENTIFY_STATE_ENABLED_TOGGLE_STATE_ON;
					}
				} break;

				case IDENTIFY_EVENT_SET_OFF: {
					this->sendIdentify(false);
					this->KillTimer(TIMER_ID_IDENTIFY);
					this->m_identifyState = IDENTIFY_STATE_DISABLED;
				} break;
			}
		} break;
	}
}

void CPropertyPageDaliDevices::sendIdentify(bool stateToggleOnOff) {
	if (NULL == this->m_pSelectedDevice) {
		return;
	}
	if (!this->getWorker().isConnected()) {
		return;
	}
	const int shortAddress = this->m_pSelectedDevice->getShortAddress();
	if (DALI_DEVICE_TYPE_INPUTCONTROL == this->m_pSelectedDevice->getType()) {
		this->getWorker().sendInputDeviceIdentify(shortAddress);
	}
	else{
		if (stateToggleOnOff) {
			this->getWorker().sendDeviceMaxLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE,shortAddress);
		}
		else {			
			this->getWorker().sendDeviceOff(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE,shortAddress);
		}
	}
}

/***********************************************************************************/
// IAppDlgDaliGui_Host	

void CPropertyPageDaliDevices::onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) {

	if (statusConnected) {
		if (this->m_viewMode == VIEWMODE_OFFLINE) {
			this->m_viewMode = VIEWMODE_ONLINE_IDLE;
		}
	}
	else {
		this->m_viewMode = VIEWMODE_OFFLINE;
	}

	// Updaten van de child dialogen..
	for (int i = 0; i < DIALOG_CHILD_MAX_DIALOGS; i++) {
		if (NULL != this->m_pArrDlgChild[i]) {
			this->m_pArrDlgChild[i]->onNotifyGuiStatusConnected(bNodeAddress,statusConnected);
		}
	}

	this->updateStateBtnIdentify();
}

// Treeview en de devices wissen bij alle addressings taken.
// Nodig omdat er andere commandos kunnen verstuurd worden die de algoritmes verstoren...
void CPropertyPageDaliDevices::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {

	this->m_viewMode = VIEWMODE_ONLINE_TASK_BUSY;

	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || APPDALI_FSM_TASK_COMMISSIONING_NEW == task || APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task) {				
		this->m_treeControl.DeleteAllItems();
		this->m_pSelectedDevice = NULL;
		this->showDeviceInfo(NULL);		
		this->showTabCtrlDeviceInfo(NULL);
		this->handleIdentifyFsm(IDENTIFY_EVENT_SET_OFF);
		this->updateStateBtnIdentify();	
	}

	// Updaten van de child dialogen..
	for (int i = 0; i < DIALOG_CHILD_MAX_DIALOGS; i++) {
		if (NULL != this->m_pArrDlgChild[i]) {
			this->m_pArrDlgChild[i]->onNotifyGuiTaskStarted(bNodeAddress,task);
		}
	}
}

void CPropertyPageDaliDevices::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {

	if (VIEWMODE_ONLINE_TASK_BUSY == this->m_viewMode) {
		this->m_viewMode = VIEWMODE_ONLINE_IDLE;
	}

	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task || 
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task || 
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task) {		
		this->updateTree( );
		this->updateStateBtnIdentify();		
	}

	if (APPDALI_FSM_TASK_ADDRESSING_SINGLE_DEVICE == task ||
		APPDALI_FSM_TASK_ADDRESSING_ERASE_DEVICE == task ||
		APPDALI_FSM_TASK_ADDRESSING_SWAP_DEVICES == task) 
	{	
		// Groups overview updaten wanneer er adressen gewijzijgd werden.
		this->getContext().setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_GROUPS);

		// Dialoog aanpassen.
		this->m_pSelectedDevice = NULL;
		this->showDeviceInfo(NULL);		
		this->showTabCtrlDeviceInfo(NULL);
		this->handleIdentifyFsm(IDENTIFY_EVENT_SET_OFF);
		this->updateStateBtnIdentify();	
		this->updateTree( );
	}

	// Updaten van de child dialogen..
	for (int i = 0; i < DIALOG_CHILD_MAX_DIALOGS; i++) {
		if (NULL != this->m_pArrDlgChild[i]) {
			this->m_pArrDlgChild[i]->onNotifyGuiTaskDone(bNodeAddress,task);
		}
	}
}

void CPropertyPageDaliDevices::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {

	// Updaten van de child dialogen..
	for (int i = 0; i < DIALOG_CHILD_MAX_DIALOGS; i++) {
		if (NULL != this->m_pArrDlgChild[i]) {
			this->m_pArrDlgChild[i]->onNotifyGuiTaskError(bNodeAddress,task,strMessage);
		}
	}
}

/***********************************************************************************************/
void CPropertyPageDaliDevices::OnTabChanging(NMHDR *pNMHDR, LRESULT *pResult) {
	*pResult = ((this->requestChildDialogCanLeave() == TRUE) ? 0 : 1);		// Result 1 wil zeggen dat er niet mag verlaten worden.
}


void CPropertyPageDaliDevices::OnTabChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	DialogID_t dialogID = DIALOG_CHILD_NONE;
	const int nIndex = m_tabDialogs.GetCurSel();		
	switch(this->m_modeTabControl) {		
		case TABCONTROL_VIEWMODE_CONTROLDEVICE: {
			dialogID = DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL;
			if (0 == nIndex) {	
				dialogID = DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL; 
			}
			else if (1 == nIndex) {	
				dialogID = DIALOG_CHILD_DALI_CONTROLDEVICE_OVERVIEW;
			}
			else if (2 == nIndex) {	 
				dialogID = DIALOG_CHILD_DALI_INSTANCES;
			}
			else {	 
				dialogID = DIALOG_CHILD_DALI_ADDRESSING;
			}		
		} break;	

		case TABCONTROL_VIEWMODE_INVISIBLE: {
			dialogID = DIALOG_CHILD_NONE;			
		} break;

		case TABCONTROL_VIEWMODE_CONTROLGEAR: {
			dialogID = DIALOG_CHILD_DALI_CONTROLGEAR_TEST;
			if (0 == nIndex) {	
				dialogID = DIALOG_CHILD_DALI_CONTROLGEAR_TEST;
			}
			else if (1 == nIndex) {
				dialogID = DIALOG_CHILD_DALI_CONTROLGEAR_PROPERTIES; 				
			}
			else if (2 == nIndex) {
				dialogID = DIALOG_CHILD_DALI_CONTROLGEAR_GROUPS;
			}
			else if (3 == nIndex) {
				dialogID = DIALOG_CHILD_DALI_CONTROLGEAR_EXTRA;
			}
			else if (4 == nIndex) {
				dialogID = DIALOG_CHILD_DALI_CONTROLGEAR_SCENES;
			}
			else {	 
				dialogID = DIALOG_CHILD_DALI_ADDRESSING;
			}			
		} break;

		case TABCONTROL_VIEWMODE_STATUS_ERROR: {
			dialogID = DIALOG_CHILD_DALI_DEVICE_ERRORINFO;
			if (0 == nIndex) {	
				dialogID = DIALOG_CHILD_DALI_DEVICE_ERRORINFO;
			}
			else {
				dialogID = DIALOG_CHILD_DALI_ADDRESSING;									   
			}
		} break;
	}
	
	// Vorige actieve dialog: On Leave
	if (this->m_previousChildDialog >= 0 && this->m_previousChildDialog < DIALOG_CHILD_MAX_DIALOGS) {	
		//if (this->m_previousChildDialog != dialogID) {
		if (NULL != this->m_pArrDlgChild[this->m_previousChildDialog]) {
			this->m_pArrDlgChild[this->m_previousChildDialog]->onTabDialogLeaveDialog();
		}
		//}	
	}

	// Nieuwe dialog actief zetten + OnEnter()
	this->showActiveDaliog(dialogID);

	// Bewaren van de vorige dialoog.
	this->m_previousChildDialog = (int) dialogID;
}

void CPropertyPageDaliDevices::showActiveDaliog(DialogID_t dialog) {
	for (int i=0;i<DIALOG_CHILD_MAX_DIALOGS;i++) {
		if ((i != dialog) && (NULL != this->m_pArrDlgChild[i])) {
			this->m_pArrDlgChild[i]->ShowWindow(SW_HIDE);		
		}
	}

	if (dialog >= DIALOG_CHILD_MAX_DIALOGS) {
		return;
	}

	CDlgChildDali* pDlg = this->m_pArrDlgChild[(int)dialog];
	ASSERT(NULL != pDlg);
	if (NULL == pDlg) {
		return;
	}
	
	RECT rc;
	m_tabDialogs.GetItemRect(0,&rc);
	pDlg->SetWindowPos( NULL, rc.left + 1, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	pDlg->SetFocus();			
	pDlg->onTabDialogEnterDialog();
}


void CPropertyPageDaliDevices::OnTvnEndlabeleditTreeDevices(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	*pResult = 0;		// not handles.

	if (NULL == pTVDispInfo) {
		return;
	}
	
	TVITEMA ptvItem = pTVDispInfo->item;		
	if (ptvItem.pszText == NULL) {			
		return;
	}
		
	const unsigned long ulIdentifier = ptvItem.lParam;
	int type = -1;
	int shortAddress = -1;
	if (!convertFromDataItem(ulIdentifier, &type, &shortAddress)) {				
		return;
	}

	ASSERT(type == DALI_DEVICE_TYPE_CONTROLGEAR || type == DALI_DEVICE_TYPE_INPUTCONTROL);
	ASSERT(shortAddress >= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST && shortAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST);

	CString strDeviceName;
	CString strAddress;					
	if (!this->getWorker().getDeviceName((DaliDeviceType_t) type, shortAddress, &strDeviceName)) {
		ASSERT(0);
		return;
	}										
	if (!this->getWorker().formatStringAddress((DaliDeviceType_t) type,shortAddress,&strDeviceName)) {
		ASSERT(0);
		return;
	}
				
	CString strDummyAddress;
	CString strNewDeviceName;
	if (false == this->parseTreeItemDeviceLabelName(ptvItem.pszText,&strDummyAddress,&strNewDeviceName)) {
		ASSERT(0);
		return;				
	}
	if (0 == strNewDeviceName.Compare(strDeviceName)) {
		return;
	}

	CString strNormalizedName;
	if (!this->setDatabaseUpdateName((DaliDeviceType_t) type,shortAddress,strNewDeviceName,&strNormalizedName)) {
		return;
	}
					
	CString strNewLabelText;
	if (!this->formatTreeItemDeviceName((DaliDeviceType_t) type,shortAddress,strNormalizedName,&strNewLabelText)) {					
		return;
	}						
							
	this->setTreeItemLabelText(ptvItem.hItem, strNewLabelText);
	this->m_editDeviceInfoName.SetWindowText(strNormalizedName);
	this->setNotifyGuiNamesChanged( );																
	*pResult = 0;
}

bool CPropertyPageDaliDevices::setDatabaseUpdateName(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress,const CString& strName,
													 CString *pStrNormalizedName) {	
	if (FALSE == this->getWorker().setDeviceName(type,shortAddress,strName,pStrNormalizedName)) {
		return false;
	}	
	return true;
}

void CPropertyPageDaliDevices::OnTvnBeginlabeleditTreeDevices(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);	
	*pResult = 0;
}

// afhandelen F2 en Enter toets.
void CPropertyPageDaliDevices::OnTvnKeydownTreeDevices(NMHDR *pNMHDR, LRESULT *pResult) {
	enum { VK_ENTER	= 13 };
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	if (NULL != pTVKeyDown) {
		HTREEITEM hItem = m_treeControl.GetSelectedItem();
		if (VK_F2 == pTVKeyDown->wVKey) {			
			if (NULL != hItem) {
				m_treeControl.EditLabel(hItem);
			}
		}
#if(0)
		else if (VK_ENTER == pTVKeyDown->wVKey) {
			hItem = m_treeControl.GetNextVisibleItem(hItem);						
			if (hItem) {
				m_treeControl.SelectItem(hItem);						
				m_treeControl.EditLabel(hItem);
			}
			else { // end of tree
				hItem = m_treeControl.GetFirstVisibleItem( );
				if (hItem) {
					m_treeControl.SelectItem(hItem);					
					m_treeControl.EditLabel(hItem);
				}
			}
		}
#endif
	}
	*pResult = 0;
}

void CPropertyPageDaliDevices::OnEnChangeDaliDeviceinfoEditName() {
	this->m_busyEditingName = true;
	this->KillTimer(TIMER_ID_EDITBOX_NAME);
	this->SetTimer(TIMER_ID_EDITBOX_NAME, 2000, (TIMERPROC) NULL);
}

void CPropertyPageDaliDevices::OnEnKillfocusDaliDeviceinfoEditName() {
	if (this->m_busyEditingName) {
		this->KillTimer(TIMER_ID_EDITBOX_NAME);
		this->GetEditBoxName();		
	}
}

void CPropertyPageDaliDevices::GetEditBoxName( ) {
	if (!this->m_busyEditingName) {
		return;
	}
	this->m_busyEditingName = false;

	if (NULL == this->m_pSelectedDevice) {
		// ASSERT(NULL != this->m_pSelectedDevice);
		return;
	}
	CString strNewName;
	const CString strOldName = this->m_pSelectedDevice->getName();
	this->m_editDeviceInfoName.GetWindowText(strNewName);
	if (0 == strNewName.Compare(strOldName)) {
		return;
	}
	const DUOTECNO::DALI::DaliDeviceType_t type = this->m_pSelectedDevice->getType();
	const int shortAddress = this->m_pSelectedDevice->getShortAddress();

	CString strNormalizedName;
	if (FALSE == this->getWorker().setDeviceName(type,shortAddress,strNewName,&strNormalizedName)) {
		return;
	}
	this->updateTreeItemDeviceName(type,shortAddress);	
	this->setNotifyGuiNamesChanged( );

	// Onderstaande is nodig om de aangepaste tekst opnieuw weer te geven in de edit box.
	// Kunnen dit niet rechtreeks in deze functie.
	// Doen dit assync via deze timer...
	if (0 != strNewName.Compare(strNormalizedName)) {
		this->m_strNormalizedName = strNormalizedName;
		this->SetTimer(TIMER_ID_EDITBOX_NAME_UPDATE,100,(TIMERPROC)NULL);
	}
}

bool CPropertyPageDaliDevices::find(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress,HTREEITEM& hTreeItem) {
	const unsigned long identifier = convert2DataItem(type,shortAddress);
	HTREEITEM hCurrent = m_treeControl.GetNextItem(TVI_ROOT, TVGN_ROOT);
	while (hCurrent != NULL) {							
		if (m_treeControl.GetItemData(hCurrent) == identifier)  {			
			hTreeItem = hCurrent;
			return true;
		}
		hCurrent = m_treeControl.GetNextItem(hCurrent, TVGN_NEXT);
	}
	return false;
}

void CPropertyPageDaliDevices::updateTreeItemDeviceName(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress) {
	HTREEITEM hTreeItem = NULL;
	if (!this->find(type,shortAddress,hTreeItem)) {
		ASSERT(0);
		return;
	}
	CString strName;
	if (false == this->getWorker().getDeviceName(type,shortAddress,&strName)) {
		ASSERT(0);
		return;
	}
	CString strLabel;
	if (!this->formatTreeItemDeviceName(type,shortAddress,strName,&strLabel)) {
		ASSERT(0);
		return;
	}
	ASSERT(NULL != hTreeItem);
	m_treeControl.SetItemText(hTreeItem,strLabel);
}

void CPropertyPageDaliDevices::setNotifyGuiNamesChanged( ) {
	CPropertyPageDaliContext& refContext = this->getContext();
	refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_DEVICES);
	refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_LOCATE);
	refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_GROUPS);	
}

int CPropertyPageDaliDevices::tabDialogsGetCurrSelectedDialog(CTabCtrl& r) {
	const int nNewSel = r.GetCurSel();
	if (nNewSel <  0) {
		return -1;
	}	
	TCITEM item;
	item.lParam = 0;
	item.mask = TCIF_PARAM;
	if (FALSE == r.GetItem(nNewSel, &item)) {  		
		return -1;
	}	
	const LPARAM lParamValue = item.lParam;
	if ((lParamValue >=0) && (lParamValue > DIALOG_CHILD_MAX_DIALOGS)) {
		ASSERT(lParamValue >= 0);
		ASSERT(lParamValue < DIALOG_CHILD_MAX_DIALOGS);
		return -1;
	}	
	return (int) lParamValue;				
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CPropertyPageDaliDevices::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}

	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_TREE_DEVICES: {
			this->m_strToolTipText = _T("List of all DALI devices found in the installation");
			handled = TRUE;			   
		} break;
		case IDC_DALI_DEVICEINFO_GTIN_EDIT: {
			this->m_strToolTipText = _T("The DALI device's Global Trade Item Number (GTIN)");
			handled = TRUE;			   
		} break;
		case IDC_DALI_DEVICEINFO_SERIAL_EDIT: {
			this->m_strToolTipText = _T("The DALI device's serial number");
			handled = TRUE;			   
		} break;
		case IDC_DALI_DEVICEINFO_TYPE_EDIT: {
			this->m_strToolTipText = _T("The type of DALI device (Controlgear or Control input device)");
			handled = TRUE;			   
		} break;
		case IDC_CHECK_IDENTIFY: {
			this->m_strToolTipText = _T("Check this box to identify/find a DALI device");
			handled = TRUE;			   
		} break;
		case IDC_EDIT_ADDRESS: {
			this->m_strToolTipText = _T("The DALI shortaddress (Range: 0-63)");
			handled = TRUE;			   
		} break;
		case IDC_DALI_DEVICEINFO_EDIT_NAME: {
			this->m_strToolTipText = _T("Enter the DALI device's name");
			handled = TRUE;			   
		} break;
		case IDC_EDIT_DALI_DEVICEINFO_DEVICETYPE: {
			this->m_strToolTipText = _T("The DALI device type (DT) number");
			handled = TRUE;			   
		} break;
		case IDC_EDIT_DALI_DEVICEINFO_FW: {
			this->m_strToolTipText = _T("The DALI device firmware version");
			handled = TRUE;			   
		} break;
		case IDC_EDIT_DALI_DEVICEINFO_DALIVERSION: {
			this->m_strToolTipText = _T("The DALI version information");
			handled = TRUE;			   
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
