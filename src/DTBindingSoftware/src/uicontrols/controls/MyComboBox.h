#if !defined(AFX_MYCOMBOBOX_H__0D0349A2_87D6_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYCOMBOBOX_H__0D0349A2_87D6_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyComboBox window

class CMyComboBox : public CComboBox
{
// Construction
public:
	CMyComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyComboBox)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCOMBOBOX_H__0D0349A2_87D6_11D8_B865_0050BAC412B1__INCLUDED_)
