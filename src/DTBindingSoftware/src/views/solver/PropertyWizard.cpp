// PropertyWizard.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizard.h"
#include "PropertyWizardPage0.h"
#include "PropertyWizardPage1.h"
#include "PropertyWizardPage2.h"
#include "PropertyWizardPage3.h"
#include "PropertyWizardPage4.h"
#include "PropertyWizardPage5.h"
#include "PropertyWizardPage6.h"
#include "PropertyWizardPage7.h"
#include "PropertyWizardPage8.h"
#include "PropertyWizardPage9.h"
//#include "PropertyWizardPage10.h"
#include "PropertyWizardPage11.h"
#include "PropertyWizardPage12.h"
#include "PropertyWizardPage13.h"
#include "MainFrm.h"
#include "app\applicactcode\applicActCodeApi.h"
#include "app/appnodedbase/appnodedbase.h"
#include "app\appstate\appstate.h"
#include "uicontrols/utils/dlghelper.h"				// GUI fonts
/*****************************************************/
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*****************************************************/
using namespace DUOTECNO;
/*****************************************************/

#define WIZARD_PAGE1	1
#define WIZARD_PAGE2	2
#define WIZARD_PAGE3	3
#define WIZARD_PAGE4	4
#define WIZARD_PAGE5	5


void CPropertySequence::Init(WizardTasks_t task,CPropertyWizard *pSheet)
{
	m_bActiveChain = 0;
	m_pWizard = pSheet;

	for (int i=0;i<WIZARDPAGE_MAX;i++) {	
		for (int j=0;j<MAX_PROPERTY_SEQUENCES;j++) {		
			m_Chain[i][j] = 0xFF;
		}
	}		

	switch (task)
	{
		case WIZARD_CONTINUE_FACTORYADDRESS :
		{
			// Sedert V16.16:
			// Enkel deze mogelijkheid tonen wanneer er nodes aanwezig zijn in het CAN-bus netwerk 
			// die een ander adres hebben in de nodedatabase van de pc-database.
			const BOOL showOptionsRestoreFromFile = CAppNodeDatabase::GetInstance()->possibleToResolveAddressFromDbase();
			if (showOptionsRestoreFromFile) {
				m_Chain[WIZARDPAGE_CHOICE_PROGRAMFILE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PROGRAMFILE;
			}

			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_MANUALASSIGN;		
			m_Chain[WIZARDPAGE_CHOICE_PNP][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PNP;

			// Sedert V16.16: 
			// Enkel deze mogelijkheid tonen wanneer er nodes kunnen vervangen worden.
			// Dit is wanneer niet alle nodes in de pc database aanwezig zijn in het netwerk.
			const BOOL showOptionReplaceNodes = CAppNodeDatabase::GetInstance()->possibleToReplaceNodes();
			if (showOptionReplaceNodes) {
				m_Chain[WIZARDPAGE_CHOICE_REPLACE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_REPLACE;
			}
		
		} break;
	
		case WIZARD_CONTINUE_DUPLICATESFOUND :
		{
			// Sedert V16.16
			// Enkel deze mogelijkheid tonen wanneer er nodes aanwezig zijn in het CAN-bus netwerk 
			// die een ander adres hebben in de nodedatabase van de pc-database.
			const BOOL showOptionsRestoreFromFile = CAppNodeDatabase::GetInstance()->possibleToResolveAddressFromDbase();
			if (showOptionsRestoreFromFile) {
				m_Chain[WIZARDPAGE_CHOICE_PROGRAMFILE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PROGRAMFILE;
			}

			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_MANUALASSIGN;
			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE2] = WIZARDPAGE_CHOICE_INIT;
			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE3] = WIZARDPAGE_CHOICE_PNP;

			m_Chain[WIZARDPAGE_CHOICE_INIT][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_INIT;
			m_Chain[WIZARDPAGE_CHOICE_INIT][WIZARD_PAGE2] = WIZARDPAGE_CHOICE_PROGRAMFILE;
			m_Chain[WIZARDPAGE_CHOICE_INIT][WIZARD_PAGE3] = WIZARDPAGE_CHOICE_PNP;

			// Sedert V16.16: 
			// Enkel deze mogelijkheid tonen wanneer er nodes kunnen vervangen worden.
			// Dit is wanneer niet alle nodes in de pc database aanwezig zijn in het netwerk.
			const BOOL showOptionReplaceNodes = CAppNodeDatabase::GetInstance()->possibleToReplaceNodes();
			if (showOptionReplaceNodes) {
				m_Chain[WIZARDPAGE_CHOICE_REPLACE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_REPLACE;
			}

		} break;

		case WIZARD_NEW_FACTORYADDRESS :
		{	
			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_MANUALASSIGN;
			m_Chain[WIZARDPAGE_CHOICE_PNP][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PNP;

		} break;

		case WIZARD_NEW_DUPLICATESFOUND :
		{
			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_MANUALASSIGN;
			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE2] = WIZARDPAGE_CHOICE_INIT;
			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE3] = WIZARDPAGE_CHOICE_PNP;

			m_Chain[WIZARDPAGE_CHOICE_INIT][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_INIT;
			m_Chain[WIZARDPAGE_CHOICE_INIT][WIZARD_PAGE2] = WIZARDPAGE_CHOICE_PNP;

		} break;

		case WIZARD_CONTINUE_FACTORYADDRESS_ASSIGN_MASTER: {

			// Sedert V16.16:
			// Enkel deze mogelijkheid tonen wanneer er nodes aanwezig zijn in het CAN-bus netwerk 
			// die een ander adres hebben in de nodedatabase van de pc-database.
			const BOOL showOptionsRestoreFromFile = CAppNodeDatabase::GetInstance()->possibleToResolveAddressFromDbase();
			if (showOptionsRestoreFromFile) {
				m_Chain[WIZARDPAGE_CHOICE_PROGRAMFILE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PROGRAMFILE;
			}

			// Manueel toekennen van een adres.
			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_MANUALASSIGN;		

			// Mogelijkheid om direct een PNP uit te voeren.
			m_Chain[WIZARDPAGE_CHOICE_PNP][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PNP;

			// Manueel toekennen van een adres.
			m_Chain[WIZARDPAGE_CHOICE_ASSIGNMASTER][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_ASSIGNMASTER;		
		} break;

		case WIZARD_NEW_FACTORYADDRESS_ASSIGN_MASTER: {
			// Manueel toekennen van een adres.
			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_MANUALASSIGN;		

			// Mogelijkheid om direct een PNP uit te voeren.
			m_Chain[WIZARDPAGE_CHOICE_PNP][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PNP;

			// Manueel toekennen van een adres.
			m_Chain[WIZARDPAGE_CHOICE_ASSIGNMASTER][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_ASSIGNMASTER;	

		} break;

		case WIZARD_CONTINUE_NODESADDED:
		{
			// Sedert V16.16:
			// Enkel tonen welke nodes er gevonden werden.
			m_Chain[WIZARDPAGE_CHOICE_NODESADDED][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_NODESADDED;

		} break;

		case WIZARD_CONTINUE_NODESREMOVED:
		{
			// Sedert V16.16:
			// Wanneer er nodes verwijderd werden / of niet gevonden werden dan enkel de optie.
			// nodes removed tonen.
			//
			// TODO ---> DE 'FINISHED' knop enkel wanneer alle nodes verwijderd werden...
			//
			m_Chain[WIZARDPAGE_CHOICE_NODESADDED][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_NODESADDED;

		} break;


		case WIZARD_CONTINUE_NODESINCOMPATIBLE:
		{
			// Sedert V16.16:
			// Indien we het probleem kunnen oplossen door de node compactible te maken dan enkel deze 
			// optie tonen.
			BOOL solveMakeCompatible = CAppNodeDatabase::GetInstance()->possibleToMakeDbaseNodesCompatible();

			m_Chain[WIZARDPAGE_CHOICE_NODESINCOMPATIBLE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_NODESINCOMPATIBLE;

			if (!solveMakeCompatible) {
				m_Chain[WIZARDPAGE_CHOICE_PROGRAMFILE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PROGRAMFILE;
			}		
			if (!solveMakeCompatible) {
				m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_MANUALASSIGN;	
			}
			if (!solveMakeCompatible) {
				m_Chain[WIZARDPAGE_CHOICE_INIT][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_INIT;
			}
			if (!solveMakeCompatible) {
				m_Chain[WIZARDPAGE_CHOICE_NODESADDED][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_NODESADDED;	// TM 20070927
			}
		}
		break;

		case WIZARD_USER_SELECTION :
		{
			// Sedert V16.16.
			// Mogelijkheid om modules te vervangen.
			// Kunnen deze optie gebruiken om offline te openen en daarna online te gaan en modules vervangen.
			m_Chain[WIZARDPAGE_CHOICE_REPLACE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_REPLACE;

			m_Chain[WIZARDPAGE_CHOICE_MANUALASSIGN][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_MANUALASSIGN;		

			m_Chain[WIZARDPAGE_CHOICE_INIT][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_INIT;

			// Sedert V16.16:
			// Mogelijkheid om de addressen toe te kennen volgens het programmatie bestand.
			m_Chain[WIZARDPAGE_CHOICE_PROGRAMFILE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_PROGRAMFILE;

		} break;

		case WIZARD_CONTINUE_PROGRAMFILEDIFFERENCES :
		{
			// Sedert V16.16:
			// Bij programmatie verschillen enkel de mogelijkheid om het configuratie bestand door te 
			// sturen.
			m_Chain[WIZARDPAGE_CHOICE_SENDPROGRAMFILE][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_SENDPROGRAMFILE;		

			
			// m_Chain[WIZARDPAGE_CHOICE_COPYNAMES][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_COPYNAMES;		

		} break;

		case WIZARD_CONTINUE_NAMEDIFFERENCES :
		{
			m_Chain[WIZARDPAGE_CHOICE_COPYNAMES][WIZARD_PAGE1] = WIZARDPAGE_CHOICE_COPYNAMES;		
			m_Chain[WIZARDPAGE_CHOICE_COPYNAMES][WIZARD_PAGE2] = WIZARDPAGE_CHOICE_SENDPROGRAMFILE;		

		} break;

		case WIZARD_LICENCE_ACTIVATION_CODE:
		{
			m_Chain[WIZARDPAGE_ENTER_ACCESSCODE][WIZARD_PAGE1] = WIZARDPAGE_ENTER_ACCESSCODE;

		} break;

		default:
		{
			ASSERT(0);
		} break;	
	}	
}

BOOL CPropertySequence::IsLastPage(WizardPages_t CurrentPage)
{
	int i;
	int nextpage = 0xFF;

	for (i=WIZARD_PAGE1;i<MAX_PROPERTY_SEQUENCES;i++) 
	{	
		if (m_Chain[m_bActiveChain][i] == CurrentPage) 
		{
			break;
		}
	}

	if (i++<MAX_PROPERTY_SEQUENCES) 
	{
		nextpage = m_Chain[m_bActiveChain][i];
	}

	if (nextpage == 0xFF) 
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CPropertySequence::SetNextPage(WizardPages_t CurrentPage)
{
	int i;
	int nextpage = 0xFF;

	for (i=WIZARD_PAGE1;i<MAX_PROPERTY_SEQUENCES;i++) 
	{	
		if (m_Chain[m_bActiveChain][i] == CurrentPage) 
		{
			break;
		}
	}

	if (++i<MAX_PROPERTY_SEQUENCES) 
	{
		nextpage = m_Chain[m_bActiveChain][i];
	}

	if (nextpage != 0xFF) 
	{
		if (m_pWizard != NULL) 
		{
			m_pWizard->SetActivePage(nextpage);
		}

		return TRUE;
	}	
	return FALSE;
}

BOOL CPropertySequence::SetPrevPage(WizardPages_t CurrentPage)
{
	int i;
	int prevpage = 0xFF;

	for (i=WIZARD_PAGE1;i<MAX_PROPERTY_SEQUENCES;i++) 
	{	
		if (m_Chain[m_bActiveChain][i] == CurrentPage) 
		{
			break;
		}
	}

	if (i==WIZARD_PAGE1)
	{
		prevpage = WIZARD_PAGE1;
	}
	else if (--i<MAX_PROPERTY_SEQUENCES) 
	{
		prevpage = m_Chain[m_bActiveChain][i]; 
	}

	if (prevpage != 0xFF) 
	{
		if (m_pWizard != NULL) 
		{
			m_pWizard->SetActivePage(prevpage);
		}
		return TRUE;
	}	
	return FALSE;
}

BOOL CPropertySequence::IsActiveChain(WizardPages_t i)
{
	if (i<WIZARDPAGE_MAX) 
	{	
		if (m_Chain[i][WIZARD_PAGE1] != 0xFF) 
		{
			return TRUE;
		}
	}
	return FALSE;
}


void CPropertySequence::SetActiveChain(WizardPages_t StartPage)
{
	if (StartPage < WIZARDPAGE_MAX) 
	{
		m_bActiveChain = StartPage;
	}
	else 
	{
		AfxMessageBox("Error - SetActiveChain");
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyWizard

IMPLEMENT_DYNAMIC(CPropertyWizard, CPropertySheet)		// CHANGES_MVS_2008 : Afleiden van CPropertySheet


CPropertyWizard::CPropertyWizard(CWnd* pParentWnd,WizardTasks_t task) :
m_task(task), m_pDlgFontHelper(NULL)
{
	
#if(0)
	if (!m_bmpWizardHeader.LoadBitmap(IDB_BITMAP_WIZARDHEADER)) 
	{
		AfxMessageBox("Error - loading bitmap IDB_BITMAP_WIZARDHEADER");
	}
#endif 

	if (!m_bmpWizardHeader.LoadBitmap(IDB_BITMAP_WIZARDHEADER2)) {
		AfxMessageBox("Error - loading bitmap IDB_BITMAP_WIZARDHEADER");
	}

	if (!m_bmpWizardWatermark.LoadBitmap(IDB_BITMAP_WIZARDWATERMARK)) {
		AfxMessageBox("Error - loading bitmap IDB_BITMAP_WIZARDWATERMARK");
	}

	Construct("Duotecno Problem Solve Wizard", pParentWnd, 0, m_bmpWizardWatermark, NULL, m_bmpWizardHeader);

	if (WIZARD_TASK_UNKNOWN==m_task) {
		InitTasks();
	}

	AddPages(m_task);

	m_fAutoPnp = FALSE;
	m_fSendNames = FALSE;
	m_fSendBindings = FALSE;
	m_fBrowseAgain = TRUE;				// Standaard op TRUE !!!!
	
	// CHANGES_MVS_2008
	// Oppassen: Moeten 0x00002000 toevoegen zodat de bitmaps van het watermark 
	// en de header juist getoond worden.
	m_psh.dwFlags |= PSH_WIZARD97 | 0x00002000;		// CHANGES_MVS_2008
}

CPropertyWizard::~CPropertyWizard()
{
	for (int i=0;i<WIZARDPAGE_MAX;i++) {
		if (m_pages[i] != NULL) {
			delete m_pages[i];
			m_pages[i] = NULL;
		}
	}
	if (NULL != m_pDlgFontHelper) {
		delete m_pDlgFontHelper;
		m_pDlgFontHelper = NULL;
	}
}

BOOL CPropertyWizard::OnInitDialog() {

	CPropertySheet::OnInitDialog();

	if (NULL == m_pDlgFontHelper) {
		m_pDlgFontHelper = new CDlgHelperFont(*this);	
		ASSERT(NULL != m_pDlgFontHelper);
		if (NULL != m_pDlgFontHelper) {
			m_pDlgFontHelper->initDialog();	
		}
	}

	// Sedert V16.44: Controle master aanwezig + weergave messagebox.
	// Indien er geen master gevonden werd in het systeem en er zijn 0xFE adressen aanwezig.
	//
	// OPPASSEN: We kunnen de messagebox enkel tonen wannner deze dialoog/wizard aktief is.
	// Daarom gebruik van PostMessage( ).
	//
	// NOTE: Weergave vanuit CMainFrame is niet mogelijk. 
	// Wanneer we de Messagebox openen vanuit CMainFrame wordt deze pas getoond na het verlaten van deze dialoog.
	// Bij het gebruiken van SendMessage() wordt de messagebox te vroeg getoond.
	// Dus enkel hier mogelijk.
	this->checkMasterFoundAndPromptUserIfNotFound( );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CPropertyWizard, CPropertySheet)		// CHANGES_MVS_2008 : Afleiden van CPropertySheet
	//{{AFX_MSG_MAP(CPropertyWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NOTIFY_NO_MASTER_FOUND,&CPropertyWizard::OnApplicationNotifyNoMasterFound)
	ON_MESSAGE(WM_NOTIFY_ADDRESS_ASSIGNED,&CPropertyWizard::OnApplicationNotifyLogicalAddressAssigned)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizard message handlers

// Sedert V16.44: Indien er geen master gevonden werd in het systeem en er zijn 0xFE adressen aanwezig.
//
// We gaan via postmessage() een dialoog aanroepen zodat dit assynchroon gebeurd.
//
// OPPASSEN: We kunnen de messagebox enkel tonen wannner deze dialoog/wizard aktief is.
// Daarom gebruik van PostMessage( ).
void CPropertyWizard::checkMasterFoundAndPromptUserIfNotFound() {
	CAppState& refAppState = CAppState::getInstance();
	const CAppBrowseError::BrowseError ec = CAppStateUtil::getAppBrowseError(refAppState);
	if (CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND == ec || CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND_ASSIGN_MASTER == ec) {
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		if ((NULL != pAppNodeDatabase) && (!pAppNodeDatabase->runtimeNodeDatabase_isMasterPresent())) {			
			// Geen master aanwezig...
			CList<int> list;
			if (pAppNodeDatabase->runtimeNodeDatabase_foundTouchscreenNodesWithFactoryAddress(list)) {
				// Er werden touchscreens gevonden met een 0xFE. Voorstellen om deze eerst als master in te stellen...
				const WPARAM wParam = 1;	// Touchsceens.
				this->PostMessage(WM_NOTIFY_NO_MASTER_FOUND, wParam, 0);			
			}
			else if (pAppNodeDatabase->runtimeNodeDatabase_foundSmartboxUltraNodesWithFactoryAddresses(list)) {			
				// Er werden gateway nodes gevonden met een 0xFE adres.
				const WPARAM wParam = 0;	// sbox ultra
				this->PostMessage(WM_NOTIFY_NO_MASTER_FOUND, wParam , 0);				
			}
		}		
	}
}

// Dialoog wordt getoond wanneer er:
//		- Geen master gevonden
//		- Factory init adressen aanwezig zijn.
//		- Touchcsreen / smartbox ultra gevonden werd met 0xFE adres.
//		
// Wordt aangeroepen vanuit de OnInitDialog() via assynchrone PostMessage( ) om ervoor te zorgen dat de CPropertyWizard
// reeds aangemaakt werd.
// 
//
// wParam: 
//  - 0 = Node type gateway gevonden met een 0xFE adres.
//	- 1 = Touchscreens gevonden met een 0xFE adres.
LONG CPropertyWizard::OnApplicationNotifyNoMasterFound(WPARAM wParam, LPARAM lParam) {

	CString strTitle("No CAN-bus master detected in the system.");
	CString strMessage;

	const CString strFirstLine("During the physical browse for nodes, no CAN-bus master was detected.\r\n\r\n");

	if (0 == wParam) {	
		strMessage += strFirstLine;
		strMessage += CString("Smartbox Ultra modules can be configured as a master using the following wizard.\r\n\r\n"
						      "Planning to add more modules later and set them as a master?\r\n"
						      "You can use the automatic PNP function to assign a logical address.\r\n\r\n"
						      "Press ok to proceed via the Problem-Solving Wizard.");	
	}
	else if (1 == wParam) {	
		strMessage += strFirstLine;
		strMessage += CString("However, touchscreens with a factory-initialized address were found. "
							  "It is recommended to set one of the touchscreens as the master.\r\n\r\n"
							  "This can be done through the touchscreen’s setup menu.\r\n"
							  "Please complete this step before proceeding.");
	}
	if (!strMessage.IsEmpty()) {	
		this->MessageBox(strMessage,strTitle,MB_OK | MB_ICONWARNING);
	}
	return TRUE;
}

LONG CPropertyWizard::OnApplicationNotifyLogicalAddressAssigned(WPARAM wParam, LPARAM lParam) {
	const int nodeAddress = (int) wParam;
	if (nodeAddress<0 || nodeAddress>0xFF) {
		ASSERT(nodeAddress>=0 && nodeAddress<=0xFF);
		return TRUE;
	}
	int i;
	for (i=0;i<WIZARDPAGE_MAX;i++) {
		if ((NULL != this->m_pages[i]) && (NULL != this->m_pages[i]->GetSafeHwnd())) {
			this->m_pages[i]->OnNotifyAssignedLogicalAddress(nodeAddress);
		}
	}
	return TRUE;
}

void CPropertyWizard::InitTasks(void)
{
	m_task = WIZARD_USER_SELECTION;	

	CAppState& refAppState = CAppState::getInstance();
	const CAppMode::Types appMode = CAppStateUtil::getAppMode(refAppState);

	switch(appMode)  {		
		case CAppMode::APP_NEW_ONLINE: {			
			const CAppBrowseError::BrowseError ec = CAppStateUtil::getAppBrowseError(refAppState);
			switch(ec)
			{
				case CAppBrowseError::BROWSE_EC_SUCCESS: {					
					if (TRUE==CAppLicActCodeAPI::IsLicenseNeeded()) {
						m_task = WIZARD_LICENCE_ACTIVATION_CODE;
					}
					else {
						m_task = WIZARD_USER_SELECTION;
					}
				} break;
				case CAppBrowseError::BROWSE_EC_CANCEL:						
				case CAppBrowseError::BROWSE_EC_ERROR:						
				case CAppBrowseError::BROWSE_EC_NO_RESPONSE:						
					m_task = WIZARD_USER_SELECTION;
				break;
				case CAppBrowseError::BROWSE_EC_DUPLICATES_FOUND:			
					m_task = WIZARD_NEW_DUPLICATESFOUND;
				break;
				case CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND:	
					m_task = WIZARD_NEW_FACTORYADDRESS;
				break;
				case CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND_ASSIGN_MASTER: {
					m_task = WIZARD_NEW_FACTORYADDRESS_ASSIGN_MASTER;										  
				} break;
			}
		} break;

		case CAppMode::APP_CONTINUE_ONLINE:		
		{		
			const CAppBrowseError::BrowseError ec = CAppStateUtil::getAppBrowseError(refAppState);
			switch(ec)
			{
				case CAppBrowseError::BROWSE_EC_SUCCESS:				
				case CAppBrowseError::BROWSE_EC_CANCEL:					
				case CAppBrowseError::BROWSE_EC_ERROR:				
				case CAppBrowseError::BROWSE_EC_NO_RESPONSE:			
				{					
					const CAppDbasePostBrowseError::Result ecResult = CAppStateUtil::getAppDbasePostBrowseErrorResult(refAppState);
					switch (ecResult) {
						case CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS: {							
							if (TRUE==CAppLicActCodeAPI::IsLicenseNeeded()) {
								m_task = WIZARD_LICENCE_ACTIVATION_CODE;
							}
							else {
								m_task = WIZARD_USER_SELECTION;				
							}
						} break;

						break;
						case CAppDbasePostBrowseError::DBASE_EC_NODES_NOT_FOUND :
							m_task = WIZARD_CONTINUE_NODESREMOVED;
						break;
						case CAppDbasePostBrowseError::DBASE_EC_NODES_ADDED :
							m_task = WIZARD_CONTINUE_NODESADDED;
						break;
						case CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE :
							m_task = WIZARD_CONTINUE_NODESINCOMPATIBLE;
						break;
						case CAppDbasePostBrowseError::DBASE_EC_PROGRAMFILE_DIFFERENCE :
							m_task = WIZARD_CONTINUE_PROGRAMFILEDIFFERENCES;
						break;
						case CAppDbasePostBrowseError::DBASE_EC_NAMES_DIFFERENCE:
							m_task = WIZARD_CONTINUE_NAMEDIFFERENCES;
						break;										
					}
				}break;
				case CAppBrowseError::BROWSE_EC_DUPLICATES_FOUND:			
					m_task = WIZARD_CONTINUE_DUPLICATESFOUND;
				break;
				case CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND:	
					m_task = WIZARD_CONTINUE_FACTORYADDRESS;
				break;
				case CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND_ASSIGN_MASTER: {
					m_task = WIZARD_CONTINUE_FACTORYADDRESS_ASSIGN_MASTER;												  
				} break;
			}
		} break;
	}
}

void CPropertyWizard::AddPages(WizardTasks_t tasks) {
	int i;
	m_SequenceManager.Init(tasks,this);

	for (i=0;i<WIZARDPAGE_MAX;i++) {
		m_pages[i] = NULL;
	}
	
	m_pages[WIZARDPAGE_INFO] = new CPropertyWizardPage0();
	m_pages[WIZARDPAGE_CHOICES] = new CPropertyWizardPage1();	
	m_pages[WIZARDPAGE_CHOICE_PROGRAMFILE] = new CPropertyWizardPage2();
	m_pages[WIZARDPAGE_CHOICE_MANUALASSIGN] = new CPropertyWizardPage3();
	m_pages[WIZARDPAGE_CHOICE_ASSIGNMASTER] = new CPropertyWizardPage13();
	m_pages[WIZARDPAGE_CHOICE_PNP] = new CPropertyWizardPage4();
	m_pages[WIZARDPAGE_CHOICE_REPLACE] = new CPropertyWizardPage5();
	m_pages[WIZARDPAGE_CHOICE_INIT] = new CPropertyWizardPage6();
	m_pages[WIZARDPAGE_CHOICE_NODESADDED] = new CPropertyWizardPage7();
	m_pages[WIZARDPAGE_CHOICE_NODESINCOMPATIBLE] = new CPropertyWizardPage8();
	m_pages[WIZARDPAGE_CHOICE_SENDPROGRAMFILE] = new CPropertyWizardPage9();
	m_pages[WIZARDPAGE_CHOICE_COPYNAMES] = new CPropertyWizardPage12();		
	m_pages[WIZARDPAGE_ENTER_ACCESSCODE] = new CPropertyWizardPage11();

	for (i=0;i<WIZARDPAGE_MAX;i++) {
		if (m_pages[i] != NULL) {
			AddPage(m_pages[i]);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BOOL CPropertWizardTest::showWizard(WizardTasks_t task) {

	CWnd* const pMainWnd = AfxGetMainWnd();
	if (NULL == pMainWnd) {
		return FALSE;			
	}
	CPropertyWizard wizard(pMainWnd,task);
	const INT_PTR retVal = wizard.DoModal();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
