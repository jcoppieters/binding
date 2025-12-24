#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDali.h"
#include "appdlgdaliworker.h"


CDlgChildDali::CDlgChildDali(CAppDlgDaliWorker& refWorker,UINT nIDTemplate,CWnd* pParent) : 
	CDialog(nIDTemplate,pParent) , m_refWorker(refWorker), m_pDevice(NULL), 
	m_viewMode(VIEWMODE_OFFLINE), m_flags(0), m_currDeviceShortAddress(-1) { }

// IAppDlgDaliGui_Host	
void CDlgChildDali::onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) { 
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	if (statusConnected) {
		if (VIEWMODE_OFFLINE == this->getViewMode()) {
			this->setViewMode(VIEWMODE_ONLINE);
		}
	}
	else {
		this->setViewMode(VIEWMODE_OFFLINE);
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDali::onNotifyGuiMsgCommunication(BYTE bNodeAddress, Type_t typeMessage, const CString& msg) { }
void CDlgChildDali::onNotifyGuiTaskStarted(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task) { }
void CDlgChildDali::onNotifyGuiTaskDone(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task) { }
void CDlgChildDali::onNotifyGuiTaskError(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task,const CString& strMessage) { }
void CDlgChildDali::onNotifyGuiFoundDevice(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice) { }
void CDlgChildDali::onNotifyGuiUpdateDeviceSerialNumber(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long serialNumber) { }
void CDlgChildDali::onNotifyGuiUpdateDeviceGTIN(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long GTIN) { }
void CDlgChildDali::onNotifyGuiRxDaliMessage(BYTE bNodeAddress,BYTE lengthBits,uint32_t recievedFrame) { }

void CDlgChildDali::updateViewMode() {
	if (this->getWorker().isConnected()) {
		if (this->getWorker().getDaliCfgTaskBusy()) {
			this->setViewMode(VIEWMODE_ONLINE_BUSY);
		}
		else {
			this->setViewMode(VIEWMODE_ONLINE);
		}
	}
	else {
		this->setViewMode(VIEWMODE_OFFLINE);
	}
}

BOOL CDlgChildDali::initPictureControlWarningIcon(CDlgChildDali& refThis,CStatic& refPictureControl) {
	HICON hIconWarning = (HICON)LoadImage(
		NULL,
		MAKEINTRESOURCE(IDI_WARNING),
		IMAGE_ICON,
		32, 32,
		LR_SHARED
	);
	if (NULL == hIconWarning) {
		return FALSE;
	}

	refPictureControl.SetIcon(hIconWarning);
	return true;
}

CString CDlgChildDali::getStrMsgInfoFooter(CDlgChildDali& refThis,bool uploadBtnNeeded) {
	CString strMessage;
	if (refThis.getViewMode() == VIEWMODE_OFFLINE) {
		strMessage = "Connect online to upload changes to the DALI system bus.";
	}
	else if (refThis.getViewMode() == VIEWMODE_ONLINE_BUSY) {
		strMessage = "Configuration busy, please wait...";
	}
	else {
		if (refThis.getPropertyChanged()) {
			if (uploadBtnNeeded) {
				strMessage = "Press the Upload button to send the changes to the DALI bus.";	
			}
		}
		else if (refThis.getUploadDone()) {
			strMessage = "Uploading configuration done.";
		}	
		else if (refThis.getDownloadDone()) {
			strMessage = "Downloading configuration done.";
		}	
	}
	return strMessage;
}

	// Melding geven dat er een upload bezig is.
void CDlgChildDali::promptUserUploadBusy(CDlgChildDali& refThis) {
	const CString msgTitle("Upload busy, please wait...");
	const CString msg("Busy uploading configuration changes to the DALI system bus.\r\n"
					  "Please wait untill all changes are uploaded.");

	refThis.MessageBox(msg,msgTitle,MB_OK | MB_ICONINFORMATION);
}

// Melding geven dat er een upload bezig is.
void CDlgChildDali::promptUserDownloadBusy(CDlgChildDali& refThis) {
	const CString msgTitle("Download busy, please wait...");
	const CString msg("Busy reading configuration from the DALI system bus.\r\n"
					  "Please wait untill all data is downloaded.");

	refThis.MessageBox(msg,msgTitle,MB_OK | MB_ICONINFORMATION);
}


BOOL CDlgChildDali::promptUserUploadChangesBeforeLeavingDialog(CDlgChildDali& refThis) {
	const CString msg("Changes have been made but the data has not been uploaded.\r\n"
					  "If you leave the dialog the changes are discarded.\r\n\r\n"
				      "Select 'yes' to upload the changes to the system.\r\n"
					  "Select 'no' to discard the changes.");
	const CString msgTitle("Upload changed data?");
	if (IDYES == refThis.MessageBox(msg,msgTitle,MB_YESNO | MB_ICONQUESTION)) {
		return TRUE;
	}
	return FALSE;
}


// Vragen aan de gebruiker om de veranderingen te negeren.
// return waarde TRUE: Undo changes
BOOL CDlgChildDali::promptUserUndoChanges(CDlgChildDali& refThis) {
	CString strTitle("Undo changes?");
	CString strMsg("Do you want to undo the last changes?");
	if (IDNO == refThis.MessageBox(strMsg, strTitle, MB_ICONQUESTION | MB_YESNO)) {	    
		return FALSE;
	}
	return TRUE;
}

// Vragen aan de gebruiker om een restore to defaults uit te voeren.
// return waarde TRUE: restore defaults.
BOOL CDlgChildDali::promptUserRestoreDefaults(CDlgChildDali& refThis) {
	CString strTitle("Restore to defaults?");
	CString strMsg("Do you want to restore to the default settings?");
	if (IDNO == refThis.MessageBox(strMsg, strTitle, MB_ICONQUESTION | MB_YESNO)) {	    
		return FALSE;
	}
	return TRUE;
}
	


static void helper_setFlag(unsigned short& v, unsigned short value, bool flagStateOnOff) {
	if (flagStateOnOff) {
		v |= value;
	}
	else {
		v &= ~value;
	}
}

// De waarschuwing C4800 in MSVC2008 wordt gegenereerd wanneer een bool-type impliciet wordt geconverteerd van een niet-bool-type. 
#pragma warning(push)							// Sla de huidige waarschuwingstoestand op
#pragma warning(disable : 4800)					// Schakel waarschuwing C4800 uit

void CDlgChildDali::setPropertyChanged(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_PROPERTY_CHANGED, flagStateOnOff);
}

bool CDlgChildDali::getPropertyChanged( ) {
	return (this->m_flags & FLAG_PROPERTY_CHANGED); 
}

void CDlgChildDali::setUploadDone(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_UPLOAD_DONE, flagStateOnOff);
}
bool CDlgChildDali::getUploadDone( ) {
	return (this->m_flags & FLAG_UPLOAD_DONE); 
}

