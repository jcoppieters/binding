// PropertyWizardPage10.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage10.h"


#if(0)

#include "PropertyWizard.h"
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
// CPropertyWizardPage10 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage10, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyWizardPage10::CPropertyWizardPage10() :
CPropertyWizardPageBase((UINT) CPropertyWizardPage10::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE10)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage10)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyWizardPage10::~CPropertyWizardPage10()
{
}

void CPropertyWizardPage10::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage10)
	DDX_Control(pDX, IDC_COMBO_NODE, m_CmbNetworkNodes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage10, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	//{{AFX_MSG_MAP(CPropertyWizardPage10)
	ON_BN_CLICKED(IDC_BUTTON_COPYALL, OnButtonCopyall)
	ON_BN_CLICKED(IDC_BUTTON_COPYNODE, OnButtonCopynode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage10 message handlers

BOOL CPropertyWizardPage10::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();

	m_CmbNetworkNodes.ResetContent();

	/**
	 * Here we must initialise combobox with all nodes that have units of
	 * which names are not the same ...
	 */

	int nNodes;	
	if (TMGetRuntimeNodes()->NROfNodes(&nNodes))
	{
		CCanNode *pDocNode,*pRTNode;	
		for (int i=0;i<nNodes;i++) {
			if (TMGetRuntimeNodes()->GetNode(i,&pRTNode)) {
				int j;
				if (TMGetDocumentNodes()->FindNode(pRTNode->bLogicalAddress, &j, &pDocNode)) {
					if (CompareNodeUnitNames(pDocNode,pRTNode) == FALSE) {
						AddNodeToComboBox(pDocNode);						
					}
					
				} // end if (->FindNode
				ASSERT(pDocNode);
			} // end if (->GetNode(i	
			ASSERT(pRTNode);
		}
	}	
	UpdateControls();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPropertyWizardPage10::OnWizardBack() 
{
	CPropertyWizard *pSheet = NULL;		
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) {
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_COPYNAMES));
	}
	return FALSE;

}

LRESULT CPropertyWizardPage10::OnWizardNext() 
{
	CPropertyWizard *pSheet = NULL;	
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) {
		return(pSheet->NextPage(WIZARDPAGE_CHOICE_COPYNAMES));
	}
	return CPropertyWizardPageBase::OnWizardNext();
}

BOOL CPropertyWizardPage10::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class	
	return CPropertyWizardPageBase::OnWizardFinish();
}

BOOL CPropertyWizardPage10::OnSetActive() 
{	
	CString szText;
	CPropertyWizard *pSheet = NULL;	
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) {
		DWORD dwStyle;
		if (pSheet->IsLastPage(WIZARDPAGE_CHOICE_COPYNAMES)) {
			dwStyle = PSWIZB_FINISH | PSWIZB_BACK;
			szText.Format("When done press finish. All names and bindings will be send...");
		}
		else {
			dwStyle = PSWIZB_NEXT | PSWIZB_BACK;
			szText.Format("To proceed press next");
		}
		pSheet->SetWizardButtons(dwStyle);
	}

	CWnd *pWnd = NULL;
	if ((pWnd = GetDlgItem(IDC_STATIC_TEXT2)) != NULL) 
	{
		pWnd->SetWindowText(szText);
	}
	return CPropertyWizardPageBase::OnSetActive();
}

void CPropertyWizardPage10::OnButtonCopyall() 
{			
	int nNodes;	
	if (TMGetRuntimeNodes()->NROfNodes(&nNodes))
	{			
		CCanNode *pDocNode,*pRTNode;
		for (int i=0;i<nNodes;i++) {
			if (TMGetRuntimeNodes()->GetNode(i,&pRTNode)) {
				int j;
				if (TMGetDocumentNodes()->FindNode(pRTNode->bLogicalAddress,&j,&pDocNode)) {
					CopyUnitNames(pDocNode,pRTNode);						
				} // if (->FindNode
				ASSERT(pDocNode);
			} // if (->GetNode
			ASSERT(pRTNode);
		} // for (int i=0;i<nNodes;i++) 
	}// if (>NROfNodes(&nNodes))
}

void CPropertyWizardPage10::OnButtonCopynode() 
{				
	const BYTE bNodeAddress = GetAddressOfCurrSel();	
	if (bNodeAddress != 0xFF) 
	{
		CCanNode *pDocNode,*pRTNode;
		int i;

		if (TMGetDocumentNodes()->FindNode(bNodeAddress,&i,&pDocNode)) {
			if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&i,&pRTNode))  {			
				// BUG-0102: CopyNodeNames( ) bijgevoegd.
				if (CopyNodeNames(pDocNode,pRTNode)) {
					if (CopyUnitNames(pDocNode,pRTNode)) {
						DeleteNodeFromComboBox(pRTNode->bLogicalAddress);
					}
				}				
			} // if (->FindNode
			ASSERT(pRTNode);			
		}//	if (->FindNode(
		ASSERT(pDocNode);
	}
	UpdateControls();	
}


