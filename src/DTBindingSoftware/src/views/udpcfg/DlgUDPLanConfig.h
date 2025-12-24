#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "DlgChildUDPBase.h"

#include "uicontrols/controls/MyProgressCtrl.h"

/******************************************************************************/
class CDlgUDPConfigFsm;			

namespace DUOTECNO {
	class CDlgHelperFont;
};

// CDlgUDPLanConfig dialog
#define WM_UPDATE_CONTROLS		(WM_USER + 150)
/******************************************************************************/


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
class CDlgUDPLanConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgUDPLanConfig)

public:
	CDlgUDPLanConfig(IAppDlgUDPLanCfg_Host& refHost, const AppUDPDevice& refDevice, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUDPLanConfig();

	// Public static 
	static BOOL Show(CWnd* pWndParent, IAppDlgUDPLanCfg_Host& refHost, const AppUDPDevice& refDevice);
	static CDlgUDPLanConfig* getInstance();	
	static BOOL IsActive();	

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();	
	afx_msg void OnTcnSelchangeTabSelect(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTabSelect(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	enum {
		TIMER_ID_POLL = 1,
		TIMER_ID_PROGRESSBAR = 2,
	};

// Dialog Data
	enum { IDD = IDD_DIALOG_UDPCONFIG };

protected:
	typedef enum {
		TAB_SELECTION_NWKSETTINGS = 0,		// Niet veranderen - index in array
		TAB_SELECTION_HOMESERVER = 1,		// Niet veranderen - index in array	
		TAB_SELECTION_APICFG = 2,			// Niet veranderen - index in array	
		TAB_SELECTION_DNS  = 3,				// Niet veranderen - index in array
		TAB_SELECTION_DYNDNS  = 4,			// Niet veranderen - index in array
		TAB_SELECTION_NTP = 5,				// Niet veranderen - index in array
		TAB_SELECTION_SYSTEMMODE = 6,		// Niet veranderen - index in array
		TAB_SELECTION_TCPIP = 7,			// Niet veranderen - index in array
		TAB_SELECTION_MAX					// Niet veranderen - Laatste
	} TabSelection_t;

	void show(const AppUDPDevice& refDevice);
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual void PostNcDestroy();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	BOOL Create();
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	void initStaticControls( );
	void createTabs( );
	void addTab(int index, TabSelection_t tab);
	void addDialog(TabSelection_t tab);
	void showActiveDialog(TabSelection_t tab);
	BOOL getCurrentSelected(TabSelection_t *tab);
	void removeTabCtrlItems( );
	void deallocSubPages( );

private:
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_BUSY,	
	} State_t;

	static CDlgUDPLanConfig* m_pInstance;
	CDlgUDPConfigCtx* const m_pCtx;
	CDlgUDPConfigFsm* const m_pFsm;
	DUOTECNO::CDlgHelperFont* m_pDlgFontHelper;
	State_t m_state;
	CMyProgressCtrl	m_myProgressCtrl;	// Progressctrl (zelfde als in de taak dialog).
	CTabCtrl m_tabCtrlSelection;		// Ctrl: Tab control selectie...
	CStatic m_staticInfoMsg;			// Ctrl: Static info msg.
	CEdit m_editDeviceName;				// Ctrl: Device Name.
	CDlgChildUDPBase* m_pChildDialogs[TAB_SELECTION_MAX];
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
// FSM voor de dialoog
class CDlgUDPConfigFsm
{
public:
	// Mogelijke events..
	typedef enum {
		EV_POLL = 0,		// Aangroepen vanuit een timer...	
	} Event_t;

	// Return waarde voor de pollFsm.
	typedef enum {
		RESULT_SUCCESS = 0,
		RESULT_EC_GENERAL = -1
	} Result_t;

	typedef enum {
		FSM_STATE_IDLE = 0,
		FSM_STATE_BUSY,
		FSM_STATE_ERROR,
		FSM_STATE_DONE	
	} GlobalFsmState_t;

	CDlgUDPConfigFsm(CDlgUDPConfigCtx &refCtx);
	GlobalFsmState_t pollFsm(Event_t ev);	
	void reset( );	// Opnieuw starten.
	
protected:
	GlobalFsmState_t getState() const;

	enum { 
		UDPCFG_REQUEST_VALUE_WAIT_FOR_RESPONSE_MSEC = 2500,	// Tijd na het versturen dat er gewacht wordt op een antwoord 
		UDPCFG_REQUEST_NR_OF_RETRIES = 10,						// Aantal keren dat de cfg opnieuw verstuurd wordt.			
	};
		
private:
	typedef enum {		
		STATE_IDLE = 0,
		STATE_REQUEST_FLAGS,		// Opvragen van de supported flags.
		STATE_REQUEST_HS,			// Opvragen van de Homeserver app.
		STATE_REQUEST_DDNS,			// Opvragen van de Dyn Dns
		STATE_REQUEST_SYSMODE,
		STATE_REQUEST_TCPIPCLIENT,
		STATE_REQUEST_DNSPROVIDER,
		STATE_REQUEST_SNTP,
		STATE_REQUEST_TIME,
		STATE_REQUEST_DONE,
		STATE_REQUEST_ERROR,
	} State_t;

	CDlgUDPConfigCtx &m_refCtx;			// de context.
	State_t m_state;					// de toestand --> Of deze bijhouden in de ctx??
	int m_retryCounter;
};
/*****************************************************************/