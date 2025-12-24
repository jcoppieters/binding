#if !defined(AFX_MYBUTTON_H__A79E84A1_67C9_46C6_B4B8_9DE9F99889CC__INCLUDED_)
#define AFX_MYBUTTON_H__A79E84A1_67C9_46C6_B4B8_9DE9F99889CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyButton window




class CMyButton : public CButton
{
// Construction
public:
	CMyButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyButton();

	void SetButtonState(BOOL flag,BOOL fRedraw = TRUE);
	void SetButtonIndex(int i)		{iButtonIndex = i; };

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyButton)
	afx_msg void OnClicked();
	//}}AFX_MSG

	BOOL fButtonState;
	int	 iButtonIndex;

	static CBrush m_brush;
	static CBrush m_brush2;
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYBUTTON_H__A79E84A1_67C9_46C6_B4B8_9DE9F99889CC__INCLUDED_)
