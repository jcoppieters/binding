#pragma once

#include "DlgChildUDPBase.h"
#include "afxwin.h"
#include "afxcmn.h"

// CDlgChildUDPDns dialog

class CDlgChildUDPDns : public CDlgChildUDPBase
{
	DECLARE_DYNAMIC(CDlgChildUDPDns)

public:
	CDlgChildUDPDns(CWnd* pParent,CDlgUDPConfigCtx& refCtx);   // standard constructor
	virtual ~CDlgChildUDPDns();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DNS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonConfigure();
	virtual void initData(AppUDPDevice& refDevice);					// Algemene data.
	virtual void initStaticControls(AppUDPDevice& refDevice);		// In de init - overschrijven door afgeleide klassen.
	virtual void updateDynamicControls(AppUDPDevice& refDevice);	// Wordt assync aangeroepen.
	virtual void onTimerExpiry(AppUDPDevice& refDevice,UINT_PTR nIDEvent);
	virtual BOOL dataChanged( );
	virtual void onActive(BOOL flag);
	virtual const CString& getStrDescription( ) const;
	BOOL busy();
	bool sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);

private:
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_BUSY	
	} State_t;

	State_t m_state;
	CButton m_btnConfigure;
	CIPAddressCtrl m_ctrlIpAddress;
	int m_proxyIpaddress[4];
	int m_retryCounter;
};
