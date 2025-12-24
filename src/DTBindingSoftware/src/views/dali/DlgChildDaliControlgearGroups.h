#pragma once


// CDlgChildDaliControlgearGroups dialog
#include "dlgchilddali.h"

class CDlgChildDaliControlgearGroups : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliControlgearGroups)

public:
	CDlgChildDaliControlgearGroups(CAppDlgDaliWorker& refWorker,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliControlgearGroups();

	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave() { return FALSE; }

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_CONTROLGEAR_GROUPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnButtonClickedGroup(UINT nID);
	afx_msg void OnBnClickedButtonReadConfig();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	void updateModel();	// Namen updaten...

private:
	CStatic m_staticFooterInfoText;	
	CButton m_btnReadConfig;
	CMap<int,int,CString,CString> m_mapGroupNames;			// Deze bevat de namen van de groepen.
	CString m_strToolTipText;
};
