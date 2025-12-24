// DlgChildDaliGroupsOverviewGroups.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliGroupsOverviewGroups.h"
#include "appdlgdaliworker.h"	
#include "utils/mfc/mfcCollections.h"

using namespace DUOTECNO::DALI;		// Nodig voor de enums.

// CDlgChildDaliGroupsOverviewGroups dialog

IMPLEMENT_DYNAMIC(CDlgChildDaliGroupsOverviewGroups, CDialog)

CDlgChildDaliGroupsOverviewGroups::CDlgChildDaliGroupsOverviewGroups(CAppDlgDaliWorker& refWorker, CWnd* pParent) : 
	CDlgChildDaliGroupsOverviewBase(refWorker, CDlgChildDaliGroupsOverviewGroups::IDD, pParent), m_modelNamesChanged(FALSE)
{
	for (int i=0;i<MAX_DALI_ADDRESSES;++i) {
		this->m_stateEnabled[i] = FALSE;	
		this->m_stateChecked[i] = FALSE;
	}
}

CDlgChildDaliGroupsOverviewGroups::~CDlgChildDaliGroupsOverviewGroups()
{
}

void CDlgChildDaliGroupsOverviewGroups::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgChildDaliGroupsOverviewGroups, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_MESSAGE(WM_UPDATE_MODEL, OnUpdateModel)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_A0, IDC_CHECK_A63, OnButtonClickedAddress)	
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CDlgChildDaliGroupsOverviewGroups::OnToolTipNeedText)
END_MESSAGE_MAP()


// CDlgChildDaliGroupsOverviewGroups message handlers
void CDlgChildDaliGroupsOverviewGroups::init(int groupNumber) {	
	this->m_modelNamesChanged = FALSE;
	this->setSelectedGroup(groupNumber);

	// WM_UPDATE_MODEL + TRUE = Altijd de controls updaten.
	this->PostMessage(WM_UPDATE_MODEL, TRUE);			
}

// Wanneer de parent de active dialoog wordt en deze open staat dan wordt het overzicht van de groepen 
// opnieuw upgedate.
//
// zie BUG-0184: Moeten het model/controls assynchroon updaten.
// Reden: Wanneer we op de pagina van de devices de naam van een control gear device wijzigen en gaan 
// naar deze pagina dan is de naam mogelijks nog niet upgedate. (--> WM_KILLFOCUS wordt assynchroon afgehandeld.)
void CDlgChildDaliGroupsOverviewGroups::OnParentPropertyPageActive(int groupNumber,bool activeDialog) {
	if (activeDialog) {		
		this->setSelectedGroup(groupNumber);
		// WM_UPDATE_MODEL + FALSE = Enkel bij verandering aan het model de GUI updaten.
		this->PostMessage(WM_UPDATE_MODEL, FALSE);
	}	
}

BOOL CDlgChildDaliGroupsOverviewGroups::OnParentPropertyPageLeave() {
	return FALSE;
}

void CDlgChildDaliGroupsOverviewGroups::onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) {
	this->CDlgChildDaliGroupsOverviewBase::onNotifyGuiStatusConnected(bNodeAddress,statusConnected);
	this->updateModel( );	
}


// zie BUG-0184
// wParam = TRUE, Altijd de GUI updaten.
LONG CDlgChildDaliGroupsOverviewGroups::OnUpdateModel(WPARAM wParam, LPARAM lParam) {
	if ((this->updateModel()) || (TRUE==wParam)) {
		this->PostMessage(WM_UPDATE_CONTROLS);
	}
	return TRUE;
}

BOOL CDlgChildDaliGroupsOverviewGroups::updateModel() {
	BOOL result = FALSE;
	if (this->updateViewMode()) 
		result = TRUE;
	if (this->updateModelDeviceFound()) 
		result= TRUE;	
	if (this->updateModelDeviceInGroup()) 
		result = TRUE;	
	if (this->updateModelDeviceNames())
		result = TRUE;
	return result;		// TRUE=model changes, FALSE=model niet veranderd.
}

BOOL CDlgChildDaliGroupsOverviewGroups::updateModelDeviceFound() {
	BOOL result = FALSE;
	if (VIEWMODE_OFFLINE == this->getViewMode() || VIEWMODE_ONLINE_BUSY == this->getViewMode()) {		
		for (int i=0;i<MAX_DALI_ADDRESSES;i++) {
			if (this->m_stateEnabled[i] != FALSE) {
				this->m_stateEnabled[i] = FALSE;
				result = TRUE;
			}			
		}	
	}
	else {
		CList<int> list;
		this->getWorker().getListControlGearAddress(&list);
		for (int i=0;i<MAX_DALI_ADDRESSES;i++) {
			if (list.Find(i)) {
				if (this->m_stateEnabled[i] == FALSE) {
					this->m_stateEnabled[i] = TRUE;
					result = TRUE;
				}			
			}
			else {
				if (this->m_stateEnabled[i] == TRUE) {
					this->m_stateEnabled[i] = FALSE;
					result = TRUE;
				}
			}
		}
	}
	return result;
}

BOOL CDlgChildDaliGroupsOverviewGroups::updateModelDeviceInGroup() {
	if (this->getSelectedGroup() < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->getSelectedGroup() > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return FALSE;
	}

	BOOL result = FALSE;
	CList<int> list;
	this->getWorker().getListControlGearGroupMembers(this->getSelectedGroup(),&list);
	for (int i=0;i<MAX_DALI_ADDRESSES;i++) {
		if (list.Find(i)) {
			if (this->m_stateChecked[i] == FALSE) {
				this->m_stateChecked[i] = TRUE;
				result = TRUE;
			}
		}
		else {
			if (this->m_stateChecked[i] == TRUE) {
				this->m_stateChecked[i] = FALSE;
				result = TRUE;
			}
		}		
	}
	return result;
}

