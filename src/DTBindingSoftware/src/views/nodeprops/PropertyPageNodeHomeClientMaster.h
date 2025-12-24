#pragma once

#include "PropertyPageNode.h"
#include "iappdlgtasks.h"				// Interface worker...
#include "afxwin.h"

class CPropertyPageNodeHomeClientMaster;

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class CPropertyPageNodeHomeClientMaster_WorkerEvents : public IAppDlgHomeClientMasterTasks_Host::IAppDlgEvents_Host 
{
public:
	CPropertyPageNodeHomeClientMaster_WorkerEvents(CPropertyPageNodeHomeClientMaster& parent);

	void setStateIdle();
	void setStateBusy();
	void addInfoMsg(const CString& msg);
	void addErrorMsg(const CString& msg);
	void setStateConnected(bool connected);
	void update( );

private:
	CPropertyPageNodeHomeClientMaster& m_parent;
};
/****************************************************************************/


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
// CPropertyPageNodeHomeClientMaster dialog

class CPropertyPageNodeHomeClientMaster : public CPropertyPageNode
{
	DECLARE_DYNAMIC(CPropertyPageNodeHomeClientMaster)

public:
	CPropertyPageNodeHomeClientMaster();
	CPropertyPageNodeHomeClientMaster(CCanNode* pNode,IAppDlgHomeClientMasterTasks_Host* pDlgWorker);
	virtual ~CPropertyPageNodeHomeClientMaster();

// Dialog Data
	enum { IDD = IDD_DIALOG_NODE_HOMECLIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void UpdateRunTimeControls();
	virtual BOOL OnInitDialog();	
	afx_msg void OnBnClickedBtnMasterPnp();
	afx_msg void OnBnClickedBtnMasterBrowse();
	afx_msg void OnBnClickedBtnMasterBrowseclear2();
	afx_msg void OnBnClickedBtnMasterUpdate();
	afx_msg void OnBnClickedBtnMasterCancel();
	afx_msg void OnBnClickedCheckConnect2();
	afx_msg void OnBnClickedButtonEditcfg();
	afx_msg void OnRadioButtonsType(UINT nID);
	DECLARE_MESSAGE_MAP()

	enum 
	{ 
		   WM_UPDATE_CONTROLS	= (WM_USER + 150),
		   WM_DISPLAY_ERRORMESSAGE						// Display error msg via assync call in messagebox.
	};

	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam); 
	LONG OnDisplayErrorMessage(WPARAM wParam,LPARAM lParam); 
	void AddMessage(const CString& msg,BOOL clearMessages = FALSE);
	void showErrorMessage(const CString& msg);

private:
	typedef CPropertyPageNode Base;
	friend class CPropertyPageNodeHomeClientMaster_WorkerEvents;

	CPropertyPageNodeHomeClientMaster_WorkerEvents* m_pDlgHelperEvents;
	IAppDlgHomeClientMasterTasks_Host* const m_pDlgWorker;
	CString m_errorString;				// Display error msg via assync call in messagebox.
	CButton m_btnStartPnp;
	CButton m_btnStartBrowse;
	CButton m_btnStartBrowseClear;
	CButton m_btnStartUpdate;
	CButton m_btnCancelTask;
	CListBox m_listboxInfo;
	CEdit m_EditTaskAppState;
	CEdit m_EditNodeDatabase;
	CStatic m_groupConnect;
	CButton m_checkConnect;
	bool m_stateIdle;
	bool m_showControlsConnect;
	bool m_initDone;
	bool m_busyConnect;	
	bool m_stateConnected;
	CButton m_radioLocal;
	CButton m_radioRemote;
	CButton m_btnEdit;
	int m_selectionSettings;			// Selectie local or remote tcp settings...

};
/****************************************************************************/
