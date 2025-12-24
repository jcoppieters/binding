#pragma once

#include "dlgchilddali.h"

// CDlgChildDaliControlDevice dialog

class CDlgChildDaliControlDevice : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliControlDevice)

public:
	CDlgChildDaliControlDevice(CAppDlgDaliWorker& refWorker,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliControlDevice();

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave() { return FALSE; }
	virtual void onTabDialogEnterDialog( );

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_CONTROLDEVICE_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnButtonClickedAppControler();
	afx_msg void OnButtonClickedPowerCycle(); 
	afx_msg void OnButtonClickedInstances();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	void updateGUIControls(); 
	void updateModel();

private:
	CButton m_btnCheckAppController;
	CButton m_btnCheckPowerCycleNotify;
	CButton	m_btnCheckInstancesEnabled;
	CStatic m_staticFooterInfoText;	
	CStatic m_staticTextWarning;	
	CStatic m_staticWarningIcon;
	CString m_strToolTipText;
	bool m_appControllerEnabled;
	bool m_appControllerPresent;
	bool m_appControllerAlwaysOn;
	bool m_powerCycleNotification;
};
