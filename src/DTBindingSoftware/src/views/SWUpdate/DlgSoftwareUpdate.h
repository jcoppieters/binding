#pragma once

class AppIPCAdvInstallerUpdater;

// CDlgSoftwareUpdate dialog

class CDlgSoftwareUpdate : public CDialog
{
	DECLARE_DYNAMIC(CDlgSoftwareUpdate)

public:
	CDlgSoftwareUpdate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSoftwareUpdate();

	afx_msg void OnBnClickedButtonSearch();

// Dialog Data
	enum { IDD = IDD_DIALOG_SOFTWAREUPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	AppIPCAdvInstallerUpdater& getImpl( );

private:
	int m_typeSelection;
	AppIPCAdvInstallerUpdater* m_pImpl;	
};
