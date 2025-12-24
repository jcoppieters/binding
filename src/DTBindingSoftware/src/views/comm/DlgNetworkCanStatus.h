#if !defined(AFX_DLGNETWORKCANSTATUS_H__61C87147_8146_4575_B190_2CD815A3910F__INCLUDED_)
#define AFX_DLGNETWORKCANSTATUS_H__61C87147_8146_4575_B190_2CD815A3910F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNetworkCanStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNetworkCanStatus dialog

class CCanNode;

class CDlgNetworkCanStatus : public CDialog
{
	#define WM_UPDATE_CONTROLS	(WM_USER + 150)

protected:
	static CDlgNetworkCanStatus* m_pDialog;

protected:
	CDlgNetworkCanStatus(CWnd* pParent = NULL);   // standard constructor

// Construction
public:
	static void Show();
	static void Update(BYTE bNodeAddress);
	static void Destroy();

// Dialog Data
	//{{AFX_DATA(CDlgNetworkCanStatus)
	enum { IDD = IDD_DIALOG_NETWORK_CANSTATUS };
	CStatic	m_StaticNodeInfo;
	CListBox	m_ListBoxErrorsCan;
	CListBox	m_ListBoxErrorsNetwork;
	CComboBox	m_ComboName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNetworkCanStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL Create()
	{ 
		return CDialog::Create(IDD_DIALOG_NETWORK_CANSTATUS,
							   AfxGetMainWnd()); 
	}

	void UpdateNodeInfo(CCanNode* pNode);
	void UpdateNodeCanErrors(CCanNode* pNode);
	CCanNode* GetNode(CComboBox* pControl);
	void UpdateNodeList();

	// Generated message map functions
	//{{AFX_MSG(CDlgNetworkCanStatus)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonRefresh();
	afx_msg void OnSelchangeComboNodename();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNETWORKCANSTATUS_H__61C87147_8146_4575_B190_2CD815A3910F__INCLUDED_)
