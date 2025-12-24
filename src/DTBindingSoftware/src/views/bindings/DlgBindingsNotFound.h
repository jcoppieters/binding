#if !defined(AFX_DLGBINDINGSNOTFOUND_H__36A6F504_B606_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGSNOTFOUND_H__36A6F504_B606_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingsNotFound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingsNotFound dialog

class CDlgBindingsNotFound : public CDialog
{
// Construction
	CStringArray *pBindingsNotFound;
public:
	CDlgBindingsNotFound(CWnd* pParent = NULL,CStringArray *pStrings = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingsNotFound)
	enum { IDD = IDD_DIALOG_MANUAL_BINDINGS };
	CListBox	m_ListBindings;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingsNotFound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingsNotFound)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGSNOTFOUND_H__36A6F504_B606_11D8_B865_0050BAC412B1__INCLUDED_)
