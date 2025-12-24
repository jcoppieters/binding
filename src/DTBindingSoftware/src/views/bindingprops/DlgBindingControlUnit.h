#if !defined(AFX_DLGBINDINGCONTROLUNIT_H__86C38123_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGCONTROLUNIT_H__86C38123_87CB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingControlUnit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingControlUnit dialog

#include "DlgBindingUnitProp.h"
class CBindingControlUnit;

class CDlgBindingControlUnit : public CDlgBindingUnitProp
{
// Construction
public:
	CDlgBindingControlUnit(CWnd* pParent = NULL);   // standard constructor
	CBindingControlUnit *pBindingControlUnit;

// Dialog Data
	//{{AFX_DATA(CDlgBindingControlUnit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingControlUnit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep) {};
	void UpdateControls(void);
	// Generated message map functions
	//{{AFX_MSG(CDlgBindingControlUnit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGCONTROLUNIT_H__86C38123_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
