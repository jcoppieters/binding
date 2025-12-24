// DlgChildDaliInstances.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliInstances.h"
#include "appdlgdaliworker.h"
#include "app/appdali/appdali_def.h"

/***************************************************************************************************************/
using namespace DUOTECNO::DALI;
/***************************************************************************************************************/

// Opvragen van de device instance die lokaal bijgehouden wordt.
static CAppDaliInputControlDeviceInstance* getDeviceInstance(CAppDaliInputControlDeviceInstances* pInstances, int instanceNumber) {
	if (NULL == pInstances) {
		REQUIRE(NULL != pInstances);
		return NULL;
	}
	ASSERT((instanceNumber >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (instanceNumber <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));

	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (false == pInstances->find(instanceNumber, &pInstance)) {
		return NULL;
	}

	ASSERT(NULL != pInstance);
	return pInstance;
}

// Opvragen van het aantal instances.
static int getDeviceNumberInstances(CAppDaliInputControlDeviceInstances* pInstances) {
	if (NULL == pInstances) {
		REQUIRE(NULL != pInstances);
		return 0;
	}
	return pInstances->getCountNumberOfInstances();
}

// Opvragen van de originele info.
// Deze kan gebruikt worden wanneer de undo knop ingedrukt wordt.
// Oppassen return waarde controleren...
static CAppDaliInputControlDeviceInstance* getOriginalDeviceInstance(CAppDaliDevice* pDevice, int instanceNumber) {
	REQUIRE((instanceNumber >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && (instanceNumber <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST));
	if (NULL == pDevice) {
		REQUIRE(NULL != pDevice);
		return NULL;
	}
	CAppDaliDeviceProperties_InputControlDevice* const properties = 
		dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDevice->getProperties());
	if (NULL == properties) {
		return NULL;
	}
	return getDeviceInstance(&properties->getInstances(),instanceNumber);
}


// DALI-2 Control Input Devices: Ozuno Universele Interface
// Herstellen van het instance type naar een 'push-button' instance. 
// Dit is momenteel een tijdelijke/snelle oplossing.
static bool getShowRestoreInstanceTypeButton(DUOTECNO::DALI::CAppDaliDevice* pDevice,int instanceType) {
	static const unsigned long long GTIN_OZUNO_UNIVERSAL_INPUT = 9355691000015;	// GTIN van de universele ozuno input.
	if (NULL == pDevice) {
		return false;
	}
	if (GTIN_OZUNO_UNIVERSAL_INPUT != pDevice->getGTIN()) {
		return false;
	}
	if (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON == instanceType) {
		return false;
	}
	return TRUE;
}

/***************************************************************************************************************/


/*********************************************************************************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
const CDlgChildDaliInstances::STRUCT_ENTRY CDlgChildDaliInstances::m_mapCheckButtonsEventFilters_Undefined[CDlgChildDaliInstances::MAX_EVENTFILTERS_CHECKBUTTONS] = {
	{ FALSE, 0, (int)-1, CString() },		// 0
	{ FALSE, 1, (int)-1, CString() },		// 1
	{ FALSE, 2, (int)-1, CString() },		// 2
	{ FALSE, 3, (int)-1, CString() },		// 3
	{ FALSE, 4, (int)-1, CString() },		// 4
	{ FALSE, 5, (int)-1, CString() },		// 5
	{ FALSE, 6, (int)-1, CString() },		// 6
	{ FALSE, 7, (int)-1, CString() }		// 7
};

const CDlgChildDaliInstances::STRUCT_ENTRY CDlgChildDaliInstances::m_mapCheckButtonsEventFilters_PushButtonEvents[CDlgChildDaliInstances::MAX_EVENTFILTERS_CHECKBUTTONS] = {
	{ TRUE, 0, (int)DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_RELEASED, CString("Button released") },			// 0
	{ TRUE, 1, (int)DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_PRESSED, CString("Button pressed") },			// 1
	{ TRUE, 2, (int)DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_SHORT_PRESS, CString("Short press") },			// 2
	{ TRUE, 3, (int)DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_DOUBLE_PRESS, CString("Double press") },			// 3
	{ TRUE, 4, (int)DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_LONG_START, CString("Long press start") },		// 4
	{ TRUE, 5, (int)DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_LONG_REPEAT, CString("Long press repeat") },		// 5
	{ TRUE, 6, (int)DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_LONG_STOP, CString("Long press stop") },			// 6
	{ TRUE, 7, (int)DALI_CONTROLDEVICE_PUSHBUTTON_EVENT_ENABLE_STUCK, ("Button stuck/free") }					// 7
};

const CDlgChildDaliInstances::STRUCT_ENTRY CDlgChildDaliInstances::m_mapCheckButtonsEventFilters_OccupancyDetector[CDlgChildDaliInstances::MAX_EVENTFILTERS_CHECKBUTTONS] = {
	{ TRUE, 0, (int)DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_OCCUPIED, CString("Occupied") },									// 0
	{ TRUE, 1, (int)DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_VACANT, CString("Vacant") },										// 1
	{ TRUE, 2, (int)DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_STILL_OCCUPIED_VACANT, CString("Still occupied/vacant") },			// 2
	{ TRUE, 3, (int)DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_MOVEMENT, CString("Movement") },									// 3
	{ TRUE, 4, (int)DALI_CONTROLDEVICE_DETECTOR_EVENT_ENABLE_NO_MOVEMENT, CString("No movement") },								// 4
	{ FALSE, 5, (int)-1, CString("") },																							// 5
	{ FALSE, 6, (int)-1, CString("") },																							// 6
	{ FALSE, 7, (int)-1, ("") }																									// 7
};

const CDlgChildDaliInstances::STRUCT_ENTRY CDlgChildDaliInstances::m_mapCheckButtonsEventFilters_AbsInputDevice[CDlgChildDaliInstances::MAX_EVENTFILTERS_CHECKBUTTONS] = {
	{ TRUE, 0, (int)DALI_CONTROLDEVICE_ABSINPUT_EVENT_ENABLE_POSITION, CString("Position event") },								// 0
	{ FALSE, 1, (int)-1, CString("") },																							// 1
	{ FALSE, 2, (int)-1, CString("") },																							// 2
	{ FALSE, 3, (int)-1, CString("") },																							// 3
	{ FALSE, 4, (int)-1, CString("") },																							// 4
	{ FALSE, 5, (int)-1, CString("") },																							// 5
	{ FALSE, 6, (int)-1, CString("") },																							// 6
	{ FALSE, 7, (int)-1, ("") }																									// 7
};

/*********************************************************************************************************************/
const CDlgChildDaliInstances::STRUCT_ENTRY* CDlgChildDaliInstances::getMapCheckButtonsInstanceType(int instanceType) {
	switch (instanceType) {
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON: {
			return m_mapCheckButtonsEventFilters_PushButtonEvents;
		} break;
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY: {
			return m_mapCheckButtonsEventFilters_OccupancyDetector;
		} break;
		case DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_ABS_INPUT: {
			return m_mapCheckButtonsEventFilters_AbsInputDevice;							
		} break;
	}
	return m_mapCheckButtonsEventFilters_Undefined;
}
/*********************************************************************************************************************/
// CDlgChildDaliInstances dialog

IMPLEMENT_DYNAMIC(CDlgChildDaliInstances, CDialog)

CDlgChildDaliInstances::CDlgChildDaliInstances(CAppDlgDaliWorker& refWorker, CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker, CDlgChildDaliInstances::IDD, pParent),
	m_currentInstanceType(-1), m_currentInstanceNumber(0), m_currentEventScheme(-1),
	m_flagChangedDevice(false), m_flagChangedInstanceNumber(false), m_flagChangedInstanceType(false), 
	m_flagChangedEventFilters(false), m_validSettings(true), m_restoreInstanceTypeEnable(false), m_properties(NULL) {
	m_arrayButtonsEventFilters[0] = &m_btnCheckEventFilter0;
	m_arrayButtonsEventFilters[1] = &m_btnCheckEventFilter1;
	m_arrayButtonsEventFilters[2] = &m_btnCheckEventFilter2;
	m_arrayButtonsEventFilters[3] = &m_btnCheckEventFilter3;
	m_arrayButtonsEventFilters[4] = &m_btnCheckEventFilter4;
	m_arrayButtonsEventFilters[5] = &m_btnCheckEventFilter5;
	m_arrayButtonsEventFilters[6] = &m_btnCheckEventFilter6;
	m_arrayButtonsEventFilters[7] = &m_btnCheckEventFilter7;
}

CDlgChildDaliInstances::~CDlgChildDaliInstances()
{
	this->deallocProperties();
}

void CDlgChildDaliInstances::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_EVENTFILTERS_0, m_btnCheckEventFilter0);	
	DDX_Control(pDX, IDC_CHECK_EVENTFILTERS_1, m_btnCheckEventFilter1);	
	DDX_Control(pDX, IDC_CHECK_EVENTFILTERS_2, m_btnCheckEventFilter2);	
	DDX_Control(pDX, IDC_CHECK_EVENTFILTERS_3, m_btnCheckEventFilter3);	
	DDX_Control(pDX, IDC_CHECK_EVENTFILTERS_4, m_btnCheckEventFilter4);	
	DDX_Control(pDX, IDC_CHECK_EVENTFILTERS_5, m_btnCheckEventFilter5);	
	DDX_Control(pDX, IDC_CHECK_EVENTFILTERS_6, m_btnCheckEventFilter6);	
	DDX_Control(pDX, IDC_CHECK_EVENTFILTERS_7, m_btnCheckEventFilter7);	
	DDX_Control(pDX, IDC_COMBO_INSTANCE_EVENTSCHEME, m_cmbEventScheme);
	DDX_Control(pDX, IDC_COMBO_INSTANCE_NUMBER, m_cmbInstanceNumber);
	DDX_Control(pDX, IDC_EDIT_INSTANCETYPE, m_editInstanceType);
	DDX_Control(pDX, IDC_CHECK_ENABLE_EVENTS, m_btnCheckEnabled);
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_btnUpload);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, m_btnRestoreDefaults);	
	DDX_Control(pDX, IDC_BUTTON_UNDO, m_btnUndoChanges);	
	DDX_Control(pDX, IDC_STATIC_ICON_WARNING, m_iconWarningSettings);	
	DDX_Control(pDX, IDC_STATIC_GROUP_EVENTFILTERS, m_staticGroupEventFilters);
	DDX_Control(pDX, IDC_BUTTON_INSTANCETYPE_RESTORE, m_btnRestoreInstanceType);
}

