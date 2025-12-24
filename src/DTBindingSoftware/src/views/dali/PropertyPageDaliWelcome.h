#pragma once

#include "PropertyPageDaliBaseClass.h"
#include "afxcmn.h"
#include "afxwin.h"

// CPropertyPageDaliWelcome dialog

class CPropertyPageDaliWelcome : public CPropertyPageDaliBaseClass
{
	DECLARE_DYNAMIC(CPropertyPageDaliWelcome)

public:
	CPropertyPageDaliWelcome(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext);
	virtual ~CPropertyPageDaliWelcome();

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnCbnSelchangeComboNodeSelect();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	void initStaticControls( );
	void initDynamicControls( );
	virtual BOOL OnSetActive();			// Wanneer de pagina aktief komt.
	virtual BOOL OnKillActive();		// Wanneer de pagina verlaten wordt.
	void updateGUIStateCfgBridgePossible( );

private:
	// Opslaan van de gegevens wanneer er properties aangepast werden.
	// - Deze functie wordt periodiek aangeroepen via de timer TIMER_ID_POLL_SAVE
	// - Bij verandering van de smartbox ultra selectie (nodig wanneer er meerdere voorzien zijn van een DALI-bus).
	void pollSaveChanges( );

private:
	enum { 
		TIMER_ID_POLL_SAVE			= 0,
		TIMER_SAVE_INTERVAL_MSEC	= 5000
	};

	CComboBox m_comboBoxSelection;
	CButton m_btnConfigure;
	CRichEditCtrl m_richEditWelcomeText;
	CStatic m_staticStateInfoIcon;
	CStatic m_staticStateInfoText;
	CString m_strToolTipText;
	bool m_initDone;
	int m_selectedNodeAddress;
};
