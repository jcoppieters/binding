#if !defined(AFX_DLGENGINEERINGRS232_H__06D92BA4_F9D5_47C9_8C6E_D0D30A90DEE7__INCLUDED_)
#define AFX_DLGENGINEERINGRS232_H__06D92BA4_F9D5_47C9_8C6E_D0D30A90DEE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEngineeringRS232.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEngineeringRS232 dialog

class CDlgEngineeringRS232 : public CDialog
{
	#define WM_UPDATE_CONTROLS	(WM_USER + 150)

protected:
	static CDlgEngineeringRS232* m_pThis;

public:
	static CDlgEngineeringRS232* GetInstance() 
	{
		return m_pThis;
	}

	static void Show(void);
	static void Add(CString s);
	static void AddInfo(CString s);
	static void ClearInfo();
	
	static BOOL IsActive() 
	{
		return (!(m_pThis == NULL));
	}

protected:
	int m_nID;	
	BOOL fConnected;
	BOOL fBusyConnecting;
	int m_comport;				// compoort 1 tem 255

	BOOL Create(){ 
		return CDialog::Create(m_nID,AfxGetMainWnd()); //m_pParent);
	}

	void AddMessage(CString szData);
	void AddMessageInfo(CString szData);


// Construction
public:
	CDlgEngineeringRS232(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEngineeringRS232)
	enum { IDD = IDD_DIALOG_ENGINEERING_COMMUNICATION };
	CListBox	m_ListBoxMessages;
	CButton	m_CheckOnline;
	CComboBox	m_CmbBoxPort;
	CListBox	m_ListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEngineeringRS232)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEngineeringRS232)
	afx_msg void OnCheckConnect();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonReset();
	afx_msg void OnButtonStatus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	LONG OnCommunicationModemStatus(WPARAM wParam,LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGENGINEERINGRS232_H__06D92BA4_F9D5_47C9_8C6E_D0D30A90DEE7__INCLUDED_)