// V16.46: Code optimaliseren.
// Enkel wanneer er een naam anders is de vlag m_modelNamesChanged op TRUE zetten en 
// return waarde TRUE.
BOOL CDlgChildDaliGroupsOverviewGroups::updateModelDeviceNames() {

	using namespace DUOTECNO::MFC_HELPER;
	CMap<int, int, CString, CString> mapPreviousNames;
	BOOL result = FALSE;
	
	// Copieren van de map.
	CopyCMap(this->m_mapNames, mapPreviousNames);	
	
	// Wissen van de map.
	this->m_mapNames.RemoveAll();

	CList<int> list;
	this->getWorker().getListControlGearAddress(&list);
	for (int i = 0; i < MAX_DALI_ADDRESSES; i++) {
		if (list.Find(i)) {
			CString strName;
			if (!this->getWorker().getDeviceName(DALI_DEVICE_TYPE_CONTROLGEAR, i, &strName)) {
				strName.Empty();			
			}

			this->m_mapNames.SetAt(i,strName);

			// Optimalisatie.
			// Controle doen of de naam aanwezig was of veranderd werd tov de previousNames map
			BOOL difference = TRUE;	
			CString strPreviousName;
			if (mapPreviousNames.Lookup(i,strPreviousName)) {
				if (0 == strPreviousName.Compare(strName)) {
					difference = FALSE;		// Zelfde naam.
				}
			}				
			else {
				if (strName.IsEmpty()) 
					difference = FALSE;
			}
			if (difference) {
				this->m_modelNamesChanged = TRUE;
				result = TRUE;
			}
		}
	}
	return result;
}

LONG CDlgChildDaliGroupsOverviewGroups::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	CWaitCursor wait;	// Wait cursor tijdens het updaten van de GUI.

	if (!this->getInitDone()) {			
		this->EnableToolTips(TRUE);
		this->setInitDone(true);
	}
			
	for (int i = 0;i<MAX_DALI_ADDRESSES;++i) {				
		CWnd* pWnd = this->GetDlgItem(IDC_CHECK_A0 + i);
		if (NULL != pWnd) {
			CButton* const pBtn = (CButton*)(pWnd);
			if (NULL != pBtn) {
				pBtn->EnableWindow(this->m_stateEnabled[i]);
				pBtn->SetCheck(this->m_stateChecked[i]);
				this->UpdateBtnName(pBtn, i);				
			}
		}	
	}	

	this->m_modelNamesChanged = FALSE;
	return TRUE;
}

void CDlgChildDaliGroupsOverviewGroups::UpdateBtnName(CButton* pButton, int shortAddress) {
	if (!this->m_modelNamesChanged) {
		return;
	}
	CString strName;	
	if (!this->m_mapNames.Lookup(shortAddress, strName)) {
		strName.Empty();
	}		

	CString strAddress;
	this->getWorker().formatStringAddress(DALI_DEVICE_TYPE_CONTROLGEAR, shortAddress, &strAddress);	

	CString strLabel;
	if (strName.IsEmpty()) {
		strLabel = strAddress;
	}
	else {
		strLabel = strAddress + " - " + strName;
	}
	pButton->SetWindowText(strLabel);	
}

void CDlgChildDaliGroupsOverviewGroups::OnButtonClickedAddress(UINT nID) {	
	if (false == this->getWorker().isConnected()) {
		ASSERT(this->getWorker().isConnected());
		return;
	}
	if (this->getSelectedGroup() < DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST || this->getSelectedGroup() > DALI_SYSTEM_CONTROLGEAR_GROUP_LAST) {
		ASSERT(0);
		return;
	}
	const int shortAddress = nID - IDC_CHECK_A0;
	if (shortAddress < DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST || shortAddress > DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST) {
		ASSERT(0);
		return;
	}
	CWnd* pWnd = this->GetDlgItem(nID);
	if (NULL == pWnd) {
		return;
	}	
	CButton* const pBtn = (CButton*)(pWnd);
	if (NULL == pBtn) {
		return;
	}	

	if (pBtn->GetCheck()) {
		if (this->getWorker().checkDeviceAndPromptAddToGroup(this,this->getSelectedGroup(), shortAddress)) {
			this->getWorker().controlGearAddToGroup(shortAddress, this->getSelectedGroup());
		}
	}
	else {
		this->getWorker().controlGearRemoveFromGroup(shortAddress,this->getSelectedGroup());
	}

	// WM_UPDATE_MODEL + FALSE = Enkel bij verandering aan het model de GUI updaten.
	this->PostMessage(WM_UPDATE_MODEL, FALSE);		
}

// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CDlgChildDaliGroupsOverviewGroups::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	if (identifier>=IDC_CHECK_A0 && identifier<=IDC_CHECK_A63) {
		const int address = identifier - IDC_CHECK_A0;
		bool member = false;
		CButton* const pBtn = (CButton*)(this->GetDlgItem(identifier));
		if (NULL != pBtn) {
			if (BST_CHECKED == pBtn->GetCheck()) {
				member = true;
			}
		}

		CString strDeviceName;
		m_mapNames.Lookup(address,strDeviceName);		
		if (strDeviceName.IsEmpty()) {
			strDeviceName.Format("A%d",address);
		}
		if (member) {
			this->m_strToolTipText.Format("Remove '%s' from the group (changes are applied immediately)", strDeviceName);			
		}
		else {
			this->m_strToolTipText.Format("Add '%s' to the group (changes are applied immediately)", strDeviceName);
		}
		handled = TRUE;
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}


