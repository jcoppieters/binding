// PropertyWizardPage2.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage2.h"
#include "TMGlobals.h"
#include "PropertyWizard.h"
#include "MainFrm.h"
#include "TXNodeMessage.h"
// -----------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage2 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage2, CPropertyPage)		

CPropertyWizardPage2::CPropertyWizardPage2() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage2::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE2), 
	m_state(STATE_INIT), m_countNrOfAddresses(0), m_addresses()
{
	//{{AFX_DATA_INIT(CPropertyWizardPage2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyWizardPage2::~CPropertyWizardPage2()
{
}

void CPropertyWizardPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage2, CPropertyWizardPageBase)		
	//{{AFX_MSG_MAP(CPropertyWizardPage2)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, OnButtonRestore)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage2 message handlers
BOOL CPropertyWizardPage2::OnSetActive() 
{
	if (STATE_INIT == m_state) { 
		CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();	
		if (pSheet != NULL) {
			if (pSheet->IsLastPage(WIZARDPAGE_CHOICE_PROGRAMFILE)) {
				m_state = STATE_FINISH_IDLE;
			}
			else {
				m_state = STATE_NEXT;
			}
		}				
	}
	PostMessage(WM_UPDATE_CONTROLS);
	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage2::OnWizardBack() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet != NULL) {
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_PROGRAMFILE));
	}
	return FALSE;
}

LRESULT CPropertyWizardPage2::OnWizardNext() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();	
	if (pSheet != NULL) {
		return(pSheet->NextPage(WIZARDPAGE_CHOICE_PROGRAMFILE));
	}
	return CPropertyWizardPageBase::OnWizardNext();
}

BOOL CPropertyWizardPage2::OnInitDialog() {
	CPropertyWizardPageBase::OnInitDialog();
	PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;
}

// Alle nodes in de doc database opnieuw hetzelfde node adres geven.
void CPropertyWizardPage2::OnButtonRestore() {		

	int nNodes = 0;
	
	this->m_countNrOfAddresses = 0;
	this->m_addresses.RemoveAll();

	if (TMGetDocumentNodes()->NROfNodes(&nNodes)) {			 		
		CCanNode *pDocNode;			
		for (int i=0;i<nNodes;i++) {				
			if (TMGetDocumentNodes()->GetNode(i, &pDocNode)) {	
				BOOL holdsFactoryAddress = FALSE;		// vlag: Heeft de node in de runtime database een 0xFE adres.
				CCanNode* pRunTimeNode = NULL;
				int dummy = 0;
				if (TMGetRuntimeNodes()->FindPhysicalNode(pDocNode->ulPhysicalAddress,&dummy,&pRunTimeNode)) {
					if (0xFE == pRunTimeNode->getNodeAddress()) {
						holdsFactoryAddress = TRUE;
					}
				}
				if (CTXBrowsePnpMessages::getInstance()->AssignPhysicalAddress(pDocNode->ulPhysicalAddress,pDocNode->bLogicalAddress)) {	
					if (holdsFactoryAddress) {
						// Nodes die momenteel een factory adres hebben in de network database toevoegen aan de lijst.						
						this->m_addresses.AddTail(pDocNode->bLogicalAddress);
					}					
					this->m_countNrOfAddresses++;
				}
			} // if (->GetNode		
		} // for (int i=0;i<nNodes;i++) 

		// if (STATE_FINISH_IDLE == m_state) {
		// this->m_state = STATE_FINISH_BUSY;
		// }


		if (this->m_countNrOfAddresses > 0) {
			// 1 seconde + extra 200msec per node.
			const int timerInterval_msec = 1000 + (this->m_countNrOfAddresses * 200);
			this->SetTimer(TIMER_ID_ASSIGN_BUSY, timerInterval_msec, (TIMERPROC) NULL);
			this->m_state = STATE_FINISH_BUSY;
		}
		PostMessage(WM_UPDATE_CONTROLS);

	} // if (->NROfNodes(&nNodes))	
}


