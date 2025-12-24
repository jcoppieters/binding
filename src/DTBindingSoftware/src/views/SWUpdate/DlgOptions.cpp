// DlgOptions.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgOptions.h"

#include "app/appipc/appipcadvinstupdater/appipcadvinstupdater.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalSettings;
// ------------------------------------

// CDlgOptions dialog

IMPLEMENT_DYNAMIC(CDlgOptions, CDialog)

CDlgOptions::CDlgOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOptions::IDD, pParent), m_pData(NULL), m_searchForUpdates(TRUE), m_searchForBeta(FALSE)
{

}

CDlgOptions::~CDlgOptions()
{
	if (NULL != m_pData) {
		delete m_pData;
		m_pData = NULL;
	}
}

void CDlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_OPTIONS_SEARCH, m_btnCheckSearch);
	DDX_Control(pDX, IDC_CHECK_OPTIONS_BETA, m_btnCheckBeta);
}


BEGIN_MESSAGE_MAP(CDlgOptions, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
END_MESSAGE_MAP()


// CDlgOptions message handlers
BOOL CDlgOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// opvragen van de instellingen...
	AppIPCAdvInstallerUpdater& r = this->getData();
	m_searchForUpdates = r.getOptFlagCheckAtStartup();
	m_searchForBeta = r.getOptFlagCheckForBeta();

	// Updaten van de GUI.
	PostMessage( WM_UPDATE_CONTROLS );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptions::OnOK()
{
	// Indien er aanpassingen zijn dan de doorgeven (+ updaten aan het register.)
	AppIPCAdvInstallerUpdater& r = this->getData();

	const BOOL searchForUpdates = (m_btnCheckSearch.GetCheck() == BST_CHECKED);
	const BOOL searchForBeta = (m_btnCheckBeta.GetCheck() == BST_CHECKED);

	if (m_searchForUpdates != searchForUpdates) {
		r.setOptFlagCheckAtStartup(searchForUpdates);	
	}
	if (m_searchForBeta != searchForBeta) {
		r.setOptFlagCheckForBeta(searchForBeta);
	}

	CDialog::OnOK();
}

AppIPCAdvInstallerUpdater& CDlgOptions::getData( ) {
	if (NULL == m_pData) {
		m_pData = new AppIPCAdvInstallerUpdater( );
	}
	ASSERT(NULL != m_pData);
	return *m_pData;
}

// Search for updates at startup: Checkbox is enkel enabled in view mode expert.
LRESULT CDlgOptions::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	m_btnCheckSearch.EnableWindow(TMGetDisplaySettings()->IsModeExtended());
	m_btnCheckSearch.SetCheck(m_searchForUpdates == TRUE ? 1 : 0);
	m_btnCheckBeta.SetCheck(m_searchForBeta == TRUE ? 1 : 0);
	return TRUE;
}