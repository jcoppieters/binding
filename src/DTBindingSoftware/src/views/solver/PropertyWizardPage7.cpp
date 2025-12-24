// PropertyWizardPage7.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage7.h"

#include "PropertyWizard.h"
#include "MainFrm.h"

#include "NodeDatabaseVerify.h"
#include "NodeDatabaseVerify_Handlers.h"

// -----------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
using namespace TMBindingDatabase;

#include "TMConvenience.h"
using namespace TMConvenience;
// ------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage7 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage7, CPropertyPage)		

CPropertyWizardPage7::CPropertyWizardPage7() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage7::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE7)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage7)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyWizardPage7::~CPropertyWizardPage7()
{
}

void CPropertyWizardPage7::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage7)
	DDX_Control(pDX, IDC_EDIT_BINDINGS, m_EditBindings);
	DDX_Control(pDX, IDC_BUTTON_REMOVEADD2, m_BtnRemoveAdd);
	DDX_Control(pDX, IDC_EDIT_UNITS, m_EditUnits);
	DDX_Control(pDX, IDC_EDIT_NAME, m_EditName);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_EditAddress);
	DDX_Control(pDX, IDC_COMBO_PHYSICAL, m_CmbPhysical);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage7, CPropertyWizardPageBase)		
	//{{AFX_MSG_MAP(CPropertyWizardPage7)
	ON_CBN_SELCHANGE(IDC_COMBO_PHYSICAL,OnCbnSelchangeComboPhysical)
	ON_CBN_CLOSEUP(IDC_COMBO_PHYSICAL, OnCloseupComboPhysical)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEADD2, OnButtonRemoveadd2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage7 message handlers

BOOL CPropertyWizardPage7::OnSetActive() 
{
	UpdateControls( );

	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage7::OnWizardBack() 
{
	CPropertyWizard *pSheet = NULL;
	
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_NODESADDED));
	}

	return FALSE;
}


// Sedert V16.16:
// Indien er nodes toegevoegd werden dan geen browse meer uitvoeren...
BOOL CPropertyWizardPage7::OnWizardFinish() {

	CPropertyWizard *pSheet = NULL;		
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) {		
		const WizardTasks_t Task = pSheet->GetTask();
		if (WIZARD_CONTINUE_NODESADDED == Task) {
			pSheet->SetBrowseAgain(FALSE);				
		}
		if (WIZARD_CONTINUE_NODESREMOVED == Task) {
			if(m_CmbPhysical.GetCount()==0) {
				pSheet->SetBrowseAgain(FALSE);
			}
		}
	}
	return CPropertyWizardPageBase::OnWizardFinish();
}

void CPropertyWizardPage7::SetComboBox(int index,const CCanNode& Node)
{
	CString szMessage;
	szMessage.Format(_T("%06x"),Node.ulPhysicalAddress);

	m_CmbPhysical.InsertString(index,szMessage);
	m_CmbPhysical.SetItemData(index,Node.bLogicalAddress);
}


