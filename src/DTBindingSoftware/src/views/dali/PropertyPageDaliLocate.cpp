// PropertyPageDaliLocate.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliLocate.h"
#include "appdlgdaliworker.h"	
#include "app/appdali/appdali_def.h"				// Nodig voor de enum van de instance types (filters).
#include "utils/mfc/controls_util.h"
/************************************************************************************************/
#undef DALI_LOCATE_SIMULATE_ENTRIES
#ifdef _DEBUG
	#define DALI_LOCATE_SIMULATE_ENTRIES			1
#endif
/************************************************************************************************/
using namespace DUOTECNO::DALI;
using namespace DUOTECNO::MFC_HELPER;
/************************************************************************************************/
enum { WM_UPDATE_CONTROLS = (WM_USER + 150) };
/************************************************************************************************/
static const uint32_t ITEMDATA_VALUE_SEPERATOR = 0xffffff;
static uint32_t convertToItemData(int shortAddress, int instanceNumber) {
	if (shortAddress < 0 || shortAddress > 0xFF || instanceNumber < 0 || instanceNumber > 0xff) {
		return ITEMDATA_VALUE_SEPERATOR;
	}
	return ((shortAddress << 8) + (instanceNumber));
}

static BOOL convertFromItemData(uint32_t data,int* shortAddress,int* instanceNumber) {
	if (ITEMDATA_VALUE_SEPERATOR == data) {
		*shortAddress = -1;
		*instanceNumber = -1;
		return FALSE;
	}	
	*shortAddress = (data & 0xFF00) >> 8;
	*instanceNumber = (data & 0xFF);
	return TRUE;
}

static int getShortAddressFromItemData(uint32_t data) {
	return ((data & 0xFF00) >> 8);
}

/*********************************************************************************************/

// CPropertyPageDaliLocate dialog
IMPLEMENT_DYNAMIC(CPropertyPageDaliLocate, CPropertyPage)

CPropertyPageDaliLocate::CPropertyPageDaliLocate(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext)
	: CPropertyPageDaliBaseClass(CPropertyPageDaliLocate::IDD, refWorker, refContext), m_viewMode(VIEWMODE_OFFLINE),
	  m_deviceShortAddress(-1), m_deviceInstanceNumber(-1), m_currentDaliInterfaceNodeAddress(-1), m_strNameDevice( ), m_strNameInstance( ), m_busyDeviceNameEditing(false),
	  m_busyInstanceNameEditing(false), m_initListBoxCtrlDone(false), m_updateListCtrlDeviceName(false), m_updateListCtrlInstanceName(false), 
	  m_updateDeviceNameEditBox(false), m_updateInstanceNameEditBox(false), m_flagsEventFilter(0) , m_stateLiveViewPaused(FALSE), m_showTopIndex(true), m_flagInitDone(FALSE)
{ }

CPropertyPageDaliLocate::~CPropertyPageDaliLocate()
{
}


void CPropertyPageDaliLocate::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_btnClear);
	DDX_Control(pDX, IDC_BUTTON_ADDLINE, m_btnAddLine);
	DDX_Control(pDX, IDC_STATIC_TEXT_CONNECT_INFO, m_staticConnectInfo);
	DDX_Control(pDX, IDC_STATIC_GROUP_DEVICE, m_staticGroupDevice);	
	DDX_Control(pDX, IDC_STATIC_DEVICE_ADDRESS, m_staticDeviceAddress);	
	DDX_Control(pDX, IDC_STATIC_DEVICE_NAME, m_staticDeviceName);	
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_editDeviceAddress);	
	DDX_Control(pDX, IDC_EDIT_DEVICE_NAME, m_editDeviceName);	
	DDX_Control(pDX, IDC_EDIT_NUMBER, m_editInstanceNumber);	
	DDX_Control(pDX, IDC_EDIT_INSTANCE_NAME, m_editInstanceName);	
	DDX_Control(pDX, IDC_STATIC_INSTANCE_NAME, m_staticInstanceName);	
	DDX_Control(pDX, IDC_LISTCTRL_EVENTS, m_listCtrlEvents);
	DDX_Control(pDX, IDC_STATIC_DEVICE_DISPLAY_NAME, m_staticDisplayName);
	DDX_Control(pDX, IDC_EDIT_DEVICE_DISPLAY_NAME, m_editDisplayName);
	DDX_Control(pDX, IDC_CHECK_FILTER_PUSHBUTTON, m_btnCheckFilterPushBtn);
	DDX_Control(pDX, IDC_CHECK_FILTER_OCCUPANCY, m_btnCheckFilterOccupancy);
	DDX_Control(pDX, IDC_CHECK_FILTER_UNSUPPORTED, m_btnCheckFilterUnsupported);
	DDX_Control(pDX, IDC_CHECK_FILTER_EVENTSCHEM, m_btnCheckFilterEventScheme);
	DDX_Control(pDX, IDC_CHECK_FILTER_LOCATED, m_btnCheckFilterLocatedInstances);
	DDX_Control(pDX, IDC_CHECK_TOPINDEX, m_btnCheckTopIndex);
	DDX_Radio(pDX, IDC_RADIO_RUNNING, m_stateLiveViewPaused);
	DDX_Control(pDX, IDC_STATIC_ICON_OFFLINE, m_staticIconWarningOffline);
	DDX_Control(pDX, IDC_STATIC_TEXT_OFFLINE, m_staticTextWarningOffline);
}

