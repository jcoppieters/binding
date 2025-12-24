// DlgChildDaliAddressing.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliAddressing.h"
#include "appdlgdaliworker.h"
#include "utils/mfc/Combobox_util.h"
/****************************************************************************************************/
using namespace DUOTECNO::DALI;
using DUOTECNO::MFC_HELPER::CComboxbox_util;
/****************************************************************************************************/
// CDlgChildDaliAddressing dialog
/****************************************************************************************************/
static const CString STRING_FOOTER_OFFLINE("Connect online to change the device address.");
static const CString STRING_FOOTER_ONLINE("Change the device address. The changes will be applied immediately.");
/****************************************************************************************************/
IMPLEMENT_DYNAMIC(CDlgChildDaliAddressing, CDialog)

CDlgChildDaliAddressing::CDlgChildDaliAddressing(CAppDlgDaliWorker& refWorker,CWnd* pParent)
	: CDlgChildDali(refWorker,CDlgChildDaliAddressing::IDD, pParent), m_strErrorMessage()
{
	this->setInitDone(false);		// Nodig voor updaten van de GUI...
}

CDlgChildDaliAddressing::~CDlgChildDaliAddressing()
{
}

void CDlgChildDaliAddressing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TEXT_HEADER, m_staticTextHeader);
	DDX_Control(pDX, IDC_STATIC_GROUP_ASSIGN, m_staticGroupAssign);
	DDX_Control(pDX, IDC_STATIC_GROUP_DELETE, m_staticGroupDelete);
	DDX_Control(pDX, IDC_BUTTON_ASSIGN_ADDRESS, m_btnAssignAddress);
	DDX_Control(pDX, IDC_BUTTON_ERASE_ADDRESS, m_btnEraseAddress);
	DDX_Control(pDX, IDC_COMBO_ADDRESS, m_comboBoxAsignAddresses);
	DDX_Control(pDX, IDC_STATIC_PROGRESSINFO, m_staticFooterInfoText);
	DDX_Control(pDX, IDC_STATIC_GROUP_SWAP, m_staticGroupSwapAddresses);
	DDX_Control(pDX, IDC_BUTTON_SWAP_ADDRESS, m_btnSwapAddresses);
	DDX_Control(pDX, IDC_COMBO_ADDRESS_SWAP, m_comboBoxSwapAddresses);
}

