#pragma once
/*****************************************************************/

#include "app/appudpcfgserver/appudpservercfgwrapper.h"		// UDP Device wrapper

class CDlgUDPConfigCtx;
class AppUDPDevice;
// class AppUDPServerCfgWrapper;		// Nodig voor de functie pointer...
struct IAppDlgUDPLanCfg_Host;

namespace DUOTECNO {
	class CDlgHelperFont;
};

/*****************************************************************/
#define WM_UPDATE_CONTROLS	(WM_USER + 150)
/*****************************************************************/

// CDlgChildUDPBase dialog
// Basis dialoog voor de child dialogen voor UDP config.
class CDlgChildUDPBase : public CDialog
{
	DECLARE_DYNAMIC(CDlgChildUDPBase)

public:
	CDlgChildUDPBase(UINT id, CWnd* pParent,CDlgUDPConfigCtx& refCtx);   // standard constructor
	virtual ~CDlgChildUDPBase();

	// Deze wordt aangeroepen door de parent bij het veranderen van een tabblad.
	// De return waarde bepaalt of we mogen veranderen van tabblab.
	// TRUE: mag naar een andere gaan.
	// FALSE: Moeten hier blijven.
	//
	// Deze functie roept de virtuele functies busy() en dataChanged() aan.	
	BOOL onLeave( ); 

	// Deze wordt aangeroepen door de parent wanneer een dialoog actief komt of verlaten wordt.
	// Boolean bepaalt de toestand: 
	// Wanneer true active dialoog, false is verlaten van de dialoog.
	//
	// De afgeleide dialogen kunnen deze gebruiken om:
	// Bij het openen: Tekst weer te geven in de parent dialoog.
	// Bij het verlaten: Timers stoppen.
	virtual void onActive(BOOL flag);	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	//////////////////////////////////////////////////////////////////////
	// Helper functies progressbar toestand.
	void setProgressBarBusy( );
	void setProgressBarError( );
	void setProgressBarDone( );


	//////////////////////////////////////////////////////////////////////
	// Helper functies voor configuratie.
	enum { 
		UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC = 2500,		// Tijd na het versturen dat er gewacht wordt op een ACK 
		UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC = 3000,		// Tijd na het versturen dat er gewacht wordt op een antwoord.
		UDPCFG_SEND_NR_OF_RETRIES = 10,							// Aantal keren dat de cfg opnieuw verstuurd wordt.
		UDPCFG_REQUEST_NR_OF_RETRIES = 10,						// Aantal keren dat de request opnieuw verstuurd wordt.
		UDPCFG_SEND_INTERVAL_MSEC = 100							// interval in msec wachten met het het versturen van de cfg.				
	};

	bool sendCfgNetwork(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds = UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC);
	bool sendCfgDNSProvider(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds = UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC);
	bool sendCfgDynDnsClient(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds = UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC);
	bool sendCfgTCPIPClient(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds = UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC);
	bool sendCfgSystemMode(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds = UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC);
	bool sendCfgHomeserver(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds = UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC);
	bool sendCfgNTP(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds = UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC);
	bool sendCfgTimeDate(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds = UDPCFG_SEND_VALUE_WAIT_FOR_RESPONSE_MSEC);

	bool sendRequestDynDnsStatus(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode, int milliseconds = UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC);
	bool sendRequestTCPIPClientStatus(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode,int milliseconds = UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC);

	//////////////////////////////////////////////////////////////////////
	// Helper functies voor de statusMsg tekst.
	typedef enum {
		ID_MSG_STATUS_CONFIG_BUSY = 0,
		ID_MSG_STATUS_CONFIG_RETRY,
		ID_MSG_STATUS_CONFIG_DONE,
		ID_MSG_STATUS_CONFIG_FAILED,
		ID_MSG_STATUS_CONFIG_VERIFY,
		ID_MSG_STATUS_CONFIG_FORMATTER,		// Sedert V16.31
		ID_MSG_STATUS_REQUEST_BUSY,
		ID_MSG_STATUS_REQUEST_RETRY,
		ID_MSG_STATUS_REQUEST_DONE,
		ID_MSG_STATUS_REQUEST_FAILED
	} MsgIdStatusText_t;
	
	void setStatusMsg(MsgIdStatusText_t stringID);
	void setMsgDescription();

	//////////////////////////////////////////////////////////////////////
	// Helper functie.
	// Toont een messagebox wanneer er velden niet geldig zijn waardoor er
	// niet kan geconfigureerd worden.
	void showMsgBoxInvalidFields(const CString& fieldname);
	void showMsgBoxInvalidFields(const CString& fieldname,const CString& info);


