// PropertyWizardPage1.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage1.h"
#include "PropertyWizard.h"
#include "app/appnodedbase/appnodedbase.h"	// sedert V16.16

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage1 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage1, CPropertyPage)

CPropertyWizardPage1::CPropertyWizardPage1() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage1::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE1)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage1)
	m_RadioGroup = -1;
	//}}AFX_DATA_INIT
}

CPropertyWizardPage1::~CPropertyWizardPage1()
{
}

void CPropertyWizardPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage1)
	DDX_Radio(pDX, IDC_CHECK_RADIO1, m_RadioGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage1, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	//{{AFX_MSG_MAP(CPropertyWizardPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage1 message handlers

BOOL CPropertyWizardPage1::OnSetActive() 
{
	CPropertySheet* const pSheet = (CPropertySheet*) GetOwner();

	if (pSheet != NULL) 	
		pSheet->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK);

	return (CPropertyWizardPageBase::OnSetActive());
}

BOOL CPropertyWizardPage1::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();	
	CString szText;
	CPropertyWizard *pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet != NULL) 
	{	
		const WizardTasks_t Task = pSheet->GetTask();		
		switch (Task) {
			case WIZARD_NEW_FACTORYADDRESS : 
			{
				szText.LoadString(IDS_WIZARD_PROBLEM_FACTORYADDR);
				m_RadioGroup = ENUM_RADIOBUTTON_PNP;	
			} break;
			case WIZARD_CONTINUE_FACTORYADDRESS : {			
				// Sedert V16.16:
				// indien we addressen kunnen herstellen door toe te kennen via de pc database dan deze 
				// als eerste optie zetten. 
				// Anders de pnp...				
				szText.LoadString(IDS_WIZARD_PROBLEM_FACTORYADDR);				
				if (CAppNodeDatabase::GetInstance()->possibleToResolveAddressFromDbase()) {
					m_RadioGroup = ENUM_RADIOBUTTON_PROGFILE;
				}
				else {
					m_RadioGroup = ENUM_RADIOBUTTON_PNP;			
				}
			} break;
			case WIZARD_CONTINUE_DUPLICATESFOUND :
			case WIZARD_NEW_DUPLICATESFOUND : {			
				szText.LoadString(IDS_WIZARD_PROBLEM_ADDRESSES);				
				m_RadioGroup = ENUM_RADIOBUTTON_INIT;
			} break;			
			case WIZARD_CONTINUE_FACTORYADDRESS_ASSIGN_MASTER:
			case WIZARD_NEW_FACTORYADDRESS_ASSIGN_MASTER: {
				szText.LoadString(IDS_WIZARD_PROBLEM_FACTORYADDR_MASTER);				
				m_RadioGroup = ENUM_RADIOBUTTON_ASSIGNMASTER;			
			} break;
			case WIZARD_CONTINUE_NODESADDED: {			
				szText.LoadString(IDS_WIZARD_PROBLEM_DEVICES_ADD);
				m_RadioGroup = ENUM_RADIOBUTTON_NODESADDED;
			} break;
			case WIZARD_CONTINUE_NODESREMOVED: {				
				szText.LoadString(IDS_WIZARD_PROBLEM_DEVICES_REMOVED);				
				m_RadioGroup = ENUM_RADIOBUTTON_NODESADDED;
			} break;
			case WIZARD_CONTINUE_NODESINCOMPATIBLE:	 {					
				szText.LoadString(IDS_WIZARD_PROBLEM_DEVICES_INCOMPATIBLE);
				m_RadioGroup = ENUM_RADIOBUTTON_NODESINCOMPATIBLE;
			}break;
			case WIZARD_USER_SELECTION: {
				szText.Format("User Selection - Engineer mode.\n");	
				m_RadioGroup = ENUM_RADIOBUTTON_MANUALASSIGN;
			} break;
			case WIZARD_CONTINUE_PROGRAMFILEDIFFERENCES: {			
				szText.LoadString(IDS_WIZARD_PROBLEM_CONFIGFILE);
				m_RadioGroup = ENUM_RADIOBUTTON_SENDPROGRAMFILE;
			} break;
			case WIZARD_CONTINUE_NAMEDIFFERENCES: {			
				szText.LoadString(IDS_WIZARD_PROBLEM_NAMES);
				m_RadioGroup = ENUM_RADIOBUTTON_COPYNAMES;
			} break;
			case WIZARD_LICENCE_ACTIVATION_CODE: {
				szText.Format("To continue an access code is needed\n");
				m_RadioGroup = ENUM_RADIOBUTTON_ACCESSCODE;
			} break;
		}	
	}

	EnableControls(ENUM_RADIOBUTTON_PROGFILE,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_PROGRAMFILE));
	EnableControls(ENUM_RADIOBUTTON_MANUALASSIGN,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_MANUALASSIGN));
	EnableControls(ENUM_RADIOBUTTON_ASSIGNMASTER,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_ASSIGNMASTER));
	EnableControls(ENUM_RADIOBUTTON_PNP,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_PNP));
	EnableControls(ENUM_RADIOBUTTON_REPLACE,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_REPLACE));
	EnableControls(ENUM_RADIOBUTTON_INIT,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_INIT));
	EnableControls(ENUM_RADIOBUTTON_NODESADDED,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_NODESADDED));
	EnableControls(ENUM_RADIOBUTTON_NODESINCOMPATIBLE,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_NODESINCOMPATIBLE));
	EnableControls(ENUM_RADIOBUTTON_SENDPROGRAMFILE,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_SENDPROGRAMFILE));
	EnableControls(ENUM_RADIOBUTTON_COPYNAMES,pSheet->IsActiveChain(WIZARDPAGE_CHOICE_COPYNAMES));
	EnableControls(ENUM_RADIOBUTTON_ACCESSCODE,pSheet->IsActiveChain(WIZARDPAGE_ENTER_ACCESSCODE));


	this->UpdateData(FALSE);

	if (!szText.IsEmpty()) {
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_TEXT);
		if (pWnd != NULL) 
			pWnd->SetWindowText(szText);		
	}

	{
		const CString strTextBottom(_T("Press next to continue."));
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_TEXT2);
		if (pWnd != NULL) 
			pWnd->SetWindowText(strTextBottom);
	}

	this->setTextRadioButton(ENUM_RADIOBUTTON_PNP,IDS_WIZARD_OPTION2);							// IDS_WIZARD_OPTION2      "Automatically add new devices via plug and play."
	this->setTextRadioButton(ENUM_RADIOBUTTON_REPLACE,IDS_WIZARD_OPTION3);						// IDS_WIZARD_OPTION3      "Replace devices."
	this->setTextRadioButton(ENUM_RADIOBUTTON_PROGFILE,IDS_WIZARD_OPTION0);						// IDS_WIZARD_OPTION0      "Automatically restore addresses as in the PC database."
	this->setTextRadioButton(ENUM_RADIOBUTTON_MANUALASSIGN,IDS_WIZARD_OPTION1);					// IDS_WIZARD_OPTION1      "Manually assign addresses to the devices."
	this->setTextRadioButton(ENUM_RADIOBUTTON_ASSIGNMASTER,IDS_WIZARD_OPTION_ASSIGN_MASTER);	// IDS_WIZARD_OPTION_ASSIGN_MASTER      "Assign CAN-bus master"
	this->setTextRadioButton(ENUM_RADIOBUTTON_INIT,IDS_WIZARD_OPTION4);							// IDS_WIZARD_OPTION4      "Factory initialise devices."
	this->setTextRadioButton(ENUM_RADIOBUTTON_NODESADDED,IDS_WIZARD_OPTION5);					// IDS_WIZARD_OPTION5      "Show added or removed devices."
	this->setTextRadioButton(ENUM_RADIOBUTTON_NODESINCOMPATIBLE,IDS_WIZARD_OPTION6);			// IDS_WIZARD_OPTION6      "Show incompatible devices."
	this->setTextRadioButton(ENUM_RADIOBUTTON_SENDPROGRAMFILE,IDS_WIZARD_OPTION7);				// IDS_WIZARD_OPTION7      "Send configuration file to the CAN-bus."
	this->setTextRadioButton(ENUM_RADIOBUTTON_COPYNAMES,IDS_WIZARD_OPTION8);					// IDS_WIZARD_OPTION8      "Resolve name differences."
	this->setTextRadioButton(ENUM_RADIOBUTTON_ACCESSCODE,IDS_WIZARD_OPTION9);					// IDS_WIZARD_OPTION9      "Enter license activation code to continue."
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CPropertyWizardPage1::setTextRadioButton(int buttonIdx,UINT stringResourceID) {
	enum { MAX_RADIO_BUTTONS = 11 };
	static const UINT arrayRadioButtons[MAX_RADIO_BUTTONS] = {
		IDC_CHECK_RADIO1,	// 0
		IDC_RADIO2,			// 1
		IDC_RADIO3,			// 2
		IDC_RADIO4,			// 3
		IDC_RADIO5,			// 4
		IDC_RADIO6,			// 5
		IDC_RADIO7,			// 6
		IDC_RADIO8,			// 7
		IDC_RADIO9,			// 8
		IDC_RADIO10,		// 9
		IDC_RADIO11			// 10
	};
	if (buttonIdx < 0 || buttonIdx>=MAX_RADIO_BUTTONS) {
		ASSERT(0);
		return;
	}
	CWnd* const pWnd = this->GetDlgItem(arrayRadioButtons[buttonIdx]);
	if (NULL == pWnd) {
		ASSERT(NULL != pWnd);
		return;
	}
	CString strText;
	strText.LoadString(stringResourceID);				
	ASSERT(!strText.IsEmpty());	
	pWnd->SetWindowText(strText);
}

