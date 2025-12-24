// PropertyPageNodeConfigV2.cpp : implementation file
//

/////////////////////////////////////////////////////////////////////////////
// Oppassen: Dit bestand bevat implementatie van 
// - Basisklasse CPropertyPageNodeConfig_BaseV2
// - Afgeleide klasse: CPropertyPageNodeConfigV2
// - Afgeleide klasse: CPropertyPageNodeScreenmapV2
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeConfigV2.h"
#include "app\appcfg\appcfgalgview\configalgviewfactory.h"
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"		
#include "ExportTextFilesApp.h"									// since 0x0565
							
#include "utils/win32/ShellCommands.h"
#include "utils/win32/win32filefind.h"

/////////////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalSettings;
using namespace TMCommunication;
using namespace TMVaria;
/////////////////////////////////////////////////////////////////////////////
#pragma warning( disable:4355 )
/////////////////////////////////////////////////////////////////////////////

// static data

const int CPropertyPageNodeConfig_BaseV2::CHECKBUTTON_CONTROL_IDS[MAX_CHECKBUTTONS] =
{
	IDC_CHECK_CONFIG0,				// 0
	IDC_CHECK_CONFIG1,				// 1
	IDC_CHECK_CONFIG2,				// 2
	IDC_CHECK_CONFIG3,				// 3
	IDC_CHECK_CONFIG4,				// 4
	IDC_CHECK_CONFIG5,				// 5
	IDC_CHECK_CONFIG6,				// 6
	IDC_CHECK_CONFIG7,				// 7
	IDC_CHECK_CONFIG8,				// 8
	IDC_CHECK_CONFIG9,				// 9
	IDC_CHECK_CONFIG10,				// 10
	IDC_CHECK_CONFIG11				// 11
};


namespace
{	
	enum
	{
		TIMERID_UPDATE_TIMER			= 305,
		UPDATE_TIMER_VALUE				= 100
	};

	inline void showWindow(CWnd* pWnd,BOOL fState) {
		if (pWnd != 0) {
			pWnd->ShowWindow( fState );
		}
	}

	inline void enableWindow(CWnd* pWnd, BOOL fState) {
		if (pWnd != 0) {
			pWnd->EnableWindow( fState );
		}
	}

	inline void setCheck(CButton* pButton,BOOL fState) {
		if (pButton != 0) {
			pButton->SetCheck( fState );
		}
	}

	inline BOOL isChecked(CButton& btn) {
		return ((btn.GetState() & 0x03) == TRUE);
	}

	inline BOOL FindConfigurator(void) {
		return CFileFindFunctions::find(TMGetFileSettings()->dtConfigurator.getFullPathName());
	}

	BOOL CheckModemSoftware(void)
	{
		const CCommunicationStatus& refCommunicationStatus = TMCommunicationGetStatus();
		ASSERT(refCommunicationStatus.connected());
		const CCommunicationServerInfo refCommunicationServerInfo = refCommunicationStatus.getServerInfo();
		const CCommunicationServerInfo::type_t type = refCommunicationServerInfo.getCommunicationType();		
		if (CCommunicationServerInfo::TCPIP == type) {
			return TRUE;
		}
		else if (CCommunicationServerInfo::SERIAL == type) {
			const BYTE bSoftwareVersion = (refCommunicationServerInfo.getSoftwareVersion() & 0xFF00) >> 8;
			const BYTE bSoftwareRevision = (refCommunicationServerInfo.getSoftwareVersion()  & 0xFF);
			return checkModemSoftwareVersion(bSoftwareVersion,bSoftwareRevision);						
		}
		else {
			ASSERT(CCommunicationServerInfo::OFFLINE != type);
		}
		return FALSE;
	}

	inline void OpenTextEditor(const CString& szFileName) {
		TCommandShell<> cmd(szFileName, "edit");
		cmd.Execute();
	}
};

// CPropertyPageNodeConfig_BaseV2 dialog

IMPLEMENT_DYNAMIC(CPropertyPageNodeConfig_BaseV2, CPropertyPage)

CPropertyPageNodeConfig_BaseV2::CPropertyPageNodeConfig_BaseV2()
	: CPropertyPageNode(CPropertyPageNodeConfig_BaseV2::IDD),
	  m_ViewDisplay(*this),
	  m_fInitDialog(FALSE),
	  m_foundConfigurator(FALSE),
	  m_fSupportedByModemVersion(TRUE),
	  m_stringIndex( 0 ),
	  m_pAlgView(0),
	  m_MaxAlgoritmes(0)