BEGIN_MESSAGE_MAP(CDlgChildDaliInstances, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_INSTANCE_NUMBER,OnCbnSelchangeInstanceNumber)
	ON_CBN_SELCHANGE(IDC_COMBO_INSTANCE_EVENTSCHEME,OnCbnSelchangeEventScheme)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_EVENTFILTERS_0, IDC_CHECK_EVENTFILTERS_7, OnButtonClickedEventFilter)	
	ON_BN_CLICKED(IDC_CHECK_ENABLE_EVENTS,OnButtonClickedEnable)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CDlgChildDaliInstances::OnBnClickedButtonUpload)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CDlgChildDaliInstances::OnBnClickedButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, &CDlgChildDaliInstances::OnBnClickedButtonUndo)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliInstances::OnToolTipNeedText)
	ON_BN_CLICKED(IDC_BUTTON_INSTANCETYPE_RESTORE, &CDlgChildDaliInstances::onBnClickedRestoreInstanceType)	
END_MESSAGE_MAP()


void CDlgChildDaliInstances::assignDeviceProperties(DUOTECNO::DALI::CAppDaliDevice* pDevice) {

	this->deallocProperties( );	

	if (NULL != pDevice) {	
		CAppDaliDeviceProperties_InputControlDevice* prop = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDevice->getProperties());
		ASSERT(0 != prop);
		if (NULL != prop) {
			this->m_properties = prop->getInstances().clone();
			ASSERT(NULL != this->m_properties);
		}
		this->setCurrDeviceShortAddress(pDevice->getShortAddress());	
	}	
}

