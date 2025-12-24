#pragma once

#include "PropertyPageNewApp.h"
#include "afxwin.h"

// Sedert V16.26

// CPropertyPageNewAppPage8 dialog
// Instellen van het tcp/ip device als master of als slave in het CAN-bus netwerk.
//
// NOTE-POLLING.
// We gebruiken een polling mechanisme om te controleren of het device een logisch adres / als master ingesteld is.
// Na het configureren starten we een timer. Na het verlopen van deze timer vragen we de node info op.
// A.d.h.v. deze node info kunnen we controleren of de node veranderd is van een 0xFE naar een master of naar
// een slave met een node address.


// TODO: 
// Bij een bestaande installatie een controle doen op het physical adres.
// Indien het fysisch adres ook aanwezig is in de node database dan gewoon een replace doen met
// het vorige node address.
class CPropertyPageNewAppPage8 : public CPropertyPageNewApp
{
	DECLARE_DYNAMIC(CPropertyPageNewAppPage8)

public:
	CPropertyPageNewAppPage8(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx);
	virtual ~CPropertyPageNewAppPage8();

	virtual BOOL OnInitDialog();
	virtual void UpdateCommunicationStatus();
	virtual void StoreData( );

// Dialog Data
	enum { IDD = IDD_DIALOG_NEWAPP_PAGE8 };
	enum { TIMER_ID_BUSY = 1 };

	enum {
		// NOTE-POLLING.
		// Tijd die er gewacht wordt na het versturen van een assign master/slave node address om de 
		// node info op te vragen.
		TIMER_VALUE_WAIT_REQUEST_MSEC = 1000,

		// NOTE-POLLING.
		// Tijd die er gewacht wordt na het versturen van een request node info en controleren van de node vlaggen.
		// Er wordt een langere tijd gewacht omdat de tcp/ip andere data kan versturen en hierdoor niet direct
		// antwoord op de request.
		// In de toestand 'STATE_BUSY_VERIFY' wordt er een aantal keren opnieuw geprobeerd. zie COUNTER_WAIT_RESPONSE
		TIMER_VALUE_VERIFY_MSEC	 = 1500
	};
	enum { 
		// Het aantal keren dat we node info opnieuw opvragen in de toestand STATE_BUSY_VERIFY.
		COUNTER_WAIT_RESPONSE = 8	
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	LRESULT OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnRadioButtons(UINT nID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void initComboBoxNodeAddresses();
	bool verifyCfg(CString* pMsgError);

private:	
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_BUSY,			// NOTE-POLLING.
		STATE_BUSY_VERIFY,	// NOTE-POLLING.
		STATE_DONE,			// NOTE-POLLING.
		STATE_ERROR
	} State_t;

	enum {
		RADIOBUTTON_SELECTION_MASTER = 0,	// Radio button selectie waarde.
		RADIOBUTTON_SELECTION_SLAVE = 1		// Radio button selectie waarde.
	};

	enum {
		SLAVE_NODEADDRESS_FIRST = 0x03,		// eerste slave node address
		SLAVE_NODEADDRESS_LAST = 0xfb,		// laatste slave node address.
		SLAVE_NODEADDRESS_DEFAULT = 0xf0	// Deze wordt als standaard geselekteerd.
	};

	State_t m_state;					// De toestand.
	CButton m_btnConfigure;				// Configure button: als slave of master.
	CComboBox m_cmbNodeAddress;			// Combo met het logisch adres.
	CStatic m_staticNodeAddress;		// Static text buddy van de combobox.
	CStatic m_staticInfoHeader;			// Algemene text info in de header van de dialoog.
	CStatic m_staticInfoFooter;			// Algemene text info in de footer van de dialoog.
	CString m_msgError;					// String die een foutbericht bevat. Deze wordt assynchroon weergeven in de OnUpdateControls().
	CString m_msgInfo;					// String die getoond wordt in een pop-up wanneer de actie voltooid is.
	unsigned long m_physicalNodeAddress;// Het fysich adres van de server.
	BYTE m_selectedNodeAddress;			// Het node address die geselecteerd werd om in te stellen bij de slave.
	int m_assignedNodeAddress;			// Het node address die toegekend werd (als slave of als master).
	int m_selectionCfgNodeAddress;		// radio button selection: zie enum master / slave.
	int m_responseCounter;				// Extra counter om te verifieren of het device ingesteld werd - zie COUNTER_WAIT_RESPONSE.
	bool m_assignMasterSupported;		// Bepalen in de onInitDialog() of het device als master kan ingesteld worden. nodig voor de gui.
	bool m_showPopupMsgBox;				// Foefel die ervoor zorgt dat de messagebox getoond worden nadat de GUI upgedate werd.
};
