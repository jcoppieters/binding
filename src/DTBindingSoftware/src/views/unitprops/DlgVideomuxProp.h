#if !defined(AFX_DLGVIDEOMUXPROP_H__E39A3FEA_FD47_4533_A784_91051368815E__INCLUDED_)
#define AFX_DLGVIDEOMUXPROP_H__E39A3FEA_FD47_4533_A784_91051368815E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgVideomuxProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgVideomuxProp dialog
#include "DlgUnitProp.h"
#include "afxwin.h"
class CDlgVideomuxProp : public CDlgUnitProp
{
// Construction
public:
	CDlgVideomuxProp(CWnd* pParent = NULL);   // standard constructor

	virtual void InitControls(void); // Sedert versie 0x0900 zonder params

// Dialog Data
	//{{AFX_DATA(CDlgVideomuxProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVideomuxProp)
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
	//{{AFX_MSG(CDlgVideomuxProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVIDEOMUXPROP_H__E39A3FEA_FD47_4533_A784_91051368815E__INCLUDED_)
