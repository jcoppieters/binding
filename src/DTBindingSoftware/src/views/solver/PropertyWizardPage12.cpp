// PropertyWizardPage12.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage12.h"
#include "PropertyWizard.h"
#include "app/appnodedbase/appnodedbase.h"

/*
// -----------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ------------------------------------
*/

// CPropertyWizardPage12 dialog

IMPLEMENT_DYNAMIC(CPropertyWizardPage12, CPropertyPage)

CPropertyWizardPage12::CPropertyWizardPage12()
	: CPropertyWizardPageBase((UINT) CPropertyWizardPage12::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE12),
	  m_state(STATE_OPTION_SEND), m_selectionOption(0)
{

}

CPropertyWizardPage12::~CPropertyWizardPage12()
{
}

void CPropertyWizardPage12::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_SEND, m_selectionOption);
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage12, CPropertyWizardPageBase)
	ON_BN_CLICKED(IDC_RADIO_SEND, &CPropertyWizardPage12::OnBnClickedRadioSend)
	ON_BN_CLICKED(IDC_RADIO_COPY, &CPropertyWizardPage12::OnBnClickedRadioCopy)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
END_MESSAGE_MAP()


// CPropertyWizardPage12 message handlers
LONG CPropertyWizardPage12::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

	const CString szTextTop("Name differences have been found.\n\n"
						    "Select option to resolve this problem.");
	CString szTextBottom;

	switch(m_state) {
		case STATE_OPTION_SEND: {
			szTextBottom = "Press finish to send all names to the CAN-bus.";				
		} break;
		case STATE_OPTION_COPY: {
			szTextBottom = "Press finish to copy all names to the pc database.";					
		} break;
	}

	{
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_TOP);
		if (NULL != pWnd) {
			pWnd->SetWindowText(szTextTop);
		}
	}
	{
		CWnd* const pWnd = GetDlgItem(IDC_STATIC_BOTTOM);
		if (NULL != pWnd) {
			pWnd->SetWindowText(szTextBottom);
		}
	}

	m_selectionOption = (int) m_state;
	UpdateData(TRUE);		// Zorgt ervoor dat de radio button correct staat...
	return TRUE;
}

BOOL CPropertyWizardPage12::OnInitDialog() {
	CPropertyWizardPageBase::OnInitDialog();
	PostMessage(WM_UPDATE_CONTROLS);
	return TRUE;	
}

BOOL CPropertyWizardPage12::OnSetActive() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();	
	if (pSheet == NULL) {
		return TRUE;
	}
	const DWORD dwStyle = PSWIZB_FINISH | PSWIZB_BACK;
	pSheet->SetWizardButtons(dwStyle);

	PostMessage(WM_UPDATE_CONTROLS);
	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage12::OnWizardBack() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet != NULL) {
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_COPYNAMES));
	}
	return FALSE;
}

BOOL CPropertyWizardPage12::OnWizardFinish() 
{
	CPropertyWizard *pSheet = NULL;	
	if ((pSheet = (CPropertyWizard*) GetOwner()) == NULL) {	
		return FALSE;
	}
	switch(m_state) {
		case STATE_OPTION_SEND: {
			pSheet->SetSendNames(TRUE);								
		} break;
		case STATE_OPTION_COPY: {
			// Sedert V16.16: Geen browse meer uitvoeren.
			pSheet->SetSendNames(FALSE);	
			pSheet->SetBrowseAgain(FALSE);
			CAppNodeDatabase::GetInstance()->copyNamesFromNetwork();
		} break;
	}
	return CPropertyWizardPageBase::OnWizardFinish();
}
void CPropertyWizardPage12::OnBnClickedRadioSend()
{
	m_state = STATE_OPTION_SEND;
	PostMessage(WM_UPDATE_CONTROLS);
}

void CPropertyWizardPage12::OnBnClickedRadioCopy()
{
	m_state = STATE_OPTION_COPY;
	PostMessage(WM_UPDATE_CONTROLS);
}