#ifdef SKIP
	  ,m_fChanges(FALSE)
#endif
{

}

CPropertyPageNodeConfig_BaseV2::CPropertyPageNodeConfig_BaseV2(CCanNode* pNode,int maxAlgoritmes)
	: CPropertyPageNode(CPropertyPageNodeConfig_BaseV2::IDD,pNode),
	  m_MaxAlgoritmes(maxAlgoritmes),
	  m_ViewDisplay(*this),
	  m_fInitDialog(FALSE),
	  m_foundConfigurator(FALSE),
	  m_fSupportedByModemVersion(TRUE),
	  m_stringIndex( 0 ),
	  m_pAlgView(0)
#ifdef SKIP
	  ,m_fChanges(FALSE)
#endif
{	
	int i;

	m_pAlgView = new CConfigAlgView*[m_MaxAlgoritmes];

	for (i=0;i<m_MaxAlgoritmes;i++)
	{
		m_pAlgView[i] = 0;
	}
}

CPropertyPageNodeConfig_BaseV2::~CPropertyPageNodeConfig_BaseV2()
{
	if (m_fInitDialog)
	{
		deallocAlgoritmes( );
	}
}

void CPropertyPageNodeConfig_BaseV2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STATUS, m_listStatus);
}

BEGIN_MESSAGE_MAP(CPropertyPageNodeConfig_BaseV2, CPropertyPage)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_CONFIG0, IDC_CHECK_CONFIG11, OnCheckButtonClicked)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_RECEIVE, &CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonReceive)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_SEND_TO_ALL, &CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonSendToAll)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_OVERRIDE_HTTP, &CPropertyPageNodeConfig_BaseV2::OnBnClickedCheckOverrideHttp)
END_MESSAGE_MAP()


// CPropertyPageNodeConfig_BaseV2 message handlers

void CPropertyPageNodeConfig_BaseV2::deallocAlgoritmes(void)
{
	if (m_pAlgView != 0)
	{
		for (int i=0;i<m_MaxAlgoritmes;i++)
		{
			if (m_pAlgView[i] != 0)
			{
				CConfigAlgViewFactory::Destroy(&m_pAlgView[i]);
			}
		}

		delete [] m_pAlgView;
	}
}

void CPropertyPageNodeConfig_BaseV2::attachAlg2Buttons(void)
{
	int i;

	// Alle buttons disablen.
	for (i=0;i<MAX_CHECKBUTTONS;i++)
	{
		CButton* const pBtn  = (CButton*) GetDlgItem(CHECKBUTTON_CONTROL_IDS[i]);	
		showWindow(pBtn,FALSE);		
	}

	// Voor ieder gebruikt algoritme wordt een knop getoond.
	for (i=0;i<m_MaxAlgoritmes;i++)
	{
		CButton* const pBtn  = (CButton*) GetDlgItem(CHECKBUTTON_CONTROL_IDS[i]);
		if (pBtn)
		{
			if (m_pAlgView[i] != 0)
			{						
				showWindow(pBtn,TRUE);
				pBtn->SetWindowText(m_pAlgView[i]->getName());			
			}			
		}
	}
}

BOOL CPropertyPageNodeConfig_BaseV2::OnKillActive( )
{
	TRACE("OnKillActive()\r\n");

#ifdef SKIP

	if (m_fChanges)
	{
		m_fChanges = FALSE;

		// Indien er op OK gedrukt wordt dan zal de dialoog niet van toestand veranderen.
		// Indien op cancel gedrukt wordt gaat gewoon naar andere dialoog.
		if (IDOK == 
			AfxMessageBox("Do not forget to upload your changes",
						  MB_ICONINFORMATION | MB_OKCANCEL))
		{
			return 0;
		}
	}
#endif

	return CPropertyPage::OnKillActive();
}

void CPropertyPageNodeConfig_BaseV2::UpdateRunTimeControls()
{
	if (m_fInitDialog) { }
}


