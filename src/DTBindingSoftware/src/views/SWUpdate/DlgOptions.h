#pragma once
#include "afxwin.h"

class AppIPCAdvInstallerUpdater;

// CDlgOptions dialog

#define WM_UPDATE_CONTROLS (WM_USER + 150)

class CDlgOptions : public CDialog
{
	DECLARE_DYNAMIC(CDlgOptions)

public:
	CDlgOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOptions();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();

	AppIPCAdvInstallerUpdater& getData( );
	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);

private:
	AppIPCAdvInstallerUpdater* m_pData;
	BOOL m_searchForUpdates;
	BOOL m_searchForBeta;
	CButton m_btnCheckSearch;
	CButton m_btnCheckBeta;
};
