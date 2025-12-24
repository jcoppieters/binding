// DlgChildDaliControlgearTest.cpp : implementation file
//
#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliControlgearTest.h"
#include "appdlgdaliworker.h"
/****************************************************************************************************/
using namespace DUOTECNO::DALI;
/****************************************************************************************************/

/****************************************************************************************************/
enum {
	TIMER_VALUE_QUERY_ACTUAL_VALUE_ENTER_DIALOG_MSEC	= 1000,			// Bij het actief komen van de dialoog.
	TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND		= 100,			// Na de bediening van een set commando.
	TIMER_VALUE_QUERY_ACTUAL_VALUE_POLLING				= 3000,			// Normale polling.
};

/****************************************************************************************************/
static const CString STRING_FOOTER_OFFLINE("Connect online to send test commands to the device.");
static const CString STRING_FOOTER_ONLINE("Test the device online using commands.");
/****************************************************************************************************/
// CDlgChildDaliControlgearTest dialog
IMPLEMENT_DYNAMIC(CDlgChildDaliControlgearTest, CDialog)

CDlgChildDaliControlgearTest::CDlgChildDaliControlgearTest(CAppDlgDaliWorker& refWorker,CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker,CDlgChildDaliControlgearTest::IDD, pParent), m_deviceActualValueRead(-1), m_mapSceneNames()  
{ 
	this->setInitDone(false);
	this->setPageCurrActive(false);
}

CDlgChildDaliControlgearTest::~CDlgChildDaliControlgearTest() { }

void CDlgChildDaliControlgearTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_OFF, m_btnSetOff);
	DDX_Control(pDX, IDC_BUTTON_MINIMUM, m_btnSetMin);
	DDX_Control(pDX, IDC_BUTTON_MAXIMUM, m_btnSetMax);
	DDX_Control(pDX, IDC_BUTTON_DIM_DOWN, m_btnDimDown);
	DDX_Control(pDX, IDC_BUTTON_DIM_UP, m_btnDimUp);
	DDX_Control(pDX, IDC_BUTTON_DIM_STEP_UP, m_btnDimStepUp);
	DDX_Control(pDX, IDC_BUTTON_DIM_STEP_DOWN, m_btnDimStepDown);
	DDX_Control(pDX, IDC_BUTTON_SET, m_btnSetValue);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_editValue);
	DDX_Control(pDX, IDC_SLIDER_VALUE, m_sliderDimValue);
	DDX_Control(pDX, IDC_STATIC_GROUP_LEVEL, m_staticGroupLevel);
	DDX_Control(pDX, IDC_EDIT_VALUE_READ, m_editValueRead);
	DDX_Control(pDX, IDC_STATIC_VALUE_READ, m_staticValueRead);
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
}

BEGIN_MESSAGE_MAP(CDlgChildDaliControlgearTest, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OFF,OnButtonClickedSetOff)
	ON_BN_CLICKED(IDC_BUTTON_MINIMUM,OnButtonClickedSetMin)
	ON_BN_CLICKED(IDC_BUTTON_MAXIMUM,OnButtonClickedSetMax)
	ON_BN_CLICKED(IDC_BUTTON_DIM_DOWN,OnButtonClickedDimDown)
	ON_BN_CLICKED(IDC_BUTTON_DIM_UP,OnButtonClickedDimUp)
	ON_BN_CLICKED(IDC_BUTTON_DIM_STEP_UP,OnButtonClickedDimStepUp)
	ON_BN_CLICKED(IDC_BUTTON_DIM_STEP_DOWN,OnButtonClickedDimStepDown)
	ON_BN_CLICKED(IDC_BUTTON_SET,OnButtonClickedSetValue)
	ON_MESSAGE(WM_UPDATE_CONTROLS, OnUpdateControls)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VALUE, &CDlgChildDaliControlgearTest::OnNMReleasedcaptureSliderValue)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_SCENE0, IDC_BUTTON_SCENE15, OnButtonClickedSceneTest)	
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliControlgearTest::OnToolTipNeedText)
END_MESSAGE_MAP()

