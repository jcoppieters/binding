#pragma once
#include "afxwin.h"

#include "PropertyWizardPageBase.h"

// CPropertyWizardPage11 dialog

class CPropertyWizardPage11 : public CPropertyWizardPageBase
{
	DECLARE_DYNAMIC(CPropertyWizardPage11)

public:
	CPropertyWizardPage11();
	virtual ~CPropertyWizardPage11();

	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	afx_msg void OnBnClickedButtonSubmit();

// Dialog Data
	enum { IDD = IDD_DIALOG_WIZARDPAGE11 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	virtual BOOL OnInitDialog();
	void UpdateControls(BOOL fCodeOk);

	DECLARE_MESSAGE_MAP()

private:
	CEdit m_editCode;
	CStatic m_staticInfo;
};
