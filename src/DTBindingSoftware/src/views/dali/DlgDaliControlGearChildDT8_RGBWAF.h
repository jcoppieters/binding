#pragma once

#include "DlgDaliControlGearChildDT8.h"
// CDlgDaliControlGearChildDT8_RGBWAF dialog

class CDlgDaliControlGearChildDT8_RGBWAF : public CDlgDaliControlGearChildDT8
{
	DECLARE_DYNAMIC(CDlgDaliControlGearChildDT8_RGBWAF)

public:
	typedef enum {
		MODE_COLOR_TYPE_RGBWAF = 0,
		MODE_COLOR_TYPE_PRIMARY_N
	} ModeColorType_t;

	CDlgDaliControlGearChildDT8_RGBWAF(CWnd* pParent, DUOTECNO::DALI::CAppDaliObjectDT8ColorData* pSceneDT8ColorData,ModeColorType_t mode,int nrOfChannels);   
	virtual ~CDlgDaliControlGearChildDT8_RGBWAF();
	virtual void updateColorData(BOOL saveToProperties);
	virtual void clearData(BOOL saveToProperties = FALSE);

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_SCENE_DT8_CHILD_RGBWAF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMReleasedcaptureSliderValue0(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderValue1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderValue2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderValue3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderValue4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderValue5(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeValue0();
	afx_msg void OnEnChangeValue1();
	afx_msg void OnEnChangeValue2();
	afx_msg void OnEnChangeValue3();
	afx_msg void OnEnChangeValue4();
	afx_msg void OnEnChangeValue5();
	afx_msg void OnEnKillfocusValue0();
	afx_msg void OnEnKillfocusValue1();
	afx_msg void OnEnKillfocusValue2();
	afx_msg void OnEnKillfocusValue3();
	afx_msg void OnEnKillfocusValue4();
	afx_msg void OnEnKillfocusValue5();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCbnSelchangePresets();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	void initControls();
	void initControlComboColorPresets();
	void initControlsStaticChannels();
	void updateControlsEnableChannels();
	void initModel();
	void updateControlEditBox(CEdit* pCtr,int value);	
	void updatePreviewImage( );
	void onEnChangeValue(int channelNR);
	void OnEnKillfocusValue(int channelNR);
	void GetEditBoxValue(int channelNR);
	int GetEditBoxValue(CEdit* pCtr);

private:
	enum { TIMER_ID_EDITBOX_VALUE = 1};

	enum {
		ID_CHANNEL_RED = 0,
		ID_CHANNEL_GREEN,
		ID_CHANNEL_BLUE,
		ID_CHANNEL_WHITE,
		ID_CHANNEL_AMBER,
		ID_CHANNEL_FREECOLOR,
		MAX_NR_CHANNELS
	};

	enum {
		RGBWAF_MASK_VALUE = 255,
		PRIMARY_CHANNEL_N_MASK_VALUE = 0xFFFF,
	};

	CComboBox m_comboPresets;
	CSliderCtrl m_sliderValue0;
	CSliderCtrl m_sliderValue1;
	CSliderCtrl m_sliderValue2;
	CSliderCtrl m_sliderValue3;
	CSliderCtrl m_sliderValue4;
	CSliderCtrl m_sliderValue5;
	CEdit m_editValue0;		
	CEdit m_editValue1;	
	CEdit m_editValue2;	
	CEdit m_editValue3;	
	CEdit m_editValue4;	
	CEdit m_editValue5;	
	CStatic m_infoText;
	CString m_strToolTipText;
	DUOTECNO::MFC_HELPER::CColorPreviewStatic m_staticColorPreview;			// toont een preview van de kleur.
	const ModeColorType_t m_modeColorType;
	int m_nrOfChannels;
	int m_values[MAX_NR_CHANNELS];
	int m_busyEditingEditBoxNR;
	BOOL m_busyEditingValue;
	BOOL m_busyUpdatingControls;

	typedef CDlgDaliControlGearChildDT8 Base;
};