void CPropertyPageNodeConfig_BaseV2::UpdateStaticControls()
{
	BOOL fStateLoadButton = FALSE;
	BOOL fStateEditButton = FALSE;
	BOOL fStateUpload = FALSE;
	BOOL fStateUploadToAll = FALSE;
	BOOL fStateDownload = FALSE;
	BOOL fStateOverrideHTTP = FALSE;		// Checkbox + text tonen dat het mogelijk is om via de CAN-bus te versturen ipv HTTP.
	BOOL fStateUseUploadHTTPConfig = FALSE; // Bepaalt of de upload gebeurd via CAN of via HTTP.

	const BOOL fConnected = TMIsConnected(); 

	if (m_fInitDialog)
	{		 
		CConfigAlgView* pConfigAlgView = 0;

		if (TRUE == FindSelectedAlgoritme(&pConfigAlgView))
		{	
			ASSERT(pConfigAlgView != 0);	
					
			// Enkel Indien het bestand aanwezig is kan het mogelijks geladen worden.
			if (TRUE == pConfigAlgView->IsFileFound())
			{
				fStateLoadButton = pConfigAlgView->IsLoadSupported( );
			}
			
			// Indien de configurator nodig is controleren of deze aanwezig is.
			if (TRUE == pConfigAlgView->IsUseConfigurator())
			{
				if (m_foundConfigurator)
				{
					fStateEditButton = pConfigAlgView->IsSupported( );
				}
			}
			else
			{
				fStateEditButton = pConfigAlgView->IsSupported( );
			}

			// Wanneer de configuratie doorgestuurd wordt via het CAN FTP protocol
			// dan moet controleren of de modem software dit reeds ondersteund.
			if (TRUE == pConfigAlgView->IsFileFound())
			{
				if (pConfigAlgView->IsUseFTPProtocol( ))
				{
					if (m_fSupportedByModemVersion)
					{
						fStateUpload = pConfigAlgView->IsUploadCanSupported( );
						fStateUploadToAll = pConfigAlgView->IsUploadCanAllSupported( );	
						fStateDownload = pConfigAlgView->IsDownloadCanSupported( );
					}							
				}
				else
				{
					// Configuratie via External Config wordt ook door oudere 
					// modem versies ondersteund.
					fStateUpload = pConfigAlgView->IsUploadCanSupported( );
					fStateUploadToAll = pConfigAlgView->IsUploadCanAllSupported( );	
					fStateDownload = pConfigAlgView->IsDownloadCanSupported( );
				}
			}

			// Wanneer de cfg via HTTP gebeurd kan deze ook overriden worden om via CAN 
			// door te sturen. Nodig wanneer de node niet met het netwerk verbonden is.
			if (fConnected && (fStateUpload || fStateUploadToAll)) {
				fStateOverrideHTTP = pConfigAlgView->IsSupportedHTTPProtocol();
				if (fStateOverrideHTTP) {
					fStateUseUploadHTTPConfig = pConfigAlgView->IsUseUploadHTTPProtocol();
				}
			}

			// Sedert V16.01: Extra info weergeven in de listbox... zie CHANGES_SMAP_ONLY_FOR_APP
			if (0 == m_listStatus.GetCount()) {
				CStringArray messages;
				if (pConfigAlgView->getInfoMessages(&messages)) {
					const int count = messages.GetCount();
					for (int i=0;i<count;++i) {
						AddMessage(messages.GetAt(i));
					}				
				}
			}
			// Aanpassingen tot hier...

		}		

		// Hier de knoppen enablen/disablen...
		enableWindow(GetDlgItem(IDC_BUTTON_RECEIVE), fStateDownload && fConnected);	
		enableWindow(GetDlgItem(IDC_BUTTON_SEND), fStateUpload && fConnected && !fStateUseUploadHTTPConfig);	
		enableWindow(GetDlgItem(IDC_BUTTON_SEND_TO_ALL), fStateUploadToAll && fConnected && !fStateUseUploadHTTPConfig);
		enableWindow(GetDlgItem(IDC_BUTTON_LOAD), fStateLoadButton);
		enableWindow(GetDlgItem(IDC_BUTTON_EDIT), fStateEditButton);	
		showWindow(GetDlgItem(IDC_CHECK_OVERRIDE_HTTP), fStateOverrideHTTP);	// Override HTTP upload door CAN checkbox
		showWindow(GetDlgItem(IDC_STATIC_OVERRIDE_HTTP), fStateOverrideHTTP);	// Override HTTP upload door CAN text
		setCheck((CButton*)GetDlgItem(IDC_CHECK_OVERRIDE_HTTP), !fStateUseUploadHTTPConfig);
	}
}

