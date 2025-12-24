#if !defined(AFX_DLGCOMMTRACER_H__9E997785_3787_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGCOMMTRACER_H__9E997785_3787_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCommTracer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCommTracer dialog

class CDlgCommTracer : public CDialog
{
// Construction

protected:
	static CDlgCommTracer *m_pDlgTracer;

public:
	static void Show(void);
	static void AddMsgTracerComm(const CString& s);
	static void AddMsgTracerError(const CString& s);
	
	static BOOL IsActive() {
		return (!(m_pDlgTracer == NULL));
	}

public:
	CDlgCommTracer(CWnd* pParent = NULL);   // standard constructor

protected:

	int m_nID;

	BOOL Create(){ 
		return CDialog::Create(m_nID,AfxGetMainWnd()); //m_pParent);
	}

	void AddMessage(CListBox* pListBox,const CString& szData,int maxMessages=10000,int topIndex = 10);

// Dialog Data
	//{{AFX_DATA(CDlgCommTracer)
	enum { IDD = IDD_DLG_COMMTRACER };
	CListBox	m_ListBoxCommunication;
	CListBox	m_ListBoxErrors;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCommTracer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgCommTracer)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOMMTRACER_H__9E997785_3787_11D8_B865_0050BAC412B1__INCLUDED_)
