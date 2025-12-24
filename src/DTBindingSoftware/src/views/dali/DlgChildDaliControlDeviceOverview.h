#pragma once

// CDlgChildDaliControlDeviceOverview dialog
#include "dlgchilddali.h"

class CDlgChildDaliControlDeviceOverview : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliControlDeviceOverview)

public:
	CDlgChildDaliControlDeviceOverview(CAppDlgDaliWorker& refWorker,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliControlDeviceOverview();

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave();
	virtual void onTabDialogEnterDialog( );

	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_CONTROLDEVICE_OVERVIEW };

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	void updateModel( );
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	afx_msg void OnButtonClicked(UINT nID);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEnKillfocusEditBox(UINT nID);
	afx_msg void OnEnSetfocusEditBox(UINT nID);
	afx_msg void OnEnChangeEditControls(UINT nID);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	BOOL updateInstanceNameFromEditBox( );
	BOOL helperKillFocusEditBox(UINT nID);

private:
	typedef enum {
		STATE_CHECKBOX_DISABLED = 0,
		STATE_CHECKBOX_ON,
		STATE_CHECKBOX_OFF
	} StateCheckBox_t;

	enum { 
		MAX_INSTANCES = DUOTECNO::DALI::CAppDaliInputControlDeviceInstances::LAST_INSTANCE_NUMBER + 1,

		TIMER_ID_EDITBOX_INSTANCENAME = 0,
	};

	CStatic m_staticFooterInfoText;	
	StateCheckBox_t m_statesCheckBox[MAX_INSTANCES];
	CMap<int, int, CString, CString> m_mapNames;	
	CString m_strPreviousName;
	CString m_strToolTipText;
	int m_currentInstanceName;
	bool m_busyEditingName;
	bool m_showWarning;
};
