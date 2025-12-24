// PropertyPageDaliErrorsOverview.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliErrorsOverview.h"


// CPropertyPageDaliErrorsOverview dialog

IMPLEMENT_DYNAMIC(CPropertyPageDaliErrorsOverview, CPropertyPage)

CPropertyPageDaliErrorsOverview::CPropertyPageDaliErrorsOverview(CAppDlgDaliWorker& refWorker,CPropertyPageDaliErrorsContext& refContext)
	: CPropertyPageDaliErrorsBaseClass(CPropertyPageDaliErrorsOverview::IDD,refWorker,refContext)
{

}

CPropertyPageDaliErrorsOverview::~CPropertyPageDaliErrorsOverview()
{
}

void CPropertyPageDaliErrorsOverview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfoHeader);
	DDX_Control(pDX, IDC_STATIC_FOOTER, m_staticInfoFooter);
	DDX_Control(pDX, IDC_STATIC_ICON, m_staticIcon);
}

BEGIN_MESSAGE_MAP(CPropertyPageDaliErrorsOverview, CPropertyPage)
END_MESSAGE_MAP()


// CPropertyPageDaliErrorsOverview message handlers
BOOL CPropertyPageDaliErrorsOverview::OnInitDialog() {
	CPropertyPage::OnInitDialog();	

	if (!this->isInitDone()) {		
		this->initStaticTextHeader();
		this->initStaticIcon();
		this->initStaticTextFooter();
		this->setInitDone(true);
	}
	return TRUE;
}


void CPropertyPageDaliErrorsOverview::initStaticTextHeader( ) {

	CString strText = _T("Issues Detected in the DALI Bus System\r\n\r\n\r\n"
					     "Inspect and Troubleshoot:\r\n"
					     "Check the DALI bus system for issues and address any problems you find.\r\n"
						 "First, check the physical connections in the DALI bus system.\r\n"
						 "You can change or remove device addresses in the software if needed.\r\n"
						 "Only do this if necessary, as it may affect other settings.\r\n\r\n"						 						
					     "Verify Repairs:\r\n"
				         "Once you believe the issues are resolved, perform a new 'Search devices' to confirm that all problems have been eliminated.\r\n"
						 "The 'Search Devices' option can be found under the 'Network' tab.\r\n\r\n"
                         "Confirm Completion:\r\n"
                         "Make sure all issues are fully resolved before making any changes to the DALI Duotecno Bridge configuration.\r\n\r\n\r\n"						 
						 "For clarity: DALI devices with errors are highlighted in red in the 'Devices' tab.");

	this->m_staticInfoHeader.SetWindowText(strText);
}


void CPropertyPageDaliErrorsOverview::initStaticTextFooter( ) {

	CString strText("See the next page 'details' for more info...");

	this->m_staticInfoFooter.SetWindowText(strText);

}

void CPropertyPageDaliErrorsOverview::initStaticIcon( ) {
	HICON hIconLarge = (HICON)LoadImage(
        NULL,
        MAKEINTRESOURCE(IDI_WARNING),
        IMAGE_ICON,
        48, 48,
        LR_SHARED
    );

	if (NULL != hIconLarge) {
		this->m_staticIcon.SetIcon(hIconLarge);
	}
}