#if !defined(AFX_MYSCROLLVIEW_H__22EB542A_4824_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYSCROLLVIEW_H__22EB542A_4824_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyScrollView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyScrollView view

class CMyScrollView : public CScrollView
{
	LOGFONT logFont;
	UINT uCurrentPosition;
	COLORREF OldColor;
	COLORREF NewColor;

	//CFont *pOldFont;
	//CFont *pNewFont;

protected:
	CMyScrollView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyScrollView)

	void ClearMessages(void);
	void AddMessage(CString szText);

	CStringArray Messages;
	int nNumLines;
	BOOL fUpdate;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyScrollView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyScrollView();
	
	LONG OnAddScrollMessage(WPARAM wParam,LPARAM lParam);
	LONG OnClearScrollView(WPARAM wParam,LPARAM lParam);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMyScrollView)
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSCROLLVIEW_H__22EB542A_4824_11D8_B865_0050BAC412B1__INCLUDED_)
