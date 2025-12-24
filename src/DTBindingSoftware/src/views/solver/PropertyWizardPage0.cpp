// PropertyWizardPage0.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage0.h"
#include "PropertyWizard.h"
#include "Mainfrm.h"
#include "NodeDatabaseverify.h"
#include "NodeDatabaseVerify_Handlers.h"
#include "app/appnodedbase/appnodedbase.h"
#include "app\applicactcode\applicActCodeApi.h"
#include "AppLogging.h"
// -----------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ------------------------------------

#ifdef _DEBUG
	#define DEBUG_HORIZONTAL_SCROLL					0
#else
	#define DEBUG_HORIZONTAL_SCROLL					0
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage0 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage0, CPropertyPage)

CPropertyWizardPage0::CPropertyWizardPage0() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage0::IDD,(UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE0)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage0)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HIDEHEADER;
}

CPropertyWizardPage0::~CPropertyWizardPage0()
{
}

void CPropertyWizardPage0::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage0)
	DDX_Control(pDX, IDC_EDIT_TEXT1, m_EditText1);
	DDX_Control(pDX, IDC_LIST_ERRORS, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage0, CPropertyWizardPageBase)
	//{{AFX_MSG_MAP(CPropertyWizardPage0)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage0 message handlers

LRESULT CPropertyWizardPage0::OnWizardNext() 
{
	return CPropertyWizardPageBase::OnWizardNext();
}

BOOL CPropertyWizardPage0::OnSetActive() 
{
	CPropertySheet* const pSheet = (CPropertySheet*) GetOwner();		
	if (pSheet != NULL) 
	{
		pSheet->SetWizardButtons(PSWIZB_NEXT );
	}
	return CPropertyWizardPageBase::OnSetActive();
}