BEGIN_MESSAGE_MAP(CPropertyPageDaliLocate, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CPropertyPageDaliLocate::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_ADDLINE, &CPropertyPageDaliLocate::OnBnClickedButtonAddline)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)	
	ON_EN_CHANGE(IDC_EDIT_DEVICE_NAME, &CPropertyPageDaliLocate::OnEnChangeEditDeviceName)
	ON_EN_KILLFOCUS(IDC_EDIT_DEVICE_NAME, &CPropertyPageDaliLocate::OnEnKillfocusEditDeviceName)
	ON_EN_CHANGE(IDC_EDIT_INSTANCE_NAME, &CPropertyPageDaliLocate::OnEnChangeEditInstanceName)
	ON_EN_KILLFOCUS(IDC_EDIT_INSTANCE_NAME, &CPropertyPageDaliLocate::OnEnKillfocusEditInstanceName)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL_EVENTS, &CPropertyPageDaliLocate::OnLvnItemchangedListctrlEvents)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LISTCTRL_EVENTS, &CPropertyPageDaliLocate::OnNMKillfocusListctrlEvents)
	ON_NOTIFY(NM_SETFOCUS, IDC_LISTCTRL_EVENTS, &CPropertyPageDaliLocate::OnNMSetfocusListctrlEvents)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LISTCTRL_EVENTS, &CPropertyPageDaliLocate::OnNMCustomdrawListCtrl)
	ON_BN_CLICKED(IDC_CHECK_FILTER_PUSHBUTTON, &CPropertyPageDaliLocate::OnBnClickedCheckFilterPushbutton)
	ON_BN_CLICKED(IDC_CHECK_FILTER_OCCUPANCY, &CPropertyPageDaliLocate::OnBnClickedCheckFilterOccupancy)
	ON_BN_CLICKED(IDC_CHECK_FILTER_UNSUPPORTED, &CPropertyPageDaliLocate::OnBnClickedCheckFilterUnsupported)
	ON_BN_CLICKED(IDC_CHECK_FILTER_EVENTSCHEM, &CPropertyPageDaliLocate::OnBnClickedCheckFilterEventschem)
	ON_BN_CLICKED(IDC_CHECK_FILTER_LOCATED, &CPropertyPageDaliLocate::OnBnClickedCheckFilterLocated)
	ON_BN_CLICKED(IDC_RADIO_RUNNING, &CPropertyPageDaliLocate::OnBnClickedRadioRunning)
	ON_BN_CLICKED(IDC_RADIO_PAUSED, &CPropertyPageDaliLocate::OnBnClickedRadioPaused)
	ON_BN_CLICKED(IDC_CHECK_TOPINDEX, &CPropertyPageDaliLocate::OnBnClickedCheckTopindex)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CPropertyPageDaliLocate::OnToolTipNeedText)
END_MESSAGE_MAP()


#if(0)
void CPropertyPageDaliLocate::initListCtrlSubClass( ) {
	if (false == this->m_initListBoxCtrlDone) {
		HWND hWndListBox = GetDlgItem(IDC_LISTCTRL_EVENTS)->m_hWnd;
		if (NULL != hWndListBox) {
			this->m_listCtrlEvents.SubclassWindow(hWndListBox);
		}     
        this->m_initListBoxCtrlDone = true;
    }
}
#endif 


BOOL CPropertyPageDaliLocate::OnInitDialog() {
		
#if(0)
	this->initListCtrlSubClass( );	// Oppassen moet voor de CPropertyPage::OnInitDialog()
#endif 

	CPropertyPage::OnInitDialog();	

	this->m_stateLiveViewPaused = FALSE;
	this->m_flagsEventFilter = (EVENT_FILTER_FLAG_EVENTS_TYPE_PUSHBUTTON + 
							    EVENT_FILTER_FLAG_EVENTS_TYPE_EVENTSCHEME_UNSUPPORTED + 
								EVENT_FILTER_FLAG_EVENTS_TYPE_LOCATED_INSTANCES);
	this->m_showTopIndex = true;

	this->initListCtrlEvents();
	this->initDialog();			
	return TRUE;
}

BOOL CPropertyPageDaliLocate::OnSetActive() {
	const BOOL result = CPropertyPage::OnSetActive();
	if (this->getContext().getFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_LOCATE)) {
		this->helper_clearDataName();
		this->initDialog();		
		this->getContext().clearFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_LOCATE);
	}
	return result;
}

// Afhandelen van de enter toets.
// - Als de focus op een edit control staat, stuur Enter niet door		
// - Als de focus op de naam van het device staat gaan we naar de naam van de instance.
// - Als de focus op de naam van de instance staat gaan we naar de listbox van de events.
BOOL CPropertyPageDaliLocate::PreTranslateMessage(MSG* pMsg) {
	// Controleren of het bericht van de Enter-toets komt in een edit control
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		CWnd* pWnd = GetFocus();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CEdit))) {			
			if (IDC_EDIT_DEVICE_NAME == pWnd->GetDlgCtrlID()) {
				this->m_editInstanceName.SetFocus();
			}
			else {				
				this->m_listCtrlEvents.SetFocus();
			}
			return TRUE; // Negeer het bericht
		}
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CPropertyPageDaliLocate::initDialog() {
	if (this->getWorker().isConnected()) {
		this->m_viewMode = VIEWMODE_ONLINE;
	}
	else {
		this->m_viewMode = VIEWMODE_OFFLINE;
	}
	this->m_busyDeviceNameEditing = false;
	this->m_busyInstanceNameEditing = false;

	if (this->m_currentDaliInterfaceNodeAddress != this->getWorker().getCurrentNodeAddress()) {
		this->m_listCtrlEvents.DeleteAllItems();
		this->dbg_listCtrlAddDummyEvents();			// zie DALI_LOCATE_SIMULATE_ENTRIES
		this->m_currentDaliInterfaceNodeAddress = this->getWorker().getCurrentNodeAddress();
	}
	else {
		this->updateListCtrlEvents( );
	}
	this->EnableToolTips(TRUE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageDaliLocate::initListCtrlEvents( ) {
	int nCol = -1;
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listCtrlEvents,COLUMN_EVENT_TIME,"time",60);
	ASSERT(COLUMN_EVENT_TIME == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listCtrlEvents,COLUMN_DEVICE_ADDRESS,"A²",40);
	ASSERT(COLUMN_DEVICE_ADDRESS == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listCtrlEvents,COLUMN_DEVICE_INSTANCE_ADSRESS,"#I",30);
	ASSERT(COLUMN_DEVICE_INSTANCE_ADSRESS == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listCtrlEvents,COLUMN_DEVICE_NAME,"Device name",115);
	ASSERT(COLUMN_DEVICE_NAME == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listCtrlEvents,COLUMN_DEVICE_INSTANCE_NAME,"Name",60);
	ASSERT(COLUMN_DEVICE_INSTANCE_NAME == nCol);
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listCtrlEvents,COLUMN_INSTANCE_TYPE,"Type",100);
	ASSERT(COLUMN_INSTANCE_TYPE == nCol);		
	nCol = MFCGuiHelper_ListCtrl::listCtrlInitAddColumn(&m_listCtrlEvents,COLUMN_EVENT_NAME,"Event",185);
	ASSERT(COLUMN_EVENT_NAME == nCol);	

	// LVS_EX_FULLROWSELECT style zorgt ervoor dat de volledige rij geselecteerd is.
	const DWORD dwExtendendStyle = LVS_EX_FULLROWSELECT; 
	m_listCtrlEvents.SetExtendedStyle(m_listCtrlEvents.GetExtendedStyle() | dwExtendendStyle);
}