BOOL CPropertyWizardPage10::IsUnitNameDifferent(STRUCT_UNIT *pUnit1,STRUCT_UNIT *pUnit2)
{
	if ((pUnit1 == NULL) || (pUnit2==NULL)) {
		return FALSE;
	}	
	if (strcmp(pUnit1->szUnitName,pUnit2->szUnitName) == 0) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropertyWizardPage10::CopyUnitNames(STRUCT_UNIT *pDest,STRUCT_UNIT *pSrc)
{
	if ((pDest == NULL) || (pSrc==NULL)) {
		return FALSE;
	}
	strcpy(pDest->szUnitName,pSrc->szUnitName);
	return TRUE;
}

BOOL CPropertyWizardPage10::CopyNodeNames(CCanNode *pDest,CCanNode *pSrc)
{
	if ((pDest == NULL) || (pSrc==NULL)) {
		return FALSE;
	}
	strcpy(pDest->szNodeName,pSrc->szNodeName);
	return TRUE;
}


BOOL CPropertyWizardPage10::CompareNodeUnitNames(CCanNode *pDocNode,CCanNode *pRTNode)
{				
	if ((pDocNode != NULL) && (pRTNode != NULL)) 
	{
		STRUCT_UNIT *pRTUnit,*pDocUnit;
		for (int i=0;i<pRTNode->bNROfUnits;i++) {					
			if (TMGetRuntimeNodes()->GetUnit(pRTNode,i,&pRTUnit)) {
				if (TMGetDocumentNodes()->GetUnit(pDocNode,i,&pDocUnit)) {				
					if (IsUnitNameDifferent(pRTUnit,pDocUnit) == TRUE) {
						return FALSE;
					}					
				} // if (GetUnit(pDocNo									
				ASSERT(pDocUnit);				
			} // if (GetUnit(pRTNode
			ASSERT(pRTUnit);
		}// for (int i=0;i<pRTNode->bNROfUnits;i++) 			
	} // if ((pDocNode != NULL) && (pRTNode != NULL)) 	
	return TRUE;
}


void CPropertyWizardPage10::AddNodeToComboBox(CCanNode *pDocNode)
{
	if (pDocNode != NULL) {
		const DWORD dwData = (DWORD) pDocNode->bLogicalAddress;
		CString szData;
		szData.Format(_T("[0x%02x],'%s'"),pDocNode->bLogicalAddress,pDocNode->szNodeName);
		const int index = m_CmbNetworkNodes.AddString(szData);
		if (index != CB_ERR) {
			m_CmbNetworkNodes.SetItemData(index,dwData);
		}
	}
}

BYTE CPropertyWizardPage10::GetAddressOfCurrSel(void)
{
	const int index = m_CmbNetworkNodes.GetCurSel();
	if (index != CB_ERR) {
		return ((BYTE) m_CmbNetworkNodes.GetItemData(index));		
	}	
	return 0xFF;
}


BOOL CPropertyWizardPage10::CopyUnitNames(CCanNode *pDocNode,CCanNode *pRTNode)
{		
	if ((pDocNode != NULL) && (pRTNode!=NULL)) {
		STRUCT_UNIT *pRTUnit,*pDocUnit;
		for (int i=0;i<pRTNode->bNROfUnits;i++) {
			if (TMGetRuntimeNodes()->GetUnit(pRTNode, i, &pRTUnit)) {
				if (TMGetDocumentNodes()->GetUnit(pDocNode, i, &pDocUnit)) {					
					if (CopyUnitNames(pDocUnit,pRTUnit) == FALSE) {
						return FALSE;
					}
				} // if (->GetUnit
				ASSERT(pDocUnit);
			}// if (->GetUnit(pRTNode
			ASSERT(pRTUnit);
		}//	for (int i=0;i<pRTNode->bNROfUnits;i++)			
	} // if ((pDocNode != NULL) && (pRTNode!=NULL))	
	return TRUE;
}

void CPropertyWizardPage10::DeleteNodeFromComboBox(BYTE bNodeAddress)
{	
	for (int i=0;i<m_CmbNetworkNodes.GetCount();i++) {
		const DWORD dwData = m_CmbNetworkNodes.GetItemData(i);
		if ((BYTE) (dwData & 0xFF) == bNodeAddress) {
			m_CmbNetworkNodes.DeleteString(i);
		}
	}
}

void CPropertyWizardPage10::UpdateControls(void) {
	m_CmbNetworkNodes.SetCurSel(0);
}


#endif