// CDlgChildDaliControlgearTest message handlers
void CDlgChildDaliControlgearTest::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
	// this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ENTER_DIALOG_MSEC, (TIMERPROC)NULL);
	this->m_deviceActualValueRead = -1;
	this->setDevice(pDevice);
	this->updateViewMode();	
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearTest::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) {	
	this->KillTimer(TIMER_ID_QUERY);
	if (activeDialog) {		
		this->getWorker().getSceneNames(&m_mapSceneNames);
		this->setPageCurrActive(true);
		this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ENTER_DIALOG_MSEC, (TIMERPROC)NULL);
	}
	else {		
		this->setPageCurrActive(false);
	}
}
BOOL CDlgChildDaliControlgearTest::OnParentPropertyPageLeave() {
	this->KillTimer(TIMER_ID_QUERY);
	this->setPageCurrActive(false);
	return FALSE;
}

void CDlgChildDaliControlgearTest::onTabDialogLeaveDialog( ) {
	this->KillTimer(TIMER_ID_QUERY);
	this->setPageCurrActive(false);
}

void CDlgChildDaliControlgearTest::onTabDialogEnterDialog( ) {
	this->getWorker().getSceneNames(&m_mapSceneNames);
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ENTER_DIALOG_MSEC, (TIMERPROC)NULL);
	this->setPageCurrActive(true);
}

void CDlgChildDaliControlgearTest::OnButtonClickedSetOff() {
	if (VIEWMODE_ONLINE != this->getViewMode())
		return;

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) 
		return;

	this->KillTimer(TIMER_ID_QUERY);
	this->getWorker().sendDeviceOff(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE, pDevice->getShortAddress());
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND, (TIMERPROC)NULL);
}

void CDlgChildDaliControlgearTest::OnButtonClickedSetMin() {
	if (VIEWMODE_ONLINE != this->getViewMode())
		return;

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) 
		return;

	this->KillTimer(TIMER_ID_QUERY);
	this->getWorker().sendDeviceMinLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE, pDevice->getShortAddress());
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND, (TIMERPROC)NULL);
}

void CDlgChildDaliControlgearTest::OnButtonClickedSetMax() {
	if (VIEWMODE_ONLINE != this->getViewMode())
		return;

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) 
		return;

	this->KillTimer(TIMER_ID_QUERY);
	this->getWorker().sendDeviceMaxLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE, pDevice->getShortAddress());
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND, (TIMERPROC)NULL);
}

void CDlgChildDaliControlgearTest::OnButtonClickedDimStepUp() {
	if (VIEWMODE_ONLINE != this->getViewMode())
		return;

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) 
		return;

	this->KillTimer(TIMER_ID_QUERY);
	this->getWorker().sendDeviceDimStepUp(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE, pDevice->getShortAddress());
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND, (TIMERPROC)NULL);
}

void CDlgChildDaliControlgearTest::OnButtonClickedDimStepDown() {
	if (VIEWMODE_ONLINE != this->getViewMode())
		return;

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) 
		return;

	this->KillTimer(TIMER_ID_QUERY);
	this->getWorker().sendDeviceDimStepDown(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE, pDevice->getShortAddress());
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND, (TIMERPROC)NULL);
}

void CDlgChildDaliControlgearTest::OnButtonClickedDimUp() {
	if (VIEWMODE_ONLINE != this->getViewMode())
		return;

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) 
		return;

	this->KillTimer(TIMER_ID_QUERY);
	this->getWorker().sendDeviceDimUp(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE, pDevice->getShortAddress());
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND, (TIMERPROC)NULL);
}

void CDlgChildDaliControlgearTest::OnButtonClickedDimDown() {
	if (VIEWMODE_ONLINE != this->getViewMode())
		return;

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) 
		return;

	this->KillTimer(TIMER_ID_QUERY);
	this->getWorker().sendDeviceDimDown(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE, pDevice->getShortAddress());
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND, (TIMERPROC)NULL);
}