BOOL CPropertyWizardPage7::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();
	
	CString szText;
		
	m_CmbPhysical.Clear();

	CPropertyWizard *pSheet = NULL;		
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{		
		const WizardTasks_t Task = pSheet->GetTask();

		switch (Task)
		{
			// TM 20070927: add chance to remove a node from the document database.
			case WIZARD_CONTINUE_NODESINCOMPATIBLE :
			{
				szText.Format("Incompatible devices have been found on the CAN-bus.\n"
							  "Removing devices from the PC database can solve this problem. "
							  "But all configuration for these devices will also be removed.\n"
							  "Press the 'Remove' button to remove a device from the PC database.");
			
				CNodeDatabaseVerify_Handler1 Handler(*this);
				CNodeDatabaseVerify Algoritme(&Handler);

				if (Algoritme.Check_Incompatible_Nodes() > 0)				
					m_CmbPhysical.SetCurSel(0);	
				
			} break;

			case WIZARD_CONTINUE_NODESREMOVED :
			{
				szText.Format("Not all devices have been found on the CAN-bus.\n"
					"When the devices were removed from the CAN-bus they also have to be removed out of the PC configuration file.\n"
					"Press the 'Remove' button to remove a device from the PC database.\n");

				CNodeDatabaseVerify_Handler1 Handler(*this);
				CNodeDatabaseVerify Algoritme(&Handler);

				if (Algoritme.Check_Removed_Nodes() > 0)
					m_CmbPhysical.SetCurSel(0);	

			} break;

			
			case WIZARD_CONTINUE_NODESADDED :
			{
				szText.Format(_T("New devices have been added to the CAN-bus."));
					
				m_BtnRemoveAdd.ShowWindow(FALSE);

				CNodeDatabaseVerify_Handler1 Handler(*this);
				CNodeDatabaseVerify Algoritme(&Handler);

				if (Algoritme.Check_Added_Nodes() > 0)
					m_CmbPhysical.SetCurSel(0);	

			}break;
			
			default:
			{
				ASSERT(0);

			}break;
		}
	}

	CWnd *pWnd = NULL;
	if ((pWnd = GetDlgItem(IDC_STATIC_TEXT)) != NULL) {
		pWnd->SetWindowText(szText);
	}

	UpdateControls();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyWizardPage7::UpdateControls()
{
	CCanNode *pNode = NULL;
	CPropertyWizard *pSheet = NULL;
	
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		const BOOL lastPage = pSheet->IsLastPage(WIZARDPAGE_CHOICE_NODESADDED);

		CString szText;			
		if (lastPage) {			
			const WizardTasks_t Task = pSheet->GetTask();
			if (WIZARD_CONTINUE_NODESADDED == Task) {
				szText.Format("Press finish to continue...");
			}	
			else if (WIZARD_CONTINUE_NODESREMOVED == Task) {				
				if(m_CmbPhysical.GetCount()==0) {
					szText.Format("Press finish to continue...");
				}
				else {
					szText.Format("Press cancel to search for the nodes again...");
				}
			}
			else {
				szText.Format("When done press finish. The browse algoritme will start again...");
			}
		}
		else  {			
			szText.Format("When finished press continue for the next step.");
			//szText.Format("To proceed press next");
		}			
		
		CWnd *pWnd = NULL;
		if ((pWnd = GetDlgItem(IDC_STATIC_TEXT2)) != NULL) {
			pWnd->SetWindowText(szText);
		}	


		DWORD dwStyle;
		const WizardTasks_t Task = pSheet->GetTask();		
		const BYTE bNodeAddress = GetNodeAddress();
		switch (Task)
		{
			case WIZARD_CONTINUE_NODESINCOMPATIBLE :	// TM 20070927
			case WIZARD_CONTINUE_NODESREMOVED :
			{			
				int i;
				TMGetDocumentNodes()->FindNode(bNodeAddress, &i, &pNode);				
				
				if (lastPage) {				
					if(m_CmbPhysical.GetCount()==0) {
						dwStyle = PSWIZB_FINISH | PSWIZB_BACK;
					}
					else {
						dwStyle = PSWIZB_DISABLEDFINISH | PSWIZB_BACK;
					}
				}
				else {
					dwStyle = PSWIZB_NEXT | PSWIZB_BACK;
				}				
			} break;
			case WIZARD_CONTINUE_NODESADDED :
			{			
				int i;			
				TMGetRuntimeNodes()->FindNode(bNodeAddress, &i, &pNode);		

				if (lastPage) {	
					dwStyle = PSWIZB_FINISH | PSWIZB_BACK;
				}
				else {
					dwStyle = PSWIZB_NEXT | PSWIZB_BACK;
				}

			} break;		
		}
		pSheet->SetWizardButtons(dwStyle);
	}
	

	if (pNode != NULL) 
	{
		CString szText;
		szText.Format(_T("%s"),pNode->szNodeName);
		m_EditName.SetWindowText(szText);
		m_EditName.EnableWindow(TRUE);

		szText.Format(_T("0x%02x"),pNode->bLogicalAddress);
		m_EditAddress.SetWindowText(szText);
		m_EditAddress.EnableWindow(TRUE);

		szText.Format(_T("%d"),pNode->bNROfUnits);
		m_EditUnits.SetWindowText(szText);
		m_EditUnits.EnableWindow(TRUE);

		szText.Format(_T("%d"),pNode->getNodeBindingsNrEntries());
		m_EditBindings.SetWindowText(szText);	
		m_EditBindings.EnableWindow(TRUE);
	}
	else 
	{
		CString szText;
		szText.Empty();
		m_EditName.SetWindowText(szText);
		m_EditName.EnableWindow(FALSE);

		szText.Empty();
		m_EditAddress.SetWindowText(szText);
		m_EditAddress.EnableWindow(FALSE);
		
		szText.Empty();
		m_EditUnits.SetWindowText(szText);	
		m_EditUnits.EnableWindow(FALSE);
		
		szText.Empty();
		m_EditBindings.SetWindowText(szText);	
		m_EditBindings.EnableWindow(FALSE);

		if(m_CmbPhysical.GetCount()==0) 
		{
			m_CmbPhysical.EnableWindow(FALSE);
			m_BtnRemoveAdd.ShowWindow(FALSE);
		}
	}
}

void CPropertyWizardPage7::OnCloseupComboPhysical() 
{
	UpdateControls();
}

void CPropertyWizardPage7::OnCbnSelchangeComboPhysical() 
{
	UpdateControls();
}

// TM 20070927: Function for WIZARD_CONTINUE_NODESREMOVED + WIZARD_CONTINUE_NODESINCOMPATIBLE
void CPropertyWizardPage7::OnButtonRemoveadd2() 
{						
	CCanNode *pDocNode;
	int t;
	const BYTE bNodeAddress = GetNodeAddress();	
	if (TMGetDocumentNodes()->FindNode(bNodeAddress, &t, &pDocNode)) {
		if (AfxGetMainWnd() != NULL) {	
			CString szMessage(_T("Are you sure you want to remove this device from the installation?\n"
								 "The device and all his configuration will be removed from the PC configuration file."));
		
			if (AfxMessageBox(szMessage,MB_ICONQUESTION | MB_YESNO) == IDYES) {																		
				const unsigned short nBindings = TMDeleteNodefromDatabases(bNodeAddress);			
				for (int i = 0; i<m_CmbPhysical.GetCount(); i++) {
					if ( m_CmbPhysical.GetItemData(i) == bNodeAddress) {
						m_CmbPhysical.DeleteString(i);
						break;
					}
				}				
				m_CmbPhysical.SetCurSel(0);	
				UpdateControls();
			}
		}
	}
}

BYTE CPropertyWizardPage7::GetNodeAddress()
{	
	const int iSel = m_CmbPhysical.GetCurSel();
	if (iSel != CB_ERR)  {	
		return ((BYTE) m_CmbPhysical.GetItemData(iSel));
	}
	return 0xFF;
}