void CPropertyPageDaliLocate::dbg_listCtrlAddDummyEvents( ) {
#if (DALI_LOCATE_SIMULATE_ENTRIES == 1)

	static int dbgCounter = 0;

	int shortAddress = 63;
	int maxDevices = 12;
	int maxInstances = 6;

	if (1 == (dbgCounter % 2)) {
		shortAddress = 32;
		maxDevices = 2;
		maxInstances = 4;
	}
	else if (0 == (dbgCounter % 2)) {
		shortAddress = 63;
		maxDevices = 12;
		maxInstances = 6;
	}

	const CTime time(CTime::GetCurrentTime());
	const CString strTime = time.Format("%H:%M:%S");			
	for (int j = 0; j < maxDevices; ++j) {	
		for (int instanceNumber = 0; instanceNumber < maxInstances; ++instanceNumber) {
			CString strDeviceName; 
			CString strInstanceName;
			CString strEventMsg("Puls");
			CString strInstanceType("Pushbutton");
			strDeviceName.Format("Device A%d²",shortAddress);
			strInstanceName.Format("#I%d",instanceNumber);
			this->listCtrlEventsAddMessage(strTime, shortAddress, instanceNumber, strDeviceName, strInstanceName, strEventMsg, strInstanceType);
		}
		shortAddress--;
	}

	dbgCounter++;
	
#endif
}

void CPropertyPageDaliLocate::updateListCtrlEvents( ) {

	m_listCtrlEvents.SetRedraw(FALSE);

	int retVal = -1;
	const int numberOfItems = m_listCtrlEvents.GetItemCount();
	for (int i=0; i < numberOfItems; i++) {
		int shortAddress = -1;
		int instanceNumber = -1;	
		if (convertFromItemData(m_listCtrlEvents.GetItemData(i),&shortAddress,&instanceNumber)) {		
			CString strDeviceName;
			CString strInstanceName;
			if (this->getWorker().getDeviceName(DALI_DEVICE_TYPE_INPUTCONTROL,shortAddress,&strDeviceName)) {
				m_listCtrlEvents.SetItemText(i,COLUMN_DEVICE_NAME,strDeviceName);
			}
			if (this->getWorker().getControlDeviceInstanceName(shortAddress,instanceNumber,&strInstanceName)) {
				m_listCtrlEvents.SetItemText(i,COLUMN_DEVICE_INSTANCE_NAME,strInstanceName);
			}
		}
	}
	m_listCtrlEvents.SetRedraw(TRUE);
	m_listCtrlEvents.UpdateWindow( );
}

void CPropertyPageDaliLocate::listCtrlEventsAddMessage(const CString& strTime,int deviceAddress, int instanceAddress, const CString& strDeviceName, 
													   const CString& strInstanceName, const CString& strEvent,const CString& strInstanceType)  {
	const DWORD dwItemData = convertToItemData(deviceAddress,instanceAddress);

	CString strAddress("?");
	CString strAddressInstance("?");
	if (deviceAddress >= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST && deviceAddress <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
		if (!this->getWorker().formatStringAddress(DALI_DEVICE_TYPE_INPUTCONTROL , deviceAddress, &strAddress)) {
			ASSERT(0);
		}
	}
	if (instanceAddress >=0 && instanceAddress<=31) {
		strAddressInstance.Format("%d",instanceAddress);
	}

	const int itemIndex = m_listCtrlEvents.GetItemCount();
	LVITEM lvItem;	
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = itemIndex;				// Op het einde van de lijst bij voegen..
	lvItem.iSubItem = 0;
	lvItem.pszText = "name"; 
	const int nItem = m_listCtrlEvents.InsertItem(&lvItem);
	ASSERT(nItem>=0);	
	m_listCtrlEvents.SetItemText(nItem, COLUMN_EVENT_TIME, strTime);	
	m_listCtrlEvents.SetItemText(nItem, COLUMN_DEVICE_ADDRESS, strAddress);			
	m_listCtrlEvents.SetItemText(nItem, COLUMN_DEVICE_INSTANCE_ADSRESS, strAddressInstance);
	m_listCtrlEvents.SetItemText(nItem, COLUMN_DEVICE_NAME, strDeviceName);
	m_listCtrlEvents.SetItemText(nItem, COLUMN_DEVICE_INSTANCE_NAME, strInstanceName);
	m_listCtrlEvents.SetItemText(nItem, COLUMN_INSTANCE_TYPE, strInstanceType);
	m_listCtrlEvents.SetItemText(nItem, COLUMN_EVENT_NAME, strEvent);
	m_listCtrlEvents.SetItemData(nItem, dwItemData);

	this->listCtrlSetTopIndex( );
}