void CDlgChildDaliControlgearTest::OnButtonClickedSetValue() {
	if (this->getViewMode() != VIEWMODE_ONLINE) {
		return;
	}	
	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) {
		return;
	}			
	this->KillTimer(TIMER_ID_QUERY);
	const int value = m_sliderDimValue.GetPos( );
	if (value >=0 && value<=254) {
		this->getWorker().sendDeviceDimActualLevel(CAppDlgDaliWorker::CONTROLGEAR_MSG_DEVICE,pDevice->getShortAddress(),value);
	}
	this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_ON_SETCOMMAND, (TIMERPROC)NULL);
}

void CDlgChildDaliControlgearTest::OnTimer(UINT nIDEvent) {
	if ((TIMER_ID_QUERY == nIDEvent) && (this->getPageCurrActive())) {			
		this->KillTimer(TIMER_ID_QUERY);
		if (this->getViewMode() == VIEWMODE_ONLINE) {
			CAppDaliDevice* const pDevice = this->getDevice();
			if (NULL != pDevice) {								
				if (this->IsWindowVisible()) {
					this->getWorker().startControlGearQueryActualLevel(pDevice->getShortAddress());
				}							
			}
		}
		this->SetTimer(TIMER_ID_QUERY, TIMER_VALUE_QUERY_ACTUAL_VALUE_POLLING, (TIMERPROC)NULL);
	}
}

LRESULT CDlgChildDaliControlgearTest::OnUpdateControls(WPARAM wParam, LPARAM lParam) {
	const BOOL windowsEnabled = (VIEWMODE_OFFLINE == this->getViewMode() ? FALSE : TRUE);
	this->setBusyGuiControlsUpdate(true);
	
	if (!this->getInitDone()) {
		m_sliderDimValue.SetRange(0,254);
		m_sliderDimValue.SetTicFreq(16);
		m_sliderDimValue.SetPos(0);
		this->EnableToolTips(TRUE);
		this->setInitDone(true);
	}

	this->m_staticFooterInfoText.SetWindowText((VIEWMODE_OFFLINE == this->getViewMode() ? STRING_FOOTER_OFFLINE : STRING_FOOTER_ONLINE));

	if (windowsEnabled) {
		{
			CString strValue;
			const int value = m_sliderDimValue.GetPos( );
			if (value >=0 && value<=254) {			
				strValue.Format("%d",value);			
			}
			m_editValue.SetWindowText(strValue);
		}
		{
			CString strValue;
			if (this->m_deviceActualValueRead >= 0) {
				strValue.Format("%d",this->m_deviceActualValueRead);	
			}
			m_editValueRead.SetWindowText(strValue);
		}
	}

	// Scene test buttons.
	for (int id=IDC_BUTTON_SCENE0;id<=IDC_BUTTON_SCENE15;++id) {
		CWnd* const pWnd = this->GetDlgItem(id);
		if (NULL != pWnd) {
			pWnd->EnableWindow(windowsEnabled);
		}
	}
	{
		CWnd* const pWnd = this->GetDlgItem(IDC_STATIC_GROUP_SCENES);
		if (NULL != pWnd) {
			pWnd->EnableWindow(windowsEnabled);
		}
	}

	m_btnSetOff.EnableWindow(windowsEnabled);
	m_btnSetMin.EnableWindow(windowsEnabled);
	m_btnSetMax.EnableWindow(windowsEnabled);
	m_btnDimDown.EnableWindow(windowsEnabled);
	m_btnDimUp.EnableWindow(windowsEnabled);
	m_btnDimStepUp.EnableWindow(windowsEnabled);
	m_btnDimStepDown.EnableWindow(windowsEnabled);
	m_btnSetValue.EnableWindow(windowsEnabled);
	m_editValue.EnableWindow(windowsEnabled);
	m_sliderDimValue.EnableWindow(windowsEnabled);
	m_staticGroupLevel.EnableWindow(windowsEnabled);
	m_editValueRead.EnableWindow(windowsEnabled);
	m_staticValueRead.EnableWindow(windowsEnabled);

	this->setBusyGuiControlsUpdate(false);
	return TRUE;
}

