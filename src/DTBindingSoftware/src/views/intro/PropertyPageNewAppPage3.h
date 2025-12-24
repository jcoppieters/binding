#pragma once

#include "PropertyPageNewApp.h"
#include "afxcmn.h"
#include "afxwin.h"

#include "communication\base\communication_def.h"

// CPropertyPageNewAppPage3 dialog
// Connectie maken met de TCP/IP server.
// Mogelijkheid om lokaal & remote te connecteren.
//
// Sedert V16.26: Detectie node address 0xFE van de tcp/ip server.
// Indien de tcp/ip server een 0xFE adres heeft naar een volgende pagina gaan om deze te configuren 
// als slave / master.

class CPropertyPageNewAppPage3 : public CPropertyPageNewApp
{
	DECLARE_DYNAMIC(CPropertyPageNewAppPage3)

public:
	CPropertyPageNewAppPage3(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx);
	virtual ~CPropertyPageNewAppPage3();

// Dialog Data
	enum { IDD = IDD_DIALOG_NEWAPP_PAGE3 };

	virtual BOOL OnInitDialog();
	virtual void UpdateCommunicationStatus();
	virtual void StoreData( );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);	
	void updateCtrlBtnConnect( );
	afx_msg void OnRadioButtons(UINT nID);
	afx_msg void OnBnClickedCheckConnect();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual LRESULT OnWizardNext();

	enum { WM_TIMER_UPDATE_BTN_CONNECT = 1000 };

	BOOL helper_getSettings(ConnectionMethod_t method, CString* hostAddress, int* port,
		CString* password, CString* cloudServerLoginToken);

private:
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_CONNECT_BUSY,
		STATE_CONNECTED	
	} State_t;

	State_t m_state;						// De toestand.
	int m_radioLocalRemote;					// selectie lokaal/remote connectie.
	CIPAddressCtrl m_CtrlLocalIPAddress;	// Lokaal IP-adres control.
	CEdit m_CtrlLocalIPPort;				// Lokaal IP-port editbox
	CEdit m_CtrlRemoteDns;					// Remote dns editbox
	CEdit m_CtrlRemotePort;					// Remote port editbox.
	CEdit m_CtrlPassword;					// Password edit box
	CButton m_CtrlBtnConnect;				// Checkbox connect: connect/disconnect.
	CStatic m_staticFooter;					// Info message footer
	CString m_msgInfoBox;					// Text pop-up bericht die assync getoond wordt.
};