BOOL CPropertyPageNodeConfig_BaseV2::OnInitDialog() 
{
	CPropertyPageNode::OnInitDialog();		

	if (!m_fInitDialog)
	{
		// Zoeken of de Configurator tool aanwezig is.
		m_foundConfigurator = FindConfigurator( );
				
		if (TRUE == TMIsConnected()) {
			if (FALSE == CheckModemSoftware()) {
				CString szMessage("Please update your modem software,some functions will not work");
				AddMessage( szMessage );
				m_fSupportedByModemVersion = FALSE;	
			}
		}

/*
		// Controle op de modem software versie.	
		if (FALSE == CheckModemSoftware())
		{
			if (TRUE == TMIsConnected())
			{
				CString szMessage("Please update your modem software," 
								  "some functions will not work");
				AddMessage( szMessage );
			}
			m_fSupportedByModemVersion = FALSE;	// !!!!!
		}		
*/
		allocAlgoritmes(m_pSelNode);	
		attachAlg2Buttons();			
		m_fInitDialog = TRUE;
	}

#ifdef SKIP
	m_fChanges = FALSE;
#endif

	UpdateStaticControls( );
	return TRUE;
}

void CPropertyPageNodeConfig_BaseV2::StartUpdateTimer( )
{
	SetTimer(TIMERID_UPDATE_TIMER, UPDATE_TIMER_VALUE, (TIMERPROC)NULL);
}
void CPropertyPageNodeConfig_BaseV2::StopUpdateTimer( )
{
	KillTimer( TIMERID_UPDATE_TIMER );
}

void CPropertyPageNodeConfig_BaseV2::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMERID_UPDATE_TIMER)
	{
		StopUpdateTimer( );
		UpdateStaticControls( );	
	}	
	CPropertyPageNode::OnTimer(nIDEvent);
}

void CPropertyPageNodeConfig_BaseV2::clearMessages( )
{
	m_listStatus.ResetContent( );
	m_stringIndex = 0;
}

void CPropertyPageNodeConfig_BaseV2::OnCheckButtonClicked(UINT nID)
{
	int nButton = nID - IDC_CHECK_CONFIG0;
    ASSERT( nButton >= 0 && nButton < 12 );

	clearMessages( );

	CButton* const pBtnSelected  = (CButton*) GetDlgItem(CHECKBUTTON_CONTROL_IDS[nButton]);
	if (pBtnSelected)
	{
		// Alle buttons check disablen, enkel de geselecteerde
		// laten zoals het is.
		for (int i=0;i<MAX_CHECKBUTTONS;i++)
		{
			CButton* const pBtn  = (CButton*) GetDlgItem(CHECKBUTTON_CONTROL_IDS[i]);			
			if (pBtn !=pBtnSelected)
			{
				setCheck(pBtn, FALSE);
			}			
		}
	}

	// Update timer starten.
	StartUpdateTimer( );	
}

void CPropertyPageNodeConfig_BaseV2::AddMessage(const char* s)
{
	m_listStatus.InsertString(m_stringIndex++,s);	
}

// Sedert V16.40:
// De 'CConfigAlgView' klasse kan zelf de configuratie openen.
void CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonEdit() {
	CConfigAlgView* pConfigAlgView = 0;
	if (TRUE == FindSelectedAlgoritme(&pConfigAlgView)) {	
		const BYTE bNodeAddress = m_pSelNode->getNodeAddress();
		if (TRUE == pConfigAlgView->openConfiguration(bNodeAddress)) {
			// OK configuratie geopend.
		}
		else if (TRUE == pConfigAlgView->IsUseConfigurator( )) {
			using namespace DUOTECNO::PLUGIN;
			View view = (DUOTECNO::PLUGIN::View) pConfigAlgView->GetViewID( );
			if (VIEW_NONE != view) {
				// Eerst worden alle txt + json bestanden opgeslagen die nodig zijn voor de configurator.
				// Daarna wordt de configurator geopend met de juiste view.
				TMShowConfigurator(view, bNodeAddress);
			}				
		}
		else {
			// Tekst bestand openen in text editor.
			CString szFullPathName;
			if (TRUE == pConfigAlgView->getFullPathName(0,&szFullPathName)) {
				if (szFullPathName.IsEmpty() == FALSE) {
					OpenTextEditor(szFullPathName);		
				}
			}
		}				
	}
}

// Sedert Versie 0x0E10: zie BUG_SOLVE_0158, Controle of het algoritme niet bezig is.
void CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonLoad()
{
	// Oppassen:
	// Indien de Upload niet mogelijk is bij een algoritme mag deze knop niet zichtbaar zijn...
	CConfigAlgView* pConfigAlgView = 0;
	if (TRUE == FindSelectedAlgoritme(&pConfigAlgView))
	{	
		ASSERT(0 != pConfigAlgView);	
		if (pConfigAlgView && pConfigAlgView->IsReady())
		{
			clearMessages( );	
			ASSERT(TRUE == pConfigAlgView->IsLoadSupported());			
			pConfigAlgView->Load( );				
		}
	}	
}

void CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonReceive()
{
	// TODO: Add your control notification handler code here
}

// Sedert Versie 0x0E10: zie BUG_SOLVE_0158, Controle of het algoritme niet bezig is.
void CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonSend()
{
	// Oppassen:
	// Indien de Upload niet mogelijk is bij een algoritme mag deze knop niet zichtbaar zijn...
	CConfigAlgView* pConfigAlgView = 0;
	if (TRUE == FindSelectedAlgoritme(&pConfigAlgView))
	{	
		ASSERT(0 != pConfigAlgView);	
		if (pConfigAlgView && pConfigAlgView->IsReady())
		{
			clearMessages( );				
			ASSERT(TRUE == pConfigAlgView->IsUploadCanSupported());		
			pConfigAlgView->Upload( );				
		}
	}
}

// Sedert Versie 0x0E10: zie BUG_SOLVE_0158, Controle of het algoritme niet bezig is.
void CPropertyPageNodeConfig_BaseV2::OnBnClickedButtonSendToAll()
{
	// Oppassen:
	// Indien de SendToAll niet mogelijk is bij een algoritme mag deze knop niet zichtbaar zijn...
	CConfigAlgView* pConfigAlgView = 0;
	if (TRUE == FindSelectedAlgoritme(&pConfigAlgView))
	{	
		ASSERT(0 != pConfigAlgView);	
		if (pConfigAlgView && pConfigAlgView->IsReady())
		{
			clearMessages( );			
			ASSERT(TRUE == pConfigAlgView->IsUploadCanAllSupported());
			pConfigAlgView->UploadToAll( );				
		}
	}
}

BOOL CPropertyPageNodeConfig_BaseV2::FindSelectedAlgoritme(CConfigAlgView** p)
{
	BOOL result = FALSE;
	int i;

	for (i=0;i<MAX_CHECKBUTTONS;i++)
	{
		CButton* const pBtn  = (CButton*) GetDlgItem(CHECKBUTTON_CONTROL_IDS[i]);

		if (pBtn != 0 && TRUE == isChecked(*pBtn))
		{
			CConfigAlgView* const pConfigAlgView = m_pAlgView[i];
			if (pConfigAlgView != 0)
			{
				*p = pConfigAlgView;
				result = TRUE;
				break;
			}
		}
	}
	return result;
}

void CPropertyPageNodeConfig_BaseV2::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CPropertyPageNode::OnActivate(nState, pWndOther, bMinimized);
	switch (nState)
	{
		case WA_INACTIVE: { } break;

		case WA_ACTIVE:
		case WA_CLICKACTIVE:
		{			
			// Wanneer de dialoog terug aktief wordt de controls updaten.
			// Verandering Online / Offline
			// Verandering configuratie gegevens.
			StartUpdateTimer( );
		} break;
	}
}


void CPropertyPageNodeConfig_BaseV2::OnClose()
{
	TRACE("OnClose\r\n");
	CPropertyPageNode::OnClose();
}

// Via de checkbox mogelijk maken dat de buttons om te uploaden via CAN ook mogelijk zijn.
void CPropertyPageNodeConfig_BaseV2::OnBnClickedCheckOverrideHttp()
{
	if (m_fInitDialog) {		 
		CConfigAlgView* pConfigAlgView = 0;
		if (TRUE == FindSelectedAlgoritme(&pConfigAlgView)) {
			CButton* const pBtn  = (CButton*) GetDlgItem(IDC_CHECK_OVERRIDE_HTTP);
			if (NULL != pBtn) {		
				pConfigAlgView->SetUseUploadHTTPProtocol(BST_UNCHECKED == pBtn->GetCheck());
			}
		}
		StartUpdateTimer();
	}	
}



// 
// CPropertyPageNodeConfigV2

