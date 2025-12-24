#if !defined(AFX_DLGMOUSEPROP_H__A89D7F53_BFB7_4E8D_9A5B_E7DBBE660C18__INCLUDED_)
#define AFX_DLGMOUSEPROP_H__A89D7F53_BFB7_4E8D_9A5B_E7DBBE660C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMouseProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMouseProp dialog

#include "DlgUnitProp.h"

class CDlgMouseProp : public CDlgUnitProp
{
// Construction
public:
	CDlgMouseProp(CWnd* pParent = NULL);   // standard constructor

	virtual void InitControls(void); // Sedert versie 0x0900 zonder params

// Dialog Data
	//{{AFX_DATA(CDlgMouseProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMouseProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual void ShowFullDialog(BOOL fShow);
	virtual void CleanUp(void);
	virtual void RequestRuntimeProperties(void);
	
	BOOL fControlsCreated;

	// Generated message map functions
	//{{AFX_MSG(CDlgMouseProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMOUSEPROP_H__A89D7F53_BFB7_4E8D_9A5B_E7DBBE660C18__INCLUDED_)
