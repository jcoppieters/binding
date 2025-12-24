// DlgChildUDPTcpIp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildUDPBase.h"

#include "app/appudpcfgserver/appudpservercfg.h"			// UDPdevice
#include "app/appudpcfgserver/appudpservercfgwrapper.h"		// UDP Device wrapper

#include "uicontrols/utils/dlghelper.h"				// GUI fonts
/******************************************************************************/
using namespace DUOTECNO;
/******************************************************************************/
IMPLEMENT_DYNAMIC(CDlgChildUDPBase, CDialog)

CDlgChildUDPBase::CDlgChildUDPBase(UINT id,CWnd* pParent,CDlgUDPConfigCtx& refCtx)
	: CDialog(id, pParent), m_refCtx(refCtx), m_pDlgFontHelper(NULL), m_fWaiting(FALSE)	, m_fShowMsgHardwareReset(FALSE) { }

CDlgChildUDPBase::~CDlgChildUDPBase() { 
	if (NULL != m_pDlgFontHelper) {
		delete m_pDlgFontHelper;
		m_pDlgFontHelper = NULL;
	}
}

void CDlgChildUDPBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChildUDPBase, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgChildUDPTcpIp message handlers
BOOL CDlgChildUDPBase::onLeave( ) {
	BOOL result = TRUE;
	if (this->dataChanged()) {
		if (IDNO == MessageBox("Configuration has not been send.\r\n"
							   "Discard the changes and leave this page anyway?", NULL, MB_ICONQUESTION | MB_YESNO)) {
			result = FALSE;				
		}	
	}
	if (this->busy()) {		
		MessageBox("Configuration is busy. Please wait.",NULL);
		result = FALSE;
	
	}
	return result;
}

CDlgUDPConfigCtx& CDlgChildUDPBase::getUdpCfgCtx() {
	return m_refCtx;
}

BOOL CDlgChildUDPBase::OnInitDialog() {
	CDialog::OnInitDialog();	

	if (NULL == m_pDlgFontHelper) {
		m_pDlgFontHelper = new CDlgHelperFont(*this);	
		ASSERT(NULL != m_pDlgFontHelper);
		if (NULL != m_pDlgFontHelper) {
			m_pDlgFontHelper->initDialog();	
		}
	}

	m_fShowMsgHardwareReset = FALSE;
	AppUDPDevice& refDevice = m_refCtx.getUDPDevice();	
	this->initData(refDevice);
	this->initStaticControls(refDevice);
	PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;
}

