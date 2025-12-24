// PropertyWizardPage6.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage6.h"

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
// CPropertyWizardPage6 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage6, CPropertyPage)	// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyWizardPage6::CPropertyWizardPage6() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage6::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE6)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage6)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyWizardPage6::~CPropertyWizardPage6()
{
}

void CPropertyWizardPage6::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage6)
	DDX_Control(pDX, IDC_COMBO_LOGICAL, m_ComboInit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage6, CPropertyWizardPageBase)		
	//{{AFX_MSG_MAP(CPropertyWizardPage6)
	ON_CBN_SELCHANGE(IDC_COMBO_LOGICAL, OnSelchangeComboLogical)
	ON_BN_CLICKED(IDC_BUTTON_INIT, OnButtonInit)
	ON_CBN_EDITCHANGE(IDC_COMBO_LOGICAL, OnEditchangeComboLogical)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage6 message handlers
BOOL CPropertyWizardPage6::OnSetActive() 
{
	CWnd *pWnd = NULL;
	CString szText;

	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	
	if (pSheet != NULL) 
	{
		DWORD dwStyle;

		if (pSheet->IsLastPage(WIZARDPAGE_CHOICE_INIT)) 
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

/*
	//szText.Format(_T("Initialise node to factory settings"));
	szText.Empty();
	pWnd = GetDlgItem(IDC_STATIC_TEXT);
	if (pWnd != NULL) pWnd->SetWindowText(szText);
*/
	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage6::OnWizardBack() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	
	if (pSheet != NULL) 
	{
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_INIT));
	}
	return FALSE;
}

LRESULT CPropertyWizardPage6::OnWizardNext() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();

	if (pSheet != NULL) 
	{
		return(pSheet->NextPage(WIZARDPAGE_CHOICE_INIT));
	}
	return CPropertyWizardPageBase::OnWizardNext();
}



BOOL CPropertyWizardPage6::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();

	m_ComboInit.Clear();


	if (1)
	{		
		CWnd *pWnd = NULL;
		if ((pWnd = GetDlgItem(IDC_STATIC_TEXT)) != NULL) {
			CString szInfo("The factory init will remove all information from the CAN-Bus device.\n"
						   "Caution: When multiple devices share the same logical address both devices will be initialized.\n");
			pWnd->SetWindowText(szInfo);
		}
	}

	
	int nRtNodes;			
	if (TMGetRuntimeNodes()->NROfNodes(&nRtNodes))
	{
		if (nRtNodes != 0) 
		{					
			BYTE *NodeAddress = NULL;

			if ((NodeAddress = new BYTE [nRtNodes]) != NULL)
			{
				int i;
				int iCount = 0;
				CCanNode *pNode,*pRTNode;	
			
				for (i=0;i<nRtNodes;i++) 
				{				
					// For all nodes in Runtime database ...
					if (TMGetRuntimeNodes()->GetNode(i, &pRTNode)) 
					{										
						// Logical nodeaadress not in array -
						// Compare all nodes in RT nodedatabase with Current node,
						// if not same node but logical address is same -> Duplicate found !!

						for (int j=0;j<nRtNodes;j++) 
						{					
							if (TMGetRuntimeNodes()->GetNode(j, &pNode)) 
							{							
								if ((pNode->bLogicalAddress == pRTNode->bLogicalAddress) &&
									(pNode->ulPhysicalAddress != pRTNode->ulPhysicalAddress)) 
								{		
									int t;

									for (t=0;t<iCount;t++) 
									{
										if (NodeAddress[t] == pRTNode->bLogicalAddress)
										{
											break;
										}
									}
									
									if (t==iCount)
									{											
										NodeAddress[iCount++]=pRTNode->bLogicalAddress;
									}										
								}										
							}
						} // end for (j=0;j<nRtNodes;j++) 
					}
				} // for (int i=0;i<nRtNodes;i++) 		

				CString szNodeAddress;

				for (i=0;i<iCount;i++) 
				{				
					szNodeAddress.Format("0x%02x",NodeAddress[i]);
					m_ComboInit.InsertString(i,szNodeAddress);
					m_ComboInit.SetItemData(i,NodeAddress[i]);
				}
				
				delete [] NodeAddress;
				NodeAddress = NULL;
				
				m_ComboInit.SetCurSel(0);
			}
		}
	}
		
	UpdateControls();			
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyWizardPage6::OnSelchangeComboLogical() 
{
	UpdateControls();
}

