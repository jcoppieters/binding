#pragma once

#include "dlgchilddali.h"
#include "afxwin.h"


// CDlgChildDaliInstances dialog
class CDlgChildDaliInstances : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliInstances)

public:
	CDlgChildDaliInstances(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliInstances();

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave();
	virtual void onTabDialogEnterDialog( );
	virtual BOOL onTabDialogCanLeaveDialog( );

	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress,DUOTECNO::DALI::Task_t task,const CString& strMessage);

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_INSTANCES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCbnSelchangeInstanceNumber();
	afx_msg void OnButtonClickedEventFilter(UINT nID);
	afx_msg void OnCbnSelchangeEventScheme();
	afx_msg void OnButtonClickedEnable();
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonRestore();
	afx_msg void OnBnClickedButtonUndo();
	afx_msg void onBnClickedRestoreInstanceType();
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);

	// void initIconWarning( );
	void deallocProperties( );
	void uploadData( );
	void updateModel( );
	BOOL restoreToDefaults( );
	BOOL undoChanges( );

private:	
	bool updateGUIControls_ComboInstanceNumber( );		// Oppassen return waarde controleren.
	void updateGUIControls_EventFilters();
	void updateGUIControls_EventScheme( );
	void updateGuiControls_InstanceType( );
	void updateGUIControls_InstanceEnabled( );
	void updateMsgProgressInfo();

	// Wanneer de locale m_properties opnieuw moeten aangemaakt worden.
	// bvb wanneer we van een ander tabblad komen.
	void assignDeviceProperties(DUOTECNO::DALI::CAppDaliDevice* pDevice);

private:
	struct STRUCT_ENTRY {
		BOOL m_enabled;
		int m_indexButton;
		int m_maskEventFilter;
		CString m_strName;
	};

	enum { MAX_EVENTFILTERS_CHECKBUTTONS = 8 };
	static const STRUCT_ENTRY m_mapCheckButtonsEventFilters_Undefined[CDlgChildDaliInstances::MAX_EVENTFILTERS_CHECKBUTTONS];
	static const STRUCT_ENTRY m_mapCheckButtonsEventFilters_PushButtonEvents[CDlgChildDaliInstances::MAX_EVENTFILTERS_CHECKBUTTONS];
	static const STRUCT_ENTRY m_mapCheckButtonsEventFilters_OccupancyDetector[CDlgChildDaliInstances::MAX_EVENTFILTERS_CHECKBUTTONS];
	static const STRUCT_ENTRY m_mapCheckButtonsEventFilters_AbsInputDevice[CDlgChildDaliInstances::MAX_EVENTFILTERS_CHECKBUTTONS];	
	const STRUCT_ENTRY* getMapCheckButtonsInstanceType(int instanceType);

private:
	int m_currentInstanceNumber;
	int m_currentInstanceType;					// zie enum
	int m_currentEventScheme;
	bool m_flagChangedDevice;					// Ander device toegekend - moeten de combo box nr of instances updaten + type instance.
	bool m_flagChangedInstanceNumber;
	bool m_flagChangedInstanceType;
	bool m_flagChangedEventFilters;
	bool m_validSettings;
	bool m_restoreInstanceTypeEnable;			// Nodig voor de ozuno devices wanneer het instance type verkeerd ingesteld is.
	CString m_strToolTipText;
	// GUI.
	CButton m_btnCheckEventFilter0;
	CButton m_btnCheckEventFilter1;
	CButton m_btnCheckEventFilter2;
	CButton m_btnCheckEventFilter3;
	CButton m_btnCheckEventFilter4;
	CButton m_btnCheckEventFilter5;
	CButton m_btnCheckEventFilter6;
	CButton m_btnCheckEventFilter7;
	CStatic m_staticGroupEventFilters;
	CButton m_btnCheckEnabled;
	CButton m_btnRestoreInstanceType; 
	CComboBox m_cmbEventScheme;
	CComboBox m_cmbInstanceNumber;
	CEdit m_editInstanceType;
	CStatic m_staticFooterInfoText;	
	CButton m_btnUpload;
	CButton m_btnRestoreDefaults;
	CButton m_btnUndoChanges;
	CStatic m_iconWarningSettings;
	CButton* m_arrayButtonsEventFilters[MAX_EVENTFILTERS_CHECKBUTTONS];												// array van de event check buttons...
	// CToolTipCtrl m_toolTipWarningInfo;
	DUOTECNO::DALI::CAppDaliInputControlDeviceInstances* m_properties;
};