BEGIN_MESSAGE_MAP(CDlgChildDaliAddressing, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_BN_CLICKED(IDC_BUTTON_ASSIGN_ADDRESS, &CDlgChildDaliAddressing::OnBnClickedButtonAssignAddress)
	ON_BN_CLICKED(IDC_BUTTON_ERASE_ADDRESS, &CDlgChildDaliAddressing::OnBnClickedButtonEraseAddress)
	ON_BN_CLICKED(IDC_BUTTON_SWAP_ADDRESS, &CDlgChildDaliAddressing::OnBnClickedButtonSwapAddresses)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliAddressing::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgChildDaliAddressing message handlers
void CDlgChildDaliAddressing::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {	
	this->setDevice(pDevice);
	this->updateViewMode();	
	this->setInitDone(false);					// Nodig voor het updaten van de combobox
	this->m_strErrorMessage.Empty();
	this->EnableToolTips(TRUE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliAddressing::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) {
	if (this->getDevice() != pDevice) {		
		this->setInitDone(false);				// Nodig voor het updaten van de combobox
	}
	this->setDevice(pDevice);
	this->updateViewMode();	
	this->PostMessage(WM_UPDATE_CONTROLS);
}

LONG CDlgChildDaliAddressing::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	bool enableControls = TRUE;
	CString strTextHeader;
	this->setBusyGuiControlsUpdate(true);
	switch(this->getViewMode()) {	
		case VIEWMODE_OFFLINE: {
			enableControls = FALSE;	
			strTextHeader = "Connect online to change the address of the device";
		} break;
		case VIEWMODE_ONLINE: {						  
			strTextHeader =  "Assign a new address to a DALI device or remove a DALI device from the DALI bus system.\r\n"						
							 "When removing the device the address and configuration will be lost.\r\n\r\n"
							 "WARNING: The Duotecno DALI bridge may need configuration updates with the new addresses.";
		} break;	

		case VIEWMODE_ONLINE_BUSY: {
			strTextHeader = "Busy configuration, please wait...";
			enableControls = FALSE;							
		} break;
	}

	this->m_staticFooterInfoText.SetWindowText((VIEWMODE_OFFLINE == this->getViewMode() ? STRING_FOOTER_OFFLINE : STRING_FOOTER_ONLINE));

	// Update van de combobox.
	if (!this->getInitDone()) {
		this->updateDataComboBoxAssignAddresses( );
		this->updateDataComboBoxSwapAddresses();
		this->setInitDone(true);
	}

	m_staticTextHeader.EnableWindow(TRUE);
	m_staticTextHeader.SetWindowText(strTextHeader);

	m_staticGroupAssign.EnableWindow(enableControls);	

	{
		m_comboBoxAsignAddresses.EnableWindow(enableControls);
		m_btnAssignAddress.EnableWindow(enableControls);
	}

	{
		m_staticGroupDelete.EnableWindow(enableControls);
		m_btnEraseAddress.EnableWindow(enableControls);
	}

	{
		// De controls enkel tonen wanneer er minimum 1 adres aanwezig in de combo box...
		const bool enableSwap = (this->m_comboBoxSwapAddresses.GetCount() >= 1 ? true : false);	
		m_staticGroupSwapAddresses.EnableWindow(enableControls && enableSwap);
		m_btnSwapAddresses.EnableWindow(enableControls && enableSwap);
		m_comboBoxSwapAddresses.EnableWindow(enableControls && enableSwap);
	}

	// Foutmelding weergeven in een messageBox.	// WORDT MOMENTEEL NIET GEBRUIKT.
	if (!this->m_strErrorMessage.IsEmpty()) {
		this->MessageBox(this->m_strErrorMessage,NULL,MB_ICONEXCLAMATION);		
		this->m_strErrorMessage.Empty();
	}
	this->setBusyGuiControlsUpdate(false);
	return TRUE;
}

static CString formatStrAddress(const DaliDeviceType_t deviceType,int address) {
	CString str;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == deviceType) {
		str.Format("A%d",address);
	}
	else {
		str.Format("A%d²",address);
	}
	return str;
}

void CDlgChildDaliAddressing::OnBnClickedButtonAssignAddress()
{
	const int addressToAssign = this->comboBoxAssignAddressesGetCurrSelAddress();
	if (addressToAssign < 0) {
		ASSERT(0);
		return;
	}
	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) {
		ASSERT(0);
		return;
	}
	const DaliDeviceType_t deviceType = pDevice->getType();
	const CString strNewAddress(formatStrAddress(deviceType,addressToAssign));
	const CString strOldAddress(formatStrAddress(deviceType,pDevice->getShortAddress()));

	CString strMessage;
	const CString strTitle("Assign a new address to a DALI device.");
	strMessage.Format("This option allows you to assign a new address to a DALI device.\r\n\r\n"
					  "A new address '%s' will be assigned to the device currently using address '%s'.\r\n\r\n"
					  "Warning: Make sure the new address is not already assigned to another device.\r\n\r\n"										 
					  "After these changes the Duotecno DALI bridge may need configuration updates with the new addresses.\r\n\r\n"
					  "Do you want to proceed with assigning the new address?",
					  strNewAddress, strOldAddress);

	if (IDNO == this->MessageBox(strMessage,strTitle,MB_YESNO | MB_ICONQUESTION)) {
		return;
	}

	if (false == this->getWorker().startAddressingSingleDevice(deviceType,pDevice->getShortAddress(),addressToAssign)) {
		// Todo: error
	}
}

