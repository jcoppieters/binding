#pragma once

#include "PropertyPageNewApp.h"
#include "afxwin.h"
#include "uicontrols/controls/MyProgressCtrl.h"

// Sedert V16.26

class AppUDPDevice;
class AppUDPDevices;

// CPropertyPageNewAppPage4 dialog
// Zoekt naar devices in het LAN-network.
// Indien mislukt mogelijkheid om opnieuw te zoeken + evt een andere netwerk adapter te selecteren.
// Wanneer er devices gevonden zijn dan worden deze weergegeven + worden de eigenschappen getoond.
class CPropertyPageNewAppPage4 : public CPropertyPageNewApp
{
	DECLARE_DYNAMIC(CPropertyPageNewAppPage4)

	enum  { 
		TIMER_ID_UPDATEDEVICES = 1,				// Timer om combobox up te daten met gevonden devices.
		TIMER_ID_REQUESTDEVICEINFO = 2			// Timer om een Request uit te voeren (gebruikt om assync call).
	};

public:
	CPropertyPageNewAppPage4(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx);
	virtual ~CPropertyPageNewAppPage4();
	virtual BOOL OnInitDialog();
	virtual void UpdateCommunicationStatus();
	virtual void StoreData( );

// Dialog Data
	enum { IDD = IDD_DIALOG_NEWAPP_PAGE4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	LRESULT OnUpdateControls(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();			// Wanneer de pagina aktief komt.
	virtual BOOL OnKillActive();		// Wanneer de pagina verlaten wordt.
	afx_msg void OnCbnCloseupComboDevice();
	afx_msg void OnCbnDropdownComboDevice();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckConnect();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnBnClickedButtonSettings();
	void updateListDevicesFound();
	void updateListDevicesFound(AppUDPDevices& refDevices);
	void addDevice(AppUDPDevice& refDevice);	
	bool requestDeviceProperties();
	void showUDPDeviceProperties(BOOL show,AppUDPDevice* pDevice);
	void searchAgain(BOOL clearList);

private:
	typedef enum {
		STATE_INIT = 0,					// Bij het openen van de dialoog.
		STATE_SEARCH_DEVICES,			// Zoeken naar devices.		
		STATE_SEARCH_DEVICES_TIMEOUT,	// Zoeken naar devices - geen gevonden.
		STATE_DEVICE_REQUEST,			// Opvragen van de properties van een device.
		STATE_DEVICE_RESPONSE_OK,		// Properties ontvangen - kunnen met deze connecteren.
		STATE_DEVICE_RESPONSE_EC		// Kunnen niet met deze connecteren - Afzonderlijke error code... 
	} State_t;

	typedef enum {
		ERROR_NONE = 0,
		ERROR_RESPONSE_TIMEOUT = -1,
		ERROR_PARAMETERS = -2,
		ERROR_GENERAL = -3
	} ErrorCode_t;

	enum {
		// Max aantal seconden dat er gezocht wordt naar UDP devices op het LAN network.
#if (_DEBUG)
		COUNTER_VALUE_SEARCH_DEVICES_SEC			= 30,
#else
		COUNTER_VALUE_SEARCH_DEVICES_SEC			= 120,
#endif 

		// Aantal keren dat er geprobeerd wordt om de info op te vragen van een UDP device
#if (_DEBUG)
		COUNTER_THRESSHOLD_REQUEST_DEVICEINFO		= 3,
#else
		COUNTER_THRESSHOLD_REQUEST_DEVICEINFO		= 10,		
#endif 
	};

	State_t m_state;					// De toestand van de dialoog. zie enum klasse.
	ErrorCode_t m_errorCode;			// De huidige fout. zie enum klasse.
	int m_retryCounter;					// Wordt gebruikt als extra counter voor serach interval en retry counter voor response van een UDP device.
	CMyProgressCtrl	m_myProgressCtrl;	// Progressctrl (zelfde als in de taak dialog).
	CStatic m_staticStatusInfo;			// Status tekst			
	CStatic m_groupDeviceInfo;			// Group box device info.
	CStatic m_staticDevice;				// Static tekst device.
	CStatic m_staticIPAddress;			// Static tekst
	CStatic m_staticIPPort;				// Static tekst
	CStatic m_staticPassword;			// Static tekst
	CStatic m_staticFooter;				// Status text onderaan.
	CEdit m_editIPAddress;				// Weergave UDP device instellingen.
	CEdit m_editIPPort;					// Weergave UDP device instellingen.
	CEdit m_editPassword;				// Weergave UDP device instellingen.
	CButton m_btnSearch;				// Button om oppnieuw te zoeken bij een fout.
	CButton m_btnSettings;				// Button om de dialoog van de netwerk adapters te openen.
	CComboBox m_comboDevice;			// Combobox found devices.
	BOOL m_fUserSelectionBusy;			// Gebruiker is bezig met een selectie te maken in de combobox.
	BOOL m_fWaiting;					// Aan het wachten op response van een UDP device - Wordt gebruikt voor de Wait cursor.	
};
