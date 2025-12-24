#pragma once
#include "afxwin.h"

#define WM_UPDATE_CONTROLS		(WM_USER + 150)

// CDlgCommCanLog dialog

class CDlgCommCanLog : public CDialog
{
	DECLARE_DYNAMIC(CDlgCommCanLog)

protected:
	static CDlgCommCanLog *m_pDlgTracer;

public:
	static void Show(void);
	static void Add(const CString& s);	
	static BOOL IsActive() {
		return (!(m_pDlgTracer == NULL));
	}
	static void Update(void);

public:
	CDlgCommCanLog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCommCanLog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButtonAddline();
	afx_msg void OnBnClickedButtonClear();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_COMMCANLOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()

	BOOL Create(){ 
		return CDialog::Create(m_nID,AfxGetMainWnd()); //m_pParent);
	}
	virtual void OnCancel();

	void AddMessage(const CString& szData);
	void helper_AddMessage(const CString& szData);
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);

	void fileLoggingStart(bool flagOverwrite);
	void fileLoggingStop();
	void fileLoggingAppendData();
	void fileLoggingAppendData(FILE* fp);

private:
	int m_nID;
	CListBox m_ListBox;
	bool m_fileLoggingEnable;
	CButton m_btnCheckLog;
};
