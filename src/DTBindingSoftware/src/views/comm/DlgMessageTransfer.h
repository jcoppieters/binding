#if !defined(AFX_DLGMESSAGETRANSFER_H__4204E21D_DDDC_4F08_B32E_1C02E746D7E8__INCLUDED_)
#define AFX_DLGMESSAGETRANSFER_H__4204E21D_DDDC_4F08_B32E_1C02E746D7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMessageTransfer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageTransfer dialog

class CDlgMessageTransfer : public CDialog
{
protected:
	static CDlgMessageTransfer* m_pDialog;

// Construction
public:
	CDlgMessageTransfer(CWnd* pParent = NULL);   // standard constructor

	
	static void Show();	
	static void Destroy();

// Dialog Data
	//{{AFX_DATA(CDlgMessageTransfer)
	enum { IDD = IDD_DIALOG_MESSAGETRANSFER };
	CEdit	m_EditUnitAddress;
	CEdit	m_EditNodeAddress;
	CEdit	m_EditMessageData;
	CEdit	m_EditMessageCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMessageTransfer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int GetData(CEdit* pControl,CString& MessageData);
	BOOL GetData(CEdit* pControl,int* pValue);

	BOOL Create() { 
		return CDialog::Create(IDD_DIALOG_MESSAGETRANSFER,
							   AfxGetMainWnd()); 
	}

	// Generated message map functions
	//{{AFX_MSG(CDlgMessageTransfer)
	virtual void OnOK();
	afx_msg void OnButtonSend();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMESSAGETRANSFER_H__4204E21D_DDDC_4F08_B32E_1C02E746D7E8__INCLUDED_)
