#pragma once

#include "PropertyPageDaliBaseClass.h"

// CPropertyPageDaliNwk dialog

class CPropertyPageDaliNwk : public CPropertyPageDaliBaseClass
{
	DECLARE_DYNAMIC(CPropertyPageDaliNwk)

public:
	CPropertyPageDaliNwk(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext);
	virtual ~CPropertyPageDaliNwk();

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_NETWORK };

public:
	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected);
	virtual void onNotifyGuiMsgCommunication(BYTE bNodeAddress,Type_t typeMessage, const CString& msg);
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);
	virtual void onNotifyGuiFoundDevice(BYTE bNodeAddress,DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice);
	virtual void onNotifyGuiUpdateDeviceSerialNumber(BYTE bNodeAddress,DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long serialNumber);
	virtual void onNotifyGuiUpdateDeviceGTIN(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long GTIN);

	virtual BOOL taskBusy( ) const {
		return (VIEWMODE_ONLINE_BUSY == this->m_viewMode ? TRUE : FALSE);
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();			// Wanneer de pagina aktief komt.
	virtual BOOL OnKillActive();		// Wanneer de pagina verlaten wordt.
	void initDialog();
	// int listCtrlInitAddColumn(CListCtrl* pCtrl,int columnNR,const char* szName,int sizeX);
	void listCtrlAddDevice(CListCtrl* pList,DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice);

	DECLARE_MESSAGE_MAP()
	void updateControls( );
	void updateControlsConnectSelLocalRemote( );
	void AddMessage(const CString& msg,BOOL clearMessages = FALSE);	
	afx_msg void OnBnClickedCheckConnect();
	afx_msg void OnBnClickedBtnDaliSearch();
	afx_msg void OnBnClickedBtnDaliContinue();
	afx_msg void OnBnClickedBtnDaliNew();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedButtonEditcfg();	
	afx_msg void OnBnClickedBtnAllOff();		
	afx_msg void OnBnClickedBtnAllMax();	
	afx_msg void OnBnClickedBtnAllMin();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	LONG OnDisplayErrorMessage(WPARAM wParam,LPARAM lParam); 

	enum { 
		WM_DISPLAY_ERRORMESSAGE = (WM_USER + 150),

		TIMER_ID_REFRESH_CONNECTSETTINGS = 0,
		TIMER_RELOADVALUE_REFRESH_CONNECTSETTINGS_MSEC = 1000
	};

private:

	enum {
		COLUMN_DEVICE_COUNT = 0,
		COLUMN_DEVICE_NAME = 1,
		COLUMN_DEVICE_TYPE = 2,
		COLUMN_DEVICE_NEW = 3,
		COLUMN_DEVICE_SERIALNUMBER = 4,
		COLUMN_DEVICE_GTIN = 5,
	};

	typedef enum {
		VIEWMODE_OFFLINE = 0,
		VIEWMODE_CONNECT_WAIT,
		VIEWMODE_ONLINE,
		VIEWMODE_ONLINE_BUSY 
	} ViewMode_t;

	ViewMode_t m_viewMode;	
	CButton m_btnSearch;
	CButton m_btnAddrContinue;
	CButton m_btnAddrNewInstallation;
	CButton m_btnCancel;
	CButton m_checkConnect;
	CButton m_radioLocal;
	CButton m_radioRemote;
	CButton m_btnEdit;
	CButton m_btnAllOff;
	CButton m_btnAllMax;
	CButton m_btnAllMin;
	CListBox m_listboxInfo;
	CEdit m_editTaskState;
	int m_selectionSettings;					// Selectie local or remote tcp settings...
	CListCtrl m_listDevices;					// ListCtrl waar de devices getoond worden.
	CStatic m_staticSummary;					// Weergave aantal gevonden devices.
	CString m_strToolTipText;
	int m_counterFoundControlInputDevices;		// Weergave aantal gevonden control devices.
	int m_counterFoundControlGearDevices;		// Weergave aantal gevonden control devices.
};
