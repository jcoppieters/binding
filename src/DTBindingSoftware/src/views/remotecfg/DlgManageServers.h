#pragma once

#ifndef INCLUDED_APPREMOTECFGSERVERS_H
	#include "appremotecfg/AppRemoteCfgServers.h"
#endif 
#ifndef INCLUDED_IAPPREMOTECFGAPI_H
	#include "appremotecfg/IAppRemoteCfgApi.h"
#endif 

// CDlgManageServers dialog
class CDlgManageServers : public CDialog
{
	DECLARE_DYNAMIC(CDlgManageServers)

public:
	CDlgManageServers(CWnd* pParent,DUOTECNO::IAppRemoteCfgApi& refRemoteCfg);   // standard constructor
	virtual ~CDlgManageServers();

// Dialog Data
	enum { IDD = IDD_DIALOG_SERVERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonAdjust();
	afx_msg void OnCbnSelchangeComboServer();
	afx_msg void OnBnClickedCheckSecured();

	virtual BOOL OnInitDialog();
	virtual BOOL getDataControls(DUOTECNO::AppRemoteCfgServer* p);
	virtual BOOL updateDataControls(const DUOTECNO::AppRemoteCfgServer* server);
	LONG onWmUpdateControls(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void helper_ComboBoxloadItems();

private:
	typedef enum {
		MODE_VIEW = 0, MODE_EDIT
	} Mode_t;

	// Dialog data
	DUOTECNO::IAppRemoteCfgApi& m_refRemoteCfg;
	int m_currentSelected;
	Mode_t m_mode;

	// MFC Controls.
	CComboBox m_comboServers;
	CEdit m_editName;
	CEdit m_editHostName;
	CEdit m_editPort;
	CButton m_checkSecured;
	CEdit m_editPassword;
	CStatic m_staticPassword;
	CButton m_btnAdjust;
	CButton m_btnAdd;
	CButton m_btnRemove;
};
