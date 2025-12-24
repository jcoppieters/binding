// PropertyWizardPage8.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage8.h"

#include "PropertyWizard.h"
#include "MainFrm.h"

#include "NodeDatabaseVerify_Handlers.h"
#include "NodeDatabaseVerify.h"

// -----------------------------------
#include "TMGlobals.h"
#include "TMConvenience.h"			// since 0x0562
using namespace TMNodeDatabase;
using namespace TMConvenience;		// since 0x0562
// ------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage8 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage8, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyWizardPage8::CPropertyWizardPage8() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage8::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE8) 
{
	//{{AFX_DATA_INIT(CPropertyWizardPage8)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyWizardPage8::~CPropertyWizardPage8()
{
}

void CPropertyWizardPage8::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage8)
	DDX_Control(pDX, IDC_BUTTON_UPGRADE, m_btnUpgrade);
	DDX_Control(pDX, IDC_EDIT_PHYSICAL_RT, m_EditPhysicalRT);
	DDX_Control(pDX, IDC_EDIT_PHYSICAL_DOC, m_EditPhysicalDoc);
	DDX_Control(pDX, IDC_EDIT_NUNITS_RT, m_EditUnitsRT);
	DDX_Control(pDX, IDC_EDIT_NUNITS_DOC, m_EditUnitsDoc);
	DDX_Control(pDX, IDC_EDIT_NAME_RT, m_EditNameRt);
	DDX_Control(pDX, IDC_EDIT_NAME_DOC, m_EditNameDoc);
	DDX_Control(pDX, IDC_COMBO_LOGICAL_ADDRESS, m_CmbLogical);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage8, CPropertyWizardPageBase)
	//{{AFX_MSG_MAP(CPropertyWizardPage8)
	ON_CBN_SELCHANGE(IDC_COMBO_LOGICAL_ADDRESS,OnCbnSelchangeComboLogicalAddress)
	ON_CBN_CLOSEUP(IDC_COMBO_LOGICAL_ADDRESS, OnCloseupComboLogicalAddress)
	ON_BN_CLICKED(IDC_BUTTON_UPGRADE, OnButtonUpgrade)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage8 message handlers


BOOL CPropertyWizardPage8::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();

	m_btnUpgrade.EnableWindow(FALSE);	// Since v0562
	m_CmbLogical.ResetContent();	

	CNodeDatabaseVerify_Handler0 Handler(*this);
	CNodeDatabaseVerify Algoritme(&Handler);
	const int nCount = Algoritme.Check_Incompatible_Nodes( );
	if (nCount > 0) {
		m_CmbLogical.SetCurSel(0);	
	}

	PostMessage(WM_UPDATE_CONTROLS);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyWizardPage8::AddNodeToComboBox(int index,const CCanNode& Node)
{
	CString szMessage;	
	szMessage.Format(_T("0x%02x"),Node.bLogicalAddress);															 
	m_CmbLogical.InsertString(index,szMessage);						
	m_CmbLogical.SetItemData(index,Node.bLogicalAddress);
}

LRESULT CPropertyWizardPage8::OnWizardBack() 
{
	CPropertyWizard *pSheet = NULL;
	
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_NODESINCOMPATIBLE));
	}
	return FALSE;
}

BOOL CPropertyWizardPage8::OnSetActive() 
{
	PostMessage(WM_UPDATE_CONTROLS);
	return CPropertyWizardPageBase::OnSetActive();
}

void CPropertyWizardPage8::OnCloseupComboLogicalAddress() 
{
	//UpdateControls();
}

void CPropertyWizardPage8::OnCbnSelchangeComboLogicalAddress()
{
	PostMessage(WM_UPDATE_CONTROLS);
}


void CPropertyWizardPage8::ShowDocNodeInfo(const CCanNode* pNode)
{
	CString szName("");
	CString szUnits("");
	CString szPhysicalAddress("");

	if (pNode)
	{		
		szName.Format(_T("%s"),pNode->szNodeName);
		szUnits.Format(_T("%d"),pNode->bNROfUnits);
		szPhysicalAddress.Format(_T("0x%06x"),pNode->ulPhysicalAddress);	
	}
	
	m_EditNameDoc.SetWindowText( szName );
	m_EditUnitsDoc.SetWindowText( szUnits );
	m_EditPhysicalDoc.SetWindowText( szPhysicalAddress );
}
void CPropertyWizardPage8::ShowRTNodeInfo(const CCanNode* pNode)
{
	CString szName("");
	CString szUnits("");
	CString szPhysicalAddress("");

	if (pNode)
	{
		szName.Format(_T("%s"),pNode->szNodeName);
		szUnits.Format(_T("%d"),pNode->bNROfUnits);
		szPhysicalAddress.Format(_T("0x%06x"),pNode->ulPhysicalAddress);
	}
	m_EditNameRt.SetWindowText( szName );
	m_EditUnitsRT.SetWindowText( szUnits );
	m_EditPhysicalRT.SetWindowText( szPhysicalAddress );	
}

