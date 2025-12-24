// PropertyPageNodeConfig.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeConfig.h"

/////////////////////////////////////////////////////////////////////////////
#include "cannode.h"
/////////////////////////////////////////////////////////////////////////////
#include "App/AppCfg/AppCfgMM/ConfigMultiMediaInfo.h"
#include "App/AppCfg/AppCfgMM/ConfigMultiMediafile.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"		

#include "utils/win32/win32filefind.h"
#include "utils/win32/ShellFunctions.h"	
#include "utils/win32/ShellCommands.h"

#include "TMAlgoritmesCfg.h"

/////////////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
using namespace TMCommunication;
using namespace TMVaria;
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeConfig property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeConfig, CPropertyPage)

CPropertyPageNodeConfig::CPropertyPageNodeConfig() : CPropertyPageNode(CPropertyPageNodeConfig::IDD)
{
	//{{AFX_DATA_INIT(CPropertyPageNodeConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


CPropertyPageNodeConfig::CPropertyPageNodeConfig(CCanNode *pNode):
CPropertyPageNode(CPropertyPageNodeConfig::IDD,pNode),
m_fInitControls(FALSE),
m_stringIndex( 0 ),
m_foundConfigurator( FALSE ),
m_foundMMConfig( FALSE )
{			
	ASSERT(m_pSelNode);
	ASSERT(m_fInitControls == FALSE);
	
	m_pButtons[0] = &m_CheckMultimedia;
	m_pButtons[1] = &m_CheckUnits;
	m_pButtons[2] = &m_CheckMoods;
	m_pButtons[3] = &m_CheckIRRX;
	m_pButtons[4] = &m_CheckIRTX;
	m_pButtons[5] = &m_CheckSchedule;
	m_pButtons[6] = &m_CheckBitmaps;
	m_pButtons[7] = &m_CheckSmapMain;
	m_pButtons[8] = &m_CheckSmapMyhome;
	m_pButtons[9] = &m_CheckSmapAVRoom;
	m_pButtons[10] = &m_CheckConfigGateway;
	m_pButtons[11] = &m_CheckConfigRoomAudio;

	for (int i=0;i<MAX_CHECKBUTTONS;i++)
	{
		ASSERT( m_pButtons[i] != 0 );
	}
}

CPropertyPageNodeConfig::~CPropertyPageNodeConfig()
{
	//StopUpdateTimer( );
}

void CPropertyPageNodeConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeConfig)
	DDX_Control(pDX, IDC_CHECK_CONFIG_ROOMCTRL_AUDIO, m_CheckConfigRoomAudio);
	DDX_Control(pDX, IDC_CHECK_CONFIG_GATEWAY, m_CheckConfigGateway);
	DDX_Control(pDX, IDC_CHECK_CONFIG_SMAP_AVROOM, m_CheckSmapAVRoom);
	DDX_Control(pDX, IDC_CHECK_CONFIG_SMAP_MYHOME, m_CheckSmapMyhome);
	DDX_Control(pDX, IDC_CHECK_CONFIG_SMAP_MAIN, m_CheckSmapMain);
	DDX_Control(pDX, IDC_CHECK_CONFIG_BITMAPS, m_CheckBitmaps);
	DDX_Control(pDX, IDC_CHECK_CONFIG_SCHEDULE, m_CheckSchedule);
	DDX_Control(pDX, IDC_CHECK_CONFIG_IRTX, m_CheckIRTX);
	DDX_Control(pDX, IDC_CHECK_CONFIG_IRRX, m_CheckIRRX);
	DDX_Control(pDX, IDC_CHECK_CONFIG_MULTIMEDIA, m_CheckMultimedia);
	DDX_Control(pDX, IDC_CHECK_CONFIG_UNITS, m_CheckUnits);
	DDX_Control(pDX, IDC_CHECK_CONFIG_MOODS, m_CheckMoods);
	DDX_Control(pDX, IDC_LIST_STATUS, m_listStatus);
	//}}AFX_DATA_MAP			
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeConfig, CPropertyPageNode)
	//{{AFX_MSG_MAP(CPropertyPageNodeConfig)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_MOODS, OnCheckConfigMoods)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_UNITS, OnCheckConfigUnits)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_SEND_TO_ALL, OnButtonSendToAll)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_MULTIMEDIA, OnCheckConfigMultimedia)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_IRRX, OnCheckConfigIrrx)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_IRTX, OnCheckConfigIrtx)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_SCHEDULE, OnCheckConfigSchedule)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_BITMAPS, OnCheckConfigBitmaps)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_SMAP_MAIN, OnCheckConfigSmapMain)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_SMAP_MYHOME, OnCheckConfigSmapMyhome)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_SMAP_AVROOM, OnCheckConfigSmapAVRoom)
	ON_BN_CLICKED(IDC_CHECK_CONFIG_GATEWAY, &CPropertyPageNodeConfig::OnCheckConfigGateway)
