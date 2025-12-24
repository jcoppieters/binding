#pragma once


#include "dlgchilddali.h"

// CDlgChildDaliControlgearTest dialog

class CDlgChildDaliControlgearTest : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliControlgearTest)

public:
	CDlgChildDaliControlgearTest(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliControlgearTest();

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave();
	virtual void onTabDialogLeaveDialog( );
	virtual void onTabDialogEnterDialog( ); 

	// IAppDlgDaliGui_Host	
	// virtual void onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected);
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task, const CString& strMessage);

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_CONTROLGEAR_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnButtonClickedSetOff();
	afx_msg void OnButtonClickedSetMin();
	afx_msg void OnButtonClickedSetMax();
	afx_msg void OnButtonClickedDimStepUp();
	afx_msg void OnButtonClickedDimStepDown();
	afx_msg void OnButtonClickedDimUp();
	afx_msg void OnButtonClickedDimDown();
	afx_msg void OnButtonClickedSetValue();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNMReleasedcaptureSliderValue(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg void OnButtonClickedSceneTest(UINT nID);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

private:
	enum { 
		TIMER_ID_QUERY = 0,
	};
	CButton m_btnSetOff;
	CButton m_btnSetMin;
	CButton m_btnSetMax;
	CButton m_btnDimUp;
	CButton m_btnDimDown;
	CButton m_btnDimStepUp;
	CButton m_btnDimStepDown;
	CButton m_btnSetValue;
	CStatic m_staticGroupLevel;
	CEdit m_editValue;
	CSliderCtrl m_sliderDimValue;
	CEdit m_editValueRead;
	CStatic m_staticValueRead;
	CStatic m_staticFooterInfoText;	
	int m_deviceActualValueRead;
	CMap<int,int,CString,CString> m_mapSceneNames;
	CString m_strToolTipText;
};

