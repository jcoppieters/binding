#pragma once

#include "PropertyPageDaliErrorsBaseClass.h"

// CPropertyPageDaliErrorsOverview dialog

class CPropertyPageDaliErrorsOverview : public CPropertyPageDaliErrorsBaseClass
{
	DECLARE_DYNAMIC(CPropertyPageDaliErrorsOverview)

public:
	CPropertyPageDaliErrorsOverview(CAppDlgDaliWorker& refWorker,CPropertyPageDaliErrorsContext& refContext);   
	virtual ~CPropertyPageDaliErrorsOverview();

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_ERRORS_OVERVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	void initStaticTextHeader( );
	void initStaticTextFooter( );
	void initStaticIcon( );

private:
	CStatic m_staticInfoHeader;	
	CStatic m_staticInfoFooter;	
	CStatic m_staticIcon;
};
