#if !defined(AFX_DLGCONTROLPROP_H__C0579A01_6E82_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGCONTROLPROP_H__C0579A01_6E82_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgControlProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgControlProp dialog

#include "DlgUnitProp.h"

class CDlgControlProp : public CDlgUnitProp
{
// Construction
public:
	CDlgControlProp(CWnd* pParent = NULL);   // standard constructor

	virtual void InitControls(void); // Sedert versie 0x0900 zonder params

// Dialog Data
	//{{AFX_DATA(CDlgControlProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgControlProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void ShowFullDialog(BOOL fShow);
	virtual void CleanUp(void);
	virtual void RequestRuntimeProperties(void);

private:
	BOOL fControlsCreated;

	// Generated message map functions
	//{{AFX_MSG(CDlgControlProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONTROLPROP_H__C0579A01_6E82_11D8_B865_0050BAC412B1__INCLUDED_)
