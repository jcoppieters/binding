#pragma once

#include "PropertyPageNewApp.h"
#include "afxwin.h"


// CPropertyPageNewAppPage2 dialog

class CPropertyPageNewAppPage2 : public CPropertyPageNewApp
{
	DECLARE_DYNAMIC(CPropertyPageNewAppPage2)

public:
	CPropertyPageNewAppPage2(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx);
	virtual ~CPropertyPageNewAppPage2();

// Dialog Data
	enum { IDD = IDD_DIALOG_NEWAPP_PAGE2 };

	virtual BOOL OnInitDialog();
	virtual void UpdateCommunicationStatus();
	virtual void StoreData( );
	virtual LRESULT OnWizardNext();		// Sedert V16.26
	virtual LRESULT OnWizardBack();		// Sedert V16.26

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);	
	afx_msg void OnRadioButtonsMode(UINT nID);
	afx_msg void OnCbnCloseupComboSettingsPort();
	afx_msg void OnCbnDropdownComboSettingsPort();
	afx_msg void OnBnClickedCheckUsbconnect();
	afx_msg void OnBnClickedCheckLANSearch();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void updateListComports();
	int getDefaultUSBPort( );		// waarde tss 1 en 255, -1 is ongeldig
	void displayMsgDefaultUsbPort(int port);

private:
	enum { WM_TIMER_UPDATE_USB_PORTS = 0 };

	BOOL m_fUserSelectionBusy;			// Busy selection in USB Com poort combobox.
	BOOL m_fBusyConnecting;				// Bezig met connecteren.
	BOOL m_fConnected;					// Geconnecteerd.
	BOOL m_fLocalTCPIPSettingsAvailable;
	int m_settingsUsbPort;				// Com-poort 0 tem 254			
	CString m_displayMsg;				// Bevat een bericht die getoond wordt bij het assyncupdaten van de controls. 
										// Zorgt ervoor dat alle andere berichten in de scrollview gewist worden 
	int m_radioMode;
	CStatic m_staticGroupUSBModem;
	CStatic m_staticUSBPort;
	CComboBox m_ComboUsbPort;
	CButton m_BtnCheckConnect;
	CStatic m_staticGroupSearchLAN;
	CButton m_btnCheckSearchLAN;
	CStatic m_staticInfoFooter;
	CStatic m_staticInfoHeader;
};
