#pragma once


// CDlgChildDaliControlgearScenes dialog
#include "dlgchilddali.h"
#include "app/appdali/appdaliprotocol_def.h"


class CDlgDaliControlGearDT8Scene;

class CDlgChildDaliControlgearScenes : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliControlgearScenes)

public:
	CDlgChildDaliControlgearScenes(CAppDlgDaliWorker& refWorker,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliControlgearScenes();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave() { return FALSE; }
	virtual BOOL onTabDialogCanLeaveDialog( );

	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_CONTROLGEAR_SCENES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnButtonClickedScene(UINT nID);
	afx_msg void OnButtonClickedColor(UINT nID);
	afx_msg void OnEnKillfocusEditBox(UINT nID);
	afx_msg void OnEnSetfocusEditBox(UINT nID);
	afx_msg void OnEnChangeEditControls(UINT nID);
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonReadConfig();
	afx_msg void OnBnClickedButtonDaliSceneExecute();
	afx_msg void OnBnClickedButtonRestore();
	afx_msg void OnBnClickedButtonUndo();
	afx_msg void OnCbnSelchangeSceneNumber();
	afx_msg void OnEnChangeEditSceneName();			
	afx_msg void OnEnKillfocusEditSceneName();			
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	void initModel( );
	void updateModel( );
	BOOL updateDeviceProperties( );	// Lokale dialog status copieren naar de m_properties.
	void uploadData( );
	BOOL restoreToDefaults( );
	BOOL undoChanges( );
	void updateMsgProgressInfo();
	void updateEditBoxSceneName(int sceneNumber);
	void GetEditBoxName();
	void UpdateEditBoxGroupName();
	BOOL helperUpdateProperties(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	void deallocateDlgDT8Scenes();

private:
	void deallocProperties( );

private:
	enum { 			
		TIMER_ID_EDITBOX_NAME = 0,
		TIMER_ID_EDITBOX_NAME_UPDATE = 1			// Nodig om de normalized tekst weer te geven in de edit control.
	};

	CDlgDaliControlGearDT8Scene* m_pDialogDT8Scenes;
	CButton m_btnUpload;
	CButton m_btnReadConfig;
	CStatic m_staticFooterInfoText;	
	CButton m_btnRestoreDefaults;
	CButton m_btnUndoChanges;
	CComboBox m_cmbSceneSelect;
	CStatic m_groupSceneTest;
	CButton m_btnTestScene;
	CEdit m_editSceneName;
	CString m_strPreviousValue;
	CString m_strSceneName;
	CString m_strToolTipText;
	bool m_busyEditingName;
	bool m_supportDT8ColorObjects;				// DT8 - Weergave van de extra color buttons
	int m_selectedScene;
	int m_sceneLevel[DUOTECNO::DALI::DALI_SYSTEM_CONTROLGEAR_SCENE_MAX];			// Weergave editbox.
	bool m_sceneUsed[DUOTECNO::DALI::DALI_SYSTEM_CONTROLGEAR_SCENE_MAX];			// Status van de CheckBox.
	DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* m_properties;
	CMap<int,int,CString,CString> m_mapNames;
};