BOOL CPropertyWizardPage0::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();

	CString szText;
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet) 
	{		
		const WizardTasks_t Task = pSheet->GetTask();	
		switch (Task) 
		{
			case WIZARD_CONTINUE_FACTORYADDRESS:
			case WIZARD_NEW_FACTORYADDRESS: 
			{															
				szText.LoadString(IDS_WIZARD_PROBLEM_FACTORYADDR);
				AddToListBox(LISTBOX_INFO_FACTORYADDRESSES);						
			} break;
			
			case WIZARD_NEW_DUPLICATESFOUND :
			case WIZARD_CONTINUE_DUPLICATESFOUND :
			{
				szText.LoadString(IDS_WIZARD_PROBLEM_ADDRESSES);
				AddToListBox(LISTBOX_INFO_DUPLICATES);
			} break;

			case WIZARD_NEW_FACTORYADDRESS_ASSIGN_MASTER :
			case WIZARD_CONTINUE_FACTORYADDRESS_ASSIGN_MASTER :
			{			
				szText.LoadString(IDS_WIZARD_PROBLEM_FACTORYADDR_MASTER);

				CList<int> list;
				CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
				if (NULL != pAppNodeDatabase) {
					pAppNodeDatabase->runtimeNodeDatabase_foundSmartboxUltraNodesWithFactoryAddresses(list);
				}

				// Lijst weergave van de nodes die als master kunnen ingesteld worden.
				if (!list.IsEmpty()) {
					CString strTemp;										
					if (list.GetCount() == 1) {
						strTemp = "Possible to set this node as master:";
					}
					else {
						strTemp = "Possible to set these nodes as master:";
					}
					m_ListBox.AddString(strTemp);

					POSITION pos = list.GetHeadPosition();
					while (pos != NULL) {				
						const int value = list.GetNext(pos); 						
						CString strEntry;
						strEntry.Format("CAN-bus device: 0x%06x",value);
						m_ListBox.AddString(strEntry);
					}
				}

				// Lijst weergave van alle nodes met een factory address.
				if (m_ListBox.GetCount() > 0) {
					CString szTemp;
					m_ListBox.AddString(szTemp);

					szTemp = "Nodes found with factory addresses:";
					m_ListBox.AddString(szTemp);

					// Deze zorgt ervoor dat de node met 0xFE addressen getoond worden.
					AddToListBox(LISTBOX_INFO_FACTORYADDRESSES,FALSE);	
				}							
			} break;

			case WIZARD_CONTINUE_NODESADDED:
			{				
				szText.LoadString(IDS_WIZARD_PROBLEM_DEVICES_ADD);

				CNodeDatabaseVerify_Handler2 Handler(*this);
				CNodeDatabaseVerify Algoritme(&Handler);

				m_ListBox.ResetContent();
				Algoritme.Check_Added_Nodes();
								
			} break;
			
			case WIZARD_CONTINUE_NODESREMOVED :	
			{										
				szText.LoadString(IDS_WIZARD_PROBLEM_DEVICES_REMOVED);

				CNodeDatabaseVerify_Handler2 Handler(*this);
				CNodeDatabaseVerify Algoritme(&Handler);

				m_ListBox.ResetContent();
				Algoritme.Check_Removed_Nodes();
			
			} break;

			case WIZARD_CONTINUE_NODESINCOMPATIBLE :
			{
				szText.LoadString(IDS_WIZARD_PROBLEM_DEVICES_INCOMPATIBLE);

				CNodeDatabaseVerify_Handler2 Handler(*this);
				CNodeDatabaseVerify Algoritme(&Handler);

				m_ListBox.ResetContent();
				Algoritme.Check_Incompatible_Nodes();
			} break;

			case WIZARD_USER_SELECTION :
			{
				szText.Format("User selection - configuration change");		
				
				m_ListBox.ResetContent();

				CString szTemp;
				szTemp.Format(_T("No Information"));
				m_ListBox.AddString(szTemp);
				
			} break;

			case WIZARD_CONTINUE_PROGRAMFILEDIFFERENCES:
			{							
				szText.LoadString(IDS_WIZARD_PROBLEM_CONFIGFILE);
			
				AddToListBox(LISTBOX_INFO_PROGRAMFILEDIFFERENCES);

				if (m_ListBox.GetCount() > 0) {
					CString szTemp("---------------------------------------------------");
					m_ListBox.AddString(szTemp);
				}
				
				AddToListBox(LISTBOX_INFO_DIFFERENTNAMES,FALSE);

			} break;

			case WIZARD_CONTINUE_NAMEDIFFERENCES:
			{														
				szText.LoadString(IDS_WIZARD_PROBLEM_NAMES);
				AddToListBox(LISTBOX_INFO_DIFFERENTNAMES);			

			} break;

			case WIZARD_LICENCE_ACTIVATION_CODE:
			{
				m_ListBox.ResetContent();

				szText.Format("To continue a license activation code is needed.");
				
				// CHANGES_ACTIVATION_LICENSE_CODE
				// Physical node addressen tonen van alle nodes die een licentie 
				// activatie nodig hebben.

				enum {MAX_ITEMS = 255};
				CCanNode* pNodes[MAX_ITEMS];

				for (int i=0;i<MAX_ITEMS;++i) {
					pNodes[i] = 0;
				}

				const int count = CAppLicActCodeAPI::FindNodesLicActCodeNeeded(					
					reinterpret_cast<CCanNode**>(&pNodes),			// TRICKY ?
					MAX_ITEMS);

				if (count > 0)
				{
					AddToListBox("These physical nodes need a license activation code:");
					AddToListBox("");
					for (int i=0;i<count;++i) {
						if (0 != pNodes[i]) {
							CString szMessage;
							szMessage.Format("Physical address=0x%06x",pNodes[i]->getPhysicalAddress());
							AddToListBox(szMessage);
						}
					}
				}
				else {
					ASSERT(count != 0);
				}
			} break;

			default :

			break;		
		}
	}	

	m_EditText1.SetWindowText(szText);

	CWnd *pWnd = NULL;
	if ((pWnd = GetDlgItem(IDC_STATIC_TEXT2)) != NULL)
	{
		CString szText2(_T("The wizard will guide you through the needed steps."));
		pWnd->SetWindowText(szText2);
	}


#if(DEBUG_HORIZONTAL_SCROLL == 1)
	for (int i=0;i<50;++i) {
		AddToListBox("ABCDEFGHIJKLMNOIPQRTSUVWXYZ - 01234567890 - ABCDEFGHIJKLMNOIPQRTSUVWXYZ - 01234567890");
	}