void CPropertyPageDaliLocate::listCtrlEventsAddSeperator() {
	const DWORD dwItemData = convertToItemData(-1,-1);
	static const CString strEmpty("-");

	const int itemIndex = m_listCtrlEvents.GetItemCount();
	LVITEM lvItem;	
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = itemIndex;				// Op het einde van de lijst bij voegen..
	lvItem.iSubItem = 0;
	lvItem.pszText = "name"; 
	const int nItem = m_listCtrlEvents.InsertItem(&lvItem);
	ASSERT(nItem>=0);	
	m_listCtrlEvents.SetItemText(nItem, COLUMN_EVENT_TIME, strEmpty);	
	m_listCtrlEvents.SetItemText(nItem, COLUMN_DEVICE_ADDRESS, strEmpty);			
	m_listCtrlEvents.SetItemText(nItem, COLUMN_DEVICE_INSTANCE_ADSRESS, strEmpty);
	m_listCtrlEvents.SetItemText(nItem, COLUMN_DEVICE_NAME, strEmpty);
	m_listCtrlEvents.SetItemText(nItem, COLUMN_DEVICE_INSTANCE_NAME, strEmpty);
	m_listCtrlEvents.SetItemText(nItem, COLUMN_INSTANCE_TYPE, strEmpty);
	m_listCtrlEvents.SetItemText(nItem, COLUMN_EVENT_NAME, strEmpty);
	m_listCtrlEvents.SetItemData(nItem, dwItemData);

	this->listCtrlSetTopIndex( );
}

void CPropertyPageDaliLocate::listCtrlSetTopIndex( ) {
	if (this->m_showTopIndex) {
		MFCGuiHelper_ListCtrl::setTopIndex(&m_listCtrlEvents);
	}
}


void CPropertyPageDaliLocate::listCtrlUpdateDeviceName(int shortAddress,const CString& strName) {
	int retVal = -1;
	const int numberOfItems = this->m_listCtrlEvents.GetItemCount();
	for (int i=0; i < numberOfItems; i++) {
		const int dwItemData = this->m_listCtrlEvents.GetItemData(i);
		if (ITEMDATA_VALUE_SEPERATOR != dwItemData) {
			if (shortAddress == getShortAddressFromItemData(dwItemData)) {			
				this->m_listCtrlEvents.SetItemText(i,COLUMN_DEVICE_NAME, strName);			
			}	
		}
	}
}

void CPropertyPageDaliLocate::listCtrlUpdateDeviceInstanceName(int shortAddress,int instanceNumber,const CString& strName) {
	const DWORD dwItemData = convertToItemData(shortAddress,instanceNumber);
	int retVal = -1;
	const int numberOfItems = this->m_listCtrlEvents.GetItemCount();
	for (int i=0; i < numberOfItems; i++) {		
		if (dwItemData == this->m_listCtrlEvents.GetItemData(i)) {					
			this->m_listCtrlEvents.SetItemText(i,COLUMN_DEVICE_INSTANCE_NAME, strName);							
		}
	}
}

void CPropertyPageDaliLocate::setDisplayDeviceInstanceName( ) {
	CString str;
	if (!this->m_strNameDevice.IsEmpty()) {
		str = this->m_strNameDevice;
		if (!this->m_strNameInstance.IsEmpty()) {
			str += " " + this->m_strNameInstance;
		}
	}
	else {
		str = this->m_strNameInstance;
	}		
	ASSERT(str.GetLength() < 16);		// 15 char + een null char.
	this->m_editDisplayName.SetWindowText(str);
}


void CPropertyPageDaliLocate::initStaticIconWarning( ) {
	HICON hIconWarning = (HICON)LoadImage(
		NULL,
		MAKEINTRESOURCE(IDI_WARNING),
		IMAGE_ICON,
		32, 32,
		LR_SHARED
	);
	if (NULL != hIconWarning) {
		this->m_staticIconWarningOffline.SetIcon(hIconWarning);
	}		
}

LONG CPropertyPageDaliLocate::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

#if (DALI_LOCATE_SIMULATE_ENTRIES == 1)
	const BOOL flagShowEventControls = TRUE;
#else 
	const BOOL flagShowEventControls = this->m_viewMode == VIEWMODE_ONLINE ? TRUE : FALSE;
