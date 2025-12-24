#pragma once

#include "PropertyPageNewApp.h"
#include "afxwin.h"
#include "uicontrols/controls/MyProgressCtrl.h"

// Sedert V16.26

// CPropertyPageNewAppPage6 dialog
// Weergave en instellen van de TCP/IP homeserver inloggegevens van een UDP device.
// Instellen van IP-Port en Password.
//
// Het versturen van de config gebeurd in het timer event. Er wordt gewacht op een 
// antwoord van de UDP server (=tcp/ip device).
// Zolang er geen ACK ontvangen wordt wordt het bericht opnieuw verstuurd.

class CPropertyPageNewAppPage6 : public CPropertyPageNewApp
{
	DECLARE_DYNAMIC(CPropertyPageNewAppPage6)

public:
	CPropertyPageNewAppPage6(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx);
	virtual ~CPropertyPageNewAppPage6();

	virtual BOOL OnInitDialog();
	virtual void UpdateCommunicationStatus();
	virtual void StoreData( );

// Dialog Data
	enum { IDD = IDD_DIALOG_NEWAPP_PAGE6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	LRESULT OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual LRESULT OnWizardNext();
	void showSettings(BOOL enabled,AppUDPDevice* pDevice);
	bool sendConfig(AppUDPDevice& refDevice);
	bool checkData(int* port,CString* pszPassword,CString* strErrorMsg);
	bool checkDefaultPassword( );

private:
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,	
		STATE_CONFIGURE,
		STATE_CONFIGURE_ERROR,
		STATE_CONFIGURE_DONE
	} State_t;

	enum  { 	
		TIMER_ID_SENDCONFIG = 1			// Timer om een Request uit te voeren (gebruikt om assync call).
	};
	enum { 
		NUMBER_OF_RETRIES_SENDCONFIG = 15	// Aantal retries om een config bericht te versturen.
	};

	State_t m_state;					// De toestand.
	CEdit m_editIPPort;					// Edit box IP port
	CEdit m_editPassword;				// Edit box password.
	CButton m_btnConfigure;				// Button configure.
	CStatic m_staticInfo;				// Static info header.
	CStatic m_staticFooter;				// Static info footer.
	CEdit m_editDeviceName;				// Edit box device name
	BOOL m_fWaiting;					// Wordt gebruikt voor het veranderen van de cursor.
	CString m_strMsgBoxInfo;			// Info Bericht die getoond wordt in een pop-up. Wordt assync aangeroepen.
	CString m_strMsgBoxError;			// Error Bericht die getoond wordt in een pop-up. Wordt assync aangeroepen.
	int m_retryCounter;					// Aantal retries om een config bericht te versturen. Zie NUMBER_OF_RETRIES_SENDCONFIG
};

