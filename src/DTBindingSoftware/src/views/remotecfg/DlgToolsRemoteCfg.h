#pragma once
#include "afxwin.h"

#if (USE_TOOLS_APPREMOTECFG == 1)

#include "dtapprcfgmfclib/appremotecfg/AppRemoteCfgApi.h"
// CDlgToolsRemoteCfg dialog

class CDlgToolsRemoteCfg : public CDialog , public DUOTECNO::IAppRemoteCfgGUI
{
	DECLARE_DYNAMIC(CDlgToolsRemoteCfg)

public:
	CDlgToolsRemoteCfg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgToolsRemoteCfg();

	static CDlgToolsRemoteCfg* getInstance( );
	static bool isActive( );
	void Show(void);


// Dialog Data
	enum { IDD = IDD_DIALOG_REMOTECFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedButtonConnect();	
	afx_msg void OnBnClickedButtonBindingsErase();
	afx_msg void OnBnClickedButtonBindingsWrite();
	afx_msg void OnBnClickedButtonBindingsWriteModified();
	afx_msg void OnBnClickedButtonBackupUpload();
	afx_msg void OnBnClickedButtonUploadNames();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnServers();
	afx_msg void OnCbnSelchangeComboServers();
	BOOL Create();
	DECLARE_MESSAGE_MAP()
	LONG onWmTaskError(WPARAM wParam, LPARAM lParam);
	LONG onWmTaskCheckErrorLog(WPARAM wParam, LPARAM lParam);

	// IAppRemoteCfgGUI
	virtual void addMessage(const char* msg, bool eraseOld);
	virtual void addMessages(CStringList& messages, bool eraseOld);
	virtual void taskStatus(DUOTECNO::AppRemoteCfgTasks_t task, DUOTECNO::AppRemoteCfgTaskStatus_t status,
		HttpError_t errorCode = HTTP_EC_NONE, const char* msgError = 0);

	// Timer Handlers...
	bool timerhandler_UpdateGUI(UINT_PTR nIDEvent);
	bool timerhandler_UpdateListBox(UINT_PTR nIDEvent);

	CString getDlgPassword(const CString& oldPassword);

	void loadComboBoxServers();
	void setComboBoxSelectedserver();

private:
	static CDlgToolsRemoteCfg* m_pInstance;
	int m_nID;	
	typedef bool (CDlgToolsRemoteCfg::*PF_TIMER_HANDLERS)(UINT_PTR );
	CArray<PF_TIMER_HANDLERS> m_arrTimerHandlers;
	BOOL m_fileLoaded;		// temp
	int m_currentSelServer;
	// Gui Controls...
	
	CButton m_BtnBindingsErase;
	CButton m_BtnBindingsWrite;
	CButton m_BtnBindingsWriteModified;
	CButton m_BtnUpoadNames;
	CButton m_btnBackupUpload;
	CButton m_btnConnect;
	CListBox m_listboxTracer;	
	CStringList m_stringList;
	CComboBox m_comboBoxServers;
	CButton m_btnServers;
};


#endif