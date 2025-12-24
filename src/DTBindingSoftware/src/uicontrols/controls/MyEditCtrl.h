#if !defined(AFX_MYEDITCTRL_H__F93A7329_727F_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYEDITCTRL_H__F93A7329_727F_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyEditCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyEditCtrl window

#define WM_NOTIFYPARENT_KEYENTER	2000

class CMyEditCtrl : public CEdit
{
// Construction
public:
	CMyEditCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyEditCtrl)
	//}}AFX_VIRTUAL

	void SetReadOnly(BOOL flag) {
		fReadOnly = flag;
	}

// Implementation
public:
	virtual ~CMyEditCtrl();

	// Generated message map functions
protected:
	void UpdateControl(void);
	void HandleKeyEnter(void);
	//{{AFX_MSG(CMyEditCtrl)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	BOOL fReadOnly;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYEDITCTRL_H__F93A7329_727F_11D8_B865_0050BAC412B1__INCLUDED_)
