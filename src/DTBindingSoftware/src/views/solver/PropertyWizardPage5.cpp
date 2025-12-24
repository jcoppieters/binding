// PropertyWizardPage5.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage5.h"
#include "PropertyWizard.h"
#include "TXNodeMessage.h"
#include "MainFrm.h"
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
// CPropertyWizardPage5 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage5, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyWizardPage5::CPropertyWizardPage5() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage5::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE5),
	m_state(STATE_IDLE), m_nodeAddress(-1), m_ulNewPhysicalAddress(0), m_ulOldPhysicalAddress(0), m_styleWizardButtons(0)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage5)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyWizardPage5::~CPropertyWizardPage5()
{
}

void CPropertyWizardPage5::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage5)
	DDX_Control(pDX, IDC_COMBO_NEW, m_ComboNew);
	DDX_Control(pDX, IDC_COMBO_OLD, m_ComboOld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage5, CPropertyWizardPageBase)	
	//{{AFX_MSG_MAP(CPropertyWizardPage5)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE, OnButtonReplace)
	ON_CBN_CLOSEUP(IDC_COMBO_NEW, OnCloseupComboNew)
	ON_CBN_CLOSEUP(IDC_COMBO_OLD, OnCloseupComboOld)
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATE_WIZARD_CONTROLS, OnUpdateWizardControls)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage5 message handlers
BOOL CPropertyWizardPage5::OnSetActive() 
{		
	CString szText;
	szText.Empty();

	CPropertyWizard *pSheet = NULL;
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		DWORD dwStyle;
		if (pSheet->IsLastPage(WIZARDPAGE_CHOICE_REPLACE)) {
			dwStyle = PSWIZB_FINISH | PSWIZB_BACK;
			szText.Format("When done press Finished. The browse algoritme will start again...");
		}
		else {
			dwStyle = PSWIZB_NEXT | PSWIZB_BACK;
			szText.Format("When finished press continue for the next step.");
			//szText.Format("To proceed press next");
		}	
		this->m_styleWizardButtons = dwStyle;		// V17.01: De toestand van de wizard buttons.
		pSheet->SetWizardButtons(dwStyle);
	}

	CWnd *pWnd = NULL;
	if ((pWnd = GetDlgItem(IDC_STATIC_TEXT2)) != NULL) 
	{
		pWnd->SetWindowText(szText);
	}
	
	if ((pWnd = GetDlgItem(IDC_STATIC_TEXT)) != NULL)
	{ 
		szText.Empty();
		szText.Format("Replace devices by assigning the address from an old device to a new device.\n"			        
					  "The devices must be the same product type.\n"					 
					  "Make sure the old device is removed and the new device is powered on and connected to the CAN-bus.\n");

		pWnd->SetWindowText(szText);
	}

	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage5::OnWizardBack() 
{
	CPropertyWizard *pSheet = NULL;
	
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_REPLACE));
	}
	return FALSE;
}

LRESULT CPropertyWizardPage5::OnWizardNext() 
{
	CPropertyWizard *pSheet = NULL;

	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		return(pSheet->NextPage(WIZARDPAGE_CHOICE_REPLACE));
	}
	return CPropertyWizardPageBase::OnWizardNext();
}

//////////////////////////////////////////////////////////////////////////
/**
 * Here we must search for the Logical node address for
 * the node which physical address
 */
