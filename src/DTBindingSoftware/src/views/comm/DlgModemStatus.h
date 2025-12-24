#if !defined(AFX_DLGMODEMSTATUS_H__A2414EEE_BDE1_431E_8194_3B6E5D556961__INCLUDED_)
#define AFX_DLGMODEMSTATUS_H__A2414EEE_BDE1_431E_8194_3B6E5D556961__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgModemStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgModemStatus dialog

//class CComApplication;


class CDlgModemStatus : public CDialog
{
protected:
	static CDlgModemStatus *m_pDlg;

	//static CComApplication *m_pComApplication;

public:
	static void Show(void);
	static void Update();

/*
	static void Attach(CComApplication *p) {
		m_pComApplication = p;
	}	
*/

	#define WM_UPDATE_CONTROLS	(WM_USER + 150)
	#define WM_CLEAR_CONTROLS	(WM_USER + 151)


// Construction
public:
	CDlgModemStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgModemStatus)
	enum { IDD = IDD_DIALOG_MODEMSTATUS };
	CEdit	m_EditAddress;
	CEdit	m_EditV24State;
	CEdit	m_EditCurrentState;
	CEdit	m_EditVersion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgModemStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	int m_nID;

	BOOL Create()
	{ 
		return CDialog::Create(m_nID,AfxGetMainWnd());
	}


	// Generated message map functions
	//{{AFX_MSG(CDlgModemStatus)
	virtual void OnOK();
	afx_msg void OnButtonReset();
	afx_msg void OnButtonStatus();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAddress();
	afx_msg void OnClose();
	afx_msg void OnButtonVersion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);
	LONG OnClearControls(WPARAM wParam,LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMODEMSTATUS_H__A2414EEE_BDE1_431E_8194_3B6E5D556961__INCLUDED_)