LONG CPropertyWizardPage2::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();	
	if (pSheet == NULL) {
		return TRUE;
	}
	const CString szTextTop("Restore device node addresses from the PC database to the CAN-bus.\n"
					        "Only the devices present in the PC database will be restored."); 
	CString szTextBottom;
	DWORD dwStyle = 0;

	switch(m_state) {
		case STATE_INIT: {
			dwStyle = PSWIZB_BACK;				// ??
		} break;
		case STATE_NEXT: {
			// Next button tonen.
			dwStyle = PSWIZB_NEXT | PSWIZB_BACK;
			szTextBottom.Format("When finished press continue for the next step.");
		} break;
		case STATE_FINISH_IDLE: {
			// Finish button disablen - Moeten nog de taak uitvoeren.
			 dwStyle = PSWIZB_DISABLEDFINISH | PSWIZB_BACK;
			 szTextBottom.Empty();
		} break;

		case STATE_FINISH_BUSY: {
			// Geen knoppen.
			dwStyle = 0;
			szTextBottom.Format("Assign busy, please wait.");
		} break;

		case STATE_FINISH_DONE: {
			// Finish button enablen - Moeten nog de taak uitvoeren.
			dwStyle = PSWIZB_FINISH | PSWIZB_BACK;
			szTextBottom.Format("When done press finish. The browse algoritme will start again...");
		} break;	
	}
	pSheet->SetWizardButtons(dwStyle);

	{
		CWnd* const pWnd = GetDlgItem(IDC_BUTTON_RESTORE);
		if (pWnd != NULL) {
			pWnd->EnableWindow(this->m_state == STATE_FINISH_BUSY ? FALSE : TRUE);
		}
	}
	{
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_TEXT2);
		if (pWnd != NULL) {
			pWnd->SetWindowText(szTextBottom);
		}
	}
	{
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_TEXT);
		if (pWnd != NULL) {
			pWnd->SetWindowText(szTextTop);
		}
	}
	return TRUE;
}


// Virtuele functie van de basis 
void CPropertyWizardPage2::OnNotifyAssignedLogicalAddress(int nodeAddress) {
	if (STATE_FINISH_BUSY == this->m_state) {											
		const POSITION  pos = this->m_addresses.Find(nodeAddress);
		if (NULL != pos) {		
			int dummy = 0;
			CCanNode* pNode = NULL;
			unsigned long physicalAddress = 0;
			if (TMGetDocumentNodes()->FindNode(nodeAddress,&dummy,&pNode)) {
				physicalAddress = pNode->ulPhysicalAddress;
			}

			// sedert V16.47: CHANGES_UPLOAD_UNITS
			// Deze node had in de runtime node database een 0xFE adres.
			// De applicatie op de hoogte brengen dat het node adres hersteld werd.
			CWnd* const pMainWnd = AfxGetMainWnd();
			if (NULL != pMainWnd) {
				pMainWnd->PostMessage(WM_NOTIFY_PROBLEMSOLVED_RESTORED_NODEADDRESS, nodeAddress, physicalAddress);
			}
		}	

		if (--this->m_countNrOfAddresses <= 0) {
			this->m_state = STATE_FINISH_DONE;
			this->KillTimer(TIMER_ID_ASSIGN_BUSY);
			this->PostMessage(WM_UPDATE_CONTROLS);
		}
	}
}

void CPropertyWizardPage2::OnTimer(UINT_PTR nIDEvent) {

	this->KillTimer(nIDEvent);

	if (TIMER_ID_ASSIGN_BUSY == nIDEvent) {
		if (this->m_state == STATE_FINISH_BUSY) {
			this->m_state = STATE_FINISH_DONE;
			this->PostMessage(WM_UPDATE_CONTROLS);
		}
	}
}

// V17.01: zie BUG-0182: Zolang de FSM bezig is blijft de cursor in wait toestand...
BOOL CPropertyWizardPage2::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (STATE_FINISH_BUSY == this->m_state) {
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
		return TRUE;
	}
	return CPropertyPage::OnSetCursor(pWnd, nHitTest, message);
}
