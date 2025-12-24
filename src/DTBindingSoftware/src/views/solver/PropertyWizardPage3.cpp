// PropertyWizardPage3.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage3.h"

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
// CPropertyWizardPage3 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage3, CPropertyPage)	

CPropertyWizardPage3::CPropertyWizardPage3() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage3::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE3)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage3)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyWizardPage3::~CPropertyWizardPage3()
{
}

void CPropertyWizardPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage3)
	DDX_Control(pDX, IDC_EDIT_LOGICAL, m_EditLogical);
	DDX_Control(pDX, IDC_COMBO_PHYSICAL, m_ComboPhysical);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage3, CPropertyWizardPageBase)		
	//{{AFX_MSG_MAP(CPropertyWizardPage3)
	ON_BN_CLICKED(IDC_BUTTON_ASSIGN, OnButtonAssign)
	ON_CBN_SELCHANGE(IDC_COMBO_PHYSICAL, OnSelchangeComboPhysical)
	ON_CBN_EDITCHANGE(IDC_COMBO_PHYSICAL, OnEditchangeComboPhysical)
	ON_CBN_CLOSEUP(IDC_COMBO_PHYSICAL, OnCloseupComboPhysical)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage3 message handlers

BOOL CPropertyWizardPage3::OnSetActive() 
{
	DWORD dwStyle;
	CWnd *pWnd = NULL;
	CString szText;

	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();

	if (pSheet != NULL) 
	{
		if (pSheet->IsLastPage(WIZARDPAGE_CHOICE_MANUALASSIGN)) 
		{
			dwStyle = PSWIZB_FINISH | PSWIZB_BACK;	
			szText.Format("When done press Finish. The browse algoritme will start again...");			
		}
		else 
		{
			dwStyle = PSWIZB_NEXT | PSWIZB_BACK;
			szText.Format("When finished press continue for the next step.");
			//szText.Format("To proceed press next");
		}
		
		pSheet->SetWizardButtons(dwStyle);
	}

	pWnd = GetDlgItem(IDC_STATIC_TEXT2);
	if (pWnd != NULL) pWnd->SetWindowText(szText);

	szText.Empty();

	szText.Format("Manually assign a logical address to a CAN-bus device.\n\n"				  
				  "Caution: This option is for advanced users!\n"
				  "Make sure the logical address is not used by another device.");				 

	pWnd = NULL;	
	pWnd = GetDlgItem(IDC_STATIC_TEXT);
	if (pWnd != NULL) pWnd->SetWindowText(szText);

	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage3::OnWizardBack() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	
	if (pSheet != NULL) 
	{
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_MANUALASSIGN));
	}
	return FALSE;
}

LRESULT CPropertyWizardPage3::OnWizardNext() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	
	if (pSheet != NULL) 
	{
		return(pSheet->NextPage(WIZARDPAGE_CHOICE_MANUALASSIGN));
	}

	return CPropertyWizardPageBase::OnWizardNext();
}


void CPropertyWizardPage3::OnButtonAssign() 
{		
	BYTE bNodeAddress = 0xFF;

	if (GetNodeAddress(&bNodeAddress) == TRUE)
	{
		const unsigned long ulPhysicalAddress = GetPhysicalAddress();
		if (bNodeAddress == 0xFF) {
			CString str(_T("Please retry.\nThe device logical address must be in the range 0x03-0xFC"));
			AfxMessageBox(str);
		}
		else  {			
			if (bNodeAddress >= 0x03 && bNodeAddress <= 0xfc) {
								
				// Sedert V16.16:
				// Controle doen of er in de runtime database geen andere node is met dit logisch adres...

				UINT messageBoxStyle = MB_ICONQUESTION | MB_YESNO;
				BOOL duplicateNodeFound = FALSE;	
				CCanNode* pNodeRt = NULL;
				int dummy = 0;
				if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&dummy,&pNodeRt)) {				
					if (ulPhysicalAddress != pNodeRt->getPhysicalAddress()) {
						duplicateNodeFound = TRUE;		
					}									
				}				

				CString strQuestion;
				if ((duplicateNodeFound) && (NULL != pNodeRt)) {		
					messageBoxStyle = MB_ICONEXCLAMATION | MB_YESNO;
					strQuestion.Format("The logical address 0x%02x is already used by another CAN-bus device 0x%06x. "
									   "Assigning this address to the device 0x%06x will result in duplicate logical addresses.\n\n"									 
									   "Are you sure want to assign this logical address?",
									   bNodeAddress, pNodeRt->getPhysicalAddress(), ulPhysicalAddress);					
				}
				else {			
					
					// Sedert V16.16:
					// Controle doen of deze node in de pc database aanwezig is met een ander logisch adres.
					BOOL foundNodeInPCDatabaseOtherAddress = FALSE;
					CCanNode* pNodeDoc = NULL;
					if (TMGetDocumentNodes()->FindPhysicalNode(ulPhysicalAddress,&dummy,&pNodeDoc)) {
						if (pNodeDoc->getNodeAddress() != bNodeAddress) {
							foundNodeInPCDatabaseOtherAddress = TRUE;	// gevonden in de nodedatabase met een ander logisch adres.
						}
					}

					if ((foundNodeInPCDatabaseOtherAddress) && (NULL != pNodeDoc)) {
						messageBoxStyle = MB_ICONEXCLAMATION | MB_YESNO;
						strQuestion.Format("The device 0x%06x has logical address 0x%02x in the pc-database.\n"
										   "Are you sure you want to change the logical address of the CAN-bus device?",
											ulPhysicalAddress, pNodeDoc->getNodeAddress());										   																
					}
					else {
						messageBoxStyle = MB_ICONQUESTION | MB_YESNO;
						strQuestion.Format(_T("Are you sure you want to give the device 0x%06x a logical Address 0x%02x?"), 
										   ulPhysicalAddress, bNodeAddress);								
					}
				}
				
				ASSERT(!strQuestion.IsEmpty());
				if (AfxMessageBox(strQuestion, messageBoxStyle) == IDYES) {	
					CTXBrowsePnpMessages::getInstance()->AssignPhysicalAddress(ulPhysicalAddress, bNodeAddress);
				}
			}
			else {
				CString str(_T("Please retry.\nThe device logical address must be in the range 0x03-0xFC"));
				AfxMessageBox(str);
			}
		}
	}
	else {
		CString str(_T("Please retry.\nThe device logical address must be in the range 0x03-0xFC"));
		AfxMessageBox(str);
	}
}


