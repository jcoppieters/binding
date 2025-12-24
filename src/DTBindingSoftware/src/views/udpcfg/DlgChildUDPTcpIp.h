#pragma once

#include "DlgChildUDPBase.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgChildUDPTcpIp dialog

class CDlgChildUDPTcpIp : public CDlgChildUDPBase
{
	DECLARE_DYNAMIC(CDlgChildUDPTcpIp)

public:
	CDlgChildUDPTcpIp(CWnd* pParent,CDlgUDPConfigCtx& refCtx);   // standard constructor
	virtual ~CDlgChildUDPTcpIp();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_TCPIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnBnClickedButtonRequest();
	virtual void initData(AppUDPDevice& refDevice);					// Algemene data.
	virtual void initStaticControls(AppUDPDevice& refDevice);		// In de init - overschrijven door afgeleide klassen.
	virtual void updateDynamicControls(AppUDPDevice& refDevice);	// Wordt assync aangeroepen.
	virtual void onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent);
	virtual BOOL dataChanged( );
	virtual void onActive(BOOL flag);
	virtual const CString& getStrDescription( ) const;
	BOOL busy();
	bool sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);
	bool sendRequestStatus(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);
	int getCtrlIPPort( );

private:
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_BUSY_SEND,
		STATE_BUSY_REQUEST
	} State_t;

	State_t m_state;				// de toestand.
	CIPAddressCtrl m_ctrlIPAddress;	// Control IP-Address
	CEdit m_editIPPort;				// Control IP-port
	CButton m_btnConfigure;			// Control configure btn.
	CEdit m_editStatus;				// Control status info text: 0 = disconnected, 1 = connected.
	CButton m_btnRequest;			// Control request btn.
	int m_proxyIpaddress[4];		// proxy
	int m_proxyPort;				// proxy
	int m_proxyStatus;				// proxy 0 = disconnected, 1 = connected.
	int m_retryCounter;				// counter aantal keren opnieuw proberen.
};
