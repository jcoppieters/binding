
// AppRemoteCfgMfcWinDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "appremotecfg/AppRemoteCfgApi.h"

using namespace DUOTECNO;

// CAppRemoteCfgMfcWinDlg dialog
class CAppRemoteCfgMfcWinDlg : public CDialog, public DUOTECNO::IAppRemoteCfgGUI
{
// Construction
public:
	CAppRemoteCfgMfcWinDlg(CWnd* pParent = NULL);	// standard constructor
	~CAppRemoteCfgMfcWinDlg( );

// Dialog Data
	enum { IDD = IDD_APPREMOTECFGMFCWIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonBindingsErase();
	afx_msg void OnBnClickedButtonBindingsWrite();
	afx_msg void OnBnClickedButtonBackupUpload();
	afx_msg void OnBnClickedButtonTest01();
	afx_msg void OnBnClickedButtonTest02();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeComboServers();
	afx_msg void OnBnClickedButtonServers();

protected:
	// IAppRemoteCfgGUI
	virtual void addMessage(const char* msg, bool eraseOld);
	virtual void addMessages(CStringList& messages, bool eraseOld);
	virtual void taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
		HttpError_t errorCode = HTTP_EC_NONE, const char* msgError = 0);
	LONG onWmTaskError(WPARAM wParam, LPARAM lParam);

	// helper
	CString getDlgPassword(const CString& oldPassword);

	// Timer Handlers...
	bool timerhandler_UpdateGUI(UINT_PTR nIDEvent);
	bool timerhandler_UpdateListBox(UINT_PTR nIDEvent);
	bool timerhandler_pollFsm(UINT_PTR nIDEvent);

	typedef bool (CAppRemoteCfgMfcWinDlg::*PF_TIMER_HANDLERS)(UINT_PTR );
	CArray<PF_TIMER_HANDLERS> m_arrTimerHandlers;

	void loadComboBoxServers();
	void setComboBoxSelectedserver();

private:
	CButton m_BtnBindingsErase;
	CButton m_BtnBindingsWrite;
	CButton m_btnBackupUpload;
	CButton m_btnTest1;
	CButton m_btnTest2;
	CButton m_btnConnect;
	CButton m_btnOpenProject;
	CListBox m_listboxTracer;
	CComboBox m_comboBoxServers;
	CButton m_btnServers;

	CStringList m_stringList;
	BOOL m_fileLoaded;
	DUOTECNO::IAppRemoteCfgApi* m_pAppRemoteCfg;
	int m_currentSelServer;
};