void CPropertyWizardPage5::OnButtonReplace() 
{	
	int j;
	CCanNode *pDocNode;
		
	const unsigned long ulPhysicalOld = GetOldPhysicalAddress();

	if (TMGetDocumentNodes()->FindPhysicalNode(ulPhysicalOld, &j, &pDocNode)) 
	{	
		CCanNode *pRTNode;
		const BYTE bNodeAddress = pDocNode->bLogicalAddress;

		if (TMGetRuntimeNodes()->FindNode(bNodeAddress, &j, &pRTNode) == FALSE)
		{				
			const unsigned long ulPhysicalNew = GetNewPhysicalAddress();
			if (ulPhysicalNew != 0) 
			{
				// Here we can Assign the physical address...
				// The node pDocNode->bLogicalAddress could not be found in the Runtime database ...	
				if (CTXBrowsePnpMessages::getInstance()->AssignPhysicalAddress(ulPhysicalNew, bNodeAddress)) {
					this->m_state = STATE_REPLACE_BUSY;
					this->m_nodeAddress = bNodeAddress;
					this->m_ulNewPhysicalAddress = ulPhysicalNew;
					this->m_ulOldPhysicalAddress = ulPhysicalOld;		

					// Sedert V17.01:
					// Zolang de taak bezig is blijft de cursor in wacht toestand + blijven de navigatie controls gedisabled.
					this->SetTimer(TIMER_ID_ASSIGN_BUSY, (10*1000), (TIMERPROC)NULL);
					this->PostMessage(WM_UPDATE_WIZARD_CONTROLS);
				}
			}
			else 
			{
				CString szError;
				szError.Format(_T("Physical device address of the new device is 0.\nCannot replace the nodes"));					
				AfxMessageBox(szError);
			}
		} // if (>FindNode
		else 
		{
			// error the logical address is already in use ...
			CString szError;
			szError.Format(_T("Cannot replace the device because the logical address 0x%02"
							  "is already used by the CAN-bus device 0x%06x"),
							pRTNode->bLogicalAddress,
							pRTNode->ulPhysicalAddress);
			AfxMessageBox(szError);				

		} // else if (->FindNode		

	} // if (->FindPhysicalNode
}

/**
 * here we can search for all nodes that have physical addresses in Document database
 * but not in the Runtime database ...
 */

