#pragma once

#include "PropertyPageNewApp.h"
#include "afxwin.h"

class CPropertyPageNewAppPage1 : public CPropertyPageNewApp
{
	DECLARE_DYNAMIC(CPropertyPageNewAppPage1)

public:
	CPropertyPageNewAppPage1(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx);
	virtual ~CPropertyPageNewAppPage1();

	virtual BOOL OnInitDialog();
	virtual void UpdateCommunicationStatus();
	virtual void StoreData( ) { /* absorbe */ }

// Dialog Data
	enum { IDD = IDD_DIALOG_NEWAPP_PAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);	
	afx_msg void OnRadioButtonsType(UINT nID);
	afx_msg void OnBnClickedButtonOpen();

private:
	int m_radioProjectType;
	CButton m_btnOpenProject;
	CEdit m_editProjectName;
	CStatic m_staticGroupOpen;
	CStatic m_staticInfoFooter;
};
