#pragma once

#include "PropertyWizardPageBase.h"

// CPropertyWizardPage12 dialog
// Resolve name conflicts.

class CPropertyWizardPage12 : public CPropertyWizardPageBase
{
	DECLARE_DYNAMIC(CPropertyWizardPage12)

	#define WM_UPDATE_CONTROLS	(WM_USER + 150)

public:
	CPropertyWizardPage12();
	virtual ~CPropertyWizardPage12();

	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedRadioSend();
	afx_msg void OnBnClickedRadioCopy();

// Dialog Data
	enum { IDD = IDD_DIALOG_WIZARDPAGE12 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage12)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);

private:
	typedef enum {
		STATE_OPTION_SEND = 0,		// 0 radiobutton
		STATE_OPTION_COPY			// 1 radiobutton
	} State_t;

	State_t m_state;
	int m_selectionOption;
};