const CConfigAlgViewID CPropertyPageNodeConfigV2::CONFIG_ALGORITME_IDS[MAX_ALGORITMES] =
{
	CONFIGALGVIEW_ID_CREATE_UNITS,	// 0
	CONFIGALGVIEW_ID_MOODS,			// 1
	CONFIGALGVIEW_ID_IRTX,			// 2
	CONFIGALGVIEW_ID_IRRX,			// 3
	CONFIGALGVIEW_ID_SCHEDULE,		// 4
	CONFIGALGVIEW_ID_BITMAPS,		// 5
	CONFIGALGVIEW_ID_MULTIMEDIA,	// 6
	CONFIGALGVIEW_ID_CFG_GATEWAY,	// 7
	CONFIGALGVIEW_ID_ROOMCTRL_AV,	// 8
	CONFIGALGVIEW_ID_BOSEST,		// 9
	CONFIGALGVIEW_ID_LEDSCFG,		// 10
	CONFIGALGVIEW_ID_SONOSSRC,		// 11
	CONFIGALGVIEW_ID_AVSETTINGS,	// 12
	CONFIGALGVIEW_ID_AVMATRIX,		// 13	
	CONFIGALGVIEW_ID_TRIGGERS,		// 14   
	CONFIGALGVIEW_ID_MOBILE_APP,	// 15
	CONFIGALGVIEW_ID_DALI			// 16	Sedert V16.33
};


CPropertyPageNodeConfigV2::CPropertyPageNodeConfigV2()
	: CPropertyPageNodeConfig_BaseV2( ) { }

CPropertyPageNodeConfigV2::CPropertyPageNodeConfigV2(CCanNode* pNode)
	: CPropertyPageNodeConfig_BaseV2(pNode,MAX_ALGORITMES) { }

CPropertyPageNodeConfigV2::~CPropertyPageNodeConfigV2() { }

void CPropertyPageNodeConfigV2::allocAlgoritmes(CCanNode* pNode)
{
	int i;
	int algoritme = 0;
	
	for (i=0;i<MAX_ALGORITMES;i++) {
		m_pAlgView[i] = 0;
	}		

	// Indien een algoritme ondersteund is wordt deze toegevoegd aan
	// het array.
	// Oppassen: Algoritmes worden aangemaakt via Create( ). Indien
	// niet gebruikt moet de Destroy( ) aangeroepen worden.
	for (i=0;i<MAX_ALGORITMES;i++)
	{
		CConfigAlgView* p = CConfigAlgViewFactory::Create(
								CONFIG_ALGORITME_IDS[i],
								pNode,
								&m_ViewDisplay);
		if (p) {
			if (TRUE == p->IsSupported()) {			
				m_pAlgView[algoritme++] = p;
			}
			else {
				CConfigAlgViewFactory::Destroy(&p);					
			}
		}
	}
}

// 
// CPropertyPageNodeScreenmapV2

const CConfigAlgViewID CPropertyPageNodeScreenmapV2::CONFIG_ALGORITME_IDS[MAX_ALGORITMES] =
{
	CONFIGALGVIEW_ID_SMAP_UNITS,	// 0
	CONFIGALGVIEW_ID_SMAP_MAIN,		// 1
	CONFIGALGVIEW_ID_SMAP_MYHOME,	// 2
	CONFIGALGVIEW_ID_SMAP_AVROOM,	// 3
};

CPropertyPageNodeScreenmapV2::CPropertyPageNodeScreenmapV2()
	: CPropertyPageNodeConfig_BaseV2( ) { }

CPropertyPageNodeScreenmapV2::CPropertyPageNodeScreenmapV2(CCanNode* pNode)
	: CPropertyPageNodeConfig_BaseV2(pNode,MAX_ALGORITMES) { }

CPropertyPageNodeScreenmapV2::~CPropertyPageNodeScreenmapV2() { }

void CPropertyPageNodeScreenmapV2::allocAlgoritmes(CCanNode* pNode)
{
	int i;
	int algoritme = 0;
	
	for (i=0;i<MAX_ALGORITMES;i++) {
		m_pAlgView[i] = 0;
	}		

	// Indien een algoritme ondersteund is wordt deze toegevoegd aan
	// het array.
	// Oppassen: Algoritmes worden aangemaakt via Create( ). Indien
	// niet gebruikt moet de Destroy( ) aangeroepen worden.
	for (i=0;i<MAX_ALGORITMES;i++) {
		CConfigAlgView* p = CConfigAlgViewFactory::Create(CONFIG_ALGORITME_IDS[i],pNode,&m_ViewDisplay);
		if (p) {
			if (TRUE == p->IsSupported()) {			
				m_pAlgView[algoritme++] = p;
			}
			else {
				CConfigAlgViewFactory::Destroy(&p);					
			}
		}
	}
}

