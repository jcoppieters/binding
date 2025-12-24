#if !defined(AFX_PROPERTYWIZARDPAGE6_H__2F88942F_992B_43D7_B3DA_5923EA3EA402__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE6_H__2F88942F_992B_43D7_B3DA_5923EA3EA402__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage6.h : header file
//
#include "PropertyWizardPageBase.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage6 dialog

class CPropertyWizardPage6 : public CPropertyWizardPageBase	
{
	DECLARE_DYNCREATE(CPropertyWizardPage6)

// Construction
public:	
	CPropertyWizardPage6();
	~CPropertyWizardPage6();

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage6)
	enum { IDD = IDD_DIALOG_WIZARDPAGE6 };
	CComboBox	m_ComboInit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage6)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BYTE GetNodeLogicalAddress(void);
	void UpdateControls(void);
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage6)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboLogical();
	afx_msg void OnButtonInit();
	afx_msg void OnEditchangeComboLogical();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE6_H__2F88942F_992B_43D7_B3DA_5923EA3EA402__INCLUDED_)