LRESULT CDlgChildUDPBase::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	AppUDPDevice& refDevice = m_refCtx.getUDPDevice();

	// Controls van de child dialogen.
	this->updateDynamicControls(refDevice);			

	// Update parent window.
	CWnd* const parentWnd = this->m_refCtx.getParentWnd();
	if (NULL != parentWnd) {
		parentWnd->PostMessage(WM_UPDATE_CONTROLS);
	}		

	if (m_fShowMsgHardwareReset) {
		const CString msg("The configuration changes have been send to the device. The device needs a hardware reset to apply the changes.");
		this->MessageBox(msg,NULL,MB_OK | MB_ICONASTERISK);
		m_fShowMsgHardwareReset = FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Helper functies progressbar toestand.
void CDlgChildUDPBase::setProgressBarBusy( ) {
	this->m_refCtx.setProgressBarState(CDlgUDPConfigCtx::PROGRESSBAR_STATE_BUSY);
}
void CDlgChildUDPBase::setProgressBarError( ) {
	this->m_refCtx.setProgressBarState(CDlgUDPConfigCtx::PROGRESSBAR_STATE_ERROR);
}
void CDlgChildUDPBase::setProgressBarDone( ) {
	this->m_refCtx.setProgressBarState(CDlgUDPConfigCtx::PROGRESSBAR_STATE_DONE);
}

//////////////////////////////////////////////////////////////////////
// Helper functies voor configuratie.
bool CDlgChildUDPBase::sendCfgNetwork(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds) {
	return (this->sendCfg(refDevice,milliseconds,&AppUDPServerCfgWrapper::sendNwkConfig, pResponseCode));
}
bool CDlgChildUDPBase::sendCfgDNSProvider(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds) {
	return (this->sendCfg(refDevice,milliseconds,&AppUDPServerCfgWrapper::sendCfgDNSProvider, pResponseCode));
}
bool CDlgChildUDPBase::sendCfgDynDnsClient(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds) {
	return (this->sendCfg(refDevice,milliseconds,&AppUDPServerCfgWrapper::sendDynDNSSettings, pResponseCode));
}
bool CDlgChildUDPBase::sendCfgTCPIPClient(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds) {
	return (this->sendCfg(refDevice,milliseconds,&AppUDPServerCfgWrapper::sendCfgTCPIPClient, pResponseCode));
}
bool CDlgChildUDPBase::sendCfgSystemMode(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds) {
	return (this->sendCfg(refDevice,milliseconds,&AppUDPServerCfgWrapper::sendCfgSystemMode, pResponseCode));
}
bool CDlgChildUDPBase::sendCfgHomeserver(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds) {
	return (this->sendCfg(refDevice,milliseconds,&AppUDPServerCfgWrapper::sendCfgAppHomeSrv, pResponseCode));
}
bool CDlgChildUDPBase::sendCfgNTP(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds) {
	return (this->sendCfg(refDevice,milliseconds,&AppUDPServerCfgWrapper::sendCfgNTP, pResponseCode));
}
bool CDlgChildUDPBase::sendCfgTimeDate(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds) {
	return (this->sendCfg(refDevice,milliseconds,&AppUDPServerCfgWrapper::sendCfgTimeDate, pResponseCode));
}
bool CDlgChildUDPBase::sendRequestDynDnsStatus(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds) {
	return (this->sendRequest(&refDevice,milliseconds,&AppUDPServerCfgWrapper::sendRequestDynDNSSettings, pResponseCode));
}
bool CDlgChildUDPBase::sendRequestTCPIPClientStatus(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds) {
	return (this->sendRequest(&refDevice,milliseconds,&AppUDPServerCfgWrapper::sendRequestCfgTCPIPClient, pResponseCode));
}
//////////////////////////////////////////////////////////////////////
// Helper functies voor de statusMsg tekst.
void CDlgChildUDPBase::setStatusMsg(MsgIdStatusText_t stringID) {
	const CString* pStrMsg = NULL;
	switch(stringID) {
		case ID_MSG_STATUS_CONFIG_BUSY: {
			static const CString strMsg("Configuration busy. Please wait.");							  
			pStrMsg = &strMsg;
		} break;
		case ID_MSG_STATUS_CONFIG_RETRY: {
			static const CString strMsg("Configuration busy. Retrying. Please wait...");							  
			pStrMsg = &strMsg;							 
		} break;
		case ID_MSG_STATUS_CONFIG_DONE: {
			static const CString strMsg("Configuration done.");
			pStrMsg = &strMsg;
		} break;
		case ID_MSG_STATUS_CONFIG_FAILED: {
			static const CString strMsg("Configuration failed.");							  
			pStrMsg = &strMsg;
		} break;		
		case ID_MSG_STATUS_CONFIG_VERIFY: {
			static const CString strMsg("Configuration busy. Verifying changes. Please wait...");							  
			pStrMsg = &strMsg;						  
		} break;
		case ID_MSG_STATUS_CONFIG_FORMATTER: {
			static const CString strMsg("Configuration failed. The length of the data is too long.");
			pStrMsg = &strMsg;
		} break;
		case ID_MSG_STATUS_REQUEST_BUSY: {
			static const CString strMsg("Requesting status busy. Please wait.");
			pStrMsg = &strMsg;							 
		} break;
		case ID_MSG_STATUS_REQUEST_RETRY: {
			static const CString strMsg("Requesting status busy. Retrying. Please wait...");
			pStrMsg = &strMsg;									  
		} break;
		case ID_MSG_STATUS_REQUEST_DONE: {
			static const CString strMsg("Requesting status done.");
			pStrMsg = &strMsg;								 
		} break;
		case ID_MSG_STATUS_REQUEST_FAILED: {
			static const CString strMsg("Requesting status failed.");
			pStrMsg = &strMsg;											   
		} break;
	}
	ASSERT(NULL != pStrMsg);
	if (NULL != pStrMsg) {
		this->m_refCtx.setStatusMsg(*pStrMsg);
	}
}

void CDlgChildUDPBase::setMsgDescription() {
	CString msg;
	msg.Format("%s configuration.\r\nPress the 'Configure' button to adjust the settings.",this->getStrDescription());
	this->m_refCtx.setStatusMsg(msg);
}
void CDlgChildUDPBase::showMsgBoxInvalidFields(const CString& fieldname) {
	CString msg;
	msg.Format("Cannot send the configuration because the value for the %s is not correct.",fieldname);
	MessageBox(msg,"Invalid data",MB_ICONEXCLAMATION | MB_OK);
}
void CDlgChildUDPBase::showMsgBoxInvalidFields(const CString& fieldname,const CString& info) {
	CString msg;
	msg.Format("Cannot send the configuration because the value for the %s is not correct.\r\n%s",fieldname,info);
	MessageBox(msg,"Invalid data",MB_ICONEXCLAMATION | MB_OK);
}

void CDlgChildUDPBase::onActive(BOOL flag) {
	// Overschrijven door de afgeleide klassen.
}

BOOL CDlgChildUDPBase::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (m_fWaiting)
		return TRUE;

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

int CDlgChildUDPBase::ComboBoxAddEntry(CComboBox* pCtrl,int idx,const char* szCaption,unsigned long ulItemData)
{
	int retVal=-1;
	if (0!=pCtrl) {
		pCtrl->InsertString(idx,szCaption);
		pCtrl->SetItemData(idx,ulItemData);
		retVal=pCtrl->GetCount();
	}
	return retVal;
}

int CDlgChildUDPBase::updateComboBoxSel(CComboBox* pCtrl,int value)
{
	int result = -1;
	const int count = pCtrl->GetCount();
	for (int i=0;i<count;i++) {
		if (value == pCtrl->GetItemData(i)) {
			pCtrl->SetCurSel(i);
			result = 0;
			break;
		}
	}	
	return result;
}

int CDlgChildUDPBase::getItemData(CComboBox& refCtrl,int* pVal) {
	const int sel = refCtrl.GetCurSel();
	if (sel < 0){
		return -1;
	}	
	*pVal = refCtrl.GetItemData(sel);
	return 0;	
}

void CDlgChildUDPBase::OnTimer(UINT_PTR nIDEvent) {
	AppUDPDevice& refDevice = m_refCtx.getUDPDevice();
	this->KillTimer(nIDEvent);
	this->onTimerExpiry(refDevice,nIDEvent);
}

CString CDlgChildUDPBase::getString(CWnd& refCtrl) {
	CString strValue;
	refCtrl.GetWindowText(strValue);
	return strValue;
}

void CDlgChildUDPBase::notifyUserHardwareReset( ) {
	m_fShowMsgHardwareReset = TRUE;
}

bool CDlgChildUDPBase::sendCfg(AppUDPDevice& refDevice,int milliseconds,
							   bool (AppUDPServerCfgWrapper::*pf)(AppUDPDevice& refDevice,int milliseconds),
							   AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode)
{
	ASSERT(NULL != pf);
	HCURSOR hOldCursor = NULL;
	setCursorWaiting(TRUE);
	hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));		
	AppUDPServerCfgWrapper worker;
	const bool res = (worker.*pf)(refDevice,milliseconds);
	if (NULL != pResponseCode) {
		*pResponseCode = worker.getResponseCode();
	}
	setCursorWaiting(FALSE);
	SetCursor(hOldCursor);	
	return res;
}