#endif 
	
	const BOOL flagShowDeviceNameControls = TRUE; 
	const BOOL flagControlsOnlineMode = this->m_viewMode == VIEWMODE_ONLINE ? TRUE : FALSE;

	if (FALSE == this->m_flagInitDone) {	
		this->initStaticIconWarning( );
		m_editDisplayName.EnableWindow(FALSE);				// zorgt ervoor dat deze geen focus kan krijgen.
		m_editInstanceNumber.EnableWindow(FALSE);			// zorgt ervoor dat deze geen focus kan krijgen.
		m_editDeviceAddress.EnableWindow(FALSE);			// zorgt ervoor dat deze geen focus kan krijgen.
		this->m_flagInitDone = TRUE;
	}
	this->m_listCtrlEvents.EnableWindow(TRUE);	
	this->m_staticConnectInfo.ShowWindow(!flagShowEventControls);

	CString strDeviceAddress;
	CString strDeviceName;
	CString strInstanceAddress;
	CString strInstanceName;
	if (this->m_deviceShortAddress >= 0 && this->m_deviceInstanceNumber >= 0) {		
		strDeviceAddress.Format("A%d²",this->m_deviceShortAddress);	
		strInstanceAddress.Format("I%d",this->m_deviceInstanceNumber);
		strDeviceName = this->m_strNameDevice;		
		strInstanceName = this->m_strNameInstance;
		m_editInstanceName.EnableWindow(TRUE);
		m_editDeviceName.EnableWindow(TRUE);
	}
	else {	
		m_editDeviceName.EnableWindow(FALSE);
		m_editInstanceName.EnableWindow(FALSE);
	}

	// De naam die gevormd wordt door combinatie van de device naam en instance naam.	
	this->setDisplayDeviceInstanceName();

	// De namen in de listctrl updaten.
	// Deze vlaggen worden gezet wnr de naam gewijzigd wordt via de edit controls.
	if (this->m_updateListCtrlDeviceName) {
		this->listCtrlUpdateDeviceName(this->m_deviceShortAddress,this->m_strNameDevice);	
		this->m_updateListCtrlDeviceName = false;
	}	
	if (this->m_updateListCtrlInstanceName) {		
		this->listCtrlUpdateDeviceInstanceName(this->m_deviceShortAddress,this->m_deviceInstanceNumber,this->m_strNameInstance);
		this->m_updateListCtrlInstanceName = false;
	}

	m_editDeviceAddress.SetWindowText(strDeviceAddress);
	m_editInstanceNumber.SetWindowText(strInstanceAddress);

	if (this->m_updateDeviceNameEditBox) {
		m_editDeviceName.SetWindowText(strDeviceName);	
		this->m_updateDeviceNameEditBox = false;
	}
	if (this->m_updateInstanceNameEditBox) {
		m_editInstanceName.SetWindowText(strInstanceName);
		this->m_updateInstanceNameEditBox = false;
	}

	m_staticGroupDevice.ShowWindow(flagShowDeviceNameControls);
	m_staticDeviceAddress.ShowWindow(flagShowDeviceNameControls);
	m_staticDeviceName.ShowWindow(flagShowDeviceNameControls);
	m_editDeviceAddress.ShowWindow(flagShowDeviceNameControls);
	m_editDeviceName.ShowWindow(flagShowDeviceNameControls);
	m_staticInstanceName.ShowWindow(flagShowDeviceNameControls);
	m_editInstanceNumber.ShowWindow(flagShowDeviceNameControls);
	m_editInstanceName.ShowWindow(flagShowDeviceNameControls);
	m_staticDisplayName.ShowWindow(flagShowDeviceNameControls);
	m_editDisplayName.ShowWindow(flagShowDeviceNameControls);


	{
		// CheckBoxes van de filters...	
		m_btnCheckFilterPushBtn.EnableWindow(flagControlsOnlineMode);
		m_btnCheckFilterOccupancy.EnableWindow(flagControlsOnlineMode);
		m_btnCheckFilterUnsupported.EnableWindow(flagControlsOnlineMode);
		m_btnCheckFilterEventScheme.EnableWindow(flagControlsOnlineMode);
		m_btnCheckFilterLocatedInstances.EnableWindow(flagControlsOnlineMode);
		
		m_btnCheckFilterPushBtn.SetCheck((this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_PUSHBUTTON) ? 1 : 0);
		m_btnCheckFilterOccupancy.SetCheck((this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_DETECTOR_OCCUPANCY) ? 1 : 0);
		m_btnCheckFilterUnsupported.SetCheck((this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_UNSUPPORTED) ? 1 : 0);
		m_btnCheckFilterEventScheme.SetCheck((this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_EVENTSCHEME_UNSUPPORTED) ? 1 : 0);
		m_btnCheckFilterLocatedInstances.SetCheck((this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_LOCATED_INSTANCES) ? 1 : 0);
	}

	// Radiobuttons running/paused.
	{
		CWnd* const pWnd = this->GetDlgItem(IDC_RADIO_RUNNING);
		if (NULL != pWnd) {
			pWnd->EnableWindow(flagControlsOnlineMode);
		}
	}
	{
		CWnd* const pWnd = this->GetDlgItem(IDC_RADIO_PAUSED);
		if (NULL != pWnd) {
			pWnd->EnableWindow(flagControlsOnlineMode);
		}
	}

	// Text en icoon info warning offline + 
	this->m_staticIconWarningOffline.ShowWindow(!flagControlsOnlineMode);
	this->m_staticTextWarningOffline.ShowWindow(!flagControlsOnlineMode);

	// Button show top index...
	this->m_btnCheckTopIndex.SetCheck(this->m_showTopIndex ? 1 : 0);

	this->m_btnAddLine.ShowWindow(flagShowEventControls);	
	return TRUE;
}

