// DlgSoftwareUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgSoftwareUpdate.h"
#include "app/appipc/appipcadvinstupdater/appipcadvinstupdater.h"

// CDlgSoftwareUpdate dialog

IMPLEMENT_DYNAMIC(CDlgSoftwareUpdate, CDialog)

CDlgSoftwareUpdate::CDlgSoftwareUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSoftwareUpdate::IDD, pParent)
	, m_typeSelection(0), m_pImpl(NULL)
{

}

CDlgSoftwareUpdate::~CDlgSoftwareUpdate()
{
	if (NULL != m_pImpl) {
		delete m_pImpl;
		m_pImpl = NULL;
	}
}

void CDlgSoftwareUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_RELEASE, m_typeSelection);
	DDV_MinMaxInt(pDX, m_typeSelection, 0, 2);
}


BEGIN_MESSAGE_MAP(CDlgSoftwareUpdate, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CDlgSoftwareUpdate::OnBnClickedButtonSearch)
END_MESSAGE_MAP()


// CDlgSoftwareUpdate message handlers

void CDlgSoftwareUpdate::OnBnClickedButtonSearch()
{
	UpdateData(TRUE);

	AppIPCAdvInstallerUpdater::SearchVersionType_t type = AppIPCAdvInstallerUpdater::SEARCHVERSIONS_EXTRA_SOFTWARE;
	switch(m_typeSelection) {
		case 0: {
			type = AppIPCAdvInstallerUpdater::SEARCHVERSIONS_RELEASE_ONLY;
		} break;
		case 1: {
			type = AppIPCAdvInstallerUpdater::SEARCHVERSIONS_RELEASE_AND_BETA;
		} break;
		case 2: {
			type = AppIPCAdvInstallerUpdater::SEARCHVERSIONS_EXTRA_SOFTWARE;
		} break;	
	}

	// Wanneer er software updates beschikbaar zijn wordt er een messagebox getoond.
	// Wanneer de gebruiker toestemt om de updates te installeren wordt de applicatie afgesloten.
	//
	// NOTE: Kunnen geen WM_CLOSE versturen naar de applicatie.
	// Als er nog aanpassingen moeten opgeslagen worden blijft de applicatie openstaan (op een messagebox die vraagt 
	// of de applicatie mag afgesloten worden) en komt de updater op de voorgrond.
	if (AppIPCAdvInstallerUpdater_Util::checkSoftwareUpdates(this,getImpl(),type)) {		
		AfxGetMainWnd()->DestroyWindow();		// Shutdown applicatie (NOTE)
	}
}

AppIPCAdvInstallerUpdater& CDlgSoftwareUpdate::getImpl( ) {
	if (NULL == m_pImpl) {
		m_pImpl = new AppIPCAdvInstallerUpdater( );
	}
	ASSERT(NULL != m_pImpl);
	return *m_pImpl;
}