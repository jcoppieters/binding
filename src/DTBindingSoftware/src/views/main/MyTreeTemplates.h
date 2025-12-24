#if !defined(AFX_MYTREETEMPLATES_H__EB3998B4_8312_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYTREETEMPLATES_H__EB3998B4_8312_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyTreeTemplates.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyTreeTemplates window
class CMyTreeTemplates : public CTreeCtrl
{

// Construction
public:
	CMyTreeTemplates();

// Attributes
public:

// Operations
public:

	BOOL Update(void);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTreeTemplates)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyTreeTemplates();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyTreeTemplates)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// new ...
	LONG OnApplicationClearViews(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTREETEMPLATES_H__EB3998B4_8312_11D8_B865_0050BAC412B1__INCLUDED_)
