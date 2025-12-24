#if !defined(AFX_DLGCHILDAPPTASKSTATUS_H__23926B8C_0B6B_4F35_9D46_05A97C9D1F96__INCLUDED_)
#define AFX_DLGCHILDAPPTASKSTATUS_H__23926B8C_0B6B_4F35_9D46_05A97C9D1F96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChildAppTaskStatus.h : header file
//

#include "ApplicationDlgAppearance.h"
#include "uicontrols/controls/MyProgressCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChildAppTaskStatus dialog

class CDlgChildAppTaskStatus : public CDialog
{
protected:
	CRect m_MyWindowRect;
	CMyProgressCtrl	m_ProgressCtrl;	
	CApplicationDlgAppearance m_Appearance;

	#define WM_UPDATE_CONTROLS		(WM_USER + 150)

// Construction
public:
	CDlgChildAppTaskStatus(CWnd* pParent = NULL);   // standard constructor
	void ShowWindow(BOOL fShow);
	void SetProperties(CApplicationDlgAppearance* pAppearance);
	void ShowProgressStepUp();

// Dialog Data
	//{{AFX_DATA(CDlgChildAppTaskStatus)
	enum { IDD = IDD_DIALOG_CHILD_TASK_STATUS };
	CStatic	m_StaticInfo;
	CButton	m_ButtonUser;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChildAppTaskStatus)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgChildAppTaskStatus)
	virtual void OnOK();
	virtual BOOL OnInitDialog();	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHILDAPPTASKSTATUS_H__23926B8C_0B6B_4F35_9D46_05A97C9D1F96__INCLUDED_)
