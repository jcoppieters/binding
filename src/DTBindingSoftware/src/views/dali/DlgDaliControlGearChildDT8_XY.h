#pragma once

#include "DlgDaliControlGearChildDT8.h"

// CDlgDaliControlGearChildDT8_XY dialog

class CDlgDaliControlGearChildDT8_XY : public CDlgDaliControlGearChildDT8
{
	DECLARE_DYNAMIC(CDlgDaliControlGearChildDT8_XY)

public:
	CDlgDaliControlGearChildDT8_XY(CWnd* pParent, DUOTECNO::DALI::CAppDaliObjectDT8ColorData* pSceneDT8ColorData);  
	virtual ~CDlgDaliControlGearChildDT8_XY();
	virtual void updateColorData(BOOL saveToProperties);
	virtual void clearData(BOOL saveToProperties = FALSE);

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_SCENE_DT8_CHILD_XY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMReleasedcaptureSliderValueX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderValueY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeValueX();
	afx_msg void OnEnChangeValueY();
	afx_msg void OnEnKillfocusValueX();
	afx_msg void OnEnKillfocusValueY();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCbnSelchangePresets();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void initControls();
	void initControlComboColorPresets();
	void initModel();
	void GetEditBoxValueX();
	void GetEditBoxValueY();
	void updateControlEditBox(CEdit* pCtr,double value);	
	double GetEditBoxValue(CEdit* pCtr);
	
private:
	enum { TIMER_ID_EDITBOX_VALUE_X = 1, TIMER_ID_EDITBOX_VALUE_Y };

	CComboBox m_comboPresets;
	CSliderCtrl m_sliderValueX;
	CEdit m_editValueX;	
	CSliderCtrl m_sliderValueY;
	CEdit m_editValueY;	
	CStatic m_infoText;
	DUOTECNO::MFC_HELPER::CColorPreviewStatic m_staticColorPreview;			// toont een preview van de kleur.
	CString m_strToolTipText;
	double m_valueX;
	double m_valueY;
	BOOL m_busyEditingValue;
	BOOL m_busyUpdatingControls;

	typedef CDlgDaliControlGearChildDT8 Base;
};
