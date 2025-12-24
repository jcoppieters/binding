#if !defined(AFX_DLGAVMATRIXPROP_H__18AB6019_72DE_4D35_A91E_8EC2C487E0DF__INCLUDED_)
#define AFX_DLGAVMATRIXPROP_H__18AB6019_72DE_4D35_A91E_8EC2C487E0DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAVMatrixProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAVMatrixProp dialog

#include "DlgUnitProp.h"

class CDlgAvMatrixChildProp;


class CDlgAVMatrixProp : public CDlgUnitProp
{
// Construction
public:
	CDlgAVMatrixProp(CWnd* pParent = NULL);   // standard constructor

	virtual void InitControls(void); // Sedert versie 0x0900 zonder params

// Dialog Data
	//{{AFX_DATA(CDlgAVMatrixProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAVMatrixProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual void ShowFullDialog(BOOL fShow);
	virtual void CleanUp(void);
	virtual void RequestRuntimeProperties(void);

	// Generated message map functions
	//{{AFX_MSG(CDlgAVMatrixProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	BOOL m_fControlsCreated;
	CDlgAvMatrixChildProp *m_pDlgChild;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAVMATRIXPROP_H__18AB6019_72DE_4D35_A91E_8EC2C487E0DF__INCLUDED_)
