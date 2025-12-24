#pragma once

#include "DlgChildUDPBase.h"
#include "afxwin.h"

// CDlgChildUDPDynDns dialog

class CDlgChildUDPDynDns : public CDlgChildUDPBase
{
	DECLARE_DYNAMIC(CDlgChildUDPDynDns)

public:
	CDlgChildUDPDynDns(CWnd* pParent,CDlgUDPConfigCtx& refCtx);   // standard constructor
	virtual ~CDlgChildUDPDynDns();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DYNDNS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void initData(AppUDPDevice& refDevice);				// Algemene data.
	virtual void initStaticControls(AppUDPDevice& refDevice);
	virtual void updateDynamicControls(AppUDPDevice& refDevice);
	virtual void onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent);
	virtual BOOL dataChanged( );
	virtual void onActive(BOOL flag);
	virtual const CString& getStrDescription( ) const;
	BOOL busy();
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnBnClickedButtonRequest();
	afx_msg void OnCbnSelchangeComboDdnsProvider();
	bool sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);
	bool sendRequestStatus(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);
	
private:
	enum { PROVIDER_NONE = 0, PROVIDER_NOIP = 1 };
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_BUSY_SEND,
		STATE_BUSY_REQUEST
	} State_t;

	State_t m_state;
	CComboBox m_cmbDDNSProvider;
	CEdit m_editHostname;
	CEdit m_editUsername;
	CEdit m_editPassword;
	CButton m_btnConfigure;
	CButton m_btnRequest;
	CEdit m_editStatus;
	CStatic m_staticHostName;
	CStatic m_staticUserName;
	CStatic m_staticPassword;
	int m_proxyProvider;			// Proxy
	CString m_proxyHostName;
	CString m_proxyUserName;
	CString m_proxyPassword;
	CString m_proxyStatus;
	int m_retryCounter;
};
