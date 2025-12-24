#if !defined(AFX_DLGCHILDAPPTASKLOG_H__031853BC_A7A4_4B1C_8FBD_3056AE6EE5D5__INCLUDED_)
#define AFX_DLGCHILDAPPTASKLOG_H__031853BC_A7A4_4B1C_8FBD_3056AE6EE5D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChildAppTaskLog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgChildAppTaskLog dialog

class CDlgChildAppTaskLog : public CDialog
{
	#define WM_UPDATE_CONTROLS		(WM_USER + 150)


protected:
	CRect m_MyWindowRect;
	CString m_szErrorLog;

// Construction
public:
	CDlgChildAppTaskLog(CWnd* pParent = NULL);   // standard constructor
	void ShowWindow(BOOL fShow);
	void Set(const CString& szErrors);

// Dialog Data
	//{{AFX_DATA(CDlgChildAppTaskLog)
	enum { IDD = IDD_DIALOG_CHILD_ERRORLOG };
	CEdit	m_EditErrorLog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChildAppTaskLog)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgChildAppTaskLog)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHILDAPPTASKLOG_H__031853BC_A7A4_4B1C_8FBD_3056AE6EE5D5__INCLUDED_)
