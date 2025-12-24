#if !defined(AFX_PROPERTYWIZARDPAGE7_H__CD763B95_6441_410B_87EE_06C43989D1C8__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE7_H__CD763B95_6441_410B_87EE_06C43989D1C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage7.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage7 dialog

#include "PropertyWizardPageBase.h"

class CCanNode;

class CPropertyWizardPage7 : public CPropertyWizardPageBase	
{
	DECLARE_DYNCREATE(CPropertyWizardPage7)

// Construction
public:
	CPropertyWizardPage7();
	~CPropertyWizardPage7();

	void SetComboBox(int index,const CCanNode& Node);

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage7)
	enum { IDD = IDD_DIALOG_WIZARDPAGE7 };
	CMyEditCtrl	m_EditBindings;
	CButton	m_BtnRemoveAdd;
	CMyEditCtrl	m_EditUnits;
	CMyEditCtrl	m_EditName;
	CMyEditCtrl	m_EditAddress;
	CComboBox	m_CmbPhysical;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage7)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BYTE GetNodeAddress(void);
	void UpdateControls(void);

	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage7)
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboPhysical();
	afx_msg void OnCloseupComboPhysical();
	afx_msg void OnButtonRemoveadd2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE7_H__CD763B95_6441_410B_87EE_06C43989D1C8__INCLUDED_)
