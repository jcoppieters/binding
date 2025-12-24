#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#ifndef INCLUDED_IAPPDLGTASKS_H
	#include "iappdlgtasks.h"
#endif 

class CCanNode;

// CDlgNodeServicesCfg dialog

class CDlgNodeServicesCfg : public CDialog
{
	DECLARE_DYNAMIC(CDlgNodeServicesCfg)

public:
	CDlgNodeServicesCfg(CWnd* pParent,IAppDlgNodeServicesCfg* pHost,CCanNode* pCanNode = NULL);   // standard constructor
	virtual ~CDlgNodeServicesCfg();

// Dialog Data
	enum { IDD = IDD_DIALOG_NODESERVICES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();

	enum { WM_TIMER_UPDATE_BTN_SAVE = 1000 };

private:
	IAppDlgNodeServicesCfg* m_pHost;
	CCanNode* m_pCanNode;

	CEdit m_editNodeName;
	CEdit m_editPassword;
	CIPAddressCtrl m_CtrlLocalIPAddress;
	CEdit m_editLocalPort;
	CEdit m_editRemoteDns;
	CEdit m_editRemotePort;
public:
	CButton m_btnOk;
};
