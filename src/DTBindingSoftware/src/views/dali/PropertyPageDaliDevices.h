#pragma once

#include "PropertyPageDaliBaseClass.h"
#include "afxcmn.h"

#define USE_DEALLOC_DIALOGS		1

namespace DUOTECNO {
namespace DALI {
	class CAppDaliDevice;
};
};
class CDlgChildDali;

// CPropertyPageDaliNwk dialog

class CPropertyPageDaliDevices : public CPropertyPageDaliBaseClass
{
	DECLARE_DYNAMIC(CPropertyPageDaliDevices)

public:
	CPropertyPageDaliDevices(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext);
	virtual ~CPropertyPageDaliDevices();

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_DEVICES };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTvnSelchangedTreeDevices(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingTreeDevices(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedCheckIdentify();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTabChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTabChanging(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnTvnEndlabeleditTreeDevices(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeleditTreeDevices(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydownTreeDevices(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeDaliDeviceinfoEditName();
	afx_msg void OnEnKillfocusDaliDeviceinfoEditName();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

public:
	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected);
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();			// Wanneer de pagina aktief komt.
	virtual BOOL OnKillActive();		// Wanneer de pagina verlaten wordt.

	void initDialog();
	void initTree();
	void updateTree();
	void sortTree();
	bool formatTreeItemDeviceName(DUOTECNO::DALI::CAppDaliDevice& refDevice,CString* pStr);
	bool formatTreeItemDeviceName(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress,CString* pStr);
	bool formatTreeItemDeviceName(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress,const CString& strNewName,CString* pStr);
	bool parseTreeItemDeviceLabelName(LPSTR srcString,CString* pStrAddress,CString* pStrDeviceName);
	void setTreeItemLabelText(HTREEITEM hItem,const CString& strText);
	void addDevice(HTREEITEM parent,HTREEITEM *pNewParent,DUOTECNO::DALI::CAppDaliDevice& refDevice);
	void showDeviceInfo(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	void showTabCtrlDeviceInfo(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	void updateStateBtnIdentify( );
	bool setDatabaseUpdateName(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress,const CString& strName,CString *pStrNormalizedName);	
	void GetEditBoxName( );
	bool find(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress,HTREEITEM& hTreeItem);
	void updateTreeItemDeviceName(DUOTECNO::DALI::DaliDeviceType_t type,int shortAddress);
	void setNotifyGuiNamesChanged( );
	BOOL requestChildDialogCanLeave( );

#if(USE_DEALLOC_DIALOGS == 1)
	void deallocDialogs();
#endif 

	DECLARE_MESSAGE_MAP()

	enum { 
		TIMER_ID_IDENTIFY = 0,
		TIMER_ID_EDITBOX_NAME = 1,
		TIMER_ID_EDITBOX_NAME_UPDATE = 2			// Nodig om de normalized tekst weer te geven in de edit control.
	};

	typedef enum {
		IDENTIFY_EVENT_SET_OFF = 0,
		IDENTIFY_EVENT_SET_ON,
		IDENTIFY_EVENT_TIMEREXPIRY,
	} IdentifyEvent_t;

	void handleIdentifyFsm(IdentifyEvent_t ev);
	void sendIdentify(bool stateToggleOnOff);

	// Tabcontrol.
	void setTabCtrlModeControlInvisible( );
	void setTabCtrlModeControlStatusError(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	void setTabCtrlModeControlDevice(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	void setTabCtrlModeControlGear(DUOTECNO::DALI::CAppDaliDevice* pDevice);

	static int tabDialogsGetCurrSelectedDialog(CTabCtrl& r);

private:
	typedef enum {		
		VIEWMODE_OFFLINE = 0,
		VIEWMODE_ONLINE_IDLE,
		VIEWMODE_ONLINE_TASK_BUSY
	} ViewMode_t;

	typedef enum {
		IDENTIFY_STATE_DISABLED = 0,
		IDENTIFY_STATE_ENABLED_TOGGLE_STATE_ON,
		IDENTIFY_STATE_ENABLED_TOGGLE_STATE_OFF
	} IdentifyState_t;


	// Oppassen:
	// Bij het invoegen van een nieuwe dialoog ook 'OnTabChanged' aanpassen.
	typedef enum {
		// Control devices.
		DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL = 0,
		DIALOG_CHILD_DALI_CONTROLDEVICE_OVERVIEW,
		DIALOG_CHILD_DALI_INSTANCES,

		// Control gear.
		DIALOG_CHILD_DALI_CONTROLGEAR_TEST,
		DIALOG_CHILD_DALI_CONTROLGEAR_PROPERTIES,		
		DIALOG_CHILD_DALI_CONTROLGEAR_GROUPS,
		DIALOG_CHILD_DALI_CONTROLGEAR_EXTRA,
		DIALOG_CHILD_DALI_CONTROLGEAR_SCENES,

		// Control devices + control gear
		DIALOG_CHILD_DALI_ADDRESSING,
		DIALOG_CHILD_DALI_DEVICE_ERRORINFO,

		DIALOG_CHILD_MAX_DIALOGS,
		DIALOG_CHILD_NONE 

	} DialogID_t;

	typedef enum {
		TABCONTROL_VIEWMODE_INVISIBLE = 0,
		TABCONTROL_VIEWMODE_STATUS_ERROR,
		TABCONTROL_VIEWMODE_CONTROLDEVICE,
		TABCONTROL_VIEWMODE_CONTROLGEAR
	} TabControlViewMode_t;

	void showActiveDaliog(DialogID_t dialog);

	ViewMode_t m_viewMode;
	bool m_initDone;
	bool m_busyEditingName;
	int m_previousChildDialog;
	CString m_strNormalizedName;						// Workaround om de aangepaste tekst weer te geven in de editbox (zie TIMER_ID_EDITBOX_NAME_UPDATE)
	IdentifyState_t m_identifyState;
	DUOTECNO::DALI::CAppDaliDevice* m_pSelectedDevice;
	CTreeCtrl m_treeControl;
	CStatic m_groupDeviceInfo;
	CStatic m_staticDeviceAddress;
	CStatic m_staticDeviceInfoGTIN;
	CStatic m_staticDeviceInfoSerialNr;
	CStatic m_staticDeviceInfoType;	
	CEdit m_editDeviceAddress;
	CEdit m_editDeviceInfoGTIN;
	CEdit m_editDeviceInfoSerialNr;
	CEdit m_editDeviceInfoType;	
	CButton m_btnCheckIdentify;
	CStatic m_staticDeviceInfoName;
	CEdit m_editDeviceInfoName;
	CStatic	m_staticDeviceType;	
	CStatic	m_staticDeviceFirmwareVersion;	
	CStatic	m_staticDeviceDaliVersion;	
	CEdit m_editDeviceType;
	CEdit m_editDeviceFirmwareVersion;
	CEdit m_editDeviceDaliVersion;
	CString m_strToolTipText;
	CTabCtrl m_tabDialogs;
	TabControlViewMode_t m_modeTabControl;
	CDlgChildDali* m_pArrDlgChild[DIALOG_CHILD_MAX_DIALOGS];
};


