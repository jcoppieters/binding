#if !defined(AFX_DLGCANFTP_H__97030E7A_3D7B_4956_8474_AB185928FEDA__INCLUDED_)
#define AFX_DLGCANFTP_H__97030E7A_3D7B_4956_8474_AB185928FEDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCanFTP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCanFTP dialog

class CDlgCanFTP : public CDialog
{
// Construction
public:
	CDlgCanFTP(CWnd* pParent = NULL);   // standard constructor

	static CDlgCanFTP* getInstance( );
	static bool isActive( );
	void Show(void);

// Dialog Data
	//{{AFX_DATA(CDlgCanFTP)
	enum { IDD = IDD_DIALOG_FTP };
	CString	m_strFileNameLocal;
	CString	m_strFileNameRemote;
	CString	m_strNodeAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCanFTP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCanFTP)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonReceive();
	afx_msg void OnButtonSend();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	BOOL Create();
	BOOL GetNodeAddress(BYTE* pbNodeAddress);
	BOOL getParameters(
		BYTE* pbNodeAddress, 
		CString* pStrFileNameRemote,
		CString* pStrFileNameLocal);

private:
	static CDlgCanFTP* m_pInstance;
	int m_nID;	

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCANFTP_H__97030E7A_3D7B_4956_8474_AB185928FEDA__INCLUDED_)
