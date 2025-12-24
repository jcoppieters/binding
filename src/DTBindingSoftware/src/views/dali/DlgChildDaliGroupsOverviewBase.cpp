#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliGroupsOverviewBase.h"
#include "appdlgdaliworker.h"


CDlgChildDaliGroupsOverviewBase::CDlgChildDaliGroupsOverviewBase(CAppDlgDaliWorker& refWorker,UINT nIDTemplate,CWnd* pParent) : 
	CDialog(nIDTemplate,pParent) , m_refWorker(refWorker),
	m_viewMode(VIEWMODE_OFFLINE), m_flags(0), m_selectedGroup(-1) { }

// IAppDlgDaliGui_Host	
void CDlgChildDaliGroupsOverviewBase::onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) { 
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

void CDlgChildDaliGroupsOverviewBase::onNotifyGuiMsgCommunication(BYTE bNodeAddress, Type_t typeMessage, const CString& msg) { }
void CDlgChildDaliGroupsOverviewBase::onNotifyGuiTaskStarted(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task) { }
void CDlgChildDaliGroupsOverviewBase::onNotifyGuiTaskDone(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task) { }
void CDlgChildDaliGroupsOverviewBase::onNotifyGuiTaskError(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task,const CString& strMessage) { }
void CDlgChildDaliGroupsOverviewBase::onNotifyGuiFoundDevice(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice) { }
void CDlgChildDaliGroupsOverviewBase::onNotifyGuiUpdateDeviceSerialNumber(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long serialNumber) { }
void CDlgChildDaliGroupsOverviewBase::onNotifyGuiUpdateDeviceGTIN(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long GTIN) { }
void CDlgChildDaliGroupsOverviewBase::onNotifyGuiRxDaliMessage(BYTE bNodeAddress,BYTE lengthBits,uint32_t recievedFrame) { }

BOOL CDlgChildDaliGroupsOverviewBase::updateViewMode() {
	ViewMode_t mode = VIEWMODE_OFFLINE;
	if (this->getWorker().isConnected()) {
		if (this->getWorker().getDaliCfgTaskBusy()) {
			mode = VIEWMODE_ONLINE_BUSY;
		}
		else {
			mode = VIEWMODE_ONLINE;
		}
	}
	else {
		mode = VIEWMODE_OFFLINE;
	}
	if (mode == this->getViewMode()) {
		return FALSE;
	}
	this->setViewMode(mode);
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

void CDlgChildDaliGroupsOverviewBase::setPropertyChanged(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_PROPERTY_CHANGED, flagStateOnOff);
}

bool CDlgChildDaliGroupsOverviewBase::getPropertyChanged( ) {
	return (this->m_flags & FLAG_PROPERTY_CHANGED); 
}

void CDlgChildDaliGroupsOverviewBase::setUploadDone(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_UPLOAD_DONE, flagStateOnOff);
}
bool CDlgChildDaliGroupsOverviewBase::getUploadDone( ) {
	return (this->m_flags & FLAG_UPLOAD_DONE); 
}

void CDlgChildDaliGroupsOverviewBase::setUploadBusy(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_UPLOAD_BUSY, flagStateOnOff);
}
bool CDlgChildDaliGroupsOverviewBase::getUploadBusy( ) {
	return (this->m_flags & FLAG_UPLOAD_BUSY); 
}

void CDlgChildDaliGroupsOverviewBase::setInitDone(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_INIT_DONE, flagStateOnOff);
}

bool CDlgChildDaliGroupsOverviewBase::getInitDone() {
	return (this->m_flags & FLAG_INIT_DONE); 
}

void CDlgChildDaliGroupsOverviewBase::setPageCurrActive(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_PAGE_ACTIVE, flagStateOnOff);
}
bool CDlgChildDaliGroupsOverviewBase::getPageCurrActive( ) {
	return (this->m_flags & FLAG_PAGE_ACTIVE);
}

void CDlgChildDaliGroupsOverviewBase::setBusyGuiControlsUpdate(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_BUSY_CONTROLS_UPDATE, flagStateOnOff);
}
bool CDlgChildDaliGroupsOverviewBase::getBusyGuiControlsUpdate( ) {
	return (this->m_flags & FLAG_BUSY_CONTROLS_UPDATE);
}

void CDlgChildDaliGroupsOverviewBase::setRequestParentUpdateGUI(bool flagStateOnOff) {
	helper_setFlag(this->m_flags, FLAG_REQUEST_PARENT_UPDATE_GUI, flagStateOnOff);
}
bool CDlgChildDaliGroupsOverviewBase::getRequestParentUpdateGUI( ) {
	return (this->m_flags & FLAG_REQUEST_PARENT_UPDATE_GUI);
}

#pragma warning(pop)          // Herstel de vorige waarschuwingstoestand


void CDlgChildDaliGroupsOverviewBase::setSelectedGroup(int groupNr) {
	this->m_selectedGroup = groupNr;
}

int CDlgChildDaliGroupsOverviewBase::getSelectedGroup( ) const {
	return this->m_selectedGroup;
}