#endif 



	showHorizontalScrollBar( );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyWizardPage0::showHorizontalScrollBar( ) {

	CString str;
    CSize sz;
    int dx = 0;
	TEXTMETRIC tm;
    CDC* pDC = m_ListBox.GetDC();
	CFont *pFont = m_ListBox.GetFont();

	// Select the listbox font, save the old font
	CFont *pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	pDC->GetTextMetrics(&tm);


    for(int i=0; i < m_ListBox.GetCount();i++)
    {
        m_ListBox.GetText(i,str);
        sz = pDC->GetTextExtent(str);

		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;

        if(sz.cx > dx)
            dx = sz.cx;

    }

	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
    m_ListBox.ReleaseDC(pDC);

    if(m_ListBox.GetHorizontalExtent() < dx ) {
        m_ListBox.SetHorizontalExtent(dx);
        ASSERT(m_ListBox.GetHorizontalExtent() == dx);
    }
}

void CPropertyWizardPage0::AddToListBox(CString szText)
{
	m_ListBox.AddString(szText);
}

void CPropertyWizardPage0::AddToListBox(ENUM_LISTBOX_INFO info,BOOL fClear)
{
	if (fClear == TRUE) 
	{
		m_ListBox.ResetContent();
	}

	switch(info)
	{
		case LISTBOX_INFO_FACTORYADDRESSES :
		{			
			int nNodes;
			if (TMGetRuntimeNodes()->NROfNodes(&nNodes)) {				
				for (int i=0;i<nNodes;i++) {				
					CCanNode *pRTNode;
					if (TMGetRuntimeNodes()->GetNode(i,&pRTNode)) {
						if (pRTNode->bLogicalAddress == 0xFE) {
							CString szTemp;
							szTemp.Format(_T("CAN-bus device: 0x%06x"), pRTNode->ulPhysicalAddress);								
							m_ListBox.AddString(szTemp);
						}
					}// if (GetNode
					
					ASSERT(pRTNode);
				} // for (int i=0;i<nNodes;i++) 
			}// if (NROfNodes(&nNodes))
		} break;	

		case LISTBOX_INFO_DUPLICATES :
		{			
			int nNodes;

			if (TMGetRuntimeNodes()->NROfNodes(&nNodes))
			{
				CString szTemp;
				CCanNode *pRTNode,*pNode;

				if (nNodes != 0) 
				{
					int i,k,iCount;
					unsigned long *ulPhysicalAddreses = NULL;

					if ((ulPhysicalAddreses = new unsigned long[nNodes]) != NULL)
					{				
						iCount = 0;
						for (i=0;i<nNodes;i++) 
						{
							if (TMGetRuntimeNodes()->GetNode(i,&pRTNode)) 
							{											
								for (int t=0;t<nNodes;t++) 
								{							
									if (TMGetRuntimeNodes()->GetNode(t,&pNode)) 
									{																												
										if ((pNode->bLogicalAddress == pRTNode->bLogicalAddress) &&
											(pNode->ulPhysicalAddress != pRTNode->ulPhysicalAddress)) 
										{
											for (k=0;k<iCount;k++) 
											{
												if (pNode->ulPhysicalAddress == ulPhysicalAddreses[k])
												{
													break;
												}
											}
											
											if (k==iCount) 
											{
												ulPhysicalAddreses[iCount++] = pNode->ulPhysicalAddress;
											}						
										} // if ((pNode->bLogicalAddress == pRTNode->bLogicalAddress)										
									} // if (GetNode
									ASSERT(pNode);									
								} // end for (int t=0;t<nNodes;t++) 														
							} // if (GetNode						
							ASSERT(pRTNode);
						} // end for all nodes ...
			
						ASSERT(iCount<= nNodes);
				
						for (i=0;i<iCount;i++) 
						{
							ASSERT(ulPhysicalAddreses[i]);

							if (TMGetRuntimeNodes()->FindPhysicalNode(ulPhysicalAddreses[i],&k,&pRTNode)) 
							{						
								szTemp.Format(_T("CAN-bus device: 0x%06x.(LA=0x%02x)"),pRTNode->ulPhysicalAddress, pRTNode->bLogicalAddress);														
								m_ListBox.AddString(szTemp);
								
							} // if (FindPhysicalNode
							ASSERT(pRTNode);
						} // for (i=0;i<iCount;i++) 
								
							// clean up Memory !!
						if (ulPhysicalAddreses != NULL) 
						{
							delete [] ulPhysicalAddreses;
							ulPhysicalAddreses = NULL;
						}						
					} // if ((ulPhysicalAddreses = new unsigned long[nNodes]) != NULL)
				} // if (nNodes != 0)
				ASSERT(nNodes);
			} // if (NROfNodes(&nNodes))
		}break;

		case LISTBOX_INFO_DIFFERENTNAMES:
		{	
			int nNodes;			
			CNodeDatabase* const pNodeDatabaseRT = TMGetRuntimeNodes();
			if (pNodeDatabaseRT->NROfNodes(&nNodes)) {				
				CCanNode *pRTNode,*pDocNode;
				STRUCT_UNIT *pRTUnit,*pDocUnit;
				int k;
				BOOL firstLine=TRUE;
				for (int i=0;i<nNodes;i++)  {				
					if (pNodeDatabaseRT->GetNode(i, &pRTNode)) {				
						CNodeDatabase* const pNodeDatabaseDOC = TMGetDocumentNodes();
						if (pNodeDatabaseDOC->FindNode(pRTNode->bLogicalAddress, &k, &pDocNode)) {																		
							const int NROfUnits = pRTNode->bNROfUnits;
							for (int t=0;t<NROfUnits;t++)  {
								if (pNodeDatabaseRT->GetUnit(pRTNode,t,&pRTUnit))  {
									if (pNodeDatabaseDOC->GetUnit(pDocNode, t, &pDocUnit)) {			
										if (strcmp(pRTUnit->szUnitName,pDocUnit->szUnitName) != 0) {											
											// Name of network unit and database unit are different											
											if (firstLine) {
												m_ListBox.AddString("Name differences:");
												firstLine = FALSE;
											}

											CString szTemp;
											szTemp.Format(_T("'Device: %s',CAN-bus name:'%s',PC name:'%s'"),
														  pDocNode->szNodeName, pRTUnit->szUnitName, pDocUnit->szUnitName);
											m_ListBox.AddString(szTemp);														
										}													
									} // if (->GetUnit
									ASSERT(pDocUnit);
								}// if (->GetUnit
								ASSERT(pRTUnit);
							} // for all units in network node ...									
						}	// if (->FindNode
						ASSERT(pDocNode);
					} // ->GetNode
					ASSERT(pRTNode);
				} // end for all nodes in runtime database ...
			}			
		} break;

		case LISTBOX_INFO_PROGRAMFILEDIFFERENCES:
		{
			int nNodes;			
			if (TMGetRuntimeNodes()->NROfNodes(&nNodes)) {
				CString szTemp;
				CCanNode *pRTNode,*pDocNode;
				int t;			
				BOOL firstLine = TRUE;
				for (int i=0;i<nNodes;i++)  {											
					if (TMGetRuntimeNodes()->GetNode(i,&pRTNode)) {
						if (TMGetDocumentNodes()->FindNode(pRTNode->bLogicalAddress, &t, &pDocNode)) {
							if ((pDocNode->getNodeBindingsNrEntries() != pRTNode->getNodeBindingsNrEntries()) ||
								(pDocNode->getNodeBindingsXORChecksum() != pRTNode->getNodeBindingsXORChecksum()) )
							{													
#if(ENABLE_TRACE_BINDINGFILEINFO_INFO == 1)								
								// Sedert V16.32: Info in het logbestand - Disabled via een macro.
								CString strInfoMessage;
								strInfoMessage.Format("Node 0x%02x: Binding differences: Database Nr of Bindings=%d - XOR=0x%02x, Runtime Nr of Bindings=%d - XOR=0x%02x",
									pDocNode->getNodeAddress(), 
									pDocNode->getNodeBindingsNrEntries(),pDocNode->getNodeBindingsXORChecksum(),
									pRTNode->getNodeBindingsNrEntries(),pRTNode->getNodeBindingsXORChecksum());
								AppLogging::getInstance().addWarning(strInfoMessage);
#endif

								if (firstLine) {
									m_ListBox.AddString("Configuration differences:");
									firstLine = FALSE;
								}
								szTemp.Format(_T("CAN-bus device:'%s' (LA=0x%02x) and PC device:'%s'"),											  
											  pRTNode->szNodeName, pRTNode->bLogicalAddress,
											  pDocNode->szNodeName);									
								m_ListBox.AddString(szTemp);
							} // if ((pDocNode->GetNrEntries() != pRTNode->GetNrEntries() &&							
						} // if (					
						ASSERT(pDocNode);						
					} // if (GetNode
					ASSERT(pRTNode);						
				} // for (int i=0;i<nNodes;i++) 								
			}// if (NROfNodes(&nNodes))
		} break;				
	}// end switch info ...	
}




