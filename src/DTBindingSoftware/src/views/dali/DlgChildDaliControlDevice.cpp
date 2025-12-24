// DlgChildDaliControlDevice.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliControlDevice.h"
#include "appdlgdaliworker.h"
#include "app/appdali/appdali_def.h"

using namespace DUOTECNO::DALI;

/****************************************************************************************************/
static const CString STRING_FOOTER_OFFLINE("Connect online to change the input device configuration.");
static const CString STRING_FOOTER_ONLINE("Change the device configuration. the changes will be applied immediately.");
/****************************************************************************************************/
static CAppDaliDeviceProperties_InputControlDevice* getDeviceInstance(CAppDaliDevice* pDevice) {
	if (NULL == pDevice) {
		return NULL;
	}
	return dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDevice->getProperties());
}

// CDlgChildDaliControlDevice dialog

IMPLEMENT_DYNAMIC(CDlgChildDaliControlDevice, CDialog)

CDlgChildDaliControlDevice::CDlgChildDaliControlDevice(CAppDlgDaliWorker& refWorker, CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker,CDlgChildDaliControlDevice::IDD, pParent),
	  m_appControllerEnabled(false), m_appControllerPresent(false),m_appControllerAlwaysOn(false), m_powerCycleNotification(false)
{

}

CDlgChildDaliControlDevice::~CDlgChildDaliControlDevice()
{
}

void CDlgChildDaliControlDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_APPLICATION_CONTROLLER, m_btnCheckAppController);	
	DDX_Control(pDX, IDC_CHECK_APPLICATION_POWERCYCLE, m_btnCheckPowerCycleNotify);	
	DDX_Control(pDX, IDC_CHECK_APPLICATION_INSTANCES, m_btnCheckInstancesEnabled);	
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
	DDX_Control(pDX, IDC_STATIC_ICON_WARNING, m_staticWarningIcon);
	DDX_Control(pDX, IDC_STATIC_TEXT_WARNING, m_staticTextWarning);
}


BEGIN_MESSAGE_MAP(CDlgChildDaliControlDevice, CDialog)
	ON_BN_CLICKED(IDC_CHECK_APPLICATION_CONTROLLER,OnButtonClickedAppControler)
	ON_BN_CLICKED(IDC_CHECK_APPLICATION_POWERCYCLE,OnButtonClickedPowerCycle)
	ON_BN_CLICKED(IDC_CHECK_APPLICATION_INSTANCES,OnButtonClickedInstances)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliControlDevice::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgChildDaliControlDevice message handlers
void CDlgChildDaliControlDevice::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	this->setDevice(pDevice);	
	this->updateViewMode();
	this->updateModel();
	this->updateGUIControls();
}

void CDlgChildDaliControlDevice::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) {
	this->setDevice(pDevice);
	if (activeDialog) {
		this->updateViewMode();
		this->updateModel();
		this->updateGUIControls();
	}
}

void CDlgChildDaliControlDevice::onTabDialogEnterDialog( ) {
	this->updateViewMode();
	this->updateModel();
	this->updateGUIControls();
}

void CDlgChildDaliControlDevice::updateModel() {

	this->m_appControllerEnabled = false;
	this->m_appControllerPresent = false;
	this->m_appControllerAlwaysOn = false;
	this->m_powerCycleNotification = false;
	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) {	
		return;
	}
	CAppDaliDeviceProperties_InputControlDevice* const properties = getDeviceInstance(pDevice);
	if (NULL == properties) {
		return;
	}
	const int deviceCapabilities = properties->getDeviceCapabilities();
	if (deviceCapabilities & DALI_CONTROLDEVICE_CAPABILITIES_APPCONTROLLER_PRESENT) {						
		this->m_appControllerPresent = true;
		if (deviceCapabilities & DALI_CONTROLDEVICE_CAPABILITIES_APPCONTROLLER_ALWAYS_ON) {
			this->m_appControllerAlwaysOn = true;
		}
		this->m_appControllerEnabled = properties->getAppControllerEnabled();
	}

	this->m_powerCycleNotification = properties->getPowerCycleNotification();
}

