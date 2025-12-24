#if !defined(AFX_DLGBINDINGRC5UNIT_H__86C3812A_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGRC5UNIT_H__86C3812A_87CB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingRC5Unit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingRC5Unit dialog
#include "DlgBindingUnitProp.h"
class CBindingRC5RXUnit;

class CDlgBindingRC5Unit : public CDlgBindingUnitProp
{
// Construction
public:
	CDlgBindingRC5Unit(CWnd* pParent = NULL);   // standard constructor
	CBindingRC5RXUnit *pBindingRC5RXUnit;

// Dialog Data
	//{{AFX_DATA(CDlgBindingRC5Unit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingRC5Unit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	virtual void OnSpin2DataChanged(int iStep);
	void UpdateControls(void);

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingRC5Unit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGRC5UNIT_H__86C3812A_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