ON_BN_CLICKED(IDC_CHECK_CONFIG_ROOMCTRL_AUDIO, &CPropertyPageNodeConfig::OnBnClickedCheckConfigRoomctrlAudio)
	//}}AFX_MSG_MAP		
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeConfig message handlers


/////////////////////////////////////////////////////////////////////////////

namespace
{
	
	inline void showWindow(CWnd* pWnd,BOOL fState)
	{
		if (pWnd != 0)
		{
			pWnd->ShowWindow( fState );
		}
	}

	inline void enableWindow(CWnd* pWnd, BOOL fState)
	{
		if (pWnd != 0)
		{
			pWnd->EnableWindow( fState );
		}
	}

	inline void setCheck(CButton* pButton,BOOL fState)
	{
		if (pButton != 0)
		{
			pButton->SetCheck( fState );
		}
	}

	inline BOOL isChecked(CButton& btn)
	{
		return ((btn.GetState() & 0x03) == TRUE);
	}

	enum
	{
		TIMERID_UPDATE_TIMER			= 305,
		UPDATE_TIMER_VALUE				= 100
	};

	template <class ERROR_FILE>
		void reportErrors(CPropertyPageNodeConfig& dlg,ERROR_FILE& configFile)
	{
		const int errors = configFile.GetNrErrors();
		if (errors != 0) 
		{										
			for (int i=0;i<errors;i++) 
			{
				dlg.AddMessage(configFile.GetErrors(i));								
			}				
		}
	}
	
	void reportErrors(CPropertyPageNodeConfig& dlg,DUOTECNO::MFC_HELPER::CArrayErrors& arrErrors)
	{
		const int errors = arrErrors.GetNrErrors( );	
		if (errors != 0) 
		{										
			for (int i=0;i<errors;i++) 
			{
				const char* szString = 0;

				if (arrErrors.GetErrors(i,&szString))
				{						
					dlg.AddMessage(szString);		
				}
			}				
		}
	}

};

/////////////////////////////////////////////////////////////////////////////
void CPropertyPageNodeConfig::onCheckConfigButton(CWnd* pSelected)
{
	clearMessages( );

	// Alle buttons check disablen, enkel de geselecteerde
	// laten zoals het is.
	for (int i=0;i<MAX_CHECKBUTTONS;i++)
	{
		if (m_pButtons[i] != 0 )
		{
			if (m_pButtons[i] != pSelected)
			{
				setCheck(m_pButtons[i], FALSE);
			}
		}
	}
}

void CPropertyPageNodeConfig::StartUpdateTimer( )
{
	SetTimer(TIMERID_UPDATE_TIMER,
		     UPDATE_TIMER_VALUE,
			 (TIMERPROC)NULL);
}
void CPropertyPageNodeConfig::StopUpdateTimer( )
{
	KillTimer( TIMERID_UPDATE_TIMER );
}

