#pragma once

#include "DlgChildDaliGroupsOverviewBase.h"

namespace DUOTECNO {
namespace DALI {
	class CAppDaliDeviceProperties_ControlGear;
};
};


// CDlgChildDaliGroupsOverviewProperties dialog
class CDlgChildDaliGroupsOverviewProperties : public CDlgChildDaliGroupsOverviewBase
{
	DECLARE_DYNAMIC(CDlgChildDaliGroupsOverviewProperties)

public:
	CDlgChildDaliGroupsOverviewProperties(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);
	virtual ~CDlgChildDaliGroupsOverviewProperties();

	virtual void init(int groupNumber);

	// Parent dialoog (=property dialoog) toestand gewijzigd.
	// Gebruiken om de dialogen up te daten of te melden dat er properties veranderd werdebn die
	// nog moeten doorgestuurd worden.
	virtual void OnParentPropertyPageActive(int groupNumber,bool activeDialog);		

	// return waarde bepaalt of de parent property dialogen moeten updaten.
	// TRUE: Update, FALSE: geen update.
	virtual BOOL OnParentPropertyPageLeave();

	// Wanneer we deze child dialoog de actieve dialoog wordt.
	virtual void onTabDialogEnterDialog();	

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_GROUPOVERVIEW_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMReleasedcaptureSliderFadeTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderFadeRate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderMinimum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderMaximum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderPowerOn(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderSystemFailure(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnButtonClickedSetFadeTime();
	afx_msg void OnButtonClickedSetFadeRate();
	afx_msg void OnButtonClickedSetLevelMin();
	afx_msg void OnButtonClickedSetLevelMax();
	afx_msg void OnButtonClickedSetLevelPowerOn();
	afx_msg void OnButtonClickedSetLevelFailure();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);

	void initGUIControls();
	void initModel();

private:
	// DALI properties.
	bool m_supportExtendedProperties;										// Momenteel altijd op false.
	int m_deviceAddressProxyProperties;										// short address van het device waarvan we de properties bijhouden.
	DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* m_properties;		// Tijdelijk object om de properties bij te houden. (= copy van de properties van het eerste device in de groep.)
	CSliderCtrl m_sliderFadeTime;
	CSliderCtrl m_sliderFadeRate;
	CSliderCtrl m_sliderMinimum;
	CSliderCtrl m_sliderMaximum;
	CSliderCtrl m_sliderPowerOn;
	CSliderCtrl m_sliderSystemFailure;
	CEdit m_editFadeTime;
	CEdit m_editFadeRate;
	CEdit m_editMinimum;
	CEdit m_editMaximum;
	CEdit m_editPowerOn;
	CEdit m_editSystemFailure;
	CButton m_btnSetFadeTime;
	CButton m_btnSetFadeRate;
	CButton m_btnSetLevelMin;
	CButton m_btnSetLevelMax;
	CButton m_btnSetLevelPowerOn;
	CButton m_btnSetLevelSystemFailure;
	CString m_strToolTipText;
};
