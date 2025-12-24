#if !defined(AFX_NODETREEVIEW_H__0C79AFC2_46C6_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_NODETREEVIEW_H__0C79AFC2_46C6_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NodeTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNodeTreeView view
#include <afxcview.h>

class CNodeTreeView : public CTreeView
{
protected:
	CNodeTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNodeTreeView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNodeTreeView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CNodeTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNodeTreeView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NODETREEVIEW_H__0C79AFC2_46C6_11D8_B865_0050BAC412B1__INCLUDED_)
