#pragma once

// CDlgFirmwareUpdate dialog
#include "AppFirmwareUpdateWorker.h"

class CDlgFirmwareUpdate_Host;

class CDlgFirmwareUpdate : public CDialog
{
	DECLARE_DYNAMIC(CDlgFirmwareUpdate)

public:
	CDlgFirmwareUpdate(CWnd* pParent,const CString& title);   // standard constructor
	virtual ~CDlgFirmwareUpdate();

	void init(AppFirmwareUpdaterWorker& refWorker,const CString& strIpAddress);

// Dialog Data
	enum { IDD = IDD_DIALOG_FIRMWAREUPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Generated message map functions
	virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonNodeRefresh();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRelease();
	afx_msg void OnBnClickedButtonBeta();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnBnClickedCheckShowAdvanced();
	afx_msg void OnCbnSelchangeComboProduct();
	afx_msg void OnCbnSelchangeComboPlatform();
	afx_msg void OnCbnDropdownComboProduct();
	afx_msg void OnCbnCloseupComboProduct();
	afx_msg void OnCbnDropdownComboPlatform();
	afx_msg void OnCbnCloseupComboPlatform();
	afx_msg void OnBnClickedCheckShowBeta();
	afx_msg void OnBnClickedCheckShowDowngrade();
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	LONG onWmUpdateControls(WPARAM wParam, LPARAM lParam);

	// Helper functies.
	void updateGuiNodeInfo();					// Updaten GUI: Node info: current firmware versie,productinfo,...
	void updateGuiFirmwareAvailableInfo();		// Updaten GUI: Mogelijke firmware versies in de cloude gevonden.
	void updateGuiStatusInfoMsg();				// Updaten GUI: Status edit control
	void updateGuiMessageBox();					// Updaten GUI: Info of fout bericht tonen in een messagebox.
	void updateGUIFlags( );						// Update GUI vlaggen --> Deze worden gebruikt door de andere update functies.

public:
	void updateGuiListboxMessage(const CString& msg,bool eraseContent=false);
	void initComboPlatform(AppFirmwareUpdaterWorker::HardwarePlatform_t platform);
	void initComboProduct(AppFirmwareUpdaterWorker::Product_t product);

private:
	enum { TIMER_ID_POLL = 1, TIMER_ID_TASK_START = 2 };
	enum { TIMER_POLL_VALUE_MSEC = 250 };		// NIET MEER AANPASSEN - WORDT GEBRUIKT DOOR DE WORKER.

	enum {
		GUI_UPDATE_NODE_CONNECTED						= (1 << 0),
		GUI_UPDATE_VALIDHOST_ADDRESS					= (1 << 1),
		GUI_UPDATE_RECIEVED_FIRMWARE_CLOUD_VERSIONS		= (1 << 2),
		GUI_UPDATE_WORKERFSM_BUSY						= (1 << 3),
		GUI_UPDATE_ADVANCED_OPTIONS_SELECTED			= (1 << 4),	
	};
	
	const CString m_strDlgTitle;					// title van de dialoog: Naam en physical address van de node.
	AppFirmwareUpdaterWorker* m_pWorker;			// Implementatie worker.
	int m_guiFlags;									// Vlaggen voor het updaten van de GUI.
	bool m_updateFirmwareInfo;				// Updaten van de Available firmware controls.	--> TODO OPTIMALISEREN.
	bool m_showExpertMode;					// Mogelijkheid om enkel de Advanced opties te tonen in expert mode.
	CString m_hostAddress;					// Host address wordt meegegeven in de init()
	CString m_errorMsg;						// Error msg - Messagebox dialoog
	CString m_infoMsg;						// info msg - MessageBox dialoog
	CString m_statusMsg;					// Status bericht wordt getoond in de m_editStatus control
	CButton m_btnRefreshNodeInfo;			// Control Button: Node info Refresh
	CStatic m_staticCurrentVersion;			// Control static: Node info text.
	CEdit m_editCurrentVersion;				// Control edit: Node info - Huidige firmware versie.
	CStatic m_staticCurrProduct;			// Control static: Node info text
	CEdit m_editCurrProduct;				// Control edit: Node info - Product info
	CStatic m_staticCurrPlatform;			// Control static: Node info text
	CEdit m_editCurrPlatform;				// Control edit: Node info - Hardware platform info.
	CIPAddressCtrl m_ctrlHostAddress;		// Control ip-address: IP-Address van de node.
	CStatic m_staticGroupFirmware;			// Control Groupbox: Beschikbare Firmware versies
	CStatic m_staticRelease;				// Control static: Beschikbare Firmware versies text
	CEdit m_editRelease;					// Control edit: Beschikbare Firmware versies - Release
	CStatic m_staticBeta;					// Control static: Beschikbare Firmware versies text
	CEdit m_editBeta;						// Control edit: Beschikbare Firmware versies - Beta
	CButton m_btnRelease;					// Control button: Beschikbare Firmware versies - Update release versie.
	CButton m_btnBeta;						// Control button: Beschikbare Firmware versies - Update Beta versie.
	CListBox m_listboxProgress;				// Control Listbox: Progress & status info.
	CEdit m_editStatus;						// Control edit: Status info bericht.
	CButton m_btnCheckShowAdvOptions;		// Control checkbox: Show advanced options
	CButton m_btnCheckShowBetaVersions;		// Control checkbox: Show beta versions
	CButton m_btnCheckShowDowngradeOpt;		// Control checkbox: Show downgrade versions
	CStatic m_staticAdvOptionsProduct;		// Control static: Adv opties - tekst
	CStatic m_staticAdvOptionsPlatform;		// Control static: Adv opties - tekst
	CComboBox m_comboAdvOptionsProduct;		// Control ComboBox: Adv opties - Verschillende producten.
	CComboBox m_comboAdvOptionsPlatform;	// Control ComboBox: Adv opties - Verschillende platformen.
	CEdit m_editInfoVersions;				// Control edit: Info ivm de gevonden firmware versies.

	friend class CDlgFirmwareUpdate_Host;	
};

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class CDlgFirmwareUpdate_Host : public IAppFirmwareUpdaterWorkerWindowHost
{
public:
	CDlgFirmwareUpdate_Host(CDlgFirmwareUpdate& refDlg);
	virtual ~CDlgFirmwareUpdate_Host();

	virtual void guiAddListboxMsg(const char* msg, bool eraseContent = false);
	virtual void guiSetMsgError(const char* msg);
	virtual void guiSetMsgStatus(const char* msg);
	virtual void guiUpdate();
	virtual void startAssyncTask();
	virtual void startTimerPoll();
	virtual void setStatusFirmwareUpdata(bool statusSuccess);

private:
	CDlgFirmwareUpdate& m_refParentDlg;
};
/****************************************************************************/