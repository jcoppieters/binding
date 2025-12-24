#if !defined(AFX_PROPERTYWIZARDPAGE4_H__D225EB9E_D45E_4F9E_8106_391A71CCE22B__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE4_H__D225EB9E_D45E_4F9E_8106_391A71CCE22B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage4.h : header file
//
#include "PropertyWizardPageBase.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage4 dialog

class CPropertyWizardPage4 : public CPropertyWizardPageBase		
{
	DECLARE_DYNCREATE(CPropertyWizardPage4)

// Construction
public:
	CPropertyWizardPage4();
	~CPropertyWizardPage4();

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage4)
	enum { IDD = IDD_DIALOG_WIZARDPAGE4 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage4)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage4)
		// NOTE: the ClassWizard will add member functions here
		virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE4_H__D225EB9E_D45E_4F9E_8106_391A71CCE22B__INCLUDED_)
