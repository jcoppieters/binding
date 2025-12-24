#pragma once

#include "uicontrols/controls/MyEditCtrl.h"

namespace DUOTECNO {
	class CDlgHelperFont;
};

class CPropertyWizardPageBase : public CPropertyPage	
{
public:
	CPropertyWizardPageBase(UINT nIDTemplate, UINT nIDCaption, 
		UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0);
	~CPropertyWizardPageBase();

	virtual BOOL OnInitDialog();
	virtual void OnNotifyAssignedLogicalAddress(int nodeAddress);

private:
	DUOTECNO::CDlgHelperFont* m_pDlgFontHelper;
};