void CDlgChildDaliControlDevice::updateGUIControls( ) {
	
	this->setBusyGuiControlsUpdate(true);

	if (!this->getInitDone()) {			
		this->m_staticWarningIcon.ShowWindow(FALSE);
		initPictureControlWarningIcon(*this,this->m_staticWarningIcon);		
		this->EnableToolTips(TRUE);
		this->setInitDone(true);
	}

	const bool enableControlsOnline = (this->getViewMode() == VIEWMODE_ONLINE ? true : false);	
	if (NULL == this->getDevice()) {
		m_staticTextWarning.ShowWindow(FALSE);
		m_staticWarningIcon.ShowWindow(FALSE);
		m_btnCheckAppController.EnableWindow(FALSE);
		m_btnCheckPowerCycleNotify.EnableWindow(FALSE);
		m_btnCheckInstancesEnabled.EnableWindow(FALSE);
	}
	else{

		m_btnCheckInstancesEnabled.ShowWindow(FALSE);

		if (false == this->m_appControllerPresent) {
			m_staticTextWarning.ShowWindow(FALSE);
			m_staticWarningIcon.ShowWindow(FALSE);
			m_btnCheckAppController.EnableWindow(FALSE);
			m_btnCheckAppController.SetCheck(0);
		}
		else if (this->m_appControllerAlwaysOn) {
			static const CString msgWarning("Warning!\r\n"
											"This input control device holds an internal application controller.\r\n"
											"The internal application controller cannot be disabled for this device.\r\n"
										    "The application controller may potentially affect the operation of the DALI system.");	
			m_staticTextWarning.SetWindowText(msgWarning);
			m_staticTextWarning.ShowWindow(TRUE);
			m_staticWarningIcon.ShowWindow(TRUE);
			m_btnCheckAppController.EnableWindow(FALSE);
			m_btnCheckAppController.SetCheck(1);
		}
		else if (this->m_appControllerEnabled) {
			static const CString msgWarning("Warning!\r\n"
											"The application controller may potentially affect the operation of the DALI system.\r\n"
										    "To avoid problems disable the application controller");
			m_staticTextWarning.SetWindowText(msgWarning);
			m_staticTextWarning.ShowWindow(TRUE);
			m_staticWarningIcon.ShowWindow(TRUE);
			m_btnCheckAppController.EnableWindow(TRUE && (enableControlsOnline == true));
			m_btnCheckAppController.SetCheck(TRUE);
		}
		else {
			m_staticTextWarning.ShowWindow(FALSE);
			m_staticWarningIcon.ShowWindow(FALSE);
			m_btnCheckAppController.EnableWindow(TRUE && (enableControlsOnline == true));
			m_btnCheckAppController.SetCheck(FALSE);		
		}
		
		// Read only property...
		m_btnCheckPowerCycleNotify.EnableWindow(FALSE);
		m_btnCheckPowerCycleNotify.SetCheck(this->m_powerCycleNotification ? 1 : 0);			
	}

	this->m_staticFooterInfoText.SetWindowText((VIEWMODE_OFFLINE == this->getViewMode() ? STRING_FOOTER_OFFLINE : STRING_FOOTER_ONLINE));

	this->setBusyGuiControlsUpdate(false);
}


void CDlgChildDaliControlDevice::OnButtonClickedAppControler() {
	bool execute = true;
	const bool stateOnOff = (BST_CHECKED == m_btnCheckAppController.GetCheck() ? true : false);
	if (stateOnOff) {		
		const CString strTitle("Enable the device application controller?");
		const CString msg("It is not recommended to enable the application controller for a device.\r\n"
					      "Enabling the application controller may cause the input device to perform actions "
						  "that were not configured through this software. "
						  "Depending on the type of input device, different addresses may be assigned, "
						  "or unintended groups may be created.\r\n\r\n"
						  "Are you sure you want to enable the application's controller?");

		if (IDNO == this->MessageBox(msg,strTitle,MB_YESNO | MB_ICONQUESTION)) {
			execute = false;
		}			
	}

	if (execute) {				
		CAppDaliDevice* const pDevice = this->getDevice();
		if (NULL != pDevice) {
			CAppDaliDeviceProperties_InputControlDevice* const properties = getDeviceInstance(pDevice);
			if (NULL != properties) {				
				if (this->getWorker().controlDeviceSetApplicationControllerOnOff(pDevice->getShortAddress(),stateOnOff)) {
					properties->setAppControllerEnabled(stateOnOff);
				}
			}
		}
	}
	this->updateModel();
	this->updateGUIControls();
}

void CDlgChildDaliControlDevice::OnButtonClickedPowerCycle() {
	// Read only property...
	this->updateGUIControls();
}

void CDlgChildDaliControlDevice::OnButtonClickedInstances() {
	// Read only property...
	this->updateGUIControls();
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgChildDaliControlDevice::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_CHECK_APPLICATION_CONTROLLER: {
			this->m_strToolTipText = _T("Toggle the application controller on or off. CAUTION: Keep off unless specifically required.");
			handled = TRUE;
		} break;
		case IDC_CHECK_APPLICATION_POWERCYCLE: {
			this->m_strToolTipText = _T("Indicates power cycle notification status (read-only property).");
			handled = TRUE;
		} break;
		case IDC_CHECK_APPLICATION_INSTANCES: {
			this->m_strToolTipText = _T("Shows if control input instances are enabled (read-only property).");
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