LRESULT CPropertyWizardPage1::OnWizardNext() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet != NULL) 
	{	
		UpdateData(TRUE);

		WizardPages_t Page;
		switch (m_RadioGroup) {
			case ENUM_RADIOBUTTON_PNP:			
				Page = WIZARDPAGE_CHOICE_PNP;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
			case ENUM_RADIOBUTTON_REPLACE:			
				Page = WIZARDPAGE_CHOICE_REPLACE;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
			case ENUM_RADIOBUTTON_PROGFILE:			
				Page = WIZARDPAGE_CHOICE_PROGRAMFILE;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;

			case ENUM_RADIOBUTTON_MANUALASSIGN:			
				Page = WIZARDPAGE_CHOICE_MANUALASSIGN;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;	
			case ENUM_RADIOBUTTON_ASSIGNMASTER:
				Page = WIZARDPAGE_CHOICE_ASSIGNMASTER;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
			case ENUM_RADIOBUTTON_INIT:			
				Page = WIZARDPAGE_CHOICE_INIT;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
			case ENUM_RADIOBUTTON_NODESADDED:			
				Page = WIZARDPAGE_CHOICE_NODESADDED;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
			case ENUM_RADIOBUTTON_NODESINCOMPATIBLE:
				Page = WIZARDPAGE_CHOICE_NODESINCOMPATIBLE;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
			case ENUM_RADIOBUTTON_SENDPROGRAMFILE:
				Page = WIZARDPAGE_CHOICE_SENDPROGRAMFILE;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
			case ENUM_RADIOBUTTON_COPYNAMES:
				Page = WIZARDPAGE_CHOICE_COPYNAMES;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
			case ENUM_RADIOBUTTON_ACCESSCODE:
				Page = WIZARDPAGE_ENTER_ACCESSCODE;
				pSheet->SetActiveChain(Page);
				pSheet->SetActivePage(Page);
			return TRUE;
		}
	}
	return TRUE;	
}