// CDlgChildDaliInstances message handlers
void CDlgChildDaliInstances::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	this->setDevice(pDevice);
	this->assignDeviceProperties(pDevice);
	this->setPropertyChanged(false);
	this->setUploadDone(false);
	this->m_validSettings = true;	
	this->m_restoreInstanceTypeEnable = false;

/*
	else if (this->getCurrDeviceShortAddress() != pDevice->getShortAddress()) {
		this->deallocProperties( );
		this->setPropertyChanged(false);
		this->setUploadDone(false);
		CAppDaliDeviceProperties_InputControlDevice* prop = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDevice->getProperties());
		ASSERT(0 != prop);
		if (NULL != prop) {
			this->m_properties = prop->getInstances().clone();
			ASSERT(NULL != this->m_properties);
		}
		this->setCurrDeviceShortAddress(pDevice->getShortAddress());	
	}
*/
	this->m_flagChangedInstanceNumber = true;
	this->m_flagChangedInstanceType = true;
	this->m_flagChangedDevice = true;	
	this->updateModel();
	this->updateViewMode();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliInstances::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) {
	if (activeDialog == true) {
		this->init(pDevice);
		return;
	}

	this->updateViewMode();	
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgChildDaliInstances::OnParentPropertyPageLeave() {
	return FALSE;
}

void CDlgChildDaliInstances::onTabDialogEnterDialog( ) {
/*
	this->m_flagChangedDevice = true;	
	const CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL != pDevice) {
		this->assignDeviceProperties(pDevice);		
	}	
	this->updateModel();
	this->updateViewMode();
	this->PostMessage(WM_UPDATE_CONTROLS);
*/
	this->init(this->getDevice());
}

BOOL CDlgChildDaliInstances::onTabDialogCanLeaveDialog( ) {	
	if (this->getUploadBusy()) {
		promptUserUploadBusy(*this);
		return FALSE;
	}
	if (VIEWMODE_ONLINE != this->getViewMode()) {
		return TRUE;
	}
	if (!this->getPropertyChanged()) {
		return TRUE;
	}	
	if (FALSE == promptUserUploadChangesBeforeLeavingDialog(*this)) {
		this->undoChanges();
		return TRUE;
	}
	this->uploadData( );
	return FALSE;
}

void CDlgChildDaliInstances::updateMsgProgressInfo( ) {
	const CString strMessage = this->getStrMsgInfoFooter(*this);
	m_staticFooterInfoText.SetWindowText(strMessage);
}

// IAppDlgDaliGui_Host	
void CDlgChildDaliInstances::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCES == task) {		
		this->setUploadBusy(true);
		this->setUploadDone(false);
	}

	this->updateModel();
	this->setViewMode(VIEWMODE_ONLINE_BUSY);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliInstances::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCES == task) {	
		this->setUploadDone(true);
		this->setUploadBusy(false);
		this->setPropertyChanged(false);
	}

	this->updateModel();
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliInstances::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	this->setViewMode(VIEWMODE_ONLINE);
	if (APPDALI_FSM_TASK_CONTROLDEVICE_INSTANCES == task) {
		// this->m_strErrorMessage.Format("Error happened!\r\n%s");
		this->setUploadBusy(false);	
	}

	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliInstances::updateModel( ) {
	int instanceType = -1;
	int eventScheme = -1;

	this->m_validSettings = true;
	if (NULL != this->m_properties) {
		if ((this->m_currentInstanceNumber >= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) && 
			(this->m_currentInstanceNumber <= DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST)) {
			CAppDaliInputControlDeviceInstance* const pInstance = getDeviceInstance(this->m_properties, this->m_currentInstanceNumber); 
			if (NULL != pInstance) {
				instanceType = pInstance->getInstanceType( );
				eventScheme = pInstance->getEventScheme();
				this->m_validSettings = this->getWorker().checkValidSettingForHomeAutomation(*pInstance);
			}			
		}
	}
	if (instanceType != this->m_currentInstanceType) {
		this->m_currentInstanceType = instanceType;
		this->m_flagChangedInstanceType = true;
	}

	this->m_currentEventScheme = eventScheme;
}

