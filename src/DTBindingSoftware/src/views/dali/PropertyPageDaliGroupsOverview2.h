#pragma once

#include "PropertyPageDaliBaseClass.h"

class CDlgChildDaliGroupsOverviewBase;

// CPropertyPageDaliGroupsOverview2 dialog

class CPropertyPageDaliGroupsOverview2 : public CPropertyPageDaliBaseClass
{
	DECLARE_DYNAMIC(CPropertyPageDaliGroupsOverview2)

public:
	CPropertyPageDaliGroupsOverview2(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext);
	virtual ~CPropertyPageDaliGroupsOverview2();

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_GROUPS_OVERVIEW2 };

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
	void initTabDialogs();
	BOOL updateModel();
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);	
	void initComboBoxGroups( );
	void initSpinCtrlActualValue( );
	void initTextFooter( );
	void UpdateEditBoxGroupName();
	void requestUpdateGuiControlsUpdate();
	void GetEditBoxName();
	void setChildDialogGroupNumber(int groupNumber);
	afx_msg void OnCbnSelchangeGroup();	
	afx_msg void OnEnChangeEditGroupName();			
	afx_msg void OnEnKillfocusEditGroupName();			
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTabChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTabChanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonOff();
	afx_msg void OnBnClickedButtonMaxLevel();
	afx_msg void OnBnClickedButtonMinLevel();
	afx_msg void OnBnClickedButtonDimDown();
	afx_msg void OnBnClickedButtonDimUp();
	afx_msg void OnBnClickedButtonStepDown();
	afx_msg void OnBnClickedButtonStepUp();
	afx_msg void OnBnClickedButtonValueSet();
	afx_msg void OnDeltaposSpinCtrlActualValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

private:
	enum { 			
		TIMER_ID_EDITBOX_NAME = 0,
		TIMER_ID_EDITBOX_NAME_UPDATE = 1			// Nodig om de normalized tekst weer te geven in de edit control.
	};
	typedef enum {
		VIEWMODE_OFFLINE = 0,
		VIEWMODE_ONLINE,
		VIEWMODE_ONLINE_BUSY
	} ViewMode_t;

	typedef enum {		
		DIALOG_CHILD_DALI_GROUPSOVERVIEW_GROUPS = 0,
		DIALOG_CHILD_DALI_GROUPSOVERVIEW_PROPERTIES,	
		DIALOG_CHILD_MAX_DIALOGS,
		DIALOG_CHILD_NONE 
	} DialogID_t;

	enum {
		ACTUAL_LEVEL_MIN = 1,
		ACTUAL_LEVEL_MAX = 254
	};

	void showActiveDaliog(DialogID_t dialog);
	int tabDialogsGetCurrSelectedDialog(CTabCtrl& r);

	ViewMode_t m_mode;
	int m_selectedGroup;
	int m_previousChildDialog;
	BOOL m_fInitDone;
	BOOL m_busyUpdateGUIEditBox;
	BOOL m_pendingUpdateControls;
	BOOL m_busyEditingName;
	CString m_strGroupName;
	CString m_strToolTipText;
	CTabCtrl m_tabDialogs;
	CComboBox m_cmbGroups;
	CStatic m_staticInfoHeader;
	CEdit m_editGroupName;
	CButton m_btnOff;
	CButton m_btnMaxLevel;
	CButton m_btnMinLevel;
	CButton m_btnDimDown;
	CButton m_btnDimUp;
	CButton m_btnStepDown;
	CButton m_btnStepUp;
	CButton m_btnSetActualValue;
	CEdit m_editActualValue;
	CSpinButtonCtrl m_spinCtrlActualValue;
	CDlgChildDaliGroupsOverviewBase* m_pArrChildDialogs[DIALOG_CHILD_MAX_DIALOGS];
};