BOOL CPropertyPageNodeConfig::OnInitDialog() 
{
	CPropertyPageNode::OnInitDialog();		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

	// Doordat er nieuwe berichten in de modem software nodig zijn,
	// worden niet alle functies ondersteund.
	// Daarom de m_fSupportedByModemVersion vlag.

	m_fSupportedByModemVersion = TRUE;

	m_foundMMConfig = CFileFindFunctions::find(CConfigMultiMediaInfo::GetInstance( )->GetFullPathName( ));

	if (m_pSelNode->GetSoftwareVersion( ) == 0x65 ||
		m_pSelNode->GetSoftwareVersion( ) == 0x66 ||
		m_pSelNode->GetSoftwareVersion( ) == 0x90)
	{		
		m_foundConfigurator = CFileFindFunctions::find(TMGetFileSettings()->dtConfigurator.getFullPathName());
		
		if (TMIsConnected()) {
			const CCommunicationServerInfo& serverInfo = TMCommunicationGetStatus().getServerInfo();
			const CCommunicationServerInfo::type_t type = serverInfo.getCommunicationType();
			ASSERT(CCommunicationServerInfo::OFFLINE != type);
			if (CCommunicationServerInfo::SERIAL == type) {		
				const BYTE bSoftwareVersion = (serverInfo.getSoftwareVersion() & 0xFF00)>>8;
				const BYTE bSoftwareRevision = (serverInfo.getSoftwareVersion() & 0xFF);		
				if (!checkModemSoftwareVersion(bSoftwareVersion,bSoftwareRevision)) {
					CString szMessage("Please update your modem software,some functions will not work");
					AddMessage( szMessage );			
					m_fSupportedByModemVersion = FALSE;	// !!!!!
				}
			}
		}				
	}

	if (1)
	{	
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
				EXTERNAL_CONFIG_IRTX);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_IRTX), fShowWindow);	
	}
	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_SCHEDULE);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_SCHEDULE), fShowWindow);
	}
	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_MULTIMEDIA);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_MULTIMEDIA), fShowWindow);
	}
	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_IRRX);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_IRRX), fShowWindow);
	}
	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_UNITS_CREATE);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_UNITS), fShowWindow);
	}
	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_MOODS);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_MOODS), fShowWindow);
	}

	if (1)
	{
		// Oppassen: Bitmap configuratie is enkel mogelijk met de nieuwe
		// modem software versie.

		bool fShowWindow = false;
		if (CNodeBitmapConfig_Util::isSupported(m_pSelNode))
		{
			if (CNodeBitmapConfig_Util::getNodetype(m_pSelNode) ==
				NODEBITMAP_CONFIG_TYPE_V65)
			{
				fShowWindow = true;
			}
		}
		m_CheckBitmaps.ShowWindow( fShowWindow );		
	}

	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_SMAP_MAIN);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_SMAP_MAIN), fShowWindow);
	}

	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_SMAP_MYHOME);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_SMAP_MYHOME), fShowWindow);
	}

	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_SMAP_AVROOM);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_SMAP_AVROOM), fShowWindow);
	}

	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_GATEWAY);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_GATEWAY), fShowWindow);
	}

	if (1)
	{
		const bool fShowWindow = 
			CNodeExtConfigInfo_Util::isSupported(m_pSelNode,
						EXTERNAL_CONFIG_ROOMCTRL_AUDIO);
		showWindow(GetDlgItem(IDC_CHECK_CONFIG_ROOMCTRL_AUDIO), fShowWindow);
	}
	
	enableWindow(GetDlgItem(IDC_BUTTON_RECEIVE), FALSE);	

	m_fInitControls = TRUE;
	UpdateStaticControls( );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyPageNodeConfig::UpdateStaticControls()
{
	BOOL fStateLoadButton = FALSE;
	BOOL fStateEditButton = FALSE;
	BOOL fStateUpload = FALSE;
	BOOL fStateUploadToAll = FALSE;
	BOOL fStateButtons = FALSE;
	const BOOL fConnected = TMIsConnected();

	if (m_fInitControls)
	{
		if (isChecked(m_CheckUnits))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			// CHANGES_AV_UNIVERSAL.
			// Voor de universele AV-Interface kunnen de units niet via de CAN-Bus verstuurd worden.
			// Deze moeten via ip verstuurd worden...
			ASSERT(0!=m_pSelNode);
			if (UNITSCREATE_TYPE_UNITCONFIG_IP != CNodeCreateDynUnits_Util::getNodeType(m_pSelNode)) {
				fStateUpload = TRUE;				
			}
		}
		if (isChecked(m_CheckMoods))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateUpload = TRUE;
		}
		if (isChecked(m_CheckMultimedia))
		{			
			/***********************************************************************
			 * Edit/Config is altijd zichtbaar wnr Configurator tool aanwezig is.
			 * Kunnen enkel het bestand doorsturen wnr deze aanwezig is.
			 ***********************************************************************/

			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			if (m_foundMMConfig)
			{
				fStateUpload = TRUE;	
				fStateUploadToAll = TRUE;
			}

		}
		if (isChecked(m_CheckIRRX))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateUpload = TRUE;		
				
		}
		if (isChecked(m_CheckSchedule))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateUpload = TRUE;			
		}
		if (isChecked(m_CheckIRTX))
		{		
			// Oppassen: IRTX configuratie is enkel mogelijk met de nieuwe
			// modem software versie.

			if (m_fSupportedByModemVersion)
			{
				fStateUpload = TRUE;
			}

			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateLoadButton = FALSE;
		}

		if (isChecked(m_CheckBitmaps))
		{
			// Oppassen: Bitmap configuratie is enkel mogelijk met de nieuwe
			// modem software versie.

			if (m_fSupportedByModemVersion)
			{
				fStateUpload = TRUE;
			}

			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateLoadButton = FALSE;
		}

		if (isChecked(m_CheckSmapMain))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateUpload = TRUE;			
		}

		if (isChecked(m_CheckSmapMyhome))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateUpload = TRUE;			
		}

		if (isChecked(m_CheckSmapAVRoom))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateUpload = TRUE;			
		}

		if (isChecked(m_CheckConfigGateway))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			fStateUpload = TRUE;	
		}
		if (isChecked(m_CheckConfigRoomAudio))
		{
			if (m_foundConfigurator)
			{
				fStateEditButton = TRUE;	
			}

			// Evt nog controleren of het bestand aanwezig is om de upload knop te tonen.

			fStateUpload = TRUE;	
		}

		for (int i=0;i<MAX_CHECKBUTTONS;i++)
		{
			if( m_pButtons[i] != 0 )
			{
				if (isChecked(*m_pButtons[i]))
				{
					fStateButtons = TRUE;
				}
			}
		}

		enableWindow(GetDlgItem(IDC_BUTTON_SEND), fStateUpload && fConnected);	
		enableWindow(GetDlgItem(IDC_BUTTON_SEND_TO_ALL), fStateUploadToAll && fConnected);
		enableWindow(GetDlgItem(IDC_BUTTON_LOAD), fStateButtons && fStateLoadButton);
		enableWindow(GetDlgItem(IDC_BUTTON_EDIT), fStateButtons && fStateEditButton);		
	}
}