LRESULT CDlgChildDaliInstances::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	if (!this->getInitDone()) {			

		this->EnableToolTips(TRUE);

		if (initPictureControlWarningIcon(*this, this->m_iconWarningSettings)) {

			// Nodig voor het weergeven van de tooltip.
			// De style moet SS_NOTIFY zijn anders worden de mouse/tooltip events niet verstuurd.
			this->m_iconWarningSettings.ModifyStyle(0, SS_NOTIFY);		
			

			this->EnableToolTips(TRUE);

/*
			// De weergave van de teksten gebeurd in de OnToolTipNeedText( )  callback functie.
			const DWORD dwStyleToolTip = TTS_ALWAYSTIP | TTS_NOPREFIX | TTS_BALLOON;
			if (this->m_toolTipWarningInfo.Create(this,dwStyleToolTip)) {	
				this->m_toolTipWarningInfo.SetMaxTipWidth(500); // Voor multiline (Niet ok -> blijft singleline)
				this->m_toolTipWarningInfo.Activate(TRUE);        
			}			
*/
		}
		this->m_iconWarningSettings.ShowWindow(FALSE); 
		this->setInitDone(true);
	}

	// Oppassen: Wanneer de instance/combobox veranderd moeten we de GUI opnieuw updaten...
	if (this->updateGUIControls_ComboInstanceNumber( )) {	
		this->updateModel();
		this->SendMessage(WM_UPDATE_CONTROLS);
		return TRUE;
	}

	// Tekst in de progress info (m_staticFooterInfoText) control.
	this->updateMsgProgressInfo();
	
	{
		// Buttons upload, restore defaults, undo changes
		m_btnUpload.EnableWindow((VIEWMODE_ONLINE == this->getViewMode()) && (this->getPropertyChanged() ? TRUE : FALSE));
		m_btnRestoreDefaults.EnableWindow(TRUE);
		m_btnUndoChanges.EnableWindow(this->getPropertyChanged() ? TRUE : FALSE);
	}

	this->m_iconWarningSettings.ShowWindow(this->m_validSettings == false ? TRUE : FALSE);

	{
		// DALI-2 Control Input Devices: Ozuno Universele Interface (GTIN=9355691000015)
		// Mogelijkheid om het instance type te herstellen naar 'push-button' instance type.
		//
		// Enkel getoond wanneer GTIN=9355691000015 en het type instance is niet ingesteld als 'push-button' instance
		if (this->m_flagChangedInstanceNumber) {		
			this->m_restoreInstanceTypeEnable = getShowRestoreInstanceTypeButton(this->getDevice(),this->m_currentInstanceType);
		}
		this->m_btnRestoreInstanceType.ShowWindow(this->m_restoreInstanceTypeEnable == true ? TRUE : FALSE);		
		this->m_btnRestoreInstanceType.EnableWindow((this->m_restoreInstanceTypeEnable) && (VIEWMODE_ONLINE == this->getViewMode()));
	}

	// Specifiek.
	this->updateGUIControls_EventFilters( );
	this->updateGUIControls_EventScheme( );
	this->updateGuiControls_InstanceType( );
	this->updateGUIControls_InstanceEnabled( );

	this->m_flagChangedEventFilters = false;
	this->m_flagChangedInstanceNumber = false;
	this->m_flagChangedInstanceType = false;
	this->m_flagChangedDevice = false;
	return TRUE;
}

