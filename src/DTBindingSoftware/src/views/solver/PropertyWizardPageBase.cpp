#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPageBase.h"
#include "uicontrols/utils/dlghelper.h"				// GUI fonts
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO;
/////////////////////////////////////////////////////////////////////////////
CPropertyWizardPageBase::CPropertyWizardPageBase(UINT nIDTemplate, UINT nIDCaption, 
												 UINT nIDHeaderTitle, UINT nIDHeaderSubTitle) : 
	CPropertyPage(nIDTemplate, nIDCaption, nIDHeaderTitle), m_pDlgFontHelper(NULL) { }

CPropertyWizardPageBase::~CPropertyWizardPageBase() {
	if (NULL != m_pDlgFontHelper) {
		delete m_pDlgFontHelper;
		m_pDlgFontHelper = NULL;
	}
}

BOOL CPropertyWizardPageBase::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if (NULL == m_pDlgFontHelper) {
		m_pDlgFontHelper = new CDlgHelperFont(*this);	
		ASSERT(NULL != m_pDlgFontHelper);
		if (NULL != m_pDlgFontHelper) {
			m_pDlgFontHelper->initDialog();	
		}
	}
	return TRUE;
}

void CPropertyWizardPageBase::OnNotifyAssignedLogicalAddress(int nodeAddress) {
	// Absorbe: implementeren in de afgeleide klasssen...
}

/////////////////////////////////////////////////////////////////////////////