void CPropertyWizardPage6::OnButtonInit() 
{
	int NodeAddress = 0;

	if (1)
	{
		CString szLogicalAddress;
		m_ComboInit.GetWindowText(szLogicalAddress);

		/* TM 20080421 :
		   Check length + valid nodeAddress: troubles could happen when the
		   physical address is entered instead of the logicalAddress.
	    */
		
		szLogicalAddress.TrimRight( );
		szLogicalAddress.TrimLeft( );
		szLogicalAddress.MakeUpper( );
		szLogicalAddress.TrimLeft("0X");
		const int len = szLogicalAddress.GetLength();

		if (len <= 2)		
		{	
			if ((sscanf(szLogicalAddress,"%x",&NodeAddress) == 1) &&		// CHECKED_SSCANF_OK
				(NodeAddress >= 0x03 && NodeAddress < 0xFD)) 
			{
				CString szConfirm;
				szConfirm.Format(_T("Are you sure you want to give the device with logical address 0x%02x a factory reset?"),
									NodeAddress);

				if (IDYES == AfxMessageBox(szConfirm,MB_ICONQUESTION | MB_YESNO)) 
				{		
					CTXNodeMessage::getInstance()->FactoryResetNode((BYTE) NodeAddress);
				}
			}
			else
			{
				CString szMessage("The device logical address is invalid");
				AfxMessageBox(szMessage,MB_OK);
			}
		}
		else
		{
			CString szMessage("The device logical address has the wrong format");
			AfxMessageBox(szMessage,MB_OK);
		}
	}
}

void CPropertyWizardPage6::UpdateControls()
{
	BYTE bNodeAddress = 0xFF;
	
	if (1)
	{
		int temp = -1;
		CString szLogicalAddress;	

		m_ComboInit.GetWindowText(szLogicalAddress);
		
		if (sscanf(szLogicalAddress,"%x",&temp) == 1)	//CHANGES_MVS_2008_SSCANF
		{
			ASSERT( temp >= 0 && temp <= 0xFF );
			bNodeAddress = temp;
		}
	}

	CString szTemp,szTemp2;

	if (bNodeAddress != 0xFF) 
	{	
		int nRtNodes;			
		if (TMGetRuntimeNodes()->NROfNodes(&nRtNodes)) {
			CCanNode *pRTNode;
			unsigned long ulAddresses[255];			
			int found = 0;
			for (int i=0;i<nRtNodes;i++) {	
				szTemp.Empty();			
				if (TMGetRuntimeNodes()->GetNode(i,&pRTNode)) {				
					if (pRTNode->bLogicalAddress == bNodeAddress) {
						ulAddresses[found++] = pRTNode->ulPhysicalAddress;
					}
				}										
			} // end for (i=0;i<nRtNodes;i++) 
				
			if (found == 1) {
				szTemp.Format("This address is used by CAN-bus device 0x%06x.\n", ulAddresses[0]);
				szTemp2 += szTemp;
			}
			else if (found > 1) {									
				szTemp.Format("This address is used by multiple CAN-bus devices: ");
				szTemp2 += szTemp;
				for (int i=0;i<found;++i) {
					szTemp.Format("0x%06x ", ulAddresses[i]);
					szTemp2 += szTemp;
					if (i<found-1) {
						szTemp2 += ",";
					}
				}
				szTemp2 += "\n";
			}
		} // end if (->NROfNodes(&nRtNodes))
		

		szTemp.Empty();	

		if (TMGetDocumentNodes()->NROfNodes(&nRtNodes)) {
			if (nRtNodes != 0)	{
				CCanNode *pDocNode;
				int i;
				if (TMGetDocumentNodes()->FindNode(bNodeAddress, &i, &pDocNode)) {									
					szTemp.Format("This address is used by device 0x%06x ('%s') in the PC database", pDocNode->ulPhysicalAddress, pDocNode->szNodeName);					
				}		
				else {		
					szTemp.Format("This address is not used in the PC database");					
				}
			}		
		}
	}

	if (1)
	{
		CString szInfo;
		szInfo.Format(_T("%s\n%s"),szTemp,szTemp2);

		CWnd *pWnd = NULL;
		if ((pWnd = GetDlgItem(IDC_STATIC_INFO)) != NULL)
		{
			pWnd->SetWindowText(szInfo);
		}
	}
}

void CPropertyWizardPage6::OnEditchangeComboLogical() 
{
	UpdateControls();	
}

// ----------------------------------------------------------
BYTE CPropertyWizardPage6::GetNodeLogicalAddress()
{	
	const int iSel = m_ComboInit.GetCurSel();	
	const DWORD dwData = m_ComboInit.GetItemData(iSel);

	if (dwData == CB_ERR) 
	{
		CString szLogicalAddress;
		int TempNodeAddress = 0;
		m_ComboInit.GetWindowText(szLogicalAddress);		
		if (sscanf(szLogicalAddress,"%x",&TempNodeAddress) == 1)		// CHECKED_SSCANF_OK	
		{
			ASSERT(TempNodeAddress >= 0 && TempNodeAddress <= 0xFF);	// CHANGES_MVS_2008
			return ((BYTE) TempNodeAddress);
		}
	}
	else 
	{
		return ((BYTE) dwData);
	}	
	return 0;
}