// IAppDlgDaliGui_Host	
void CDlgChildDaliControlgearTest::onNotifyGuiTaskStarted(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_QUERY_ACTUAL_LEVEL == task) {
		// Absorbe niks doen.
	}
	this->setViewMode(VIEWMODE_ONLINE_BUSY);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearTest::onNotifyGuiTaskDone(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (APPDALI_FSM_TASK_CONTROLGEAR_QUERY_ACTUAL_LEVEL == task) {
		DUOTECNO::DALI::CAppDaliDevice* const pDevice = this->getDevice();
		this->m_deviceActualValueRead  = -1;
		if (NULL != pDevice) {
			this->m_deviceActualValueRead = this->getWorker().getDeviceActualLevel(*pDevice);
		}
	}
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliControlgearTest::onNotifyGuiTaskError(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task, const CString& strMessage) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}


void CDlgChildDaliControlgearTest::OnNMReleasedcaptureSliderValue(NMHDR *pNMHDR, LRESULT *pResult) {
	this->PostMessage(WM_UPDATE_CONTROLS);
	*pResult = 0;
}

void CDlgChildDaliControlgearTest::OnButtonClickedSceneTest(UINT nID) {
	const int sceneNumber = nID - IDC_BUTTON_SCENE0;	
	this->getWorker().sendGotoScene(CAppDlgDaliWorker::CONTROLGEAR_MSG_BROADCAST,0xff,sceneNumber);
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgChildDaliControlgearTest::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}

	BOOL handled = FALSE;
	if (identifier >= IDC_BUTTON_SCENE0 && identifier<=IDC_BUTTON_SCENE15) {
		const int sceneNumber = identifier - IDC_BUTTON_SCENE0;
		if (sceneNumber>=0 && sceneNumber<=15) {
			CString strName;
			if (m_mapSceneNames.Lookup(sceneNumber,strName)) {
				this->m_strToolTipText.Format("Test scene '%s'.",strName);
				handled = TRUE;
			}
		}		
	}
	else {
		switch(identifier) {
			case IDC_BUTTON_OFF: {
				this->m_strToolTipText = _T("Send test command: Off");
				handled = TRUE;
			} break;
			case IDC_BUTTON_MINIMUM: {
				this->m_strToolTipText = _T("Send test command: Minimum value");
				handled = TRUE;
			} break;
			case IDC_BUTTON_MAXIMUM: {
				this->m_strToolTipText = _T("Send test command: Maximum value");
				handled = TRUE;
			} break;
			case IDC_BUTTON_DIM_STEP_DOWN: {
				this->m_strToolTipText = _T("Send test command: Decrease by step");
				handled = TRUE;
			} break;
			case IDC_BUTTON_DIM_STEP_UP: {
				this->m_strToolTipText = _T("Send test command: Increase by step");
				handled = TRUE;
			} break;
			case IDC_SLIDER_VALUE: 
			case IDC_EDIT_VALUE: {
				this->m_strToolTipText = _T("Set target value (range: 0–254)");
				handled = TRUE;
			} break;
			case IDC_BUTTON_SET: {
				this->m_strToolTipText = _T("Send test command: Set value");
				handled = TRUE;
			} break;
			case IDC_BUTTON_DIM_DOWN: {
				this->m_strToolTipText = _T("Send test command: Dim down continuously");
				handled = TRUE;
			} break;
			case IDC_BUTTON_DIM_UP: {
				this->m_strToolTipText = _T("Send test command: Dim up continuously");
				handled = TRUE;
			} break;
			/*
			case IDC_EDIT_VALUE_READ: {
				this->m_strToolTipText = _T("Current actual device value (read-only)");
				handled = TRUE;
			} break;
			*/
		}					
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
