#pragma once

#include "dlgchilddali.h"
#include "afxcmn.h"

// CDlgChildDaliControlgearExtra dialog

class CDlgChildDaliControlgearExtra : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliControlgearExtra)

public:
	CDlgChildDaliControlgearExtra(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);   
	virtual ~CDlgChildDaliControlgearExtra();

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave();
	virtual BOOL onTabDialogCanLeaveDialog( );

	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_CONTROLGEAR_EXTRA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonReadConfig();
	afx_msg void OnBnClickedButtonRestore();
	afx_msg void OnBnClickedButtonUndo();
	afx_msg void OnBnClickedRadioDimmerCurveDali();
	afx_msg void OnBnClickedRadioDimmerCurveLinear();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);
	void initGUIControls( );
	void deallocProperties( );
	void updateMsgProgressInfo( );
	void uploadData( );
	void updateModel( );
	BOOL restoreToDefaults( );
	BOOL undoChanges( );
	BOOL helperUpdateProperties(DUOTECNO::DALI::CAppDaliDevice* pDevice);

private:
	CStatic m_staticHeaderInfoText;	
	CButton m_btnUpload;
	CButton m_btnReadConfig;
	CStatic m_staticFooterInfoText;	
	CButton m_btnRestoreDefaults;
	CButton m_btnUndoChanges;
	CStatic m_staticGroupSelectDimmingCurve;	
	CButton m_btnRadioDimmingCurveDali;
	CButton m_btnRadioDimmingCurveLinear;
	int m_selectDimmerCurve;
	CString m_strToolTipText;
	DUOTECNO::DALI::CAppDaliDeviceProperties_ControlGear* m_properties;
};
