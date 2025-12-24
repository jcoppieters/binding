#if !defined(AFX_DLGCHILDAPPTASKINFO_H__8B61AD4E_6779_4F44_9DFA_B4EDE441D46A__INCLUDED_)
#define AFX_DLGCHILDAPPTASKINFO_H__8B61AD4E_6779_4F44_9DFA_B4EDE441D46A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChildAppTaskInfo.h : header file
//


#include "ApplicationDlgAppearance.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgChildAppTaskInfo dialog

class CDlgChildAppTaskInfo : public CDialog
{
// Construction
public:
	CDlgChildAppTaskInfo(CWnd* pParent = NULL);   // standard constructor
	void SetProperties(CApplicationDlgInfoAppearance* pAppearance);
	void ShowWindow(BOOL fShow);

// Dialog Data
	//{{AFX_DATA(CDlgChildAppTaskInfo)
	enum { IDD = IDD_DIALOG_CHILD_TASK_INFO };
	CButton	m_BtnCancel;
	CButton	m_BtnOk;
	CStatic	m_StaticInfo;
	CStatic	m_StaticIcon;
	CButton	m_StaticGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChildAppTaskInfo)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	#define WM_UPDATE_CONTROLS		(WM_USER + 150)

	BOOL m_fUserButtonPushed;
	CRect m_MyWindowRect;
	CApplicationDlgInfoAppearance m_Appearance;

	// Generated message map functions
	//{{AFX_MSG(CDlgChildAppTaskInfo)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHILDAPPTASKINFO_H__8B61AD4E_6779_4F44_9DFA_B4EDE441D46A__INCLUDED_)
