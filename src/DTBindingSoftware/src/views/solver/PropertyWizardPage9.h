#if !defined(AFX_PROPERTYWIZARDPAGE9_H__1A6EBB5F_DA93_40CF_90A2_452236C913AB__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE9_H__1A6EBB5F_DA93_40CF_90A2_452236C913AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage9.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage9 dialog



#include "PropertyWizardPageBase.h"

// CPropertyWizardPage9 --> Send configuration changes to the CAN-Bus.

class CPropertyWizardPage9 : public CPropertyWizardPageBase		
{
	DECLARE_DYNCREATE(CPropertyWizardPage9)

// Construction
public:
	CPropertyWizardPage9();
	~CPropertyWizardPage9();

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage9)
	enum { IDD = IDD_DIALOG_WIZARDPAGE9 };
	CButton	m_CheckNames;
	CButton	m_CheckBindings;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage9)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage9)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE9_H__1A6EBB5F_DA93_40CF_90A2_452236C913AB__INCLUDED_)
