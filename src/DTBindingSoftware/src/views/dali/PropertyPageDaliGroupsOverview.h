#pragma once

#include "PropertyPageDaliBaseClass.h"
// CPropertyPageDaliGroupsOverview dialog

class CPropertyPageDaliGroupsOverview : public CPropertyPageDaliBaseClass
{
	DECLARE_DYNAMIC(CPropertyPageDaliGroupsOverview)

public:
	CPropertyPageDaliGroupsOverview(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext);  
	virtual ~CPropertyPageDaliGroupsOverview();

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_GROUPS_OVERVIEW };

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
	BOOL updateModel();
	BOOL updateModelDeviceFound();
	BOOL updateModelDeviceInGroup();
	BOOL updateModelDeviceNames();
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	void UpdateBtnName(CButton* pButton, int shortAddress);
	void UpdateEditBoxGroupName();
	void requestUpdateGuiControlsUpdate();
	void GetEditBoxName();
	afx_msg void OnCbnSelchangeGroup();
	afx_msg void OnButtonClickedAddress(UINT nID);
	afx_msg void OnBnClickedButtonOff();
	afx_msg void OnBnClickedButtonMaxLevel();
	afx_msg void OnBnClickedButtonMinLevel();
	afx_msg void OnEnChangeEditGroupName();			
	afx_msg void OnEnKillfocusEditGroupName();			
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

private:
	enum { MAX_DALI_ADDRESSES = 64 };

	enum { 			
		TIMER_ID_EDITBOX_NAME = 0,
		TIMER_ID_EDITBOX_NAME_UPDATE = 1			// Nodig om de normalized tekst weer te geven in de edit control.
	};

	typedef enum {
		VIEWMODE_OFFLINE = 0,
		VIEWMODE_ONLINE,
		VIEWMODE_ONLINE_BUSY
	} ViewMode_t;

	ViewMode_t m_mode;
	int m_selectedGroup;
	BOOL m_fInitDone;
	BOOL m_busyUpdateGUIEditBox;
	BOOL m_pendingUpdateControls;
	BOOL m_stateEnabled[MAX_DALI_ADDRESSES];
	BOOL m_stateChecked[MAX_DALI_ADDRESSES];
	CMap<int, int, CString, CString> m_mapNames;	
	BOOL m_modelNamesChanged;
	BOOL m_busyEditingName;
	CString m_strGroupName;
	CComboBox m_cmbGroups;
	CButton m_btnOff;
	CButton m_btnMaxLevel;
	CButton m_btnMinLevel;
	CStatic m_staticInfoHeader;
	CEdit m_editGroupName;
};
