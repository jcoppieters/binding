#if !defined(AFX_DLGMULTIMEDIA_H__3EAD29D5_D369_412B_9791_70BB28258E11__INCLUDED_)
#define AFX_DLGMULTIMEDIA_H__3EAD29D5_D369_412B_9791_70BB28258E11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMultimedia.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMultimedia dialog
//class CConfigMultiMediaInfo;

class CDlgMultimedia : public CDialog
{
protected:
	//CConfigMultiMediaInfo	*pConfigInfo;

// Construction
public:
	CDlgMultimedia(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMultimedia)
	enum { IDD = IDD_DIALOG_MULTIMEDIA };
	CListBox	m_ListErrors;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMultimedia)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls(void);

	// Generated message map functions
	//{{AFX_MSG(CDlgMultimedia)
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonSendtoall();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMULTIMEDIA_H__3EAD29D5_D369_412B_9791_70BB28258E11__INCLUDED_)