	//////////////////////////////////////////////////////////////////////
	// Pure virtuele functies.
	// De GUI controls updaten met data van het AppUDPDevice 
	virtual void initData(AppUDPDevice& refDevice) = 0;							// Data ophalen van het device.
	virtual void initStaticControls(AppUDPDevice& refDevice) = 0;				// In de init - overschrijven door afgeleide klassen.
	virtual void updateDynamicControls(AppUDPDevice& refDevice) = 0;			// Wordt assync aangeroepen.
	virtual void onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent) = 0;	// De timer Expiry.
	virtual const CString& getStrDescription( ) const = 0;

	///////////////////////////////////////////////////////////////////////
	// Virtuele functies.
	virtual BOOL dataChanged( ) { return FALSE; }
	virtual BOOL busy() { return FALSE; }

	// Helper functies voor de GUI.
	int ComboBoxAddEntry(CComboBox* pCtrl,int idx,const char* szCaption,unsigned long ulItemData);
	int updateComboBoxSel(CComboBox* pCtrl,int value);
	int getItemData(CComboBox& refCtrl,int* pVal);
	CString getString(CWnd& refCtrl);

	// Helper functie.
	// Bij de NXP modules moet er na het doorsturen van de cfg een hardware reset gebeuren.
	// Deze functie toont een msgbox met deze melding.
	void notifyUserHardwareReset( );

	// Opvragen van de ctx.
	CDlgUDPConfigCtx& getUdpCfgCtx();

	// Helper functie.
	static bool convertIPAddress(int* ipAddrV4, const CString& refAddress);


	enum {
		TIMER_ID_CONFIG_SEND = 1,			// Timer voor het versturen van de config + evt retries.
		TIMER_ID_STATUS_REQUEST = 2,		// Timer voor het versturen van de request
		TIMER_ID_CONFIGURE_WAIT = 3			// Timer voor de verify.
	};

private:
	// Helper functies voor de send & request...
	bool sendCfg(AppUDPDevice& refDevice,int milliseconds,bool (AppUDPServerCfgWrapper::*pf)(AppUDPDevice& refDevice,int milliseconds), 
		AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);
	bool sendRequest(AppUDPDevice* pDevice,int milliseconds,bool (AppUDPServerCfgWrapper::*pf)(AppUDPDevice* pDevice,int milliseconds), 
		AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);

	// Zie onSetCursor.
	void setCursorWaiting(BOOL flag) {
		m_fWaiting = flag;
	}

private:
	CDlgUDPConfigCtx& m_refCtx;			// De context voor de dialogen.
	DUOTECNO::CDlgHelperFont* m_pDlgFontHelper;
	BOOL m_fWaiting;					// Cursor waiting: Zie functie OnSetCursor()
	BOOL m_fShowMsgHardwareReset;		// Wordt aangeroepen door de afgeleide klasse (zie notifyUserHardwareReset())
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// Context klasse.
class CDlgUDPConfigCtx
{
public:
	CDlgUDPConfigCtx(CWnd* pParentWnd,IAppDlgUDPLanCfg_Host& refHost,const AppUDPDevice& refDevice);
	~CDlgUDPConfigCtx( );

	IAppDlgUDPLanCfg_Host& getHost() {
		return m_refHost;
	}
	
	// Status bericht.
	void setStatusMsg(const CString& msg);
	const CString& getStatusMsg( ) {
		return m_statusMsg;
	}

	// Progressbar toestand (in de hoofddialoog.)
	typedef enum {
		PROGRESSBAR_STATE_INVISIBLE = 0,
		PROGRESSBAR_STATE_BUSY,
		PROGRESSBAR_STATE_DONE,
		PROGRESSBAR_STATE_ERROR
	} ProgressBarState_t;

	ProgressBarState_t getProgressBarState( ) const {
		return m_stateProgressBar;
	}
	void setProgressBarState(ProgressBarState_t state);

	// Gui status vlaggen.
	bool statusMsgChanged( ) const;
	bool stateProgressBarChanged( ) const;
	void clearGuiChangesFlags() {
		m_guiChangesFlags = 0;
	}

	// Proxy AppUDPDevice device.
	AppUDPDevice& getUDPDevice();
	void setUDPDevice(const AppUDPDevice& r);

	// Parent window.
	CWnd* getParentWnd( ) {
		return m_pParentWnd;
	}

private:
	enum {
		FLAG_GUI_CHANGED_STATUSMSG		= (1 << 0),
		FLAG_GUI_CHANGED_PROGRESSBAR	= (1 << 1),	
	};

	// NOTE:
	// Er wordt een deep copy bijgehouden van een bestaand device uit de lijst.
	// Deze copy wordt aangepast volgens de instellingen van de dialogen en gebruikt om de config 
	// door te sturen...

	CWnd* const m_pParentWnd;
	IAppDlgUDPLanCfg_Host& m_refHost;
	AppUDPDevice* m_proxyUdpDevice;					// Is een deep copy van een bestaande device (NOTE)
	ProgressBarState_t m_stateProgressBar;
	CString m_statusMsg;
	int m_guiChangesFlags;
};

/*****************************************************************/