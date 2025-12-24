#if !defined(AFX_UNITBUTTON_H__561A6BA7_6382_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_UNITBUTTON_H__561A6BA7_6382_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnitButton window

class CUnitButton : public CButton
{
// Construction
public:
	CUnitButton();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUnitButton();

	void Init(BYTE bNodeAddress,BYTE bUnitAddress);

	// Generated message map functions
protected:
	//{{AFX_MSG(CUnitButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	BYTE bSelNodeAddress,bSelUnitAddress;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITBUTTON_H__561A6BA7_6382_11D8_B865_0050BAC412B1__INCLUDED_)
