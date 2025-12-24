#pragma once

#include "PropertyPageNewApp.h"
#include "afxwin.h"
#include "uicontrols/controls/MyProgressCtrl.h"
#include "afxcmn.h"

// Sedert V16.26

class AppUDPDevices;
class AppUDPDevice;

// CPropertyPageNewAppPage5 dialog
// Weergave en instellen van de TCP/IP Netwerk instellingen van het UDP device.
// Mogelijkheid om in te stellen met een FIX ip-adres of instellen via DHCP.

class CPropertyPageNewAppPage5 : public CPropertyPageNewApp
{
	DECLARE_DYNAMIC(CPropertyPageNewAppPage5)

public:
	CPropertyPageNewAppPage5(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx);
	virtual ~CPropertyPageNewAppPage5();

	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();			// Wanneer de pagina aktief komt.
	virtual BOOL OnKillActive();		// Wanneer de pagina verlaten wordt.
	virtual void UpdateCommunicationStatus();
	virtual void StoreData( );

// Dialog Data
	enum { IDD = IDD_DIALOG_NEWAPP_PAGE5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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
		TIMER_ID_CONFIG_SEND = 1,
		TIMER_ID_CONFIGURE_WAIT = 2
	};

	enum {
		TIMER_VALUE_CONFIGURE_WAIT_MSEC = (1 * 100),
#if(_DEBUG)
		TIMER_COUNTER_CONFIGURE			= (10 * 60)				// 60 seconden.
#else
		TIMER_COUNTER_CONFIGURE			= (10 * 120)			// 120 seconden.
#endif
	};

	DECLARE_MESSAGE_MAP()
	LRESULT OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCheckDhcp();
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void init();
	void showIPSettings(BOOL enabled,const AppUDPDevice* pDevice);
	bool sendConfig(AppUDPDevice& refDevice);

private:
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_CONFIGURE,
		STATE_CONFIGURE_ERROR	
	} State_t;

	State_t m_state;								// De toestand.
	CStatic m_staticInfo;							// Tekst info in de header.
	CStatic m_staticFooter;							// Tekst info in de footer.
	CEdit m_editDeviceName;							// Edit box naam van het device.
	CMyProgressCtrl	m_myProgressCtrl;				// Progressctrl (zelfde als in de taak dialog). Wordt niet getoond in design mode.
	CIPAddressCtrl m_ipAddressCtrlIPAddress;		// Ctrl IP-address 
	CIPAddressCtrl m_ipAddressCtrlNetmask;			// Ctrl Netmask
	CIPAddressCtrl m_ipAddressCtrlGateway;			// Ctrl Gateway.
	CButton m_btnCheckDhcp;							// Check button DHCP on/off
	CButton m_btnConfigure;							// Configure button.
	CString m_strMsgBoxInfo;						// Info bericht die getoond wordt - Assync.
	CString m_strMsgBoxError;						// Fout bericht die getoond wordt - Assync.
	BOOL m_fStateDHCP;								// Gebruikt om de toestand van de DHCP bij te houden + updaten van de GUI.
	BOOL m_fWaiting;								// Cursor staat in waiting toestand.
	int m_intervalCounter;							// Gebruikt als interval zie TIMER_VALUE_CONFIGURE_WAIT_MSEC
	int m_foundCounter;								// zie FOUND_COUNTER_THRESSHOLD. De module moet een aantal keren gevonden worden met de juiste instellingen.
	int m_retryCounter;								// zie NUMBER_OF_SEND_RETRIES
};

