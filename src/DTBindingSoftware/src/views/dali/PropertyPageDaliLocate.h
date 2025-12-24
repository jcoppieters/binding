#pragma once

#include "PropertyPageDaliBaseClass.h"
#include "afxcmn.h"

/*************************************************************************/
class CPropertyPageDaliLocate : public CPropertyPageDaliBaseClass
{
	DECLARE_DYNAMIC(CPropertyPageDaliLocate)

public:
	CPropertyPageDaliLocate(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext);
	virtual ~CPropertyPageDaliLocate();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_LOCATE };

public:
	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected);
	virtual void onNotifyGuiRxDaliMessage(BYTE bNodeAddress,BYTE lengthBits,uint32_t recievedFrame);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();			// Wanneer de pagina aktief komt.
	void initDialog();

	// List Ctrl Box.
	void initListCtrlSubClass( );
	void initListCtrlEvents( );
	void updateListCtrlEvents( );	
	void listCtrlEventsAddMessage(const CString& strTime,int deviceAddress, int instanceAddress, const CString& strDeviceName, 
		const CString& strInstanceName, const CString& strEvent,const CString& strInstanceType);
	void listCtrlEventsAddSeperator();
	void listCtrlUpdateDeviceName(int shortAddress,const CString& strName);
	void listCtrlUpdateDeviceInstanceName(int shortAddress,int instanceNumber,const CString& strName);
	void listCtrlSetTopIndex( );

	// initialiseren van het warning icoon wanneer in offline mode.
	void initStaticIconWarning( );

	void dbg_listCtrlAddDummyEvents( );

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonAddline();
	afx_msg void OnEnChangeEditDeviceName();
	afx_msg void OnEnKillfocusEditDeviceName();
	afx_msg void OnEnChangeEditInstanceName();
	afx_msg void OnEnKillfocusEditInstanceName();
	afx_msg void OnTimer(UINT nIDEvent);
	// afx_msg void OnShowWindow( BOOL  bShow, UINT  nStatus  );
	afx_msg void OnLvnItemchangedListctrlEvents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusListctrlEvents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusListctrlEvents(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedCheckFilterPushbutton();
	afx_msg void OnBnClickedCheckFilterOccupancy();
	afx_msg void OnBnClickedCheckFilterUnsupported();
	afx_msg void OnBnClickedCheckFilterEventschem();
	afx_msg void OnBnClickedCheckFilterLocated();
	afx_msg void OnBnClickedRadioRunning();
	afx_msg void OnBnClickedRadioPaused();
	afx_msg void OnBnClickedCheckTopindex();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	void OnNMCustomdrawListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	void helper_clearDataName( );
	BOOL updateDeviceNameFromEditBox( );
	BOOL updateInstanceNameFromEditBox( );
	void setNotifyGuiNamesChanged( );	
	void setDisplayDeviceInstanceName( );

private:

	enum {
		TIMER_ID_EDITBOX_DEVICENAME = 0,
		TIMER_ID_EDITBOX_INSTANCENAME = 1
	};

	typedef enum {
		VIEWMODE_OFFLINE = 0,
		VIEWMODE_ONLINE,
	} ViewMode_t;

	// Vlaggen die gebruikt worden om te bepalen welke events er in de listbox
	// getoond worden.
	enum {
		EVENT_FILTER_FLAG_EVENTS_TYPE_PUSHBUTTON				= (1<<0),		// standaard aan
		EVENT_FILTER_FLAG_EVENTS_TYPE_DETECTOR_OCCUPANCY		= (1<<1),		// standaard uit.
		EVENT_FILTER_FLAG_EVENTS_TYPE_UNSUPPORTED				= (1<<2),		// standaard uit.
		EVENT_FILTER_FLAG_EVENTS_TYPE_EVENTSCHEME_UNSUPPORTED	= (1<<3),		// Standaard aan
		EVENT_FILTER_FLAG_EVENTS_TYPE_LOCATED_INSTANCES			= (1<<4)		// Standaard aan
	
	};

	enum {
		COLUMN_EVENT_TIME = 0,
		COLUMN_DEVICE_ADDRESS = 1,
		COLUMN_DEVICE_INSTANCE_ADSRESS = 2,			
		COLUMN_DEVICE_NAME = 3,
		COLUMN_DEVICE_INSTANCE_NAME = 4,					
		COLUMN_INSTANCE_TYPE = 5,
		COLUMN_EVENT_NAME = 6,		
	};

	ViewMode_t m_viewMode;					// de viewmode.
	CListCtrl m_listCtrlEvents;
	CButton m_btnClear;
	CButton m_btnAddLine;
	CStatic m_staticConnectInfo;  
	CStatic m_staticGroupDevice;
	CStatic m_staticDeviceAddress;
	CStatic m_staticDeviceName;
	CEdit m_editDeviceAddress;
	CEdit m_editDeviceName;
	CStatic m_staticInstanceName;
	CEdit m_editInstanceNumber;
	CEdit m_editInstanceName;
	CStatic m_staticDisplayName;
	CEdit m_editDisplayName;
	CButton m_btnCheckFilterPushBtn;
	CButton m_btnCheckFilterOccupancy;
	CButton m_btnCheckFilterUnsupported;
	CButton m_btnCheckFilterEventScheme;
	CButton m_btnCheckFilterLocatedInstances;
	CButton m_btnCheckTopIndex;
	CStatic m_staticIconWarningOffline;
	CStatic m_staticTextWarningOffline;
	bool m_initListBoxCtrlDone;
	bool m_busyDeviceNameEditing;
	bool m_busyInstanceNameEditing;
	bool m_updateListCtrlDeviceName;
	bool m_updateListCtrlInstanceName;
	bool m_updateDeviceNameEditBox;
	bool m_updateInstanceNameEditBox;
	bool m_showTopIndex;
	int m_deviceShortAddress;
	int m_deviceInstanceNumber;
	int m_currentDaliInterfaceNodeAddress;				// Het node address van de DALI interface.
	CString m_strNameDevice;
	CString m_strNameInstance;
	CString m_strGroupName;
	CString m_strToolTipText;
	uint16_t m_flagsEventFilter;
	BOOL m_stateLiveViewPaused;
	BOOL m_flagInitDone;
};
