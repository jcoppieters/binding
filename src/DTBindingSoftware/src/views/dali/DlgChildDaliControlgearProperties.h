#pragma once

#include "dlgchilddali.h"
#include "afxcmn.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
	class CAppDaliDeviceProperties_ControlGear;
};
};
/***********************************************************************************/
// CDlgChildDaliControlgearProperties dialog

class CDlgChildDaliControlgearProperties : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliControlgearProperties)

public:
	CDlgChildDaliControlgearProperties(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliControlgearProperties();

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave();
	virtual BOOL onTabDialogCanLeaveDialog( );

	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_CONTROLGEAR_PROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMReleasedcaptureSliderFadeTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderFadeRate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderMinimum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderMaximum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderPowerOn(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderSystemFailure(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonReadConfig();
	afx_msg void OnBnClickedButtonRestore();
	afx_msg void OnBnClickedButtonUndo();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);
	void initGUIControls( );
	void deallocProperties( );
	void updateMsgProgressInfo( );
	void uploadData( );
	BOOL restoreToDefaults( );
	BOOL undoChanges( );
	BOOL helperUpdateProperties(DUOTECNO::DALI::CAppDaliDevice* pDevice);

private:
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
	CButton m_btnUpload;
	CButton m_btnReadConfig;
	CStatic m_staticFooterInfoText;	
	CButton m_btnRestoreDefaults;
	CButton m_btnUndoChanges;
	bool m_supportExtendedProperties;		// DALI-2 extended properties - Momenteel op false. (zie NOTE_DALI2_FRAMERATE)	
	DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* m_properties;
	CString m_strToolTipText;
};
