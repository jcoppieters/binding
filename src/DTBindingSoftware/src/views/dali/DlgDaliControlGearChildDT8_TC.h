#pragma once

#include "DlgDaliControlGearChildDT8.h"

// CDlgDaliControlGearChildDT8_TC dialog

class CDlgDaliControlGearChildDT8_TC : public CDlgDaliControlGearChildDT8
{
	DECLARE_DYNAMIC(CDlgDaliControlGearChildDT8_TC)

public:
	CDlgDaliControlGearChildDT8_TC(CWnd* pParent, DUOTECNO::DALI::CAppDaliObjectDT8ColorData* pSceneDT8ColorData);  
	virtual ~CDlgDaliControlGearChildDT8_TC();
	virtual void updateColorData(BOOL saveToProperties);
	virtual void clearData(BOOL saveToProperties = FALSE);

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_SCENE_DT8_CHILD_TC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMReleasedcaptureSliderValue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeValue();
	afx_msg void OnEnKillfocusValue();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCbnSelchangePresets();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void initControls();
	void initControlComboColorPresets();
	void initModel();
	void GetEditBoxValue();

protected:
	static const int VALUE_KELVIN_MAX;
	static const int VALUE_KELVIN_MIN;
	static const int VALUE_KELVIN_MASK;

private:
	enum { TIMER_ID_EDITBOX_VALUE = 1 };
	CComboBox m_comboPresets;
	CSliderCtrl m_sliderValue;
	CEdit m_editValue;	
	CStatic m_infoText;
	CString m_strToolTipText;
	int m_valueKelvin;
	BOOL m_busyEditingValue;
	BOOL m_busyUpdatingControls;
	typedef CDlgDaliControlGearChildDT8 Base;
};