BOOL CPropertyWizardPage5::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();

	m_ComboNew.Clear();
	m_ComboOld.Clear();

	// De toestand...
	this->m_state = STATE_IDLE;
	this->m_nodeAddress = -1;
	this->m_ulNewPhysicalAddress = 0;
	this->m_ulOldPhysicalAddress = 0;
	this->m_styleWizardButtons = 0;

				
	//Here we search for physical addresses that are in found in RT database,but not in document database ... 
	//--> find in Array ulNewPhysicalAddress[]
	int nRtNodes;
	if (TMGetRuntimeNodes()->NROfNodes(&nRtNodes)) {
		if (nRtNodes != 0)  {		
			unsigned long *ulNewPhysicalAddress = NULL;
			int iNewCount = 0;
			int j;

			if ((ulNewPhysicalAddress = new unsigned long[nRtNodes]) != NULL) {						
				CCanNode *pDocNode,*pRTNode;		
				for (int i=0;i<nRtNodes;i++) {
					if (TMGetRuntimeNodes()->GetNode(i, &pRTNode)) {							
						if (TMGetDocumentNodes()->FindPhysicalNode(pRTNode->ulPhysicalAddress,&j,&pDocNode)==FALSE)  {
							// Fysical node address not found in Doc Database but found in RT database ...													
							ulNewPhysicalAddress[iNewCount++] = pRTNode->ulPhysicalAddress;						
						}		
					} // if (GetNode(i,

					ASSERT(pRTNode);
				} // for (int i=0;i<nRtNodes;i++) 
								
				CString szPhAddress;
				for (j=0;j<iNewCount;j++) {
					szPhAddress.Format(_T("0x%06x"),ulNewPhysicalAddress[j]);
					m_ComboNew.InsertString(j,szPhAddress);
					m_ComboNew.SetItemData(j,ulNewPhysicalAddress[j]);				
				}			
				m_ComboNew.SetCurSel(0);

				// Clear memory ...
				delete [] ulNewPhysicalAddress;
				ulNewPhysicalAddress = NULL;										
			} // if ((ulNewPhysicalAddress = new unsigned long [nRtNodes]) != NULL)			
		} // if (nRtNodes != 0) 		
	} //if (NROfNodes(&nRtNodes))


	int nDocNodes;
	if (TMGetDocumentNodes()->NROfNodes(&nDocNodes)) {		
		if (nDocNodes != 0)  {
			unsigned long *ulOldPhysicalAddress = NULL;
			int iOldCount = 0;
			int j;

			// BUG-0141
			// Allocatie van array was ifv het aantal runtime nodes hierdoor crashte de 
			// applicatie.
			if ((ulOldPhysicalAddress = new unsigned long[nDocNodes]) != NULL) {			
				CCanNode *pDocNode,*pRTNode;		
				for (int i=0;i<nDocNodes;i++) {							
					if (TMGetDocumentNodes()->GetNode(i, &pDocNode)) {								
						if (TMGetRuntimeNodes()->FindPhysicalNode(pDocNode->ulPhysicalAddress,&j,&pRTNode)==FALSE) {
							//Fysical node address not found in RT database but found in Doc Database ...					
							ulOldPhysicalAddress[iOldCount++] = pDocNode->ulPhysicalAddress;
						}						
					} // if (->GetNode
				} // end for (int i=0;i<nDocNodes;i++)
					
				CString szPhAddress;

				for (j=0;j<iOldCount;j++) 
				{
					szPhAddress.Format(_T("0x%06x"),ulOldPhysicalAddress[j]);
					m_ComboOld.InsertString(j,szPhAddress);
					m_ComboOld.SetItemData(j,ulOldPhysicalAddress[j]);			
				}
		
				m_ComboOld.SetCurSel(0);

				// Clear memory ...
				delete [] ulOldPhysicalAddress;
				ulOldPhysicalAddress = NULL;													
			} // if ((ulOldPhysicalAddress = new unsigned long [nRtNodes]) != NULL)		

		} // if (nDocNodes != 0) 
	}// if (NROfNodes(&nDocNodes))
	
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyWizardPage5::UpdateControls()
{
	unsigned long ulPhysicalNew = 0;
	unsigned long ulPhysicalOld = 0;
	CString szInfo,szTemp;	


	WizardTasks_t Task;
	CPropertyWizard *pSheet = NULL;		

	szInfo.Empty();

	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL)
	{
		Task = pSheet->GetTask();
	}

	if (m_ComboOld.GetCount() == 0) 
	{
		m_ComboOld.EnableWindow(FALSE);
	}
	else 
	{
		int j;
		CCanNode *pDocNode;
		m_ComboOld.EnableWindow(TRUE);
		ulPhysicalOld = GetOldPhysicalAddress();

		// We should display node name + nr of units ...
		if (TMGetDocumentNodes()->FindPhysicalNode(ulPhysicalOld, &j, &pDocNode)) {					
			szInfo.Format(_T("Database pc Device(old):'%s',address:0x%02x, nr of units: %d\n"),
						  pDocNode->szNodeName,
						  pDocNode->bLogicalAddress,
						  pDocNode->bNROfUnits);
			
		} // if (->FindPhysicalNode
	} // else if (m_ComboOld.GetCount() == 0)

	
	szTemp.Empty();

	if (m_ComboNew.GetCount() == 0) 
	{
		m_ComboNew.EnableWindow(FALSE);
	}
	else 
	{
		CCanNode *pRTNode;
		int j;

		m_ComboNew.EnableWindow(TRUE);

		ulPhysicalNew = GetNewPhysicalAddress();
	
		if (TMGetRuntimeNodes()->FindPhysicalNode(ulPhysicalNew,&j,&pRTNode)) 
		{			
			if (pRTNode->bLogicalAddress == 0xFE) 
			{
				// Factory init addresses found
				szTemp.Format("CAN-bus device(new):Address:0xFE");
			}
			else 
			{
				if ((Task == WIZARD_NEW_DUPLICATESFOUND) || (Task == WIZARD_CONTINUE_DUPLICATESFOUND)) {
					szTemp.Format("CAN-bus device(new):Address:0x%02x", pRTNode->bLogicalAddress);
				}
				else {
					szTemp.Format("CAN-bus device(new):'%s',Address:0x%02x,nr of units:%d",
								  pRTNode->szNodeName, pRTNode->bLogicalAddress, pRTNode->bNROfUnits);
				}						
			} // else if (pRTNode->bLogicalAddress == 0xFE) 
		}
	}	

	CWnd *pWnd = NULL;

	if ((pWnd = GetDlgItem(IDC_BUTTON_REPLACE)) != NULL)
	{
		if (ulPhysicalNew ==0 || ulPhysicalOld==0) {	
			pWnd->EnableWindow(FALSE);
		}
		else {
			pWnd->EnableWindow(TRUE);
		}
	}


	if ((pWnd = GetDlgItem(IDC_STATIC_INFO)) != NULL)
	{	
		if (szInfo.IsEmpty() == TRUE)  {
			pWnd->EnableWindow(FALSE);
			pWnd->SetWindowText("");
		}
		else  {
			pWnd->EnableWindow(TRUE);		
			szInfo += szTemp;
			pWnd->SetWindowText(szInfo);
		}
	}
}