// Oppassen:
// Bij verandering van instance number moeten we de GUI updaten
bool CDlgChildDaliInstances::updateGUIControls_ComboInstanceNumber( ) {

	CAppDaliDevice* const pDevice = this->getDevice();
	if (this->m_flagChangedDevice) {
		if (NULL == pDevice) {
			m_cmbInstanceNumber.ShowWindow(FALSE);
			if (0 == m_cmbInstanceNumber.GetCount()) {
				return false;
			}			
			m_cmbInstanceNumber.ResetContent();
			return true;
		}

		ASSERT(NULL != this->m_properties);
	
		if (getDeviceNumberInstances(this->m_properties) != m_cmbInstanceNumber.GetCount()) {
			m_cmbInstanceNumber.ResetContent();		
			for (int i=0;i<DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_MAX;++i) {
				CAppDaliInputControlDeviceInstance* const pDeviceInstance = getDeviceInstance(this->m_properties, i);
				if (NULL != pDeviceInstance) {				
					CString strInstanceNumber;
					strInstanceNumber.Format("%d",i);
					int index = m_cmbInstanceNumber.AddString(strInstanceNumber);
					if (index >= 0) {
						m_cmbInstanceNumber.SetItemData(index,i);
					}
				}
			}
			if (m_cmbInstanceNumber.GetCount() > 0) {
				m_cmbInstanceNumber.SetCurSel(0);				
			}
			this->m_currentInstanceNumber = m_cmbInstanceNumber.GetCurSel();
			this->m_flagChangedInstanceNumber = true;
			return true;
		}
	}
	return false;
}

void CDlgChildDaliInstances::updateGUIControls_EventFilters() {
	
	{
		// Controls enabled enkel wanneer we online mode bezig zijn...
		const BOOL enabled = (VIEWMODE_ONLINE == this->getViewMode() ? TRUE : FALSE);
		for (int i = 0; i < MAX_EVENTFILTERS_CHECKBUTTONS; ++i) {
			m_arrayButtonsEventFilters[i]->EnableWindow(enabled);
		}
	}

	
	if (this->m_flagChangedInstanceType) {
		const STRUCT_ENTRY* const pMap = getMapCheckButtonsInstanceType(this->m_currentInstanceType);
		if (NULL == pMap) {
			ASSERT(NULL != pMap);
			return;
		}

		// Update checkboxes: enable + naam.
		BOOL globalEnable = FALSE;
		for (int i = 0; i < MAX_EVENTFILTERS_CHECKBUTTONS; ++i) {
			if (pMap[i].m_enabled) {
				globalEnable = TRUE;
			}
			m_arrayButtonsEventFilters[i]->ShowWindow(pMap[i].m_enabled);
			m_arrayButtonsEventFilters[i]->SetWindowText(pMap[i].m_strName);
		}

		// Indien er geen checkboxes enabled zijn dan de groupbox disablen.		
		// Er zijn geen eventfilters aanwezig wanneer een instance type niet ondersteund wordt.
		m_staticGroupEventFilters.ShowWindow(globalEnable);
	}

	if ((this->m_flagChangedInstanceNumber) || (this->m_flagChangedInstanceType) || (this->m_flagChangedEventFilters)) {
		ASSERT(NULL != this->m_properties);
		const STRUCT_ENTRY* const pMap = getMapCheckButtonsInstanceType(this->m_currentInstanceType);
		if (NULL == pMap) {
			ASSERT(NULL != pMap);
			return;
		}
		CAppDaliInputControlDeviceInstance* const pDeviceInstance = getDeviceInstance(this->m_properties, this->m_currentInstanceNumber);
		if (NULL == pDeviceInstance) {
			ASSERT(NULL != pDeviceInstance);
			return;
		}
		const int eventFilters = pDeviceInstance->getEventFilter();
		for (int i = 0; i < MAX_EVENTFILTERS_CHECKBUTTONS; ++i) {
			BOOL checked = FALSE;
			if (pMap[i].m_maskEventFilter & eventFilters) {
				checked = TRUE;
			}
			m_arrayButtonsEventFilters[i]->SetCheck(checked);
		}
	}
}

void CDlgChildDaliInstances::updateGUIControls_EventScheme( ) {

	{
		// Controls enabled enkel wanneer we online mode bezig zijn...
		const BOOL enabled = (VIEWMODE_ONLINE == this->getViewMode() ? TRUE : FALSE);
		m_cmbEventScheme.EnableWindow(enabled);	
	}


	// Bij een ongeldig instance type - geen adres schema weergeven.
	if ((this->m_currentInstanceType < DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_FIRST) ||
		(this->m_currentInstanceType > DALI_SYSTEM_CONTROLDEVICE_INSTANCE_NR_LAST)) {
		m_cmbEventScheme.ResetContent();
		m_cmbEventScheme.ShowWindow(FALSE);
		return;
	}

	// Eventschema tonen - indien leeg dan opnieuw invullen met entries.
	m_cmbEventScheme.ShowWindow(TRUE);
	if (m_cmbEventScheme.GetCount() == 0) {		
		int index = -1;				
		index = m_cmbEventScheme.AddString("Instance addressing");
		if (index >=0) {
			m_cmbEventScheme.SetItemData(index,DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE);
		}
		index = m_cmbEventScheme.AddString("Device addressing");
		if (index >=0) {
			m_cmbEventScheme.SetItemData(index,DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE_TYPE);
		}
		index = m_cmbEventScheme.AddString("Device/instance addressing");
		if (index >=0) {
			m_cmbEventScheme.SetItemData(index,DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE_NUMBER);
		}
		index = m_cmbEventScheme.AddString("Device group addressing");
		if (index >=0) {
			m_cmbEventScheme.SetItemData(index,DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_DEVICEGROUP);
		}
		index = m_cmbEventScheme.AddString("Instance group addressing");
		if (index >=0) {
			m_cmbEventScheme.SetItemData(index,DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCEGROUP);
		}
	}

	// Update combobox met huidige adres schema.	
	if (this->m_currentEventScheme >= 0) {
		const int countItems = m_cmbEventScheme.GetCount();
		for (int i=0;i<countItems;++i) {
			if (this->m_currentEventScheme == m_cmbEventScheme.GetItemData(i)) {
				m_cmbEventScheme.SetCurSel(i);
				break;
			}
		}
	}	
}