void CDlgChildDali::setUploadBusy(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_UPLOAD_BUSY, flagStateOnOff);
}
bool CDlgChildDali::getUploadBusy( ) {
	return (this->m_flags & FLAG_UPLOAD_BUSY); 
}

void CDlgChildDali::setDownloadDone(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_DOWNLOAD_DONE, flagStateOnOff);
}
bool CDlgChildDali::getDownloadDone( ) {
	return (this->m_flags & FLAG_DOWNLOAD_DONE); 
}

void CDlgChildDali::setDownloadBusy(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_DOWNLOAD_BUSY, flagStateOnOff);
}
bool CDlgChildDali::getDownloadBusy( ) {
	return (this->m_flags & FLAG_DOWNLOAD_BUSY); 
}

void CDlgChildDali::setInitDone(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_INIT_DONE, flagStateOnOff);
}

bool CDlgChildDali::getInitDone() {
	return (this->m_flags & FLAG_INIT_DONE); 
}

void CDlgChildDali::setPageCurrActive(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_PAGE_ACTIVE, flagStateOnOff);
}
bool CDlgChildDali::getPageCurrActive( ) {
	return (this->m_flags & FLAG_PAGE_ACTIVE);
}

void CDlgChildDali::setBusyGuiControlsUpdate(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_BUSY_CONTROLS_UPDATE, flagStateOnOff);
}
bool CDlgChildDali::getBusyGuiControlsUpdate( ) {
	return (this->m_flags & FLAG_BUSY_CONTROLS_UPDATE);
}

void CDlgChildDali::setRequestParentUpdateGUI(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_REQUEST_PARENT_UPDATE_GUI, flagStateOnOff);
}
bool CDlgChildDali::getRequestParentUpdateGUI( ) {
	return (this->m_flags & FLAG_REQUEST_PARENT_UPDATE_GUI);
}

#pragma warning(pop)          // Herstel de vorige waarschuwingstoestand