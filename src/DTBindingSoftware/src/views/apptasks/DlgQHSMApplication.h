#if !defined(AFX_DLGQHSMAPPLICATION_H__DF683262_1558_4B0B_9E4E_87CF7B6CEED9__INCLUDED_)
#define AFX_DLGQHSMAPPLICATION_H__DF683262_1558_4B0B_9E4E_87CF7B6CEED9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQHSMApplication.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgQHSMApplication dialog

typedef enum
{
	ENUM_MODE_RETRY = 0,
	ENUM_MODE_CONFIRMATION,
	ENUM_MODE_INFORMATION

} ENUM_APPLICATION_DIALOG_MODE;


class CDlgQHSMApplication : public CDialog
{
public:
	static BOOL Show(ENUM_APPLICATION_DIALOG_MODE mode,CString szMessage="");

protected:
	static CDlgQHSMApplication* m_pThis;

	UINT m_nID;
	CWnd* m_pParent;	
	ENUM_APPLICATION_DIALOG_MODE m_Mode;
	BOOL m_fUserButtonPushed;

// Construction
public:
	CDlgQHSMApplication(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgQHSMApplication)
	enum { IDD = IDD_DIALOG_APPLICATION_GLOBAL };
	CButton	m_StaticGroup;
	CStatic	m_StaticIcon;
	CButton	m_BtnOk;
	CButton	m_BtnCancel;
	CStatic	m_StaticInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQHSMApplication)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL Create(){ 
		return CDialog::Create(m_nID,m_pParent);
	}

	void Display(ENUM_APPLICATION_DIALOG_MODE mode,CString szMessage = "");

	// Generated message map functions
	//{{AFX_MSG(CDlgQHSMApplication)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQHSMAPPLICATION_H__DF683262_1558_4B0B_9E4E_87CF7B6CEED9__INCLUDED_)
