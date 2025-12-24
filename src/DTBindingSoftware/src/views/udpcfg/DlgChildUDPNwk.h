#pragma once

#include "DlgChildUDPBase.h"

// CDlgChildUDPNwk dialog

class CDlgChildUDPNwk : public CDlgChildUDPBase
{
	DECLARE_DYNAMIC(CDlgChildUDPNwk)

public:
	CDlgChildUDPNwk(CWnd* pParent,CDlgUDPConfigCtx& refCtx);   // standard constructor
	virtual ~CDlgChildUDPNwk();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_NWKSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void initData(AppUDPDevice& refDevice);				// Algemene data.
	virtual void initStaticControls(AppUDPDevice& refDevice);		// In de init - overschrijven door afgeleide klassen.
	virtual void updateDynamicControls(AppUDPDevice& refDevice);	// Wordt assync aangeroepen.
	virtual void onTimerExpiry(AppUDPDevice& refDevice,UINT_PTR nIDEvent);
	virtual BOOL dataChanged( );
	virtual BOOL busy();
	virtual void onActive(BOOL flag);
	virtual const CString& getStrDescription( ) const;
	afx_msg void OnBnClickedCheckDhcp();
	afx_msg void OnBnClickedButtonConfigure();
	bool sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);

protected:
	// Scan mode van de UDP devices.
	// Verhogen wanneer we een verify uitvoeren na de configuratie.
	void setUDPScanModeFast( );
	void restoreUDPScanModeNormal( );

private:

	enum { 
		// Het aantal keren er geprobeerd wordt om de network config door te sturen.
		NUMBER_OF_SEND_RETRIES = 5,	

		// Het aantal keren het device in het netwerk moet gevonden worden met de juiste 
		// netwerk instellingen.
		// Dit is nodig omdat het device soms nog in de lijst komt met oude instellingen 
		// of tijdens DHCP andere addressen krijgt.
		FOUND_COUNTER_THRESSHOLD = 2
	};

	enum {

		TIMER_VALUE_CONFIGURE_WAIT_MSEC = (1 * 100),

#if(_DEBUG)
		TIMER_COUNTER_CONFIGURE			= (10 * 60)				// 60 seconden.
#else
		TIMER_COUNTER_CONFIGURE			= (10 * 120)			// 120 seconden.
#endif
	};

	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE = 1,
		STATE_BUSY = 2
	} State_t;

	State_t m_state;
	CIPAddressCtrl m_ipAddressCtrlIPAddress;		// Ctrl IP-address 
	CIPAddressCtrl m_ipAddressCtrlNetmask;			// Ctrl Netmask
	CIPAddressCtrl m_ipAddressCtrlGateway;			// Ctrl Gateway.
	CButton m_btnCheckDhcp;							// Check button DHCP on/off
	CButton m_btnConfigure;							// Configure button.
	BOOL m_fControlStateDHCP;						// Dialoog data - toestand van de checkBox.
	int m_proxyIpV4[4];								// Proxy
	int m_proxyNetmask[4];							// Proxy
	int m_proxyGateway[4];							// Proxy
	BOOL m_proxyStateDHCP;							// Proxy
	int m_intervalCounter;							// Gebruikt als interval zie TIMER_VALUE_CONFIGURE_WAIT_MSEC
	int m_retryCounter;								// zie NUMBER_OF_SEND_RETRIES
	int m_foundCounter;								// zie FOUND_COUNTER_THRESSHOLD. De module moet een aantal keren gevonden worden met de juiste instellingen.
	BOOL m_fScanModeFast;							// gebruiken bij een restore
};