void CPropertyPageDaliLocate::onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) {
	if (statusConnected) {
		this->m_viewMode = VIEWMODE_ONLINE;
	}
	else {
		this->m_viewMode = VIEWMODE_OFFLINE;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageDaliLocate::onNotifyGuiRxDaliMessage(BYTE bNodeAddress,BYTE lengthBits,uint32_t recievedFrame) {

	if (this->m_stateLiveViewPaused) {
		return;
	}

	int shortAddress = -1;
	int instanceNumber = -1;
	int instanceType = -1;

	CString strEventMsg;
	CString strInstanceType;
	if (false == this->getWorker().convertEventMsgToString(lengthBits,recievedFrame,&shortAddress,&instanceNumber,&instanceType, &strEventMsg,&strInstanceType)) {
		return;
	}

	CString strDeviceName;
	CString strInstanceName;
	this->getWorker().getDeviceName(DALI_DEVICE_TYPE_INPUTCONTROL,shortAddress,&strDeviceName);			
	this->getWorker().getControlDeviceInstanceName(shortAddress,instanceNumber,&strInstanceName);

	// Bepalen via de event filters of een event entry in de listbox mag komen.
	switch(instanceType) {
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON:
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT: {				
			if (!(this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_PUSHBUTTON)) {
				return;
			}														  
		} break;
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY: {
			if (!(this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_DETECTOR_OCCUPANCY)) {
				return;
			}												   
		} break;
		case -1: {
			if (!(this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_EVENTSCHEME_UNSUPPORTED)) {
				return;
			}	
		} break;
		default: {
			if (!(this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_UNSUPPORTED)) {
				return;
			}		 				 
		} break;
	}

	// Filter op enkel devices getoond worden die nog geen naam hebbseen...
	if ((!strDeviceName.IsEmpty()) && (!strInstanceName.IsEmpty())) {
		if (!(this->m_flagsEventFilter & EVENT_FILTER_FLAG_EVENTS_TYPE_LOCATED_INSTANCES)) {
			return;
		}
	}

	const CTime time(CTime::GetCurrentTime());	
	const CString strTime = time.Format("%H:%M:%S");
	this->listCtrlEventsAddMessage(strTime,shortAddress,instanceNumber,strDeviceName,strInstanceName,strEventMsg,strInstanceType);
}

void CPropertyPageDaliLocate::OnBnClickedButtonClear() {
	const DWORD dwStyle = MB_OKCANCEL | MB_ICONQUESTION;
	if (IDCANCEL == this->MessageBox("Clear all DALI keypad event messages in the event log?",NULL,dwStyle)){
		return;
	}
	this->helper_clearDataName();
	this->m_listCtrlEvents.DeleteAllItems();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageDaliLocate::OnBnClickedButtonAddline() {
	this->listCtrlEventsAddSeperator( );
}

void CPropertyPageDaliLocate::helper_clearDataName( ) {
	this->m_deviceShortAddress = -1;
	this->m_deviceInstanceNumber = -1;
	this->m_strNameDevice.Empty();
	this->m_strNameInstance.Empty();
}

void CPropertyPageDaliLocate::OnTimer(UINT nIDEvent) {
	switch(nIDEvent) {
		case TIMER_ID_EDITBOX_DEVICENAME: {
			this->updateDeviceNameFromEditBox( );					  
		} break;

		case TIMER_ID_EDITBOX_INSTANCENAME: {
			this->updateInstanceNameFromEditBox( );					  
		} break;	
	}
	this->KillTimer(nIDEvent);
}

BOOL CPropertyPageDaliLocate::updateDeviceNameFromEditBox( ) {
	if (!this->m_busyDeviceNameEditing) {
		return FALSE;
	}

	this->m_busyDeviceNameEditing = false;
	
	CString strTemp;
	m_editDeviceName.GetWindowText(strTemp);
	if (0 == strTemp.Compare(this->m_strNameDevice)) {	
		return FALSE;
	}
		
	CString strNormalizedName;
	ASSERT(this->m_deviceShortAddress >= 0);
	if (false == this->getWorker().setDeviceName(DALI_DEVICE_TYPE_INPUTCONTROL,this->m_deviceShortAddress,strTemp,&strNormalizedName)) {
		return FALSE;
	}
			
	this->m_strNameDevice = strNormalizedName;	
	this->m_updateListCtrlDeviceName = true;
	this->m_updateDeviceNameEditBox = false;
	this->setNotifyGuiNamesChanged( );
	this->SendMessage(WM_UPDATE_CONTROLS);			// Oppassen: Mag niet assynchroon anders is er mogelijks al een nieuw item geselecteerd.
	return TRUE;
}

BOOL CPropertyPageDaliLocate::updateInstanceNameFromEditBox( ) {
	if (!this->m_busyInstanceNameEditing) {
		return FALSE;
	}

	this->m_busyInstanceNameEditing = false;

	CString strTemp;
	m_editInstanceName.GetWindowText(strTemp);
	if (0 == strTemp.Compare(this->m_strNameInstance)) {
		return FALSE;
	}

	ASSERT(this->m_deviceShortAddress >= 0);
	ASSERT(this->m_deviceInstanceNumber >= 0);

	CString strNormalizedName;
	if (false == this->getWorker().setControlDeviceInstanceName(this->m_deviceShortAddress,this->m_deviceInstanceNumber,strTemp,&strNormalizedName)) {
		return FALSE;
	}

	this->m_strNameInstance = strNormalizedName;		
	this->m_updateListCtrlInstanceName = true;
	this->m_updateInstanceNameEditBox = false;	
	this->setNotifyGuiNamesChanged( );
	this->SendMessage(WM_UPDATE_CONTROLS);			// Oppassen: Mag niet assynchroon anders is er mogelijks al een nieuw item geselecteerd.
	return TRUE;
}

void CPropertyPageDaliLocate::OnEnChangeEditDeviceName() {
	this->m_busyDeviceNameEditing = true;
	this->KillTimer(TIMER_ID_EDITBOX_DEVICENAME);
	this->SetTimer(TIMER_ID_EDITBOX_DEVICENAME,1500,(TIMERPROC) NULL);
}

void CPropertyPageDaliLocate::OnEnKillfocusEditDeviceName() {
	if (this->m_busyDeviceNameEditing) {
		this->updateDeviceNameFromEditBox( );
		this->KillTimer(TIMER_ID_EDITBOX_DEVICENAME);
	}
}

void CPropertyPageDaliLocate::OnEnChangeEditInstanceName() {
	this->m_busyInstanceNameEditing = true;
	this->KillTimer(TIMER_ID_EDITBOX_INSTANCENAME);
	this->SetTimer(TIMER_ID_EDITBOX_INSTANCENAME,1500,(TIMERPROC) NULL);
}

void CPropertyPageDaliLocate::OnEnKillfocusEditInstanceName() {
	if (this->m_busyInstanceNameEditing) {
		this->updateInstanceNameFromEditBox( );
		this->KillTimer(TIMER_ID_EDITBOX_INSTANCENAME);
	}
}

void CPropertyPageDaliLocate::setNotifyGuiNamesChanged( ) {
	CPropertyPageDaliContext& refContext = this->getContext();
	refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_DEVICES);
	refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_LOCATE);
}

void CPropertyPageDaliLocate::OnLvnItemchangedListctrlEvents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;


	if (this->m_busyDeviceNameEditing) {
		this->updateDeviceNameFromEditBox( );
		this->KillTimer(TIMER_ID_EDITBOX_DEVICENAME);
	}

	if (this->m_busyInstanceNameEditing) {
		// TRACE("m_busyInstanceNameEditing==true: updateInstanceNameFromEditBox()\r\n");
		this->updateInstanceNameFromEditBox( );
		this->KillTimer(TIMER_ID_EDITBOX_INSTANCENAME);
	}

	int selectedIndex = -1;
	if (NULL != pNMLV) {
		if ((pNMLV->uNewState & LVIS_SELECTED) && !(pNMLV->uOldState & LVIS_SELECTED)) {			
			int selectedIndex = pNMLV->iItem;
			if (selectedIndex >= 0) {
				int tempShortAddress = -1;
				int tempInstanceNumber = -1;
				const uint32_t itemData = this->m_listCtrlEvents.GetItemData(selectedIndex);	
				if (FALSE == convertFromItemData(itemData,&tempShortAddress,&tempInstanceNumber)) {
					// TRACE("helper_clearDataName() - 1\r\n");
					this->helper_clearDataName();
					this->PostMessage(WM_UPDATE_CONTROLS);
					return;
				}

				CString strDeviceName;
				if (FALSE == this->getWorker().getDeviceName(DALI_DEVICE_TYPE_INPUTCONTROL,tempShortAddress,&strDeviceName)) {
					this->m_strNameDevice.Empty();
				}
				else {
					this->m_strNameDevice = strDeviceName;
				}

				CString strInstanceName;
				if (FALSE == this->getWorker().getControlDeviceInstanceName(tempShortAddress,tempInstanceNumber,&strInstanceName)) {
					this->m_strNameInstance.Empty();
				}
				else {
					this->m_strNameInstance = strInstanceName;
				}

				this->m_updateDeviceNameEditBox = true;
				this->m_updateInstanceNameEditBox = true;
				this->m_deviceShortAddress = tempShortAddress;
				this->m_deviceInstanceNumber = tempInstanceNumber;
				this->PostMessage(WM_UPDATE_CONTROLS);
			}
		}	
		else if ((pNMLV->uOldState & LVIS_SELECTED) && !(pNMLV->uNewState & LVIS_SELECTED)) {
			if (this->m_listCtrlEvents.GetSelectedCount() == 0) {
				// TRACE("helper_clearDataName() - 2\r\n");
				this->helper_clearDataName();
				this->PostMessage(WM_UPDATE_CONTROLS);
			}
		}
	}	
}