void CDlgChildDaliAddressing::OnBnClickedButtonEraseAddress()
{
	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) {
		ASSERT(0);
		return;
	}

	const DaliDeviceType_t deviceType = pDevice->getType();	
	const CString strTitle("Remove a device from the DALI bus system.");
	const CString strMessage("This option allows you to clear the device's address and remove it from the database. "
						     "Removing the device will erase all configuration data and names.\r\n\r\n"
							 "After this action, the device can be re-added to the installation using the 'Extend Installation' option. "
							 "This option can be found on the 'network' tab\r\n\r\n"
							 "After these changes the Duotecno DALI bridge may need configuration updates with the new addresses.\r\n\r\n"
							 "Do you want to proceed with removing the device from the system?");

	if (IDNO == this->MessageBox(strMessage,strTitle,MB_YESNO | MB_ICONQUESTION)) {
		return;
	}

	if (false == this->getWorker().startEraseDeviceAddress(deviceType,pDevice->getShortAddress())) {
		// Todo: error
	}
}

void CDlgChildDaliAddressing::OnBnClickedButtonSwapAddresses() {

	const int addressToSwap = this->comboBoxSwapAddressesGetCurrSelAddress();
	if (addressToSwap < 0) {
		ASSERT(0);
		return;
	}
	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) {
		ASSERT(0);
		return;
	}
	const DaliDeviceType_t deviceType = pDevice->getType();	
	const CString strAddress1(formatStrAddress(deviceType,pDevice->getShortAddress()));
	const CString strAddress2(formatStrAddress(deviceType,addressToSwap));	

	CString strMessage;
	const CString strTitle("Swap DALI device addresses.");
	strMessage.Format("This option allows you to swap addresses between two DALI devices.\r\n\r\n"
				      "The addresses of the devices '%s' and '%s' will be swapped.\r\n\r\n"					 											 
					  "After these changes the Duotecno DALI bridge may need configuration updates with the new addresses.\r\n\r\n"
					  "Do you want to proceed with swapping the addresses?",
					  strAddress1, strAddress2);

	if (IDNO == this->MessageBox(strMessage,strTitle,MB_YESNO | MB_ICONQUESTION)) {
		return;
	}
	
	if (false == this->getWorker().startAddressingSwapDevices(deviceType,pDevice->getShortAddress(),addressToSwap)) {
		// Todo: error
	}	
}

int CDlgChildDaliAddressing::comboBoxAssignAddressesInsertItem(DUOTECNO::DALI::DaliDeviceType_t type,int address,bool addressUsed) {
	CString strItem = formatStrAddress(type,address);
	if (!addressUsed) {
		strItem += " (Free address)";
	}
	else {
		strItem += " (Used)";
	}
	return CComboxbox_util::setComboBoxItem(m_comboBoxAsignAddresses,strItem,address);
}

static int helper_getCurrSelAddress(CComboBox& refComboBox) {
	DWORD dwData = 0xffffff;
	if (!CComboxbox_util::getComboBoxCurrSelDataItem(refComboBox,&dwData)) {
		return -1;
	}
	ENSURE(dwData >= DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST && dwData <= DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST);
	return (int) dwData;
}

int CDlgChildDaliAddressing::comboBoxAssignAddressesGetCurrSelAddress( ) {
	return helper_getCurrSelAddress(m_comboBoxAsignAddresses);
}

int CDlgChildDaliAddressing::comboBoxSwapAddressesGetCurrSelAddress( ) {
	return helper_getCurrSelAddress(m_comboBoxSwapAddresses);
}

