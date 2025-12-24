#pragma once

#include "app/appdali/appdali_fwd.h"

class CAppDlgDaliWorker;
class CDlgDaliControlGearChildDT8;

namespace DUOTECNO {
	namespace DALI {
		class CAppDaliDeviceProperties_ControlGear;
		class CAppDaliObjectDT8ColorData;
	};
};

// CDlgDaliControlGearDT8Scene dialog

class CDlgDaliControlGearDT8Scene : public CDialog
{
	DECLARE_DYNAMIC(CDlgDaliControlGearDT8Scene)

public:
	CDlgDaliControlGearDT8Scene(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDaliControlGearDT8Scene();

	BOOL showDialog(int shortAddress, int selectedScene, DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* properties, int currentLevel, const CString& name);

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_SCENE_DT8 };

	int getCurrentLevel() const;
	int getCurrentScene() const;
	const CString& getSceneName() const;

	// Vragen of er een property veranderd werd die nog moet upgeload worden.
	BOOL getPropertyChanged() const;

	// Updaten van de user interface wanneer er een taak bezig is.
	void onNotifyGuiTaskStarted(DUOTECNO::DALI::Task_t task);
	void onNotifyGuiTaskDone(DUOTECNO::DALI::Task_t task);
	void onNotifyGuiTaskError(DUOTECNO::DALI::Task_t task, const CString& strMessage);

protected:
	enum { WM_UPDATE_CONTROLS = (WM_USER + 150) };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMReleasedcaptureSliderItensity(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeSceneIntensity();
	afx_msg void OnEnKillfocusSceneIntensity();
	afx_msg void OnEnKillfocusSceneName();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonUploadAll();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonUndo();
	afx_msg void OnTabChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTabChanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void updateControlsActualLevel();
	void updateControlsSceneName();
	void updateControlsSceneNumber();
	void updateControlsTaskButtons();
	void initControls();	
	void initControlTabDialogs();
	void initModel();
	void initModelViewModeColorType();
	void GetEditBoxValueIntensity();
	void deallocSubDialogs();
	void updateIntensityLevelData(BOOL saveToProperties);
	void handleClickedButtonUpload(int shortAddress);

	typedef enum {
		VIEW_MODE_COLOR_TYPE_UNKNOWN = -1,
		VIEW_MODE_COLOR_TYPE_TEMPERATURE = 0,
		VIEW_MODE_COLOR_TYPE_COORDINATE_XY,
		VIEW_MODE_COLOR_TYPE_RGBWAF,
		VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS,
	} ViewModeColorType_t;

	void showActiveDialog(ViewModeColorType_t dialog);
	ViewModeColorType_t getTabCurrentSelected();
	BOOL setTabSelected(ViewModeColorType_t mode);

	typedef enum {
		VIEWMODE_OFFLINE = 0,
		VIEWMODE_ONLINE,
		VIEWMODE_ONLINE_BUSY
	} ViewMode_t;

	void setViewMode(ViewMode_t mode);

private:
	static const CString STRING_MASK_VALUE;

	enum {
		TIMER_ID_EDITBOX_INTENSITY = 1,
	};
	enum { NR_OF_CHILD_DIALOGS = VIEW_MODE_COLOR_TYPE_PRIMARY_CHANNELS + 1 };

	CAppDlgDaliWorker& m_refWorker;
	ViewModeColorType_t m_viewModeColorType;
	ViewMode_t m_viewMode;
	DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* m_properties;
	DUOTECNO::DALI::CAppDaliObjectDT8ColorData* m_pSceneDT8ColorData;
	CDlgDaliControlGearChildDT8* m_pChildDialogs[NR_OF_CHILD_DIALOGS];
	int m_shortAddress;
	int m_selectedScene;
	int m_currentLevel;
	CString m_strSceneName;
	BOOL m_propertyChanged;
	BOOL m_busyEditingIntensity;
	BOOL m_busyUpdatingControls;
	CEdit m_editSceneName;
	CEdit m_editSceneNumber;
	CEdit m_editSceneIntensity;
	CSliderCtrl m_sliderIntensity;
	CButton m_btnUpload;
	CButton m_btnUploadAll;
	CButton m_btnTest;
	CButton m_btnClear;
	CButton m_btnUndo;
	CTabCtrl m_tabSubDialogs;
	CStatic m_staticTextFooter;
	CStatic m_staticTextHeader;
	CString m_strToolTipText;
};
