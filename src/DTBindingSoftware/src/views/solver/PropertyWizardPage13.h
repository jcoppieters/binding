#pragma once

#include "PropertyWizardPageBase.h"


// Sedert V16.44
// Mogelijkheid om een module als master te plaatsen.
// 
// Optie is mogelijk wanneer er bij de physical browse for nodes FE adressen gevonden werden + 
// er is geen master aanwezig.
//
// Momenteel kan het toekennen via de CAN-bus enkel voor een smartbox ultra firmware V66.13.08.
// Dus enkel wanneer deze aanwezig zijn met een FE adres is deze optie mogelijk.
//
// Er wordt enkel gecontroleerd in deze dialoog of er een 0xFC toegekend werd.
// We kunnen niet controleren of de node master vlaggen aanliggen.


// CPropertyWizardPage13 dialog
class CPropertyWizardPage13 : public CPropertyWizardPageBase
{
	DECLARE_DYNAMIC(CPropertyWizardPage13)	
	#define	WM_UPDATE_CONTROLS		(WM_USER + 150)

public:
	CPropertyWizardPage13();
	virtual ~CPropertyWizardPage13();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();

// Dialog Data
	enum { IDD = IDD_DIALOG_WIZARDPAGE13 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage13)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	afx_msg void OnCbnCloseupComboPhysicalAddress();
	afx_msg void OnCbnDropdownComboPhysicalAddress();
	afx_msg void OnBnClickedButtonAssign();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);

	void initTextInfo();
	void initComboPhysicalAddress();
	void updateStateTextBottom();
	void updateStateBtnAssign();
	void updateStateComboBoxPhysicalAddress();
	void updateWizardButtons();
	void sendAssignMasterNodeAddress( );

	// Virtuele functie van de basis 
	virtual void OnNotifyAssignedLogicalAddress(int nodeAddress);

private:	
	// Timer wordt gebruikt om te controleren of er een bericht ontvangen wordt van een physical LA assignment 0xFC.
	// zie OnNotifyAssignedLogicalAddress()
	enum { 
		TIMERID_ASSIGN_ADDRESS_FSM = 1,
		TIMER_VALUE_ASSIGN_ADDRESS_WAIT_MSEC = (5000)
	};


	// NOTE:
	// We kunnen enkel een 0xFC adres toekennen.
	// Dit is geen garantie dat de module als master komt te staan.
	// Wordt enkel ondersteund vanaf firmware V66.13.08.
	// Kunnen geen etra verify uitvoeren. Detectie kan enkel na de physical browse for nodes.
	typedef enum {
		STATE_IDLE = 0,								// Nog niks gedaan van actie.
		STATE_ASSIGN_BUSY,							// Assign commando verstuurd.	
		STATE_ASSIGN_DONE,							// Komt in deze toestand na het toekennen.
		STATE_ASSIGN_FAILED							// Toekennen van het adres mislukt.
	} State_t;

	State_t m_state;								// De toestand.
	BOOL m_fInitDone;								// Bepaalt of de combobox moet ingevuld worden met fysische adressen.
	BOOL m_fUserSelectionBusy;						// Wordt momenteel niet gebruikt.
	BOOL m_showMsgFailed;							// Toont een extra messagebox wanneer het mislukt.
	unsigned int m_selectedPhysicalAddress;			// Het geselecteerde fysisch adres.
	CComboBox m_cmbPhysicalAddress;					// Lijst met fysische adressen van smartbox ultra die als master kunnen geplaatst worden.
	CButton m_btnAssign;							// Toekennen van het 0xFC adres.
};
