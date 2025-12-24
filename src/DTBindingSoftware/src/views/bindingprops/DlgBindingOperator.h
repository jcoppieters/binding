#if !defined(AFX_DLGBINDINGOPERATOR_H__0D0349A1_87D6_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGOPERATOR_H__0D0349A1_87D6_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingOperator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingOperator dialog



class CBindingOperator;
class CBindingTimerOperator;
class CMainFrame;



class CDlgBindingOperator : public CDialog
{
// Construction
	//CBindingTimerOperator *m_pBindingOperator;


protected:
	static CBindingTimerOperator* m_pBindingOperator;
	//static CMainFrame* m_pMainWnd;
	static CDlgBindingOperator* m_pBindingOperatorDlg;

public:
	static BOOL Show(CBindingOperator *pBindingOperator,CPoint TopLeft);
	static BOOL Destroy(void);
	//static BOOL AttachParent(CWnd *pWnd);


public:
	CDlgBindingOperator(CWnd* pParent = NULL);   // standard constructor
	
/*
	BOOL Create()
	{ return CDialog::Create(m_nID,m_pParent);}
*/

	BOOL Create()
	{ return CDialog::Create(m_nID,AfxGetMainWnd());}

	void InitControls(CBindingOperator *pOperator);
	void MoveWindow(CPoint topleft);

// Dialog Data
	//{{AFX_DATA(CDlgBindingOperator)
	enum { IDD = IDD_DIALOG_PROP_BINDINGOPERATOR };
	CSpinButtonCtrl	m_Spin1;
	CEdit	m_EditSpin1;
	CComboBox	m_CmbTimer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingOperator)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	
	 //CWnd *m_pParent;
	 int m_nID;
	 void UpdateControls(void);


	// Generated message map functions
	//{{AFX_MSG(CDlgBindingOperator)
	afx_msg void OnSelendokComboTimer();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGOPERATOR_H__0D0349A1_87D6_11D8_B865_0050BAC412B1__INCLUDED_)
