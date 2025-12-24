// PropertyWizardPage9.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage9.h"

#include "PropertyWizard.h"
#include "MainFrm.h"
#include "app/AppNodeDbase/AppNodeDbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage9 property page
// CPropertyWizardPage9 --> Send configuration changes to the CAN-Bus.

IMPLEMENT_DYNCREATE(CPropertyWizardPage9, CPropertyPage)

CPropertyWizardPage9::CPropertyWizardPage9() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage9::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE9)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage9)
	//}}AFX_DATA_INIT
}

CPropertyWizardPage9::~CPropertyWizardPage9()
{
}

void CPropertyWizardPage9::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage9)
	DDX_Control(pDX, IDC_CHECK_SENDALLNAMES, m_CheckNames);
	DDX_Control(pDX, IDC_CHECK_SENDALLBINDINGS, m_CheckBindings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage9, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	//{{AFX_MSG_MAP(CPropertyWizardPage9)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage9 message handlers

BOOL CPropertyWizardPage9::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();

	CPropertyWizard *pSheet = NULL;
	WizardTasks_t Task = WIZARD_TASK_UNKNOWN;
	
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		Task = pSheet->GetTask();
	}

	if (Task == WIZARD_CONTINUE_PROGRAMFILEDIFFERENCES)
	{		
		CString strDateDatabase;
		CString strNetwork;
		CString szTextTop;
		CString szTextBottom;

		const CAppNodeDatabase::ProgramFileDiffResult_t diff = CAppNodeDatabase::GetInstance()->
			findTypeProgramFileDifference(strDateDatabase, strNetwork);
		
		if (CAppNodeDatabase::RESULT_DATABASE_RECENT == diff) {
			szTextTop.Format("The PC configuration file holds the latest changes.\n"
							 "Press finish to send this configuration to the CAN-bus.");
			szTextBottom.Format("When done press finish. All names and bindings will be send.");
		}
		else if (CAppNodeDatabase::RESULT_NETWORK_RECENT == diff) {
			szTextTop.Format("The PC configuration file doesn't hold the latest changes.\n"
						  "Check if you are using the latest configuration file!\n\n"
						  "PC configuration file: %s.\nCAN-bus: %s\n\n",
						  strDateDatabase, strNetwork);
			
			szTextBottom.Format("If you continue and press the finish button the PC configuration file will be send to the CAN-bus. "
								"Possible previous configuration changes will be lost.");
		}
		else {
			szTextTop = "Press finish to send all configuration to the CAN-bus.";
			
			szTextBottom.Format("When done press finish. All names and bindings will be send.");
		}

		{
			CWnd* pWnd = GetDlgItem(IDC_STATIC_TEXT_TOP);	
			if (NULL != pWnd) {
				pWnd->SetWindowText(szTextTop);
			}
		}
		
		{
			CWnd *pWnd = NULL;
			if ((pWnd = GetDlgItem(IDC_STATIC_TEXT_BOTTOM)) != NULL) {
				pWnd->SetWindowText(szTextBottom);
			}
		}

		m_CheckNames.SetCheck(1);
		m_CheckBindings.SetCheck(1);
		m_CheckNames.EnableWindow(FALSE);
		m_CheckBindings.EnableWindow(FALSE);	
	}
	else 
	{
		m_CheckNames.SetCheck(1);
		m_CheckBindings.SetCheck(0);
		m_CheckNames.EnableWindow(TRUE);
		m_CheckBindings.EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPropertyWizardPage9::OnSetActive() 
{	
	CPropertyWizard *pSheet = NULL;		
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		DWORD dwStyle;
		if (pSheet->IsLastPage(WIZARDPAGE_CHOICE_SENDPROGRAMFILE)) {
			dwStyle = PSWIZB_FINISH | PSWIZB_BACK;			
		}
		else  {
			dwStyle = PSWIZB_NEXT | PSWIZB_BACK;

			CString szText;
			// szText.Format("To proceed press next");
			szText.Format("When finished press continue for the next step.");
			CWnd *pWnd = NULL;
			if ((pWnd = GetDlgItem(IDC_STATIC_TEXT_BOTTOM)) != NULL) {
				pWnd->SetWindowText(szText);
			}
		}		
		pSheet->SetWizardButtons(dwStyle);
	}
	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage9::OnWizardBack() 
{
	CPropertyWizard *pSheet = NULL;
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) {
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_SENDPROGRAMFILE));
	}
	return FALSE;
}

BOOL CPropertyWizardPage9::OnWizardFinish() 
{
	CPropertyWizard *pSheet = NULL;
	
	if ((pSheet = (CPropertyWizard*) GetOwner()) != NULL) 
	{
		BOOL fSendNames = FALSE;
		BOOL fSendBindings = FALSE;

		// Here we must set flags ifv Check Boxes !!!!
		if (m_CheckNames.GetCheck() == 1) {
			fSendNames = TRUE;
		}		
		if (m_CheckBindings.GetCheck() == 1) {
			fSendBindings = TRUE;
		}
		pSheet->SetSendNames(fSendNames);
		pSheet->SetSendBindings(fSendBindings);
	}	
	return CPropertyWizardPageBase::OnWizardFinish();
}