// --------------------------------------------------------------
unsigned long CPropertyWizardPage5::GetNewPhysicalAddress()
{
	const int iSel = m_ComboNew.GetCurSel();
	unsigned long ulPhysicalAddress = m_ComboNew.GetItemData(iSel);
	
	if (ulPhysicalAddress == CB_ERR) {
		ulPhysicalAddress =0;
	}

	return ulPhysicalAddress;

}

unsigned long CPropertyWizardPage5::GetOldPhysicalAddress()
{
	const int iSel = m_ComboOld.GetCurSel();
	unsigned long ulPhysicalAddress = m_ComboOld.GetItemData(iSel);

	if (ulPhysicalAddress == CB_ERR) {
		ulPhysicalAddress =0;
	}
	return ulPhysicalAddress;
}
//----------------------------------------------------------------

void CPropertyWizardPage5::OnCloseupComboNew() 
{
	UpdateControls();
}

void CPropertyWizardPage5::OnCloseupComboOld() 
{
	UpdateControls();	
}

void CPropertyWizardPage5::OnNotifyAssignedLogicalAddress(int nodeAddress) {

	if ((STATE_REPLACE_BUSY == this->m_state) && (this->m_nodeAddress == nodeAddress)) {

		// sedert V16.47: CHANGES_UPLOAD_UNITS
		// De applicatie op de hoogte brengen dat het node adres hersteld werd.
		// Indien er nog andere taken moeten gebeuren (uploaden van de units) wordt dit bepaald door de hoofdapplicatie.
		// Deze taken worden dan uitgevoerd bij het afsluiten van de problem solve wizard.
		CWnd* const pMainWnd = AfxGetMainWnd();
		if (NULL != pMainWnd) {
			pMainWnd->PostMessage(WM_NOTIFY_PROBLEMSOLVED_RESTORED_NODEADDRESS, nodeAddress, this->m_ulNewPhysicalAddress);
		}

		this->KillTimer(TIMER_ID_ASSIGN_BUSY);
		
		// Melding naar de gebruiker.
		CString strHeader("Replace node done");
		CString strMessage;		
		strMessage.Format("Node 0x%06x has been replaced by the new node 0x%06x.",
						   this->m_ulOldPhysicalAddress,
						   this->m_ulNewPhysicalAddress);
		this->MessageBox(strMessage,strHeader,MB_OK | MB_ICONINFORMATION);

		// Wissen van de toestand.
		this->m_ulOldPhysicalAddress = 0;
		this->m_ulNewPhysicalAddress = 0;
		this->m_nodeAddress = -1;
		this->m_state = STATE_IDLE;

		// Updaten van de wizard buttons (na het aanpassen van de toestand.)
		this->PostMessage(WM_UPDATE_WIZARD_CONTROLS);
	}
}

// V17.01: zie BUG-0182: Zolang de FSM bezig is blijft de cursor in wait toestand...
void CPropertyWizardPage5::OnTimer(UINT_PTR nIDEvent) {
	this->KillTimer(nIDEvent);
	if (TIMER_ID_ASSIGN_BUSY == nIDEvent) {
		if (this->m_state == STATE_REPLACE_BUSY) {
			this->m_state = STATE_IDLE;
			this->PostMessage(WM_UPDATE_WIZARD_CONTROLS);	// Updaten van de wizard buttons.
		}
	}
}

// V17.01: zie BUG-0182: Zolang de FSM bezig is blijft de cursor in wait toestand...
BOOL CPropertyWizardPage5::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (STATE_REPLACE_BUSY == this->m_state) {
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
		return TRUE;
	}
	return CPropertyPage::OnSetCursor(pWnd, nHitTest, message);
}

// V17.01: zie BUG-0182: Zolang de FSM bezig is blijft de cursor in wait toestand...
// De navigatie buttons moeten disabled blijven.
// Wanneer de taak uitgevoerd is dan worden de navigatie buttons hersteld naar hun originele toestand.
LONG CPropertyWizardPage5::OnUpdateWizardControls(WPARAM wParam, LPARAM lParam) {
	CPropertyWizard *pSheet = NULL;
	if ((pSheet = (CPropertyWizard*)GetOwner()) != NULL) {
		DWORD dwStyle = 0;
		if (STATE_REPLACE_BUSY == this->m_state) {
			dwStyle = 0;
		}
		else {
			ASSERT(0 != this->m_styleWizardButtons);
			dwStyle = this->m_styleWizardButtons;
		}
		pSheet->SetWizardButtons(dwStyle);
	}
	return TRUE;
}

