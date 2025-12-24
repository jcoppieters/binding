#pragma once

#include "PropertyPageDaliErrorsBaseClass.h"

// CPropertyPageDaliErrorsDetail dialog

class CPropertyPageDaliErrorsDetail : public CPropertyPageDaliErrorsBaseClass
{
	DECLARE_DYNAMIC(CPropertyPageDaliErrorsDetail)

public:
	CPropertyPageDaliErrorsDetail(CAppDlgDaliWorker& refWorker,CPropertyPageDaliErrorsContext& refContext);   
	virtual ~CPropertyPageDaliErrorsDetail();

// Dialog Data
	enum { IDD = IDD_DIALOG_DALI_ERRORS_DETAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()

	void initListCtrl( );
	void updateListCtrl( );
	void addEntry(DUOTECNO::DALI::CAppDaliFsmErrorEntry& r);

private:

	enum {
		COLUMN_DEVICE_ADDRESS		= 0,
		COLUMN_DEVICE_TYPE			= 1,	
		COLUMN_DEVICE_DESCRIPTION	= 2
	};

	CListCtrl m_listErrorDetails;	
};
