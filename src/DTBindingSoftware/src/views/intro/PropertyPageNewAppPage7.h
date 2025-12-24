#pragma once

#include "PropertyPageNewApp.h"
#include "afxwin.h"
#include "uicontrols/controls/MyProgressCtrl.h"

// Sedert V16.26

// CPropertyPageNewAppPage7 dialog
// Dialoog die de connect uitvoert met een tcp/ip device in het LAN-network.
// Is een vereenvoudige dialoog in vgl met CPropertyPageNewAppPage3 omdat het enkel mogelijk is
// om in het LAN netwerk te connecteren.
class CPropertyPageNewAppPage7 : public CPropertyPageNewApp
{
	DECLARE_DYNAMIC(CPropertyPageNewAppPage7)

public:
	CPropertyPageNewAppPage7(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx);
	virtual ~CPropertyPageNewAppPage7();

	virtual BOOL OnInitDialog();
	virtual void UpdateCommunicationStatus();
	virtual void StoreData( );
	virtual LRESULT OnWizardNext();

// Dialog Data
	enum { IDD = IDD_DIALOG_NEWAPP_PAGE7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	LRESULT OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCheckConnect();
	void showProperties(AppUDPDevice* pDevice);
	bool verifyConnectionSettings(AppUDPDevice& refDevice,
		CString* pStrIPAddress,CString* pStrPassword, int* ipport);

private:
	typedef enum {
		STATE_INIT = 0,				// Init van de dialoog.
		STATE_IDLE,					// Idle - disconnected.
		STATE_CONNECT_BUSY,			// Bezig met het connecteren - wachten op de status.
		STATE_CONNECTED				// Geconnecteerd.
	} State_t;

	State_t m_state;				// De toestand.
	CButton m_btnConnect;			// Check button connect/disconnect.
	CStatic m_staticInfo;			// Info text in de header.
	CEdit m_editDeviceName;			// Edit box device name.
	CEdit m_editIPAddress;			// Edit box IP-address.
	CEdit m_editIPPort;				// Edit box IP-Port.
	CEdit m_editPassword;			// Edit box password.
	CStatic m_staticFooter;			// Info text in de footer.
	CString m_msgInfoBox;			// Text pop-up bericht die assync getoond wordt.
};

