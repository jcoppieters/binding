#if !defined(AFX_MYVIEW_H__0C79AFCD_46C6_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYVIEW_H__0C79AFCD_46C6_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyView view

#include "uicontrols/panels/MySplitterWnd.h"


class CMyFrameWnd;
class CMyFormView;

class CMyView : public CView
{
protected:
	CMyView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyView)

	CMyFrameWnd *pMyFrameWnd;
	CMyFormView *pMyTreeView;

// Attributes
public:
	CMySplitterWnd m_wndSplitter;
	BOOL m_wndSplitterCreated;

	//CMyFrameWnd* GetFrameWnd(void) { return pMyFrameWnd; }


// Operations
public:
	BOOL CreateViews(CCreateContext *pContext);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYVIEW_H__0C79AFCD_46C6_11D8_B865_0050BAC412B1__INCLUDED_)