void CPropertyWizardPage1::EnableControls(ENUM_RADIOBUTTONS button,BOOL fState)
{
	unsigned long ulControlID = 0;
	switch (button)
	{
		case ENUM_RADIOBUTTON_PNP:
			ulControlID=IDC_CHECK_RADIO1;
		break;
		case ENUM_RADIOBUTTON_REPLACE:
			ulControlID=IDC_RADIO2;
		break;
		case ENUM_RADIOBUTTON_PROGFILE :
			ulControlID=IDC_RADIO3;
		break;
		case ENUM_RADIOBUTTON_MANUALASSIGN :
			ulControlID=IDC_RADIO4;
		break;
		case ENUM_RADIOBUTTON_ASSIGNMASTER:
			ulControlID=IDC_RADIO5;
		break;
		case ENUM_RADIOBUTTON_INIT:
			ulControlID=IDC_RADIO6;
		break;
		case ENUM_RADIOBUTTON_NODESADDED :
			ulControlID=IDC_RADIO7;
		break;
		case ENUM_RADIOBUTTON_NODESINCOMPATIBLE :
			ulControlID=IDC_RADIO8;
		break;
		case ENUM_RADIOBUTTON_SENDPROGRAMFILE :
			ulControlID=IDC_RADIO9;
		break;
		case ENUM_RADIOBUTTON_COPYNAMES :
			ulControlID=IDC_RADIO10;
		break;
		case ENUM_RADIOBUTTON_ACCESSCODE :
			ulControlID=IDC_RADIO11;
		break;
	}
	CWnd *pWnd = GetDlgItem(ulControlID);
	if (pWnd != NULL) {
		pWnd->EnableWindow(fState);
	}
}