void CDlgChildDaliInstances::updateGuiControls_InstanceType( ) {

	{
		// Controls enabled enkel wanneer we online mode bezig zijn...
		const BOOL enabled = (VIEWMODE_ONLINE == this->getViewMode() ? TRUE : FALSE);
		m_editInstanceType.EnableWindow(enabled);	
	}

	if (this->m_flagChangedInstanceType) {
		const CString strInstanceType = this->getWorker().getStringInstanceType(this->m_currentInstanceType);
		m_editInstanceType.SetWindowText(strInstanceType);		
		m_editInstanceType.ShowWindow(NULL != this->m_properties ? TRUE : FALSE);		
	}
}

void CDlgChildDaliInstances::updateGUIControls_InstanceEnabled( ) {	

	{
		// Controls enabled enkel wanneer we online mode bezig zijn...
		const BOOL enabled = (VIEWMODE_ONLINE == this->getViewMode() ? TRUE : FALSE);
		m_btnCheckEnabled.EnableWindow(enabled);	
	}


	ASSERT(NULL != this->m_properties);
	BOOL checked = FALSE;
	CAppDaliInputControlDeviceInstance* const pDeviceInstance = getDeviceInstance(this->m_properties, this->m_currentInstanceNumber);
	if (NULL == pDeviceInstance) {
		checked = FALSE;
	}
	if (pDeviceInstance->getInstanceActive()) {
		checked = TRUE;
	}
	m_btnCheckEnabled.SetCheck(checked);	
}