void CPropertyPageDaliLocate::OnNMKillfocusListctrlEvents(NMHDR *pNMHDR, LRESULT *pResult) {
    *pResult = 0;
}

void CPropertyPageDaliLocate::OnNMSetfocusListctrlEvents(NMHDR *pNMHDR, LRESULT *pResult) {
	*pResult = 0;
}

// Maak een vetgedrukt lettertype aan als het nog niet bestaat
static CFont* getBoldFont(CWnd* pWnd) {	
	static CFont boldFont;
	if (boldFont.m_hObject == NULL) {		
		LOGFONT lf;
		CFont* pFont = pWnd->GetFont();		// Haal het huidige font van de lijst op
		pFont->GetLogFont(&lf);               // Haal fontinformatie op
		lf.lfWeight = FW_BOLD;                // Stel de dikte in op bold
		boldFont.CreateFontIndirect(&lf);     // Maak het bold font aan
	}
	return &boldFont;
}

static void restoreFont(LPNMLVCUSTOMDRAW pLVCD, CWnd* pWnd) {
	CFont* const pNormalFont = pWnd->GetFont();
	if (NULL != pNormalFont) {
		::SelectObject(pLVCD->nmcd.hdc, pNormalFont->m_hObject);
	}
}

// Oppassen:
// Onderstaande is nodig voor het weergeven van de selectie in de listbox.
// Wanneer de listbox zijn focus verliest is de geselecteerde rij niet meer zichtbaar.
void CPropertyPageDaliLocate::OnNMCustomdrawListCtrl(NMHDR *pNMHDR, LRESULT *pResult) {

	// Selected maar geen focus.
	static const COLORREF colorTextSelectedNotFocused = RGB(255, 255, 255);								// Grijze tekst
	static const COLORREF colorBackgroundSelectedNotFocused = RGB(0, 120, 215);							// Lichte achtergrond

	// Selected + Focused
	static const COLORREF colorTextSelectedFocused = RGB(255, 255, 255);								// Witte tekst
	static const COLORREF colorBackgroundSecectedFocused = RGB(0, 120, 215);							// Blauwe achtergrond

	// Paused 
	static const COLORREF colorBackgroundLiveviewPaused = RGB(212, 208, 200);	

	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);	
	// Controleer in welke fase van het tekenen we zitten
	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT) {
		*pResult = CDRF_NOTIFYITEMDRAW;  // Vraag om item-voor-item tekenen
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		// Controleer of het item geselecteerd is
		if (m_listCtrlEvents.GetItemState(nItem, LVIS_SELECTED) == LVIS_SELECTED) {
			if (m_listCtrlEvents.GetFocus() != this) {
				// Gebruik een aangepaste kleur als de control geen focus heeft
			
				pLVCD->clrText = colorTextSelectedNotFocused;	
				pLVCD->clrTextBk =colorBackgroundSelectedNotFocused;

				// Zet het bold font
				CFont* const pBoldFont = getBoldFont(&m_listCtrlEvents);
				::SelectObject(pLVCD->nmcd.hdc, pBoldFont);
			}
			else {
				// Standaard selectie kleuren wanneer de focus op de control is
				pLVCD->clrText = colorTextSelectedFocused;
				pLVCD->clrTextBk = colorBackgroundSecectedFocused;

				// Zet het bold font
				CFont* const pBoldFont = getBoldFont(&m_listCtrlEvents);
				::SelectObject(pLVCD->nmcd.hdc, pBoldFont);
			}
		}
		else {
			// Zorg ervoor dat niet-geselecteerde items het normale font gebruiken
			restoreFont(pLVCD,&m_listCtrlEvents);			

			if (this->m_stateLiveViewPaused) {
				pLVCD->clrTextBk = colorBackgroundLiveviewPaused;
			}
			else {
				pLVCD->clrTextBk = RGB(255, 255, 255);
			}
		}
		*pResult = CDRF_DODEFAULT;
	}
}


#if(0)
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

/*
IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)
*/

void CMyListCtrl::OnKillFocus(CWnd* pNewWnd)
{
    CListCtrl::OnKillFocus(pNewWnd);

    // Focus terugzetten naar de CListCtrl om selectie zichtbaar te houden
    // SetFocus();
    // Opnieuw tekenen van geselecteerde items
    int nSelected = GetNextItem(-1, LVNI_SELECTED);
    if (nSelected != -1) {
        SetItemState(nSelected, LVIS_SELECTED, LVIS_SELECTED);
        RedrawItems(nSelected, nSelected);
		// this->Invalidate();
    }

	this->RedrawWindow();
}

//#if(0)
void CMyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    int nIndex = lpDrawItemStruct->itemID;
    CRect rect = lpDrawItemStruct->rcItem;

    // Bepaal of dit item geselecteerd is
    BOOL bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);

    // Teken de achtergrond
    if (bSelected)
    {
        // Achtergrondkleur voor geselecteerde items
        pDC->FillSolidRect(rect, RGB(0, 120, 215));  // Bijvoorbeeld een blauwe kleur
    }
    else
    {
        // Achtergrondkleur voor niet-geselecteerde items
        pDC->FillSolidRect(rect, RGB(255, 255, 255)); // Wit
    }

