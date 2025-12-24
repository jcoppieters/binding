#if !defined(AFX_DLGBINDINGVIRTUALUNIT_H__86C38124_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGVIRTUALUNIT_H__86C38124_87CB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingVirtualUnit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingVirtualUnit dialog

#include "DlgBindingUnitProp.h"
class CBindingVirtualUnit;

class CDlgBindingVirtualUnit : public CDlgBindingUnitProp
{
// Construction
	CBindingVirtualUnit *pBindingVirtualUnit;
public:
	CDlgBindingVirtualUnit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingVirtualUnit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingVirtualUnit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	void UpdateControls(void);
	// Generated message map functions
	//{{AFX_MSG(CDlgBindingVirtualUnit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGVIRTUALUNIT_H__86C38124_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