/**
 * For all Runtime nodes : 
 * if Addresses == 0xFE : Store physical address in array ulPhysicalAddress[].
 *	+ Check ulPhysicalAddress[] that there are not 2 identical physical addresses.
 *
 * Check for Duplicate addresses and store in array ulPhysicalAddress[].
 *  + Check ulPhysicalAddress[] that there are not 2 identical physical addresses.
 *
 */

BOOL CPropertyWizardPage3::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();

	m_ComboPhysical.Clear();

		
	// Fill Combobox with physical addresses of nodes with logical
	// address 0xFF, Duplicate addresses , or All Addresses ...
	
	int nRtNodes;
	if (TMGetRuntimeNodes()->NROfNodes(&nRtNodes))
	{			
		if (nRtNodes != 0) 
		{					
			unsigned long *ulPhysicalAddress = NULL;
				
			if ((ulPhysicalAddress = new unsigned long [nRtNodes]) != NULL)
			{	
				CCanNode *pNode,*pRTNode;
				int iCount = 0;
				int i;
	
				for (i=0;i<nRtNodes;i++) 
				{		
					if (TMGetRuntimeNodes()->GetNode(i,&pRTNode)) 
					{				
						// Search for nodes with adres = 0xFE ...
						if (pRTNode->bLogicalAddress == 0xFE) 
						{																					
							// must look that there are never 2 duplicates ...	
							int t;
							for (t=0;t<=iCount;t++) 
							{
								if (ulPhysicalAddress[t] == pRTNode->ulPhysicalAddress) 
								{
									break;
								}
							}

							if (t>iCount) 
							{
								ulPhysicalAddress[iCount++] = pRTNode->ulPhysicalAddress;
							}							
						}
						else 
						{
							// Search for nodes with duplicate node Addresses ...
							for (int j=0;j<nRtNodes;j++) 
							{							
								if (TMGetRuntimeNodes()->GetNode(j, &pNode)) 
								{														
									/**
									 * Duplicate addresses :
									 * Logical addresses = identical, Physical address = different.
									 *
									 */
									if ((pNode->bLogicalAddress == pRTNode->bLogicalAddress) &&
										(pNode->ulPhysicalAddress != pRTNode->ulPhysicalAddress)) 
									{													
										int t;

										// must look that there are never 2 duplicates ...
										for (t=0;t<=iCount;t++) 
										{
											if (ulPhysicalAddress[t] == pRTNode->ulPhysicalAddress)
											{
												break;
											}
										}

										if (t>iCount) 
										{
											ulPhysicalAddress[iCount++] = pRTNode->ulPhysicalAddress;
										}
									}										
								} // end if (->GetNode
								ASSERT(pNode);

							} // end for (j=0;j<nRtNodes;j++)
						} // end Search for nodes with duplicate node Addresses ...

					} //end if (->GetNode
					ASSERT(pRTNode);
				} // end for (i=0;i<nRtNodes;i++)
				
				CString szPhAddress;

				for (i=0;i<iCount;i++)  {			
					szPhAddress.Format(_T("0x%06x"),ulPhysicalAddress[i]);
					m_ComboPhysical.InsertString(i,szPhAddress);
					m_ComboPhysical.SetItemData(i,ulPhysicalAddress[i]);
				}
		
				delete [] ulPhysicalAddress;

				m_ComboPhysical.SetCurSel(0);
				
			} // end if ((ulPhysicalAddress = new unsigned long [nRtNodes]) != NULL)	
				
		} // end if (nRtNodes != 0) 

	} // end if (->NROfNodes(&nRtNodes))			
	
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyWizardPage3::UpdateControls(void)
{
	CWnd *pWnd = NULL;
	CString szInfo,szTemp,szTemp2;	

	const BOOL fUpdate = IsValidPhysicalAddress();
	
	if (fUpdate == TRUE) 
	{				
		if ((pWnd = GetDlgItem(IDC_BUTTON_ASSIGN)) != NULL) 
		{
			pWnd->EnableWindow(TRUE);
		}

		// Search in Document Database for this physical node +
		// search in Runtime database for this node ...

		const unsigned long ulPhysicalAddress = GetPhysicalAddress();
		if (ulPhysicalAddress != 0) 
		{			
			CCanNode *pDocNode,*pRTNode;					
			int nDocNodes;
				
			if (TMGetDocumentNodes()->NROfNodes(&nDocNodes)) {
				int i;
				if (nDocNodes != 0) 
				{
					// Lets search for physical node in Doc. database ...				
					if (TMGetDocumentNodes()->FindPhysicalNode(ulPhysicalAddress,&i,&pDocNode)) {												
						szTemp.Format(_T("The device '%s' has logical address 0x%02x in the PC database.\n"),pDocNode->szNodeName, pDocNode->bLogicalAddress);	
						szInfo.Format(_T("0x%02x"), pDocNode->bLogicalAddress);
						m_EditLogical.SetWindowText(szInfo);										

					} // if (->FindPhysicalNode
					else  {
						szTemp.Format(_T("The device 0x%06x is not in the PC database.\n"), ulPhysicalAddress);	
						m_EditLogical.SetWindowText("");
						
					} // else if (>FindPhysicalNode
				}
				else
				{
					szTemp.Format(_T("There are no devices in the PC database."));
					m_EditLogical.SetWindowText("");

				} //if (nDocNodes != 0) 				
			} //if (>NROfNodes(&nDocNodes))
				

			// Lets search in Runtime database for this node ...
			int nRtNodes;
			if (TMGetRuntimeNodes()->NROfNodes(&nRtNodes)){
				if (nRtNodes != 0) {				
					int i;
					if (TMGetRuntimeNodes()->FindPhysicalNode(ulPhysicalAddress,&i,&pRTNode)) {																										
						szTemp2.Format("The device 0x%06x has logical address 0x%02x on the CAN-bus.",
									   pRTNode->getPhysicalAddress(), pRTNode->bLogicalAddress);
					}
						
					ASSERT(pRTNode);
				} // if (nRtNodes != 0) 

			} // if (NROfNodes(&nRtNodes))								
		}
	}
	else 
	{
		// Disable button Assign coz' no valid address is in Combobox	
		if ((pWnd = GetDlgItem(IDC_BUTTON_ASSIGN)) != NULL) 
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	// Info for user ...
	if ((pWnd = GetDlgItem(IDC_STATIC_INFO)) != NULL) 
	{
		szInfo.Format(_T("%s%s"),szTemp,szTemp2);
		pWnd->SetWindowText(szInfo);
	}	
}

void CPropertyWizardPage3::OnSelchangeComboPhysical() 
{
	UpdateControls();
}

void CPropertyWizardPage3::OnEditchangeComboPhysical() 
{
	UpdateControls();
}

void CPropertyWizardPage3::OnCloseupComboPhysical() 
{
	// UpdateControls();
}


// returns physical address : Combo Box 
unsigned long CPropertyWizardPage3::GetPhysicalAddress()
{
	CString strPhysical;
	m_ComboPhysical.GetWindowText(strPhysical);

	const int iSel = m_ComboPhysical.GetCurSel();
	unsigned long ulPhysicalAddress = m_ComboPhysical.GetItemData(iSel);

	if (ulPhysicalAddress == CB_ERR) {
		ulPhysicalAddress = 0;	
		if (sscanf(strPhysical,"%x",&ulPhysicalAddress) == 0) 
		{
			ASSERT( 0 );
		}
	}
	return ulPhysicalAddress;
}

BOOL CPropertyWizardPage3::IsValidPhysicalAddress()
{
	BOOL fValid = FALSE;

	CString strPhysical;
	m_ComboPhysical.GetWindowText(strPhysical);
	strPhysical.MakeUpper();

	int nLength = strPhysical.GetLength();
	
	if (nLength == 6) 
	{ 
		if ((strPhysical.GetAt(0) == '0') &&
			(strPhysical.GetAt(1) == 'X')) 
		{
			fValid = FALSE;
		}
		else 
		{
			fValid = TRUE;
		}
	}
	else if (nLength == 8) 
	{
		if ((strPhysical.GetAt(0) == '0') &&
			(strPhysical.GetAt(1) == 'X')) 
		{
			fValid = TRUE;
		}
	}
	return fValid;
}

/* TM20120319: Controle of node Address een BYTE is */
BOOL CPropertyWizardPage3::GetNodeAddress(BYTE* pbNodeAddress)
{		
	CString strLogical;
	m_EditLogical.GetWindowText(strLogical);

	if (!strLogical.IsEmpty()) {	
		int temp = -1;		
		if (sscanf(strLogical,"%x",&temp) == 1)	{				
			if (temp >= 0 && temp <= 0xFF) {
				*pbNodeAddress = (BYTE) temp;
				return TRUE;
			}
		}
	}
	return FALSE;
}