BYTE CPropertyWizardPage8::GetNodeAddress()
{	
	const int iSel = m_CmbLogical.GetCurSel();
	if (iSel != CB_ERR) 
	{
		return ((BYTE) m_CmbLogical.GetItemData(iSel));
	}
	return 0;
}

void CPropertyWizardPage8::OnButtonUpgrade() 
{
	const BYTE bNodeAddress = GetNodeAddress();
	if (bNodeAddress != 0) {	
		CString szConfirm(_T("Making the devices compatible can possible remove previous configuration.\n"
							 "Are you sure you want to make the devices compatible?"));

		if (IDYES == AfxMessageBox(szConfirm,MB_ICONQUESTION | MB_YESNO)) 
		{	
			TMUpgradeIncompatibleNodes( bNodeAddress );

			m_CmbLogical.ResetContent();	

			CNodeDatabaseVerify_Handler0 Handler(*this);
			CNodeDatabaseVerify Algoritme(&Handler);
			const int nCount = Algoritme.Check_Incompatible_Nodes( );
			if (nCount > 0)
			{
				m_CmbLogical.SetCurSel(0);			
			}
		}
	}
	else {
		m_CmbLogical.ResetContent();		
	}
	PostMessage(WM_UPDATE_CONTROLS);
}


// Sedert V16.16:
// Zolang er nog conflicten zijn de finish button niet enablen.
LONG CPropertyWizardPage8::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

	BOOL visibleBtnMakeCompatible = FALSE;
	BOOL foundNodes = FALSE;

	{
		CCanNode *pDocNode = 0;
		CCanNode *pRTNode = 0;
		const BYTE bNodeAddress = GetNodeAddress();
		if (bNodeAddress != 0) 
		{	
			int j;		
			if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&j,&pRTNode) && 
				TMGetDocumentNodes()->FindNode(bNodeAddress,&j,&pDocNode)) 
			{					
				foundNodes = TRUE;
				visibleBtnMakeCompatible = TMGetNodeDatabaseApp()->possibleToUpgradeIncompatibleUnits(bNodeAddress);			
			}				
		}		
		m_btnUpgrade.EnableWindow( visibleBtnMakeCompatible );		
		ShowDocNodeInfo(pDocNode);		// Mag ook wanneer de pDocNode == NULL
		ShowRTNodeInfo(pRTNode);		// Mag ook wanneer de pRTNode == NULL
	}

	{
		CWnd* const p = this->GetDlgItem(IDC_STATIC_DESCRIPTION);
		if (NULL != p) {
			if (foundNodes) {
				CString str1;
				CString str0("Incompatible devices have been found on the CAN-bus.\n");
				if (visibleBtnMakeCompatible) {
					str1 = "Press the 'Make compatible' button to make the devices compatible.";
				}
				else {
					str1 = "Devices are incompatible when they are from another product type or " 
						   "when the CAN-bus device holds more units then the PC database device.";
				}
				CString szInfo = str0 + str1;
				p->SetWindowText(szInfo);
			}
			else {
				p->SetWindowText("No more incompatible devices found.");
			}
		}
	}		

	{
		CString szText;
		CPropertyWizard *pSheet = NULL;

		if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) {		
			DWORD dwStyle;
			if (pSheet->IsLastPage(WIZARDPAGE_CHOICE_INIT)) 
			{
				if ((m_CmbLogical.GetCount() > 0) && (TRUE == foundNodes)) {
					if (!visibleBtnMakeCompatible) {					
						szText.Format("Press back to find another solution.");
					}
					dwStyle = PSWIZB_DISABLEDFINISH | PSWIZB_BACK;						
				}
				else {
					dwStyle = PSWIZB_FINISH | PSWIZB_BACK;
					szText.Format("When done press finish. The browse algoritme will start again...");
				}
			}
			else  {
				dwStyle = PSWIZB_NEXT | PSWIZB_BACK;
				szText.Format("When finished press continue for the next step.");
				//szText.Format("To proceed press next");
			}		
			pSheet->SetWizardButtons(dwStyle);
		}

		CWnd *pWnd = NULL;
		if ((pWnd = GetDlgItem(IDC_STATIC_TEXT2)) != NULL) {
			pWnd->SetWindowText(szText);
		}
	}

	return TRUE;
}