// Lijst bevat alle mogelijke adressen van de devices.
// Er is aanvullende info of het adres vrij / gebruikt is.
void CDlgChildDaliAddressing::updateDataComboBoxAssignAddresses( ) {

	m_comboBoxAsignAddresses.ResetContent();

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) {
		return;
	}
	
	int currentSelection = -1;
	const DUOTECNO::DALI::DaliDeviceType_t type = pDevice->getType();

	CList<int> listAddressesUsed;
	this->getWorker().getListDeviceAddresses(type,&listAddressesUsed);

	for (int address=0;address<64;++address) {
		const bool addressUsed = (NULL == listAddressesUsed.Find(address) ? false : true);
		const int retVal = this->comboBoxAssignAddressesInsertItem(type,address,addressUsed);
		if ((retVal >= 0) && (address == pDevice->getShortAddress())) {
			currentSelection = retVal;
		}
	}
	if (currentSelection >= 0) {
		m_comboBoxAsignAddresses.SetCurSel(currentSelection);
	}
}

// Lijst bevat gebruikte mogelijke adressen van de devices.
void CDlgChildDaliAddressing::updateDataComboBoxSwapAddresses() {

	this->m_comboBoxSwapAddresses.ResetContent( );

	CAppDaliDevice* const pDevice = this->getDevice();
	if (NULL == pDevice) {
		return;
	}

	int currentSelection = -1;
	const DUOTECNO::DALI::DaliDeviceType_t type = pDevice->getType();

	CList<int> listAddressesUsed;
	this->getWorker().getListDeviceAddresses(type,&listAddressesUsed);

	if (type == DALI_DEVICE_TYPE_CONTROLGEAR) {
		for (int address=DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;address<=DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST;++address) {
			if ((listAddressesUsed.Find(address)) && (address != pDevice->getShortAddress())) {
				const CString strItem = formatStrAddress(type,address);						
				CComboxbox_util::setComboBoxItem(this->m_comboBoxSwapAddresses,strItem,address);
			}
		}
	}
	else {
		for (int address=DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST;address>=DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;--address) {
			if ((listAddressesUsed.Find(address)) && (address != pDevice->getShortAddress())) {
				const CString strItem = formatStrAddress(type,address);						
				CComboxbox_util::setComboBoxItem(this->m_comboBoxSwapAddresses,strItem,address);
			}
		}
	}
	if (this->m_comboBoxSwapAddresses.GetCount()>0) {
		this->m_comboBoxSwapAddresses.SetCurSel(0);
	}
}

// IAppDlgDaliGui_Host	
void CDlgChildDaliAddressing::onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	this->setViewMode(VIEWMODE_ONLINE_BUSY);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliAddressing::onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());	
	this->setViewMode(VIEWMODE_ONLINE);
	this->PostMessage(WM_UPDATE_CONTROLS);
}
	
void CDlgChildDaliAddressing::onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	ASSERT(bNodeAddress == this->getWorker().getCurrentNodeAddress());
	this->setViewMode(VIEWMODE_ONLINE);
	if ((APPDALI_FSM_TASK_ADDRESSING_SINGLE_DEVICE == task) || 
		(APPDALI_FSM_TASK_ADDRESSING_ERASE_DEVICE == task) || 
		(APPDALI_FSM_TASK_ADDRESSING_SWAP_DEVICES == task)) {
		// this->m_strErrorMessage.Format("Error happened!\r\n%s");
	}
	this->PostMessage(WM_UPDATE_CONTROLS);
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgChildDaliAddressing::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_COMBO_ADDRESS:
		case IDC_COMBO_ADDRESS_SWAP: {
			this->m_strToolTipText = _T("Select a DALI address from the list");
			handled = TRUE;
		} break;
		case IDC_BUTTON_ASSIGN_ADDRESS: {
			this->m_strToolTipText = _T("Assign a DALI address to this device");
			handled = TRUE;
		} break;
		case IDC_BUTTON_ERASE_ADDRESS: {
			this->m_strToolTipText = _T("Erase the device address and remove it from the database");
			handled = TRUE;
		} break;
		case IDC_BUTTON_SWAP_ADDRESS: {
			this->m_strToolTipText = _T("Swap the DALI addresses of two devices");
			handled = TRUE;
		} break;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}