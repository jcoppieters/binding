#if !defined(AFX_DLGCONVERSIONFROMNODES_H__C95727AF_50FC_48EE_978C_C6371CA027BF__INCLUDED_)
#define AFX_DLGCONVERSIONFROMNODES_H__C95727AF_50FC_48EE_978C_C6371CA027BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConversionFromNodes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgConversionFromNodes dialog

class CDlgConversionFromNodes : public CDialog
{
protected:
	static CDlgConversionFromNodes *m_pDlgThis;

public:
	static void Show(void);
	
	static CDlgConversionFromNodes* GetInstance() 
	{
		return m_pDlgThis;
	}

	void AddInfo(CString s);

protected:

	int m_nID;

	BOOL Create()
	{ 
		return CDialog::Create(m_nID,AfxGetMainWnd()); 
	}


// Construction
public:
	CDlgConversionFromNodes(CWnd* pParent = NULL);   // standard constructor

	// IDD_DIALOG_CONVERTFROMNODES = 285 ...


// Dialog Data
	//{{AFX_DATA(CDlgConversionFromNodes)
	enum { IDD = 285 };
	CStatic	m_StaticInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConversionFromNodes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConversionFromNodes)
	afx_msg void OnButtonCancel();
	afx_msg void OnButtonStart();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonClear();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONVERSIONFROMNODES_H__C95727AF_50FC_48EE_978C_C6371CA027BF__INCLUDED_)