void CDlgChildDaliInstances::OnCbnSelchangeInstanceNumber() {

	// Dit moet altijd enabled zijn ook in de offline mode.
	// Via deze kunnen we controleren of de instellingen van een instance nummer ook zijn.

	const int currSelection = m_cmbInstanceNumber.GetCurSel();
	if (currSelection >= 0) {		
		const int instanceNumber = m_cmbInstanceNumber.GetItemData(currSelection);
		if (instanceNumber >= 0) {			
			this->m_currentInstanceNumber = instanceNumber;
			this->m_flagChangedInstanceNumber = true;		
		}
	}

	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

// Message handler:
// Aanpassen van de event berichten voor een instance.
void CDlgChildDaliInstances::OnButtonClickedEventFilter(UINT nID) {	
	const int buttonIdx = nID - IDC_CHECK_EVENTFILTERS_0;
	if ((buttonIdx < 0) || (buttonIdx >= MAX_EVENTFILTERS_CHECKBUTTONS)) {
		ASSERT((buttonIdx >= 0) && (buttonIdx < MAX_EVENTFILTERS_CHECKBUTTONS));
		return;
	}
	const STRUCT_ENTRY* const pMap = getMapCheckButtonsInstanceType(this->m_currentInstanceType);
	if (NULL == pMap) {
		ASSERT(NULL != pMap);
		return;
	}
	// Aanpassen van de properties.
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (!m_properties->find(this->m_currentInstanceNumber, &pInstance)) {
		return;
	}
	ASSERT(NULL != pInstance);
	int eventFilters = pInstance->getEventFilter();	
	const int prevEventFilters = eventFilters;
	const int state = m_arrayButtonsEventFilters[buttonIdx]->GetCheck();
	const int mask = pMap[buttonIdx].m_maskEventFilter;
	if (mask < 0) {
		ASSERT(mask >= 0);
		return;
	}
	if (state) {
		eventFilters |= mask;
	}
	else {
		eventFilters &= ~mask;
	}	
	if (prevEventFilters != eventFilters) {
		pInstance->setEventFilter(eventFilters);		
		this->setPropertyChanged(true);
	}
	this->m_flagChangedEventFilters = true;		

	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}	

void CDlgChildDaliInstances::OnCbnSelchangeEventScheme() {	
	const int selected = m_cmbEventScheme.GetCurSel();
	if (selected < 0) {
		ASSERT(selected >= 0);
		return;
	}

	bool applyChanges = true;
	const int instanceType = m_cmbEventScheme.GetItemData(selected);
	if (DALI_CONTROLDEVICE_EVENT_SCHEME_ADDRESSING_INSTANCE_NUMBER != instanceType) {	
		// Melding geven: Vragen of deze mag aangepast worden.
		CString strTitle("Change the address scheme?");
		CString msg("Are you sure you want to change the address scheme?\r\n\r\n"
				    "The address scheme determines how DALI-2 events are transmitted on the DALI-2 bus. "
					"Only events following the 'Device/Instance Number' scheme can trigger an action in "
					"the Duotecno home automation system.\r\n\r\n"
					"Do you wish to proceed with changing the address scheme?");
		if (IDNO == this->MessageBox(msg, strTitle, MB_YESNO | MB_ICONWARNING)) {
			applyChanges = false;
		}
	}

	if (applyChanges) {
		CAppDaliInputControlDeviceInstance* const pDeviceInstance = getDeviceInstance(this->m_properties, this->m_currentInstanceNumber);
		if (NULL != pDeviceInstance) {
			pDeviceInstance->setEventScheme(instanceType);		
			this->setPropertyChanged(true);
		}	
	}
		
	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliInstances::OnButtonClickedEnable() {
	const bool stateOnOff = (BST_CHECKED == m_btnCheckEnabled.GetCheck() ? true : false);

	// Aanpassen van de properties.
	CAppDaliInputControlDeviceInstance* pInstance = NULL;
	if (!m_properties->find(this->m_currentInstanceNumber, &pInstance)) {
		return;
	}

	if (false == stateOnOff) {	
		CString strTitle;
		strTitle.Format("Disable instance number %d?",this->m_currentInstanceNumber);
		CString strMessage("Disabling an unused input simplifies the Duotecno DALI bridge setup. "
						   "However, once disabled, the input will stop sending event messages on the DALI bus.\r\n\r\n"		
					       "Because it won't send event messages anymore, you won’t be able to locate it. "
						   "(If needed, you can re-enable the instance by checking the box.)\r\n\r\n"
						   "Beware if the input was previously connected to a Duotecno input unit, it will no longer work.\r\n\r\n"
						   "Are you sure you want to disable this instance number?");

		if (IDNO == this->MessageBox(strMessage, strTitle, MB_ICONQUESTION | MB_YESNO)) {
		    this->PostMessage(WM_UPDATE_CONTROLS);
			return;
		}		
	}

	ASSERT(NULL != pInstance);
	if (pInstance->getInstanceActive() != stateOnOff) {
		pInstance->setInstanceActive(stateOnOff);
		this->setPropertyChanged(true);
	}

	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliInstances::OnBnClickedButtonUpload(){
	this->uploadData();	
}

/*
void CDlgChildDaliInstances::initIconWarning( ) {
	HICON hIconWarning = (HICON)LoadImage(
		NULL,
		MAKEINTRESOURCE(IDI_WARNING),
		IMAGE_ICON,
		32, 32,
		LR_SHARED
	);
	if (NULL != hIconWarning) {
		this->m_iconWarningSettings.SetIcon(hIconWarning);
		this->m_iconWarningSettings.ShowWindow(FALSE);
	}		
}
*/

void CDlgChildDaliInstances::deallocProperties( ) {
	if (NULL != m_properties) {
		delete m_properties;
		m_properties = NULL;
	}
	this->setCurrDeviceShortAddress(-1);
}

void CDlgChildDaliInstances::uploadData( ) {
	if ((this->getCurrDeviceShortAddress() < 0) || (NULL == this->m_properties)) {
		ASSERT(this->getCurrDeviceShortAddress()>=0);
		ASSERT(0 != this->m_properties);
		return;
	}
	const bool result = this->getWorker().startUploadControlInputDeviceInstances(this->getCurrDeviceShortAddress(), *this->m_properties);
	if (result) {

	}

	this->setUploadDone(false);
	this->setUploadBusy(true);	
	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

BOOL CDlgChildDaliInstances::undoChanges( ) {
	CAppDaliDevice* const pAppDaliDevice = this->getDevice();
	if ((NULL == this->m_properties) || (NULL == pAppDaliDevice)) {
		ASSERT(NULL != pAppDaliDevice);
		ASSERT(NULL != this->m_properties);
		return FALSE;
	}
	if (this->m_currentInstanceNumber < 0) {
		ASSERT(this->m_currentInstanceNumber >= 0);
		return FALSE;
	}
	CAppDaliInputControlDeviceInstance* const pCurrentInstance = getDeviceInstance(this->m_properties, this->m_currentInstanceNumber);
	if (NULL == pCurrentInstance) {
		ASSERT(NULL != pCurrentInstance);
		return FALSE;
	}
	CAppDaliInputControlDeviceInstance* const pOriginalInstance = getOriginalDeviceInstance(pAppDaliDevice, this->m_currentInstanceNumber);
	if (NULL == pOriginalInstance) {
		ASSERT(NULL != pOriginalInstance);
		return FALSE;
	}
	CAppDaliInputControlDeviceInstance::copy(pCurrentInstance, *pOriginalInstance);	
	return TRUE;
}

BOOL CDlgChildDaliInstances::restoreToDefaults( ) {
	if (NULL == this->m_properties) {
		ASSERT(0);
		return FALSE;
	}
	if (this->m_currentInstanceNumber < 0) {
		ASSERT(this->m_currentInstanceNumber >= 0);
		return FALSE;
	}
	CAppDaliInputControlDeviceInstance* const pDeviceInstance = getDeviceInstance(this->m_properties, this->m_currentInstanceNumber);	
	if (NULL == pDeviceInstance) {
		ASSERT(0);
		return FALSE;
	}
	CAppDaliDevice_Utils::setDefaults(pDeviceInstance);
	return TRUE;
}

void CDlgChildDaliInstances::OnBnClickedButtonRestore() {
	if (FALSE == promptUserRestoreDefaults(*this)) {
		return;
	}

	this->restoreToDefaults();
							
	this->m_flagChangedEventFilters = true;
	this->m_flagChangedInstanceType = true;				// Voor later bij de Ozuno devices...

	this->setPropertyChanged(true);
	this->setUploadDone(false);
	
	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliInstances::OnBnClickedButtonUndo() {
	if (FALSE == promptUserUndoChanges(*this)) {
		return;
	}

	this->undoChanges();

	this->m_flagChangedEventFilters = true;
	this->m_flagChangedInstanceType = true;				// Voor later bij de Ozuno devices...

	this->setPropertyChanged(true);
	this->setUploadDone(false);

	this->updateModel();
	this->PostMessage(WM_UPDATE_CONTROLS);
}

// DALI-2 Control Input Devices: Ozuno Universele Interface
// Herstellen van het instance type naar een 'push-button' instance. 
// Dit is momenteel een tijdelijke/snelle oplossing.
// Reden: Het wijzigen van het instance type is alleen mogelijk in de specifieke 
// manufacturing operating mode via de Rapix-configuratiesoftware. Het is onwaarschijnlijk dat dit 
// per ongeluk naar een ander instance type wordt gewijzigd.
// Let op: Na het wijzigen van het instance type moeten het adres-eventschema en 
// de eventfilters opnieuw worden ingesteld.
void CDlgChildDaliInstances::onBnClickedRestoreInstanceType() {
	const CString strTitle("Restore Instance Type to 'Push-Button' Instance?");
	const CString strMsg("Caution: This is an advanced option!\r\n\r\n"
						 "This action restores the instance type to 'Push-Button.'\r\n"
						 "After restoring the instance type, the device may reset to its default properties.\r\n"
						 "It is recommended to perform a new 'Search for Devices' after this action. "
						 "Additionally, the properties of this instance (such as address schema and event filters) may need to be reconfigured.\r\n\r\n"
						 "Do you want to proceed?");
	
	if (IDNO == this->MessageBox(strMsg,strTitle,MB_YESNO | MB_ICONQUESTION)) {
		return;
	}

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) {
		ASSERT(0);
		return;
	}
	if (this->m_currentInstanceNumber < 0) {
		ASSERT(0);
		return;
	}

	// Let op: Er wordt geen fsm gebruikt. De commandos worden na elkaar uitgevoerd.
	if (this->getWorker().sendControlInputDeviceRestoreInstanceType(pDevice->getShortAddress(),this->m_currentInstanceNumber)) {	
		// Ok algoritme gelult.
	}
}

// Weergave van de tooltip.
BOOL CDlgChildDaliInstances::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;	
    UINT_PTR identifier = pNMHDR->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND) {
        identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
    }
	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_STATIC_ICON_WARNING: {
			this->m_strToolTipText = _T("The configuration of this instance does not meet "
										"the required specifications for Duotecno home automation. "
										"To fix this, reset to default settings.");
			handled = TRUE;
		} break;
		case IDC_COMBO_INSTANCE_NUMBER: {
			this->m_strToolTipText = _T("Instance number (equivalent to Input number)");
			handled = TRUE;
		} break;
		case IDC_CHECK_ENABLE_EVENTS: {
			this->m_strToolTipText = _T("Enable DALI-2 event messages (Leave at default value)");
			handled = TRUE;
		} break;
		case IDC_EDIT_INSTANCETYPE: {
			this->m_strToolTipText = _T("DALI-2 instance type");
			handled = TRUE;
		} break;
		case IDC_CHECK_EVENTFILTERS_0:
		case IDC_CHECK_EVENTFILTERS_1:
		case IDC_CHECK_EVENTFILTERS_2:
		case IDC_CHECK_EVENTFILTERS_3:
		case IDC_CHECK_EVENTFILTERS_4:
		case IDC_CHECK_EVENTFILTERS_5:
		case IDC_CHECK_EVENTFILTERS_6:
		case IDC_CHECK_EVENTFILTERS_7: {
			this->m_strToolTipText = _T("Enable DALI-2 event message (Leave at default value)");
			handled = TRUE;
		} break;
		case IDC_COMBO_INSTANCE_EVENTSCHEME: {
			this->m_strToolTipText = _T("DALI-2 event message scheme (Leave at default value)");
			handled = TRUE;
		} break;
		case IDC_BUTTON_UPLOAD: {
			this->m_strToolTipText = _T("Upload settings to this device");
			handled = TRUE;
		} break;
		case IDC_BUTTON_RESTORE: {
			this->m_strToolTipText = _T("Restore to default settings (Use settings required by the Duotecno system)");
			handled = TRUE;
		} break;
		case IDC_BUTTON_UNDO: {
			this->m_strToolTipText = _T("Undo the last changes");
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