bool CDlgChildUDPBase::sendRequest(AppUDPDevice* pDevice,int milliseconds,
								   bool (AppUDPServerCfgWrapper::*pf)(AppUDPDevice* pDevice,int milliseconds),
								   AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode)
{
	ASSERT(NULL != pf);
	HCURSOR hOldCursor = NULL;
	setCursorWaiting(TRUE);
	hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));		
	AppUDPServerCfgWrapper worker;
	const bool res = (worker.*pf)(pDevice,milliseconds);
	if (NULL != pResponseCode) {
		*pResponseCode = worker.getResponseCode();
	}
	setCursorWaiting(FALSE);
	SetCursor(hOldCursor);	
	return res;
}

// Helper functies...
bool CDlgChildUDPBase::convertIPAddress(int* ipAddrV4, const CString& refAddress) {
	if (refAddress.IsEmpty()) {
		return false;
	}
	int v[4] = { 0,0,0,0 };
	if(4 == sscanf((LPCSTR)refAddress,"%d.%d.%d.%d",&v[0],&v[1],&v[2],&v[3])) {
		ipAddrV4[0] = v[0];
		ipAddrV4[1] = v[1];
		ipAddrV4[2] = v[2];
		ipAddrV4[3] = v[3];
		return true;
	}
	return false;
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// Context klasse.
CDlgUDPConfigCtx::CDlgUDPConfigCtx(CWnd* pParentWnd, IAppDlgUDPLanCfg_Host& refHost,const AppUDPDevice& refDevice) :
	m_pParentWnd(pParentWnd), m_refHost(refHost), m_proxyUdpDevice(NULL), m_statusMsg( ), 
	m_stateProgressBar(PROGRESSBAR_STATE_INVISIBLE), m_guiChangesFlags(0) 
{ 
	this->setUDPDevice(refDevice);
}

CDlgUDPConfigCtx::~CDlgUDPConfigCtx( ) {
	if (NULL != m_proxyUdpDevice) {
		delete m_proxyUdpDevice;
		m_proxyUdpDevice = NULL;
	}	
}

bool CDlgUDPConfigCtx::statusMsgChanged( ) const {
	return (FLAG_GUI_CHANGED_STATUSMSG == (m_guiChangesFlags & FLAG_GUI_CHANGED_STATUSMSG));
}

bool CDlgUDPConfigCtx::stateProgressBarChanged( ) const {
	return (FLAG_GUI_CHANGED_PROGRESSBAR == (m_guiChangesFlags & FLAG_GUI_CHANGED_PROGRESSBAR));
}

void CDlgUDPConfigCtx::setStatusMsg(const CString& msg) {
	if (0 != m_statusMsg.Compare(msg)) {
		m_statusMsg = msg;	
		m_guiChangesFlags |= FLAG_GUI_CHANGED_STATUSMSG;
	}
}

void CDlgUDPConfigCtx::setProgressBarState(ProgressBarState_t state) {
	if (state != m_stateProgressBar) {
		m_stateProgressBar = state;
		m_guiChangesFlags |= FLAG_GUI_CHANGED_PROGRESSBAR;
	}
}

AppUDPDevice& CDlgUDPConfigCtx::getUDPDevice() {
	ASSERT(NULL != m_proxyUdpDevice);
	return *m_proxyUdpDevice;
}

void CDlgUDPConfigCtx::setUDPDevice(const AppUDPDevice& r) {
	if (m_proxyUdpDevice == &r) {
		return;
	}
	if (NULL != m_proxyUdpDevice) {
		delete m_proxyUdpDevice;
		m_proxyUdpDevice = NULL;
	}
	m_proxyUdpDevice = new AppUDPDevice(r);
	ASSERT(NULL != m_proxyUdpDevice);
}
/*****************************************************************/

