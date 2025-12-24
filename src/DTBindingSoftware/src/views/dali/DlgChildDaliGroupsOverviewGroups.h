#pragma once

#include "DlgChildDaliGroupsOverviewBase.h"

// CDlgChildDaliGroupsOverviewGroups dialog
class CDlgChildDaliGroupsOverviewGroups : public CDlgChildDaliGroupsOverviewBase 
{
	DECLARE_DYNAMIC(CDlgChildDaliGroupsOverviewGroups)

public:
	CDlgChildDaliGroupsOverviewGroups(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliGroupsOverviewGroups();

	virtual void init(int groupNumber);

	// Parent dialoog (=property dialoog) toestand gewijzigd.
	// Gebruiken om de dialogen up te daten of te melden dat er properties veranderd werdebn die
	// nog moeten doorgestuurd worden.
	virtual void OnParentPropertyPageActive(int groupNumber,bool activeDialog);		

	// return waarde bepaalt of de parent property dialogen moeten updaten.
	// TRUE: Update, FALSE: geen update.
	virtual BOOL OnParentPropertyPageLeave();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_GROUPOVERVIEW_GROUPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected);

	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	LONG OnUpdateModel(WPARAM wParam, LPARAM lParam);		// zie BUG-0184
	afx_msg void OnButtonClickedAddress(UINT nID);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	BOOL updateModel();
	BOOL updateModelDeviceFound();
	BOOL updateModelDeviceInGroup();
	BOOL updateModelDeviceNames();
	void UpdateBtnName(CButton* pButton, int shortAddress);

private:
	enum { MAX_DALI_ADDRESSES = 64 };
	BOOL m_modelNamesChanged;
	BOOL m_stateEnabled[MAX_DALI_ADDRESSES];
	BOOL m_stateChecked[MAX_DALI_ADDRESSES];
	CMap<int, int, CString, CString> m_mapNames;
	CString m_strToolTipText;
};