void CPropertyPageNodeConfig::UpdateRunTimeControls()
{
	if (m_fInitControls)
	{
	
	}
}

void CPropertyPageNodeConfig::OnButtonLoad() 
{
	const BYTE bNodeAddress = m_pSelNode->bLogicalAddress;
	if (1)
	{
		if (isChecked(m_CheckMultimedia))
		{
			CConfigMultiMediaInfo& configInfo =
				*CConfigMultiMediaInfo::GetInstance( );

			if (configInfo.Load(m_pSelNode))
			{
				AddMessage("Multimedia config file load done");
			}
			else
			{
				AddMessage("Multimedia config file load failed");
				reportErrors(*this, *configInfo.getErrors());
			}		
		}
	}
	StartUpdateTimer( );
}


void CPropertyPageNodeConfig::OnButtonSend() 
{	
	// Private klasse om mee te geven met TMAlgoritmesCfg

	class CNotifier : public TMAlgoritmesCfg::INotifier
	{
	public:
		CNotifier(CPropertyPageNodeConfig& ref) : m_parent(ref) {};
		~CNotifier( ) { };
		virtual void AddMessage(const char* s)
		{
			m_parent.AddMessage(s);
		}
	private:
		CPropertyPageNodeConfig& m_parent;
	};

	// ...

	clearMessages( );

	const BYTE bNodeAddress = m_pSelNode->bLogicalAddress;

	if (isChecked(m_CheckUnits)) 		
	{
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadUnitCreate(m_pSelNode,notifier);	
	}
	else if (isChecked(m_CheckMoods))		
	{
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadMoods(m_pSelNode,notifier);					
	}
	else if (isChecked(m_CheckMultimedia))		
	{
		CConfigMultiMediaInfo& configInfo =
			*CConfigMultiMediaInfo::GetInstance( );

		ASSERT( m_pSelNode );

		if (configInfo.Load(m_pSelNode))
		{
			ASSERT( configInfo.IsFileLoaded( ));

			AddMessage("Multimedia config file load done");

			const BYTE bNodeAddress = m_pSelNode->bLogicalAddress;
			if (CConfigMultiMediaInfo::GetInstance()->InitNodeEntries(FALSE,
														   TMGetDocumentNodes(),
														   bNodeAddress))
			{
				TMGetGlobalHSM()->EvOperationUploadMMConfig();	
				AddMessage("multimedia config file send...");
			}
			else
			{
				AddMessage("Error: No nodes found.");
			}
		}
		else
		{
			AddMessage("Multimedia config file load failed");
			reportErrors(*this, *configInfo.getErrors());
		}		
	}
	else if (isChecked(m_CheckIRRX))		
	{
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadIRRX(m_pSelNode,notifier);			
	}
	else if (isChecked(m_CheckSchedule))
	{
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadScheduling(m_pSelNode,notifier);				
	}
	else if (isChecked(m_CheckIRTX))
	{	
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadIRTX(m_pSelNode,notifier);			
	}
	else if (isChecked(m_CheckBitmaps))
	{	
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadBitmaps(m_pSelNode,notifier);		
	}
	else if (isChecked(m_CheckSmapMain))
	{
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadSmapMain(m_pSelNode,notifier);	
	}
	else if (isChecked(m_CheckSmapMyhome))
	{	
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadSmapMyHome(m_pSelNode,notifier);			
	}
	else if (isChecked(m_CheckSmapAVRoom))
	{	
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadSmapAVRoom(m_pSelNode,notifier);
	}
	else if (isChecked(m_CheckConfigGateway))		
	{
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadGatewayCfg(m_pSelNode,notifier);
	}
	else if (isChecked(m_CheckConfigRoomAudio))		
	{
		CNotifier notifier(*this);
		TMAlgoritmesCfg::TMUploadRoomCtrlAudioCfg(m_pSelNode,notifier);
	}

	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::AddMessage(const char* s)
{
	m_listStatus.InsertString(m_stringIndex++,s);	
}

void CPropertyPageNodeConfig::clearMessages( )
{
	m_listStatus.ResetContent( );
	m_stringIndex = 0;
}

void CPropertyPageNodeConfig::OnCheckConfigMoods() 
{
	onCheckConfigButton( &m_CheckMoods );
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::OnCheckConfigUnits() 
{
	onCheckConfigButton( &m_CheckUnits );
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::OnCheckConfigMultimedia() 
{
	onCheckConfigButton( &m_CheckMultimedia );
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::OnCheckConfigIrrx() 
{
	onCheckConfigButton( &m_CheckIRRX );
	StartUpdateTimer( );	
}
void CPropertyPageNodeConfig::OnCheckConfigIrtx() 
{
	onCheckConfigButton( &m_CheckIRTX );
	StartUpdateTimer( );		
}

void CPropertyPageNodeConfig::OnCheckConfigSchedule() 
{
	onCheckConfigButton( &m_CheckSchedule );
	StartUpdateTimer( );	
}

void CPropertyPageNodeConfig::OnCheckConfigBitmaps() 
{
	onCheckConfigButton( &m_CheckBitmaps );
	StartUpdateTimer( );	
}

void CPropertyPageNodeConfig::OnCheckConfigSmapMain() 
{
	onCheckConfigButton( &m_CheckSmapMain );
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::OnCheckConfigSmapMyhome() 
{
	onCheckConfigButton( &m_CheckSmapMyhome );
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::OnCheckConfigSmapAVRoom() 
{
	onCheckConfigButton( &m_CheckSmapAVRoom );
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::OnCheckConfigGateway()
{
	onCheckConfigButton( &m_CheckConfigGateway );
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::OnBnClickedCheckConfigRoomctrlAudio()
{
	onCheckConfigButton( &m_CheckConfigRoomAudio );
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMERID_UPDATE_TIMER)
	{
		StopUpdateTimer( );
		UpdateStaticControls( );	
	}	
	CPropertyPageNode::OnTimer(nIDEvent);
}

void CPropertyPageNodeConfig::OnButtonEdit() 
{	
	using namespace DUOTECNO::PLUGIN;
	View view = VIEW_NONE;

	if (isChecked(m_CheckUnits))
	{
		view = VIEW_UNITS;		
	}
	else if (isChecked(m_CheckMoods))
	{
		view = VIEW_MOODS;
	}
	else if (isChecked(m_CheckMultimedia))
	{		
		view = VIEW_AVMM;	
	}
	else if (isChecked(m_CheckIRRX))
	{
		view = VIEW_IRRX;	
	}
	else if (isChecked(m_CheckIRTX))
	{
		view = VIEW_IRTX;	
	}
	else if (isChecked(m_CheckSchedule))
	{
		view = VIEW_SCHEDULE;		
	}
	else if (isChecked(m_CheckBitmaps))
	{
		view = VIEW_BITMAPS;		
	}
	else if (isChecked(m_CheckSmapMain))
	{
		view = VIEW_SMAP_MAIN;
	}
	else if (isChecked(m_CheckSmapMyhome))
	{
		view = VIEW_SMAP_MYHOME;
	}
	else if (isChecked(m_CheckSmapAVRoom))
	{
		view = VIEW_SMAP_AVROOM;
	}
	else if (isChecked(m_CheckConfigGateway))
	{
		view = VIEW_CONFIG_GATEWAY;
	}
	else if (isChecked(m_CheckConfigRoomAudio))
	{
		view = VIEW_ROOMCTRL_AUDIOCFG;
	}

	if (view !=VIEW_NONE)
	{
		// Eerst worden alle txt + json bestanden opgeslagen die nodig zijn voor de configurator.
		// Daarna wordt de configurator geopend met de juiste view.
		TMShowConfigurator(view,m_pSelNode->getNodeAddress());		
	}
}

void CPropertyPageNodeConfig::OnButtonSendToAll() 
{
	ASSERT( CConfigMultiMediaInfo::GetInstance() );

	if (isChecked(m_CheckMultimedia))		
	{
		clearMessages( );

		CConfigMultiMediaInfo& configInfo =
			*CConfigMultiMediaInfo::GetInstance( );
		
		ASSERT( m_pSelNode );

		if (configInfo.Load(m_pSelNode))
		{
			ASSERT( configInfo.IsFileLoaded( ));

			configInfo.SendToAllNodes(TRUE);			
			AddMessage("Multimedia config file load done");

			if (configInfo.InitNodeEntries(TRUE,
							    TMGetDocumentNodes(),
								m_pSelNode->bLogicalAddress))
			{
				TMGetGlobalHSM()->EvOperationUploadMMConfig();			
				AddMessage("multimedia config file send to all ...");
			}
			else
			{
				AddMessage("Error: No nodes found.");
			}
		}
		else
		{
			AddMessage("Multimedia config file load failed");
			reportErrors(*this, *configInfo.getErrors());
		}
	}
	
	StartUpdateTimer( );
}

void CPropertyPageNodeConfig::PostNcDestroy() 
{
	using namespace DUOTECNO::PLUGIN;
	IConfigToolApp::getInstance().close( );	
	
	CPropertyPageNode::PostNcDestroy();
}

////////////////////////////////////////////////////////



