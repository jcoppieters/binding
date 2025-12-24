#pragma once

#include "DlgChildUDPBase.h"
#include "afxwin.h"

// CDlgChildUDPNTP dialog

class CDlgChildUDPNTP : public CDlgChildUDPBase
{
	DECLARE_DYNAMIC(CDlgChildUDPNTP)

public:
	CDlgChildUDPNTP(CWnd* pParent,CDlgUDPConfigCtx& refCtx);   // standard constructor
	virtual ~CDlgChildUDPNTP();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_NTP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnBnClickedCheckEnable();
	afx_msg void OnBnClickedCheckDst();
	virtual void initData(AppUDPDevice& refDevice);				// Algemene data.
	virtual void initStaticControls(AppUDPDevice& refDevice);
	virtual void updateDynamicControls(AppUDPDevice& refDevice);
	virtual void onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent);
	virtual BOOL dataChanged( );
	virtual void onActive(BOOL flag);
	virtual const CString& getStrDescription( ) const;
	BOOL busy();
	bool sendConfigNTP(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);
	bool sendConfigTime(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);

public:
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_BUSY_CFG_NTP,
		STATE_BUSY_CFG_TIME
	} State_t;

	State_t m_state;
	CComboBox m_cmbTimeArea;
	CComboBox m_cmbTimeUTCZone;
	CButton m_btnCheckEnable;
	CButton m_btnCheckAutoSummerWinter;
	CButton m_btnConfigure;
	CStatic m_staticArea;
	CStatic m_staticZone;
	BOOL m_fControlStateEnable;				// Dialoog data - toestand van de checkBox - Enable on/off
	BOOL m_fControlStateDST;				// Dialoog data - toestand van de checkBox - winter/summer.		
	int m_proxyArea;						// proxy.
	int m_proxyZone;						// proxy
	BOOL m_proxyEnable;						// proxy
	BOOL m_proxyAutoSummerWinter;			// proxy
	int m_retryCounter;
};
