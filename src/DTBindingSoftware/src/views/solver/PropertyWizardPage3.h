#if !defined(AFX_PROPERTYWIZARDPAGE3_H__A488A206_8E67_409B_B63D_BCE8AC8FFC99__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE3_H__A488A206_8E67_409B_B63D_BCE8AC8FFC99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage3.h : header file
//

#include "PropertyWizardPageBase.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage3 dialog

class CPropertyWizardPage3 : public CPropertyWizardPageBase	
{
	DECLARE_DYNCREATE(CPropertyWizardPage3)
	
// Construction
public:
	CPropertyWizardPage3();
	~CPropertyWizardPage3();

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage3)
	enum { IDD = IDD_DIALOG_WIZARDPAGE3 };
	CEdit	m_EditLogical;
	CComboBox	m_ComboPhysical;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage3)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL GetNodeAddress(BYTE* pbNodeAddress);		/* TM20120319 */
	BOOL IsValidPhysicalAddress(void);
	unsigned long GetPhysicalAddress(void);

	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage3)
	afx_msg void OnButtonAssign();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPhysical();
	afx_msg void OnEditchangeComboPhysical();
	afx_msg void OnCloseupComboPhysical();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateControls(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE3_H__A488A206_8E67_409B_B63D_BCE8AC8FFC99__INCLUDED_)
