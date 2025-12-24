#pragma once

#include "dlgchilddali.h"
// CDlgChildDaliAddressing dialog

class CDlgChildDaliAddressing : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliAddressing)

public:
	CDlgChildDaliAddressing(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliAddressing();

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave() { return FALSE; } 

	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_ADDRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonAssignAddress();
	afx_msg void OnBnClickedButtonEraseAddress();
	afx_msg void OnBnClickedButtonSwapAddresses();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	void updateDataComboBoxAssignAddresses( );
	void updateDataComboBoxSwapAddresses();
	int comboBoxAssignAddressesInsertItem(DUOTECNO::DALI::DaliDeviceType_t type,int address,bool addressUsed);	
	int comboBoxAssignAddressesGetCurrSelAddress( );
	int comboBoxSwapAddressesGetCurrSelAddress( );

private:	
	CString m_strErrorMessage;		// WORDT MOMENTEEL NIET GEBRUIKT.
	CStatic m_staticTextHeader;
	CStatic m_staticGroupAssign;
	CStatic m_staticGroupDelete;
	CComboBox m_comboBoxAsignAddresses;
	CButton m_btnAssignAddress;
	CButton m_btnEraseAddress;
	CStatic m_staticFooterInfoText;	
	CStatic m_staticGroupSwapAddresses;
	CButton m_btnSwapAddresses;
	CComboBox m_comboBoxSwapAddresses;
	CString m_strToolTipText;
};
