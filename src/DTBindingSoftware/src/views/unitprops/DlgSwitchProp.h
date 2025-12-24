#if !defined(AFX_DLGSWITCHPROP_H__1DEEAF4B_6DCB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGSWITCHPROP_H__1DEEAF4B_6DCB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSwitchProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSwitchProp dialog

#include "DlgUnitProp.h"
class CDlgSwitchPropChild;

class CDlgSwitchProp : public CDlgUnitProp
{
// Construction
public:
	CDlgSwitchProp(CWnd* pParent = NULL);   // standard constructor

	virtual void InitControls(void); // Sedert versie 0x0900 zonder params

// Dialog Data
	//{{AFX_DATA(CDlgSwitchProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSwitchProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	CDlgSwitchPropChild *pDlgChild;

	virtual void ShowFullDialog(BOOL fShow);
	virtual void CleanUp(void);
	virtual void RequestRuntimeProperties(void);
	
	BOOL fControlsCreated;

	///////////////////////


	// Generated message map functions
	//{{AFX_MSG(CDlgSwitchProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSWITCHPROP_H__1DEEAF4B_6DCB_11D8_B865_0050BAC412B1__INCLUDED_)