/*
    // Teken de tekst
    pDC->SetTextColor(bSelected ? RGB(255, 255, 255) : RGB(0, 0, 0)); // Wit voor geselecteerde tekst, zwart voor normale tekst
    pDC->DrawText(GetItemText(nIndex), &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
*/
}
#endif


/***************************************************************************************************/

void CPropertyPageDaliLocate::OnBnClickedCheckFilterPushbutton() {	
	if (BST_CHECKED == this->m_btnCheckFilterPushBtn.GetCheck()) {
		this->m_flagsEventFilter |= EVENT_FILTER_FLAG_EVENTS_TYPE_PUSHBUTTON;
	}
	else {
		this->m_flagsEventFilter &= ~EVENT_FILTER_FLAG_EVENTS_TYPE_PUSHBUTTON;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageDaliLocate::OnBnClickedCheckFilterOccupancy() {
	if (BST_CHECKED == this->m_btnCheckFilterOccupancy.GetCheck()) {
		this->m_flagsEventFilter |= EVENT_FILTER_FLAG_EVENTS_TYPE_DETECTOR_OCCUPANCY;
	}
	else {
		this->m_flagsEventFilter &= ~EVENT_FILTER_FLAG_EVENTS_TYPE_DETECTOR_OCCUPANCY;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageDaliLocate::OnBnClickedCheckFilterUnsupported(){
	if (BST_CHECKED == this->m_btnCheckFilterUnsupported.GetCheck()) {
		this->m_flagsEventFilter |= EVENT_FILTER_FLAG_EVENTS_TYPE_UNSUPPORTED;
	}
	else {
		this->m_flagsEventFilter &= ~EVENT_FILTER_FLAG_EVENTS_TYPE_UNSUPPORTED;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageDaliLocate::OnBnClickedCheckFilterEventschem() {
	if (BST_CHECKED == this->m_btnCheckFilterEventScheme.GetCheck()) {
		this->m_flagsEventFilter |= EVENT_FILTER_FLAG_EVENTS_TYPE_EVENTSCHEME_UNSUPPORTED;
	}
	else {
		this->m_flagsEventFilter &= ~EVENT_FILTER_FLAG_EVENTS_TYPE_EVENTSCHEME_UNSUPPORTED;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageDaliLocate::OnBnClickedCheckFilterLocated() {
	if (BST_CHECKED == this->m_btnCheckFilterLocatedInstances.GetCheck()) {
		this->m_flagsEventFilter |= EVENT_FILTER_FLAG_EVENTS_TYPE_LOCATED_INSTANCES;
	}
	else {
		this->m_flagsEventFilter &= ~EVENT_FILTER_FLAG_EVENTS_TYPE_LOCATED_INSTANCES;
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyPageDaliLocate::OnBnClickedRadioRunning() {
	this->UpdateData(TRUE);
	m_listCtrlEvents.Invalidate();
	m_listCtrlEvents.UpdateWindow();
}

void CPropertyPageDaliLocate::OnBnClickedRadioPaused() {
	this->UpdateData(TRUE);
	m_listCtrlEvents.Invalidate();
	m_listCtrlEvents.UpdateWindow();
}

void CPropertyPageDaliLocate::OnBnClickedCheckTopindex() {
	if (m_btnCheckTopIndex.GetCheck() == BST_CHECKED) {
		m_showTopIndex = true;
	}
	else {
		m_showTopIndex = false;
	}
}


// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CPropertyPageDaliLocate::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_EDIT_DEVICE_DISPLAY_NAME: {
			this->m_strToolTipText = _T("Control input name (device name + instance name).");
			handled = TRUE;
		} break;
		case IDC_EDIT_DEVICE_NAME: {
			this->m_strToolTipText = _T("Control input device name.");
			handled = TRUE;
		} break;
		case IDC_EDIT_ADDRESS: {
			this->m_strToolTipText = _T("Short address of the control input device.");
			handled = TRUE;
		} break;
		case IDC_EDIT_NUMBER: {
			this->m_strToolTipText = _T("Instance number.");
			handled = TRUE;
		} break;
		case IDC_EDIT_INSTANCE_NAME: {
			this->m_strToolTipText = _T("Control input instance name (max 2 characters).");
			handled = TRUE;
		} break;
		case IDC_LISTCTRL_EVENTS: {
			this->m_strToolTipText = _T("List of control input events.");
			handled = TRUE;
		} break;
		case IDC_BUTTON_ADDLINE: {
			this->m_strToolTipText = _T("Add a separator line to the control input event list.");
			handled = TRUE;
		} break;
		case IDC_BUTTON_CLEAR: {
			this->m_strToolTipText = _T("Clear the control input event list.");
			handled = TRUE;
		} break;
		case IDC_CHECK_FILTER_PUSHBUTTON: {
			this->m_strToolTipText = _T("Show push button events when checked.");
			handled = TRUE;
		} break;
		case IDC_CHECK_FILTER_OCCUPANCY: {
			this->m_strToolTipText = _T("Show occupancy detector events when checked.");
			handled = TRUE;
		} break;
		case IDC_CHECK_FILTER_UNSUPPORTED: {
			this->m_strToolTipText = _T("Show unsupported events when checked.");
			handled = TRUE;
		} break;
		case IDC_CHECK_FILTER_EVENTSCHEM: {
			this->m_strToolTipText = _T("Show events with incorrect event schemes when checked.");
			handled = TRUE;
		} break;
		case IDC_CHECK_FILTER_LOCATED: {
			this->m_strToolTipText = _T("Show events assigned to a named device and instance when checked.");
			handled = TRUE;
		} break;
		case IDC_RADIO_RUNNING: {
			this->m_strToolTipText = _T("Enable live view of events.");
			handled = TRUE;
		} break;
		case IDC_RADIO_PAUSED: {
			this->m_strToolTipText = _T("Pause the live view of events.");
			handled = TRUE;
		} break;
		case IDC_CHECK_TOPINDEX: {
			this->m_strToolTipText = _T("Ensure the last received event is always visible.");
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
