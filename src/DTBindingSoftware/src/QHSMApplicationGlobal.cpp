// QHSMApplicationGlobal.cpp: implementation of the CQHSMApplicationGlobal class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BindingSoftware.h"
#include "ComApplication.h"
#include "QHSMApplicationGlobal.h"
#include "QHSMGenericTask.h"
#include "QHSMCompositeTask.h"
#include "QHSMTaskBuilder.h"
#include "NodeLCDScreenmapFSMInfo.h"
#include "MainFrm.h"
#include "QMyEvent.h"
#include "QHSMBindingConversionFromNodes.h"
#include "MemoryBitmapInfo.h"
#include "MemoryDumpInfo.h"
#include "App/AppCfg/AppCfgMM/ConfigMultiMediaInfo.h"
#include "ApplicationDlgAppearance.h"
#include "CommunicationLogfile.h"
#include "MyException.h"

// views
#include "views/bindings/DlgConversionFromNodes.h"
#include "views/apptasks/DlgQHSMApplication.h"
#include "views/apptasks/DlgMainApplicationTask.h"

#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	
#include "app/appcfg/appcfgbrowse/appcfgbrowseinfo.h"
#include "app/appcfg/appcfgsmaplite/appcfgsmapliteinfo.h"		// Nodig om te bepalen of er na doorsturen van smaplite moet een melding gegeven worden.
#include "App\AppCfg\AppCfgUnits\ConfigUnitCreateInfo.h"		// sedert V16.43: Nodig om te bepalen nar welke node de unit config doorgestuurd werd.
#include "app\applicactcode\applicActCodeApi.h"


// ------------------------------------
#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMBindingDatabase;
using namespace TMGlobalSettings;

#include "TMConvenience.h"
using namespace TMConvenience;
// ------------------------------------

//////////////////////////////////////////////////////////////////////
//#define TRACER_FSM(x)			TRACE x			// DEBUGGEN: TRACER INSCHAKELEN.
#define TRACER_FSM(x)							// DEBUGGEN: TRACER UITSCHAKELEN.
//////////////////////////////////////////////////////////////////////


extern "C" void onAssert__(char const *file, unsigned line)
{
	CString s;
	s.Format("\nAssertion failed in %s, line %d",file,line);
	TRACE1("%s",s);
	ASSERT(0);
}
CQHSMApplicationGlobal* CQHSMApplicationGlobal::GetInstance()
{
	static CQHSMApplicationGlobal instance;
	return &instance;
}

//////////////////////////////////////////////////////////////////////

// CHANGES_ACTIVATION_LICENSE_CODE
// Indien deze database actie gevraagd werd, deze wissen.
// Moet aangeroepen worden na het starten van de taak.
// Zorgt ervoor dat wanneer de taak gestart is de toestand opniew idle is.
static void clearRequestedDbaseAction(CAppGlobalDbaseActions::Actions action) {			
	CAppState& refAppState = CAppState::getInstance();
	if (action==CAppStateUtil::getDbaseActions(refAppState)) {
		CAppStateUtil::setDbaseActions(refAppState,CAppGlobalDbaseActions::APPDBASE_ACTION_IDLE);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CQHSMApplicationGlobal::~CQHSMApplicationGlobal()
{
	if (m_pGenericTask != NULL)
	{
		delete m_pGenericTask;
		m_pGenericTask = NULL;
	}

	if (m_pCompositeTask != NULL)
	{
		delete m_pCompositeTask;
		m_pCompositeTask = NULL;
	}
}

/*********************************************************************
 * void CQHSMApplicationGlobal::initial(QEvent const *e)
 *
 * Create Generic & Composite Task.
 *  
 * Initial Transition : CQHSMApplicationGlobal::MainApplication
 *
 *********************************************************************/
void CQHSMApplicationGlobal::initial(QEvent const *e)
{
	SetLock(FALSE);

	m_pParent = (CMainFrame*) AfxGetMainWnd();
	ASSERT(m_pParent);

	try
	{	
		if (m_pGenericTask == NULL)
		{
			m_pGenericTask = new CQHSMGenericTask();		
		}

		if (m_pCompositeTask == NULL)
		{
			m_pCompositeTask = new CQHSMCompositeTask();		
		}
	}

	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		ASSERT(0);
	}


	Installation_OnlineBusy_HistoryState = 
		Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Installation_Online_Busy);

	Installation_Online_SafeRestore = 
		Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::NewApplication);

	Engineering_Online_Idle_History =
		Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Idle);

	Q_INIT(&CQHSMApplicationGlobal::MainApplication);
}


/***************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::MainApplication(QEvent const *e)
 * 
 * Main Application Top State 
 * 
 * - Q_ENTRY_SIG: Initialise Application + Initialise Views
 * - Q_EXIT_SIG: Clear Databases
 * - Q_INIT_SIG: Initial Transition : CQHSMApplicationGlobal::NewApplication
 * 
 * - EV_OPERATION_NEWAPPLICATION: Self Transition 
 * - EV_OPERATION_SHUTDOWN : Request ShutDown MainWindow
 *
 * Parent :CQHSMApplicationGlobal::top
 ***************************************************************************************************/
QSTATE CQHSMApplicationGlobal::MainApplication(QEvent const *e)
{	
	ASSERT(m_pGenericTask);
	ASSERT(m_pCompositeTask);

	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::MainApplication, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}
	
	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{		
			m_pParent->ClearViews();			
			m_pParent->ClearScrollView();
			CAppStateUtil::setAppStateIdle(CAppState::getInstance());		
			m_pParent->SetModifiedFlag(FALSE);

		} return 0;

		case Q_EXIT_SIG:
		{		
			TMInitialiseDatabases();

		} return 0;

		case Q_INIT_SIG:
		{
			Q_INIT(&CQHSMApplicationGlobal::NewApplication);
		
		} return 0;

		case EV_OPERATION_NEWAPPLICATION:
		{
			/**
			 * TM 20080917: Settings opslaan !!
			 */
			const BOOL fResult = TMGetGlobalSettings()->Save();
			ASSERT(fResult);

			Q_TRAN(&CQHSMApplicationGlobal::MainApplication);

		} return 0;

		case EV_OPERATION_SHUTDOWN:
		{				
			if (AfxGetMainWnd() != NULL)
			{						
				AfxGetMainWnd()->DestroyWindow();			
			}

		} return 0;

		case 0:
		{
			TRACER_FSM(("\nQ_EMPTY_SIG"));
		
		} break;

		case EV_SIGNAL_COMMUNICATION_NOTIFY_ACK:
		{
		
		} return 0;

		default:
		{
			//CString s;
			//s.Format("Unhandled event CQHSMApplicationGlobal : 0x%02x",e->sig);
			//AfxMessageBox(s);		

		} break;	// return  !!!!0
    }
	return ((QSTATE) &CQHSMApplicationGlobal::top);
}

/***************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::NewApplication(QEvent const *e)
 *
 * Show Start dialog to let user decide which application type.
 *
 * - Q_ENTRY_SIG: History State : Installation_Online_SafeRestore = NewApplication
 *				  Initialise Databases + Show Modal Start Dialog.
 *				  When Modal dialog closes Trigger(EV_SIGNAL_TASK_DONE)
 * - Q_EXIT_SIG:  
 *
 * - EV_SIGNAL_TASK_DONE: Determine application type selected by the user.
 *
 *	 - Installation Mode
 *	  + Online Create New: Transition to: CQHSMApplicationGlobal::Installation_Online_Busy_New
 *	  + Online Continue(**): Load Document + Compare binding databases.
 *						    Transition to: CQHSMApplicationGlobal::Installation_Online_Busy_Continue			
 *	  + Offline Continue(**): Load Document + Update views + show Nodes View + Compare binding databases
 *							 Transition to: CQHSMApplicationGlobal::Installation_Offline
 *   - Engineering mode
 *   - Simulation mode
 * 
 *	 --> if load document is cancelled or fails(**) or Application type is not valid :
 *			if (TryAgain) Trigger( EV_OPERATION_NEWAPPLICATION )
 *			else Trigger( EV_OPERATION_SHUTDOWN );
 *
 * Parent : CQHSMApplicationGlobal::MainApplication
 ***************************************************************************************************
 * Sedert V16.09: Toestand aangepast.
 *
 ***************************************************************************************************/
// sedert V16.47: CHANGES_UPLOAD_UNITS
// Testcode die ervoor zorgt dat na het sluiten van de 'New Application Wizard' alle units naar de 
// master doorgestuurd worden.
#define USE_TEST_NEWAPPLICATION_SEND_UNITS			0
/***************************************************************************************************/
#if (USE_TEST_NEWAPPLICATION_SEND_UNITS == 1)
	#if (_DEBUG)
		#define DEBUG_TEST_ADDTASK(task,la)				CAppFsmNodeTaskEntries_Utils::addTask(CAppState::getInstance().getTaskEntries(), task, la)
	#else
		#define DEBUG_TEST_ADDTASK(task,la)	
	#endif
#else 
	#define DEBUG_TEST_ADDTASK(task,la)	
#endif
/***************************************************************************************************/
QSTATE CQHSMApplicationGlobal::NewApplication(QEvent const *e)
{
	static BOOL fInitialised = FALSE;

	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::NewApplication, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig)  {
		case Q_ENTRY_SIG: {		
			BOOL result = TRUE;
						
			// Alle data wissen: Databases, node services, login gegegevens, filesettings, ... 
			m_pParent->initNewApplicationData();

			// Disconnect uitvoeren... --> Go_Offline();
			this->CloseConnections();

			const CAppState::Type_t appType = CAppState::getInstance().getAppType();
			if (CAppState::APP_TYPE_INSTALLATION==appType) {
				Installation_Online_SafeRestore =  Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::NewApplication);		

				SetLock(TRUE);
				m_pParent->DestroyApplication();
				if (FALSE == fInitialised) {
					fInitialised = TRUE;
				}
				// Weergave van de start dialoog.
				// Hier wordt het project type gekozen + mogelijks wordt de module geconfigueerd om 
				// als TCP/IP configuratie te gebruiken.
				result = m_pParent->ShowApplicationStartDialog();	
				SetLock(FALSE);
			}		
			if (result) {
				Trigger(EV_SIGNAL_TASK_DONE);
			}
			else {			
				Trigger(EV_SIGNAL_TASK_ERROR);			// Sedert V16.09
			}			
		} return 0;

		case Q_EXIT_SIG:
		{
			TRACER_FSM(("\nNewApplication - Q_EXIT_SIG"));
		} return 0;
	
		case EV_SIGNAL_TASK_ERROR: {
			// Wanneer de startdialoog afgesloten wordt met een cancel/fout.
			if (ShowDialog_TryAgain() == FALSE) {
				Trigger( EV_OPERATION_SHUTDOWN );
			}						   
		} return 0;
	
		case EV_SIGNAL_TASK_DONE: {			
			// Bij het afsluiten van de start dialoog.			
			const CAppState::Type_t appType = CAppState::getInstance().getAppType();
			if (CAppState::APP_TYPE_INSTALLATION==appType) {							
				const CAppMode::Types appMode = CAppStateUtil::getAppMode(CAppState::getInstance());
				if (CAppMode::APP_NEW_ONLINE==appMode) {			
					// Server login info / Nodeservices updaten.
					m_pParent->UpdateServerLoginCfg();
					Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_New);
				}
				else if (CAppMode::APP_CONTINUE_ONLINE==appMode) {	
					// Server login info / Nodeservices updaten.
					m_pParent->UpdateServerLoginCfg();
					
					// sedert V16.47: CHANGES_UPLOAD_UNITS.  
					// Testcode (zie macro USE_TEST_NEWAPPLICATION_SEND_UNITS)
					DEBUG_TEST_ADDTASK(CAppFsmNodeTaskEntry::APPFSM_TASK_SENDUNITS, 0xFC);	

					// sedert V16.47: CHANGES_UPLOAD_UNITS
					// Oplossing incompatible nodes bij het vervangen van een module.
					// Mogelijk zijn er taken (=doorsturen van de units) nodig voor er gestart wordt met het browsen.
					if (!CAppState::getInstance().getTaskEntries().isIdle()) {
						// In de start dialoog zijn er mogelijk acties gebeurd waardoor er taken moeten
						// uitgevoerd worden voor de browse for nodes.
						//
						// Sedert V16.47: Wanneer we connectie maken met een smartbox ultra en deze heeft een 
						// 0xFE adres dan wordt er in de start dialoog een adres toegekend (master of slave).
						// Om de incompatible nodes te voorkomen wordt er een taak aangemaakt om eerst de units door te sturen.
						// Na het uitvoeren van deze taal komen we in de 'Installation_Online_Busy_Continue' state en 
						// wordt de physical browse for nodes uitgevoerd.
						Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_NodeFsmTasks);						
					}
					else {
						// Er zijn geen taken die moeten uitgevoerd worden- Kunnen starten met een physical browse for nodes.
						Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue);				
					}
				}
				else if (CAppMode::APP_CONTINUE_OFFLINE==appMode) {			
					m_pParent->UpdateViews();
					m_pParent->ShowNodesView();
					Q_TRAN(&CQHSMApplicationGlobal::Installation_Offline);						
				}
				else {
					Trigger(EV_SIGNAL_TASK_ERROR);			
				}
			}
			else if (CAppState::APP_TYPE_ENGINEERING==appType) {
				Q_TRAN(&CQHSMApplicationGlobal::Engineering);
			}
			else if (CAppState::APP_TYPE_SIMULATION==appType) {
				Q_TRAN(&CQHSMApplicationGlobal::Simulation);
			}
		} return 0;

		case EV_OPERATION_MESSAGEBOX_RESPONSE: {
			/* User Response : TryAgain ? */
			const CAppState::Type_t appType = CAppState::getInstance().getAppType();
			if (CAppState::APP_TYPE_INSTALLATION==appType) {
				QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);				
				switch(pMyEvent->lParam) {
					case EV_OPERATION_USER_NO: {
						Trigger( EV_OPERATION_SHUTDOWN );
					} break;
					case EV_OPERATION_USER_YES: {					
						Trigger( EV_OPERATION_NEWAPPLICATION );						
					} break;
				}				
			}
		} return 0;	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::MainApplication);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation(QEvent const *e)
 *  
 * Installation mode : Normal operating mode ...
 *
 * EV_OPERATION_SHUTDOWN: - Send WM_QUERYENDSESSION to Mainwindow : 
 *								"Save modified ?"
 *						  - Parent state shutsdown the application ...
 *
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation(QEvent const *e)
{
	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			
		} return 0;

		case Q_EXIT_SIG:
		{
						
		} return 0;

		case EV_OPERATION_SHUTDOWN:
		{
			if (AfxGetMainWnd() != NULL)
			{
				AfxGetMainWnd()->SendMessage(WM_QUERYENDSESSION);
			}
		} break;	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::MainApplication);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online(QEvent const *e)
 * 
 * Connected succesfully to communication port.
 *
 *
 * Q_ENTRY_SIG: Future: Notify main application Connected to com-Port	
 * Q_EXIT_SIG: Shutdown all tasks : CCommApplication Factory + Stop all Timers.
 * Q_INIT_SIG: Initial transition: CQHSMApplicationGlobal::Installation_Online_Idle
 *
 * EV_OPERATION_COMMUNICATION_OFFLINE : Operation : Go Offline
 * EV_SIGNAL_COMMUNICATION_OFFLINE : Signal : Disconnected from communication port.
 * EV_OPERATION_SHUTDOWN: if modified bindings to write: ask user if he is sure to close app.
 *						  When application is to be closed: Let parent state handle signal.
 * EV_SIGNAL_TASK_CANCEL: Invoke GenericTask Cancel operation
 *
 * Parent : CQHSMApplicationGlobal::Installation
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online(QEvent const *e)
{
	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			m_fUserDisconnect = FALSE;
		} return 0;

		case Q_EXIT_SIG:
		{
			/**
			 * Going from Online --> Offline : All Tasks are destroyed by the
			 * CCommApplication Factory ...
			 */

			if (TMGetCommunicationApp() != NULL) 
			{
				TMGetCommunicationApp()->ShutDownAllTasks();
			}

		} return 0;

		case Q_INIT_SIG:
		{
			Q_INIT(&CQHSMApplicationGlobal::Installation_Online_Idle);

		} return 0;

		case EV_SIGNAL_COMMUNICATION_ONLINE:
		case EV_OPERATION_COMMUNICATION_ONLINE:
		{
			/** 
			 * Absorbe: already online ...
			 */
		
		} return 0;

		case EV_OPERATION_COMMUNICATION_OFFLINE:
		{
			m_fUserDisconnect = TRUE;
			Go_Offline();

		} return 0;

		case EV_SIGNAL_COMMUNICATION_OFFLINE:
		{		
			/**
			 * Acknowledgement --> Can go offline 
			 */

			Q_TRAN(&CQHSMApplicationGlobal::Installation_Offline);
	
			
			// Sedert V16.07:
			// Indien de disconnect niet gevraagd werd door de gebruiker dan welicht een 
			// probleem met de socket connectie.
			// Versturen een Assynchroon bericht naar de applicatie. Deze toont een pop-up 
			// messagebox.
			if (FALSE == m_fUserDisconnect) {
				AfxGetMainWnd()->PostMessage(WM_COMMUNICATION_CONNECTION_LOST);
				m_fUserDisconnect = FALSE;
			}
		} return 0;			

		case EV_OPERATION_SHUTDOWN:
		{
			if (TMGetBindingApp()->ModifiedBindingFiles() == TRUE) 
			{
				CString szMessage("Not all bindings have been send to the installation !!");
				if (ShowDialog_AreYouSure(szMessage) == TRUE)
				{				
					return 0;		// Absorbe and wait for response from user
				}
			}
		} break; // No modified binding files --> let parent state handle event!

		case EV_SIGNAL_TASK_CANCEL:
		{
			GenericTask_Invoke_CancelNotify(m_pGenericTask);

		} return 0;

		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);
				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{						
					// Absorbe shuting down ...
				} break;

				case EV_OPERATION_USER_YES:
				{
					Q_TRAN(&CQHSMApplicationGlobal::Installation);
					Trigger( EV_OPERATION_SHUTDOWN );					
				} break;				
			}
		} return 0;
    }

	return ((QSTATE) &CQHSMApplicationGlobal::Installation);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Idle(QEvent const *e)
 * 
 * Online - idle : Ready to execute new task.
 *
 * Q_ENTRY_SIG: - Clear Generic + composite task.
 *				- History state : Installation_Online_SafeRestore = Installation_Online_Idle
 *				- Set Application Idle.
 * Q_EXIT_SIG:
 *
 * EV_OPERATION_UPLOAD_ALLBINDINGS:
 * EV_OPERATION_UPLOAD_MODIFIEDBINDINGS:
 * EV_OPERATION_UPLOAD_SCREENMAPS:
 * EV_OPERATION_UPLOAD_BITMAPS:
 * EV_OPERATION_UPLOAD_MMCONFIG:
 * EV_OPERATION_UPLOAD_MEMORY:
 * EV_OPERATION_DOWNLOAD_MEMORY:
 *		- Create Generic Task + store in m_pGenericTask								
 *		- Transition to: CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask
 *
 * EV_OPERATION_UPLOAD_PROGFILE
 *		- Create composite task + store in m_pCompositeTask
 *		- Copy first task to m_pGenericTask
 *		- Transition to: CQHSMApplicationGlobal::Installation_Online_Busy_CompositeTask
 * 
 * Parent : CQHSMApplicationGlobal::Installation_Online
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Idle(QEvent const *e)
{
	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online_Idle, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{		
			ASSERT(m_pGenericTask);
			ASSERT(m_pCompositeTask);

			if (m_pGenericTask != NULL) {
				m_pGenericTask->Clear();
			}
			if (m_pCompositeTask != NULL) {
				m_pCompositeTask->Clear();
			}

			Installation_Online_SafeRestore = Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Installation_Online_Idle);

			// Sedert V16.41:
			// Notify naar de configurator.
			BOOL notifyConfigurator = FALSE;
			const CAppMode::Types appmodeType = CAppStateUtil::getAppMode(CAppState::getInstance());			
			CAppState& refAppState = CAppState::getInstance();
			if ((refAppState.getTask(CAppState::TASK_FLAG_BROWSE_FOR_NODES)) ||
				(refAppState.getTask(CAppState::TASK_FLAG_REQUEST_FOR_STATES)) ||
				(refAppState.getTask(CAppState::TASK_FLAG_PNP))) {
				if (CAppMode::APP_CONTINUE_ONLINE == appmodeType) {
					// Moeten de configurator een notify sturen dat mogelijks de data gewijzigd werd.
					// Dit is nodig wanneer deze nog openstaat op een configuratie.
					notifyConfigurator = TRUE;
				}
			}
			if (refAppState.getTask(CAppState::TASK_FLAG_BINDINGS_WRITE)) {
				TRACER_FSM(("Write bindings done."));
			}			

			if (refAppState.getTask(CAppState::TASK_FLAG_BROWSE_SINGLE_NODE)) {				
				TRACER_FSM(("Browse single node done."));		
			}			

			// AppInfo wissen.
			CAppStateUtil::setAppStateIdle(CAppState::getInstance());	

			// Sedert V16.28: setRequestUpdateServerDeviceNodes()
			// Request om de volledige lijst van de node services up te daten met de gegevens 
			// gevonden in het lokaal LAN netwerk.
			// Nodig voor een correcte mapping van node address / physical address / MAC-address / IP-adres.
			// Het updaten van deze lijst zorgt ervoor dat de applicatie automatisch gegevens aangepast of ingevuld worden.
			//
			// Sedert V16.36: De View / GUI modes updaten ifv het projecvtbestand.
			// Nodig voor de configuratie van de smartbox ultra: (=DALI subbus configuratie).
			//
			// Oppassen: wordt telkens aangeroepen na het uitvoeren van een taak.
			CMainFrame* const pMainWnd =  (CMainFrame*) AfxGetMainWnd();
			if (NULL != pMainWnd) {
				pMainWnd->setRequestUpdateServerDeviceNodes();
				pMainWnd->updateAppStateViewModes();
				pMainWnd->PostMessage(WM_APPLICATION_ONLINEIDLE, (WPARAM) notifyConfigurator);
			}
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;

		case EV_OPERATION_UPLOAD_SMAP_PAGEINFO: {
			if (CQHSMTaskBuilder::Build_UploadSmapPageInfo(m_pGenericTask) != FALSE)  {			
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);				
				return 0;
			}		
		} return 0;
		
		case EV_OPERATION_FTP: {
			if (CQHSMTaskBuilder::Build_ftpConfig(m_pGenericTask) != FALSE) {			
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);				
				return 0;
			}		
		} return 0;

		case EV_OPERATION_UPLOAD_ALLBINDINGS: {		
			if (CQHSMTaskBuilder::Build_UploadAllBindings(m_pGenericTask) != FALSE)  {			
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_BINDINGS_WRITE);		// Sedert V16.32: zie BUG-0175			
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_BINDINGS_WRITE);
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);				
				// Sedert v0x800, TM20141117 (CHANGES_ACTIVATION_LICENSE_CODE)
				clearRequestedDbaseAction(CAppGlobalDbaseActions::APPDBASE_ACTION_SENDBINDINGS);	

				return 0;
			}
		} return 0;

		case EV_OPERATION_UPLOAD_MODIFIEDBINDINGS: {
			if (CQHSMTaskBuilder::Build_UploadModifiedBindings(m_pGenericTask) != FALSE)  {	
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_BINDINGS_WRITE);		// Sedert V16.32: zie BUG-0175
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_BINDINGS_WRITE);
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}		
		} return 0;

		case EV_OPERATION_UPLOAD_SCREENMAPS: {			
			if (CQHSMTaskBuilder::Build_UploadScreenmaps(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}			
		} return 0;

		case EV_OPERATION_UPLOAD_BITMAPS: {
			if (CQHSMTaskBuilder::Build_UploadBitmaps(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}	
		} return 0;

		case EV_OPERATION_UPLOAD_MMCONFIG: {				
			if (CQHSMTaskBuilder::Build_UploadMultimediaConfig(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}							
		} return 0;

		case EV_OPERATION_UPLOAD_UNITCONFIG: {
			if (CQHSMTaskBuilder::Build_UploadUnitsCreateConfig(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}					
		} return 0;

		case EV_OPERATION_UPLOAD_MOODS: {
			if (CQHSMTaskBuilder::Build_UploadMoodConfig(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}				
		} return 0;

		case EV_OPERATION_UPLOAD_IRRX: {
			if (CQHSMTaskBuilder::Build_UploadIRRXConfig(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}				
		} return 0;

		case EV_OPERATION_UPLOAD_GATEWAYCFG:
		{
			if (CQHSMTaskBuilder::Build_UploadGatewayCfg(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}			
		} return 0;

		case EV_OPERATION_UPLOAD_ROOMCTRL_AUDIOCFG: {
			if (CQHSMTaskBuilder::Build_UploadRoomCtrlAVCfg(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}			
		} return 0;

		case EV_OPERATION_UPLOAD_BOSESTCFG:
		{
			if (CQHSMTaskBuilder::Build_UploadBoseSTCfg(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}		
		} break;

		case EV_OPERATION_UPLOAD_LEDSCFG:
		{
			if (CQHSMTaskBuilder::Build_UploadLedsCfg(m_pGenericTask) != FALSE){
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}		
		} break;

		case EV_OPERATION_UPLOAD_SMAPLITE:
		{
			if (CQHSMTaskBuilder::Build_UploadSmapLite(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}		
		} break;

		case EV_OPERATION_UPLOAD_SCHEDULE: {
			if (CQHSMTaskBuilder::Build_UploadScheduleConfig(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}				
		} return 0;

		case EV_OPERATION_UPLOAD_MEMORY: {
			if (CQHSMTaskBuilder::Build_UploadMemory(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}				
		} return 0;

		case EV_OPERATION_DOWNLOAD_MEMORY: {
			if (CQHSMTaskBuilder::Build_DownloadMemory(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}	
		} return 0;

		case EV_OPERATION_UPLOAD_NAMES: {
			if (CQHSMTaskBuilder::Build_UploadAllNames(m_pGenericTask) != FALSE)  {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				// Sedert v0x800, TM20141117 (CHANGES_ACTIVATION_LICENSE_CODE)
				clearRequestedDbaseAction(CAppGlobalDbaseActions::APPDBASE_ACTION_SENDNAMES);			
				return 0;
			}	
		} return 0;	

		case EV_OPERATION_NICNETWORK_START: {
			if (CQHSMTaskBuilder::Build_NicErrorsFsm(m_pGenericTask) != FALSE) {
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;
			}	
		} return 0;

		case EV_OPERATION_UPLOAD_SMAP_AVROOM: {
			if (CQHSMTaskBuilder::Build_UploadSmapAVRoom(m_pGenericTask) != FALSE) {				
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask);
				return 0;			
			}
		} return 0;
	
		case EV_OPERATION_UPLOAD_PROGFILE: {		
			if (CQHSMTaskBuilder::Build_UploadProgrammationFile(m_pCompositeTask) == TRUE) {
				if (m_pGenericTask != NULL) {
					CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATIONFILE_WRITE);		// Sedert V16.32: zie BUG-0175
					CQHSMGenericTask::Copy(m_pGenericTask,m_pCompositeTask->Begin());
					Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_CompositeTask);
					// Sedert v0x800, TM20141117 (CHANGES_ACTIVATION_LICENSE_CODE)
					clearRequestedDbaseAction(CAppGlobalDbaseActions::APPDBASE_ACTION_SENDPROGFILE);	
					return 0;
				}
			}
		} return 0;

		case EV_OPERATION_UPLOAD_SMAP_MYHOME: {
			if (CQHSMTaskBuilder::Build_UploadSmapMyHome(m_pCompositeTask) == TRUE) {
				if (m_pGenericTask != NULL) {
					CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
					CQHSMGenericTask::Copy(m_pGenericTask,m_pCompositeTask->Begin());
					Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_CompositeTask);
					return 0;
				}
			}			
		} return 0;

		case EV_OPERATION_UPLOAD_SMAP_MENU: {
			if (CQHSMTaskBuilder::Build_UploadSmapMainPage(m_pCompositeTask) == TRUE) {
				if (m_pGenericTask != NULL) {
					CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_CONFIGURATION);		// Sedert V16.32: moet nog uitgewerkt worden...	
					CQHSMGenericTask::Copy(m_pGenericTask,m_pCompositeTask->Begin());
					Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_CompositeTask);
					return 0;
				}
			}			
		} return 0;	

		// Sedert V16.43: Browsen op een single node --> nieuwe toestanden: Installation_Online_Busy_TaskBrowseSingleNode
		case EV_OPERATION_BROWSE_SINGLE_NODE: {
			if (CQHSMTaskBuilder::Build_BrowseSingleNode(m_pGenericTask) != FALSE) {				
				CAppState::getInstance().setAppState(CAppState::APP_STATE_BUSY_BROWSE_SINGLE_NODE);		
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_BROWSE_SINGLE_NODE);
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_Browse);
				return 0;			
			}						  											  
		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy(QEvent const *e)
 * 
 * Online - Busy: Busy executing a task.
 *
 * Q_ENTRY_SIG:
 * Q_EXIT_SIG: History state : store Installation_OnlineBusy_HistoryState 
 *			   - When going to Cancelled or Error state : The state can return to this history state.
 *
 * EV_SIGNAL_COMMUNICATION_NOTIFY_ACK: Notify CDlgApplicationTask application dialog.
 * EV_SIGNAL_TASK_DONE: Notification that a task has been done 
 *						Transition to CQHSMApplicationGlobal::Installation_Online_Idle
 *					    all substates leave this event to their parent state
 * EV_SIGNAL_TASK_ERROR: Notification that a task has stopped caused by an error.
 *							- Generic Task : Notify Error happened
 *							- Transition to CQHSMApplicationGlobal::Installation_Online_Error
 *					
 * EV_OPERATION_TASKDIALOG_USERBUTTON: User presses the cancel button
 *									   Check if there is a task to cancel: 
 *								+ No : Absorbe message
 *								+ Yes: - Cancel GenericTask 
 *									   - Transition to: CQHSMApplicationGlobal::Installation_Online_Cancelled
 *
 * EV_OPERATION_NEWAPPLICATION: Future Implementation !!
 * EV_OPERATION_COMMUNICATION_OFFLINE: if (AreYouSure() == FALSE) Absorbe Message !!
 * EV_OPERATION_SHUTDOWN: if (AreYouSure() == FALSE) Absorbe Message !!
 *
 * Parent : CQHSMApplicationGlobal::Installation_Online
 * 
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy(QEvent const *e)
{
	static BOOL fShutDown = FALSE;

	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online_Busy, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			fShutDown = FALSE;
			
		} return 0;

		case Q_EXIT_SIG:
		{		
			Installation_OnlineBusy_HistoryState = getState();		// Save History state !!!

		} return 0;

		case EV_SIGNAL_COMMUNICATION_NOTIFY_ACK:
		{
			CDlgMainApplicationTask* const pDlg = CDlgMainApplicationTask::GetInstance();
			if (0!=pDlg)
			{
				pDlg->ShowProgressStepUp();
			}

		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{	
			// Sedert versie 0x0800:
			// Controle of er een license activatie code nodig is.
			// Zolang deze nodig is kunnen niet naar de toestand idle gaan...

			const CAppGlobalDbaseActions::Actions appdbaseActions = CAppStateUtil::getDbaseActions(CAppState::getInstance());
			
			// Zie CHANGES_ACTIVATION_LICENSE_CODE:
			// Moeten na het starten van de database action deze altijd terug op APPDBASE_ACTION_IDLE zetten.
			// Er was anders een probleem wanneer een smartbox+ werd toegevoegd de dbase actie
			// nog op APPDBASE_ACTION_BROWSE stond ipv op APPDBASE_ACTION_IDLE

			if ((CAppGlobalDbaseActions::APPDBASE_ACTION_IDLE==appdbaseActions) && (TRUE==CAppLicActCodeAPI::IsLicenseNeeded())) {
				// Oppassen:
				// Mogen enkel naar deze toestand wnr appdbaseAction APPDBASE_ACTION_IDLE is.
				// Wanneer de problem solve wizard afgesloten wordt dan krijgen we een EV_SIGNAL_TASK_DONE
				// (gevolgd door Trigger( ) van de dbase actie die moet gebeuren) en gaan we eerst naar de toestand online IDLE.
				// Het Trigger event wordt afgehandeld in de toestand IDLE.

				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_HandleLicActCodeNeeded);
			}
			else
			{
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Idle);	
			}

		} return 0;

		case EV_SIGNAL_TASK_ERROR:
		{			
			if (m_pGenericTask != NULL) {
				GenericTask_Invoke_ErrorNotify(m_pGenericTask);
			}
			// Errors not handled by child state --> Go To Error state !!!	
			Q_TRAN_DYN(&CQHSMApplicationGlobal::Installation_Online_Error);	
		} return 0;

		case EV_OPERATION_TASKDIALOG_USERBUTTON:		// Cancelled by user ...
		{
			ASSERT(m_pGenericTask);

			if (m_pGenericTask != NULL)
			{
				if (m_pGenericTask->IsCancelEnabled() == FALSE) 
				{
					return 0;	// Absorbe : Can't Cancel !!!!
				}

				GenericTask_Invoke_Cancel(m_pGenericTask);
			}
		
			Q_TRAN_DYN(&CQHSMApplicationGlobal::Installation_Online_Cancelled);

		} return 0;

		case EV_OPERATION_NEWAPPLICATION:
		{		
			
		} break;	

		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);
				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{						
					// Absorbe requests
				} break;

				case EV_OPERATION_USER_YES:
				{					
					Q_TRAN(&CQHSMApplicationGlobal::Installation_Online);

					if (fShutDown == TRUE) 
					{
						Trigger(EV_OPERATION_SHUTDOWN);
					}
					else 
					{
						Trigger(EV_OPERATION_COMMUNICATION_OFFLINE);
					}
				} break;				
			}
		} return 0;

		case EV_OPERATION_COMMUNICATION_OFFLINE:
		{
			CString szMessage("The application is currently busy, "
							  "disconnecting from the modem can cause damage!");

			ShowDialog_AreYouSure(szMessage);		

		} return 0;

		case EV_OPERATION_SHUTDOWN:
		{
			CString szMessage("The application is currently busy, "
							  "Closing the program can cause damage!");

			ShowDialog_AreYouSure(szMessage);
			fShutDown = TRUE;

		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online);
}



/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_CompositeTask(QEvent const *e)
 * 
 * Online - Busy: Busy executing one or more Gerneric tasks.
 *				  Generic tasks are stored in 1 composite task.
 *
 * Q_ENTRY_SIG: - ClearScrollView()
 *				- Generic Task : Invoke Start (**)
 *				(**) If Fails : Trigger( EV_SIGNAL_TASK_ERROR );
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy()
 *
 * EV_SIGNAL_TASK_DONE: - Generic Task ; Notify Done.
 *					    - Composite Task : Check and Get Next Task (**)
 *							- ClearScrollView()
 *							- Copy Task to GernericTask
 *							- Generic Task : Invoke Start (**)
 *						- If No Task left: parent (Installation_Online_Busy) will handle the event to make
 *						  transition to idle mode ...
 *						(**) If Fails : Trigger( EV_SIGNAL_TASK_ERROR)
 *
 * Parent : CQHSMApplicationGlobal::Installation_Online_Busy
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_CompositeTask(QEvent const *e)
{
	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online_Busy_CompositeTask, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{			
			m_pParent->ClearScrollView();

			StartErrorLog();

			if (GenericTask_Invoke_Start(m_pGenericTask) == FALSE)
			{
				Trigger( EV_SIGNAL_TASK_ERROR );
			}		
		} return 0;

		case Q_EXIT_SIG:
		{
			CDlgMainApplicationTask::Destroy();
			//CDlgApplicationTask::Destroy();

			ShowErrorLog();

		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{
			GenericTask_Invoke_DoneNotify(m_pGenericTask);

			if (m_pCompositeTask != NULL)
			{
				CQHSMGenericTask *const pTask = m_pCompositeTask->Next();
				if (pTask != NULL)
				{
					m_pParent->ClearScrollView();

					CQHSMGenericTask::Copy(m_pGenericTask,pTask);						
					if (GenericTask_Invoke_Start(m_pGenericTask) == FALSE)
					{
						Trigger( EV_SIGNAL_TASK_ERROR );
					}		
					return 0;
				}						
			}
			else
			{
				Trigger( EV_SIGNAL_TASK_ERROR);
				return 0;
			}
		} break;	// No Task Left --> Leave event for parent ...
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy);
}

/**************************************************************************************************/

QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode(QEvent const *e) {
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode\n"));
	switch (e->sig) {
		case Q_EXIT_SIG: {			
			// Oppassen: NOTE_BROWSE_SINGLE_NODE_ADDRESS
			// Node address van AppCfgBrowseInfo niet aanpassen! (Nodig voor het herstellen van de
			// runtime node.)
			//
			// Mode opnieuw idle zetten.
			// Wanneer deze gecancelled werd door de user dan staat deze mogelijk al op MODE_IDLE.			
			AppCfgBrowseInfo& refAppcfgBrowsenode = AppCfgBrowseInfo::getInstance();					
			refAppcfgBrowsenode.setMode(AppCfgBrowseInfo::MODE_IDLE);
		} return 0;	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy);	// Basis.
}


QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_Browse(QEvent const *e) {
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_Browse\n"));
	switch (e->sig) {
		case Q_ENTRY_SIG: {		
			m_pParent->ClearScrollView();
			this->StartErrorLog();
			ASSERT(NULL != this->m_pGenericTask);
			if (this->GenericTask_Invoke_Start(this->m_pGenericTask) == FALSE) {
				this->Trigger( EV_SIGNAL_TASK_ERROR );
			}		
		} return 0;
		case Q_EXIT_SIG: {			
			
		} return 0;	

		case EV_SIGNAL_COMMUNICATION_OFFLINE: {
			this->Notify_OnTaskBrowseSingleNodeFailed( );											  
		} break;	// Door de parent...

		case EV_SIGNAL_TASK_DONE: {					
			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_RequestForStates);
		} return 0;	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode);	// Basis.
}

QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_RequestForStates(QEvent const *e) {
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_RequestForStates\n"));
	switch (e->sig) {
		case Q_ENTRY_SIG: {		
			m_pParent->ClearScrollView();
			if (CQHSMTaskBuilder::Build_RequestForStatesSingleNode(m_pGenericTask) != FALSE) {			
				if (TRUE == this->GenericTask_Invoke_Start(this->m_pGenericTask)) {
					CAppState::getInstance().setTask(CAppState::TASK_FLAG_REQUEST_FOR_STATES);					
				}
				else {
					this->Trigger( EV_SIGNAL_TASK_ERROR );
				}
			}	
		} return 0;
		case Q_EXIT_SIG: {			
			CDlgMainApplicationTask::Destroy();
		} return 0;

		case EV_SIGNAL_COMMUNICATION_OFFLINE: {
			this->Notify_OnTaskBrowseSingleNodeFailed( );											  
		} break;	// Door de parent...

		case EV_SIGNAL_TASK_DONE: {			
			ASSERT(NULL != this->m_pGenericTask);
			GenericTask_Invoke_DoneNotify(this->m_pGenericTask);	

			this->ShowErrorLog();

			// Gaan naar de toestand compare...
			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_CompareNodes);
		} return 0;	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode);	// Basis.
}

/**************************************************************************************************/
#if (_DEBUG)
	#define PROBLEMS_SINGLE_NODE			0
#else
	#define PROBLEMS_SINGLE_NODE			0
#endif 

#if (PROBLEMS_SINGLE_NODE == 1)
	#define DEBUG_SUMULATE_PROBLEMS_SINGLE_NODE( )	debugSimulateProblemsSingleNode( )
#else 
	#define DEBUG_SUMULATE_PROBLEMS_SINGLE_NODE( )
#endif 

static void debugSimulateProblemsSingleNode( ) {

	// Runtime node de laatste unit verwijderen.
	AppCfgBrowseInfo& refAppcfgBrowsenode = AppCfgBrowseInfo::getInstance();
	const int nodeAddress = refAppcfgBrowsenode.getNodeAddress();
	int dummy;

	CCanNode* pNode = NULL;
	if (FALSE == TMGetRuntimeNodes()->FindNode(nodeAddress,&dummy,&pNode)) {
		ASSERT(0);
		return;
	}

	if (TMGetRuntimeNodes()->DeleteUnitNR(pNode,pNode->bNROfUnits-1)) {
		// OK...
	}

}
/****************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_CompareNodes(QEvent const *e) {

	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_CompareNodes\n"));
	switch (e->sig) {
		case Q_ENTRY_SIG: {

			AppCfgBrowseInfo& refAppcfgBrowsenode = AppCfgBrowseInfo::getInstance();
			ASSERT(refAppcfgBrowsenode.getMode() == AppCfgBrowseInfo::MODE_BROWSE_SINGLE_NODE);
			const int nodeAddress = refAppcfgBrowsenode.getNodeAddress();

			DEBUG_SUMULATE_PROBLEMS_SINGLE_NODE();		// Testcode (zie hierboven)
			SetLock( TRUE );
			if (NULL != this->m_pParent) {
				this->m_pParent->CompareNodes(nodeAddress);
			}
			SetLock( FALSE );
			Trigger(EV_SIGNAL_TASK_DONE);
		} return 0;

		case Q_EXIT_SIG: {			
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{
			const CAppDbasePostBrowseError::Result appdbasePostBrowseErrors = CAppStateUtil::getAppDbasePostBrowseErrorResult(CAppState::getInstance());
			if (CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS != appdbasePostBrowseErrors) {
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_HandlePostBrowseErrors);
				return 0;
			}

			// Updaten van de Document node met de properties van de runtime node.
			// Indien er nieuwe units aanwezig zijn in het netwerk worden deze 
			// toegevoegd aan de node.
			AppCfgBrowseInfo& refAppcfgBrowsenode = AppCfgBrowseInfo::getInstance();
			ASSERT(refAppcfgBrowsenode.getMode() == AppCfgBrowseInfo::MODE_BROWSE_SINGLE_NODE);
			TMUpdateDocumentNode(refAppcfgBrowsenode.getNodeAddress());

			// Updaten van de views				
			if (NULL != m_pParent) {
				m_pParent->UpdateViews(refAppcfgBrowsenode.getNodeAddress());
			}

		} break;	// If success : Goto Installation_Online_Busy --> Installation_Online_Idle.
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode);
}

QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_HandlePostBrowseErrors(QEvent const *e) {

	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode_HandlePostBrowseErrors\n"));
	switch (e->sig)  {
		case Q_ENTRY_SIG: {					
			SetLock( TRUE );			
			CString strHeader("Incompatible nodes");
			CString strMessage("The node cannot be refreshed because the nodes are incompatible.\r\n"
							   "Possible reasons are the node types are different or the node in the network holds "
							   "less or different units compared to the node in the nodedabase.");
			AfxGetMainWnd()->MessageBox(strMessage, strHeader,MB_ICONSTOP);
			SetLock( FALSE );

			Trigger( EV_SIGNAL_TASK_DONE );		// Taak gedaan - kunnen naar de volgende toestand.

		} return 0;

		case EV_SIGNAL_TASK_DONE: {				
			// De runtime node opnieuw herstellen met de properties en units van de document node.
			AppCfgBrowseInfo& refAppcfgBrowsenode = AppCfgBrowseInfo::getInstance();
			ASSERT(refAppcfgBrowsenode.getMode() == AppCfgBrowseInfo::MODE_BROWSE_SINGLE_NODE);			
			TMRestoreRuntimeNode(refAppcfgBrowsenode.getNodeAddress());

			// Mode opnieuw idle zetten.
			// refAppcfgBrowsenode.setMode(AppCfgBrowseInfo::MODE_IDLE);

		} break;	// Verder afhandelen door de basis fsm.
	}
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_TaskBrowseSingleNode);
}
/**************************************************************************************************/


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_CompositeTask(QEvent const *e)
 * 
 * Online - Busy: Busy executing one generic task.
 *
 * Q_ENTRY_SIG: - ClearScrollView()
 *				- Generic Task : Invoke Start (**)
 *				(**) If Fails : Trigger( EV_SIGNAL_TASK_ERROR );
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy()
 * 
 * EV_SIGNAL_TASK_DONE: - Generic Task ; Notify Done.
 *						- parent (Installation_Online_Busy) will handle the event to make
 *						  transition to idle mode ...
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online_Busy
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask(QEvent const *e)
{	
	if (1) {
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online_Busy_GenericTask, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) {
		case Q_ENTRY_SIG: {
			m_pParent->ClearScrollView();
			StartErrorLog();
			if (GenericTask_Invoke_Start(m_pGenericTask) == FALSE) {
				Trigger( EV_SIGNAL_TASK_ERROR );
			}		
		} return 0;

		case Q_EXIT_SIG: {			
			CDlgMainApplicationTask::Destroy();
			ShowErrorLog();
		} return 0;

		case EV_SIGNAL_TASK_DONE: {
			GenericTask_Invoke_DoneNotify(m_pGenericTask);			
		} break;	// Parent Transition : Busy state --> Idle ...
	}
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy);
}



/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New(QEvent const *e)
 * 
 * Online Busy: Mode Creating an new installation
 *		--> Sequence of actions : Browse For Nodes, Request For states,...
 *								  HandlePreBrowseErrors. 
 *							      Automatic Pnp.
 * 
 * Q_ENTRY_SIG:
 * Q_EXIT_SIG:
 * Q_INIT_SIG: Initial Transition : CQHSMApplicationGlobal::Installation_Online_Busy_New_BrowseForNodes
 *
 * EV_SIGNAL_TASK_DONE: Notify when last task in the sequence is done.
 *						- Initialising application before going to idle mode
 *							+ ClearScrollView
 *							+ CopyRuntimeToDocDatabases
 *							+ CreateEmptyBindingDatabase
 *							+ UpdateViews + ShowNodesView + Set Application Type = Idle.
 *						- parent (Installation_Online_Busy) will handle the event to make
 *						  transition to idle mode ...
 *
 * Parent : CQHSMApplicationGlobal::Installation_Online_Busy 
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New(QEvent const *e)
{
	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online_Busy_New, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{		

		} return 0;

		case Q_EXIT_SIG:
		{		

		} return 0;

		case Q_INIT_SIG:
		{		
			Q_INIT(&CQHSMApplicationGlobal::Installation_Online_Busy_New_BrowseForNodes);
		
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{	
			SetLock( TRUE );	// !!!

			m_pParent->ClearScrollView();	

			TMCopyRuntimeToDocDatabase();
			TMCreateNewBindingFiles();	
			TMCreateUnitListDelegation();
			TMCreateUnitListRequestMeasuredValues();	// sedert versie 0x0C03

			// Sedert V16.00: USE_SENSOR_PIDCONTROL	
			// Een lijst aanmaken van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
			// Controleren of alle units die gekoppeld werden als PID control unit nog aanwezig zijn.
			// Indien er units gekoppeld werden die niet meer beschikbaar zijn de P-Binding verwijderen.
			TMCreateUnitDimListPIDControl();

			// Sedert V15.06
			TMCreateExtraPBindings();					

			// Sedert V16.17: Aanmaken van nieuwe bindings.
			// Conditie bindings master.
			TMCreateExtraBindings();

			// Sedert V16.42:
			// Extra info voor de units.
			// Momenteel wordt deze enkel gebruikt om een extra tekstlabel weer te geven bij de units.
			TMSetDatabaseUnitExtraInfo();

			m_pParent->ShowBrowseInfo();
			m_pParent->UpdateViews();
			m_pParent->ShowNodesView();

			SetLock( FALSE );	// !!!

			CAppStateUtil::setAppStateIdle(CAppState::getInstance());		

		} break;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New_BrowseForNodes(QEvent const *e)
 * 
 * Online Busy Installation mode : Create New programmation file.
 *	Task = Browse for nodes. 
 *
 *
 * Q_ENTRY_SIG: - ClearScrollView
 *				- Build Generic Task : Build_BrowseAllPhysicalNodes
 *				- Generic Task : Invoke Start.
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy()
 * 
 * EV_SIGNAL_TASK_DONE: Notify when task is done 
 *						- Generic Task: Notify Done
 *						- Transition to: 
 *							CQHSMApplicationGlobal::Installation_Online_Busy_New_RequestForStates
 *
 * EV_SIGNAL_TASK_ERROR: (OVERRIDE) Notify when task is stopped caused by error.
 *						 - Generic Task: Notify Error
 *						 - If (PreBrowseError()) 
 *							+ Yes: Transition to: 
 *								CQHSMApplicationGlobal::Installation_Online_Busy_New_HandlePreBrowseError
 *							+ no: Transition to:
 *								CQHSMApplicationGlobal::Installation_Online_Error
 *
 * Parent : &CQHSMApplicationGlobal::Installation_Online_Busy_New
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New_BrowseForNodes(QEvent const *e)
{
	
	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online_Busy_New_BrowseForNodes, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		case Q_ENTRY_SIG: {		
			m_pParent->ClearScrollView();
			if (CQHSMTaskBuilder::Build_BrowseAllPhysicalNodes(m_pGenericTask) != FALSE) {			
				GenericTask_Invoke_Start(m_pGenericTask);			
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_BROWSE_FOR_NODES);
			}			
		} return 0;

		case Q_EXIT_SIG:
		{			
			//CDlgApplicationTask::Destroy();

			CDlgMainApplicationTask::Destroy();

		} return 0;	

		case EV_SIGNAL_TASK_DONE:
		{		
			GenericTask_Invoke_DoneNotify(m_pGenericTask);	

			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_New_RequestForStates);

		} return 0;

		case EV_SIGNAL_TASK_ERROR:
		{		
			GenericTask_Invoke_ErrorNotify(m_pGenericTask);

			if (CAppStateUtil::IsPreBrowseError(CAppState::getInstance()))
			{
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_New_HandlePreBrowseErrors);
				return 0;
			}		

			Q_TRAN_DYN(&CQHSMApplicationGlobal::Installation_Online_Error);

		} return 0;	// Override !!!

    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_New);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New_RequestForStates(QEvent const *e)
 * 
 * Online Busy Installation mode : Create New programmation file.
 *	Task = Request for States: When successfully done mode (parent) can go to idle ...
 *
 * Q_ENTRY_SIG: - ClearScrollView
 *				- Build Generic Task : Build_RequestForStates
 *				- Generic Task : Invoke Start.
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy()
 * 
 * EV_SIGNAL_TASK_DONE: Notify when task is done 
 *						- Leave transition to parent : Change mode to idle 
 *
 *
 * Parent : &CQHSMApplicationGlobal::Installation_Online_Busy_New
 *
 ***************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New_RequestForStates(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_New_RequestForStates\n"));

	switch (e->sig) 	
	{
		case Q_ENTRY_SIG: {			
			m_pParent->ClearScrollView();
			if (CQHSMTaskBuilder::Build_RequestForStates(m_pGenericTask) != FALSE) {
				GenericTask_Invoke_Start(m_pGenericTask);			
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_REQUEST_FOR_STATES);
			}			
		} return 0;

		case Q_EXIT_SIG:
		{		
			//CDlgApplicationTask::Destroy();

			CDlgMainApplicationTask::Destroy();

		} return 0;	

		case EV_SIGNAL_TASK_DONE:
		{			
			GenericTask_Invoke_DoneNotify(m_pGenericTask);

		} break;	// Leave Transition to Parent!!!!	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_New);
}



/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New_HandlePreBrowseErrors(QEvent const *e)
 * 
 * Handle errors that occured during Browse for nodes : Duplicate logica addresses, Factory addresses
 *
 * Q_ENTRY_SIG: - Start_HandlePreBrowseErrors();		Modal Wizard
 *				- Trigger( EV_SIGNAL_TASK_DONE );
 * Q_EXIT_SIG:
 * 
 * EV_SIGNAL_TASK_DONE: If(Is_ActionPnp()) 
 *						+ Yes: Transition to
 *							CQHSMApplicationGlobal::Installation_Online_Busy_New_AutomaticPnp
 *						+ No: Transition to
 *							CQHSMApplicationGlobal::Installation_Online_Busy_New (**)
 *
 *						(**) Installation_Online_Busy_New : (See Device mode Idiom)
 *						 --> goes via initial transition to Browse for nodes ...
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online_Busy_New
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New_HandlePreBrowseErrors(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_New_HandlePreBrowseErrors\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			TRACER_FSM(("\nInstallation_Online_Busy_New_BrowseForNodes - Q_ENTRY_SIG"));

			SetLock( TRUE );
			Start_HandlePreBrowseError();
			SetLock( FALSE );

			Trigger( EV_SIGNAL_TASK_DONE );

		} return 0;

		case Q_EXIT_SIG:
		{
			TRACER_FSM(("\nInstallation_Online_Busy_New_BrowseForNodes - Q_EXIT_SIG"));

		} return 0;	

		case EV_SIGNAL_TASK_DONE:
		{
			const CAppGlobalDbaseActions::Actions appdbaseActions = CAppStateUtil::getDbaseActions(CAppState::getInstance());
			TRACER_FSM(("\nInstallation_Online_Busy_New_BrowseForNodes - EV_SIGNAL_TASK_DONE"));
			if (CAppGlobalDbaseActions::APPDBASE_ACTION_PNP==appdbaseActions) {
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_New_AutomaticPnp);
			}
			else  {			
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_New);
			}
		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_New);
}



/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New_AutomaticPnp(QEvent const *e)
 * 
 * Online Busy Installation mode : Create New programmation file.
 *	Task = Automatic Pnp: When successfully done mode (parent) can go to idle ...
 *
 * Q_ENTRY_SIG: - ClearScrollView()
 *				- Build Generic Task : Build_AutomaticPlugAndPlay
 *				- Generic Task : Invoke Start.
 * Q_EXIT_SIG: CDlgApplicationTask::Destroy()
 * 
 * EV_SIGNAL_TASK_DONE: Notify when task is done 
 *						- Generic Task: Notify Done
 *						- Transition to: 
 *							CQHSMApplicationGlobal::Installation_Online_Busy_New_BrowseForNodes
 *
 * Parent : CQHSMApplicationGlobal::Installation_Online_Busy_New
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_New_AutomaticPnp(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_New_AutomaticPnp\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG: {			
			m_pParent->ClearScrollView();
			if (CQHSMTaskBuilder::Build_AutomaticPlugAndPlay(m_pGenericTask) != FALSE) {
				GenericTask_Invoke_Start(m_pGenericTask);	
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_PNP);
				// Sedert v0x800, TM20141117 (CHANGES_ACTIVATION_LICENSE_CODE)
				clearRequestedDbaseAction(CAppGlobalDbaseActions::APPDBASE_ACTION_PNP);
				return 0;
			}					
		} return 0;

		case Q_EXIT_SIG:
		{		
			//CDlgApplicationTask::Destroy();

			CDlgMainApplicationTask::Destroy();

		} return 0;	

		case EV_SIGNAL_TASK_DONE:
		{
			GenericTask_Invoke_DoneNotify(m_pGenericTask);	
			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_New_BrowseForNodes);

		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_New);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue(QEvent const *e)
 *   
 * Online Continue: Mode continueing from programmation file
 *		--> Sequence of actions : Browse For Nodes, Request For states, CompareDatabases
 *								  HandlePreBrowseErrors, HandlePostBrowseErrors
 *							      Automatic Pnp.
 *
 * Q_ENTRY_SIG:
 * Q_EXIT_SIG:
 * Q_INIT_SIG: Initial transition:
 *			   - CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes
 *
 * EV_TASK_DONE: Notify when last task in the sequence is done.
 *					- Initialising application before going to idle mode
 *					  + ClearScrollView
 *					  + AssignBindingDatabaseToRTNodes();
 *					  + UpdateNodeDatabaseWithRTSpecs();
 *					  + ShowBrowseInfo()
 *					  + UpdateViews();
 *					- Check if there are certain operations have to be done, when going in Idle
 *					  mode (**)
 *					  + Is_ActionSendNames()
 *						-> Trigger( EV_OPERATION_UPLOAD_NAMES )
 *					  + Is_ActionSendBindings()
 *						-> Trigger( EV_OPERATION_UPLOAD_ALLBINDINGS )
 *					  + Is_ActionSendProgFile()
 *						-> Trigger( EV_OPERATION_UPLOAD_PROGFILE )
 *		
 *					  (**) Assynchronous operations : When QHSM receives the events the transition 
 *						   to idle mode has already been made by the parent state !!			
 *
 * Parent : CQHSMApplicationGlobal::Installation_Online_Busy 
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_Continue\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{						
		} return 0;

		case Q_EXIT_SIG:
		{	
		} return 0;

		case Q_INIT_SIG:
		{			
			Q_INIT(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes);
		
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{		
			// Komen hier wanneer de browse for nodes + controle database gebeurd is
			const CAppGlobalDbaseActions::Actions appdbaseActions = CAppStateUtil::getDbaseActions(CAppState::getInstance());
			BOOL copyNames = TRUE;

			SetLock(TRUE);
			m_pParent->ClearScrollView();

			// TM20121025: Zie BUG-0130
			// Mogen de namen van de nodes enkel copieren indien deze niet moeten 
			// doorgestuurd worden. (Via sendNames of via volledige prog bestand.)								
			if (CAppGlobalDbaseActions::APPDBASE_ACTION_SENDNAMES==appdbaseActions || CAppGlobalDbaseActions::APPDBASE_ACTION_SENDPROGFILE==appdbaseActions) {
				copyNames = FALSE;
			}

			// Sedert V16.16: Functie gebruiken.
			TMUpdateDocumentNodeDatabase(copyNames);

			// update gui
			m_pParent->ShowBrowseInfo();	
			m_pParent->UpdateViews();	
			m_pParent->ShowNodesView();	
			SetLock( FALSE );
				
			/**
			 * Assynchronous operations : When QHSM receives the events
			 * the transition to idle mode has already been made by the
			 * parent state !!			
			 */
			
			if (CAppGlobalDbaseActions::APPDBASE_ACTION_SENDNAMES==appdbaseActions) {
				Trigger( EV_OPERATION_UPLOAD_NAMES );				
			}
			else if (CAppGlobalDbaseActions::APPDBASE_ACTION_SENDBINDINGS==appdbaseActions) {
				Trigger( EV_OPERATION_UPLOAD_ALLBINDINGS );		
			}
			else if (CAppGlobalDbaseActions::APPDBASE_ACTION_SENDPROGFILE==appdbaseActions) {			
				Trigger( EV_OPERATION_UPLOAD_PROGFILE );				
			}			
		} break;

		// Sedert V16.16:
		// Opnieuw controleren van de post browse errors (zonder dat we opnieuw browsen.)
		case EV_OPERATION_CHECK_POSTBROWSE_ERRORS: {
			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_CompareDbases);
			return 0; 						   
		} break;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes(QEvent const *e)
 *   
 * Online Continue: Task: Browse For Nodes.
 *
 * Q_ENTRY_SIG: - ClearScrollView()
 *				- Build Generic Task : Build_BrowseAllPhysicalNodes
 *				- Generic Task : Invoke Start.
 *
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy();
 * 
 * EV_SIGNAL_TASK_DONE: Notify when task is done 
 *						- Generic Task: Notify Done
 *						- Transition to:
 *							CQHSMApplicationGlobal::Installation_Online_Busy_Continue_RequestForStates
 *
 * EV_SIGNAL_TASK_ERROR: (OVERRIDE) Notify when task is stopped caused by error.
 *						 - Generic Task: Notify Error
 *						 - If (PreBrowseError()) 
 *							+ Yes: Transition to: 
 *							  - CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePreBrowseErrors
 *							+ No: Transition to:
 *							  - CQHSMApplicationGlobal::Installation_Online_Error
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online_Busy_Continue
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG: {			
			m_pParent->ClearScrollView();
			if (CQHSMTaskBuilder::Build_BrowseAllPhysicalNodes(m_pGenericTask) != FALSE) {			
				GenericTask_Invoke_Start(m_pGenericTask);		
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_BROWSE_FOR_NODES);
				// Sedert v0x800, TM20141117 (CHANGES_ACTIVATION_LICENSE_CODE)
				clearRequestedDbaseAction(CAppGlobalDbaseActions::APPDBASE_ACTION_BROWSE);			
			}									
		} return 0;	

		case Q_EXIT_SIG:
		{			
			CDlgMainApplicationTask::Destroy();
		} return 0;	

		case EV_SIGNAL_TASK_DONE:
		{			
			GenericTask_Invoke_DoneNotify(m_pGenericTask);	
			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_RequestForStates);
		} return 0;

		case EV_SIGNAL_TASK_ERROR: {						
			GenericTask_Invoke_ErrorNotify(m_pGenericTask);
			if (CAppStateUtil::IsPreBrowseError(CAppState::getInstance())) {
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePreBrowseErrors);
				return 0;	
			}				
			Q_TRAN_DYN(&CQHSMApplicationGlobal::Installation_Online_Error);
		} return 0;	// Override !!!
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_RequestForStates(QEvent const *e)
 *   
 * Online Continue: Task: Request for states.
 *
 * Q_ENTRY_SIG: - ClearScrollView()
 *				- Build Generic Task : Build_RequestForStates
 *				- Generic Task : Invoke Start.
 *
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy();
 * 
 * EV_SIGNAL_TASK_DONE: Notify when task is done 
 *						- Generic Task: Notify Done
 *						- Transition to:
 *							CQHSMApplicationGlobal::Installation_Online_Busy_Continue_CompareDbases
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online_Busy_Continue
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_RequestForStates(QEvent const *e) {
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_Continue_RequestForStates\n"));
	switch (e->sig) {
		case Q_ENTRY_SIG: {		
			m_pParent->ClearScrollView();
			if (CQHSMTaskBuilder::Build_RequestForStates(m_pGenericTask) != FALSE) {
				GenericTask_Invoke_Start(m_pGenericTask);
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_REQUEST_FOR_STATES);
				return 0;
			}		
		} return 0;
		case Q_EXIT_SIG: {			
			CDlgMainApplicationTask::Destroy();
		} return 0;	

		case EV_SIGNAL_TASK_DONE: {			
			GenericTask_Invoke_DoneNotify(m_pGenericTask);	
			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_CompareDbases);

		} return 0;	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePreBrowseErrors(QEvent const *e)
 *   
 * Handle errors that occured during Browse for nodes : Duplicate logica addresses, Factory addresses
 *
 * Q_ENTRY_SIG: - Start_HandlePreBrowseErrors();		Modal Wizard
 *				- Trigger( EV_SIGNAL_TASK_DONE );
 * Q_EXIT_SIG:
 * 
 * EV_SIGNAL_TASK_DONE: If(Is_ActionPnp()) 
 *						+ Yes: Transition to
 *							CQHSMApplicationGlobal::Installation_Online_Busy_Continue_AutomaticPnp
 *						+ No: Transition to
 *							CQHSMApplicationGlobal::Installation_Online_Busy_Continue (**)
 *
 *						(**) Installation_Online_Busy_Continue : (See Device mode Idiom)
 *						 --> goes via initial transition to Browse for nodes ...
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online_Busy_Continue
 *
 **************************************************************************************************/
// sedert V16.47: CHANGES_UPLOAD_UNITS
// Testcode die ervoor zorgt dat na het sluiten van de 'Problem solve wizard' units naar de 
// nodes doorgestuurd worden.
#define USE_TEST_PROBLEMSOLVEWIZARD_SEND_UNITS			0
/***************************************************************************************************/
#if (USE_TEST_PROBLEMSOLVEWIZARD_SEND_UNITS == 1)
	#if (_DEBUG)
		#define DEBUG_TEST_ADDTASK2(task,la)				CAppFsmNodeTaskEntries_Utils::addTask(CAppState::getInstance().getTaskEntries(), task, la)
	#else
		#define DEBUG_TEST_ADDTASK2(task,la)	
	#endif
#else 
	#define DEBUG_TEST_ADDTASK2(task,la)	
#endif
/****************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePreBrowseErrors(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePreBrowseErrors\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{	
			SetLock( TRUE );
			Start_HandlePreBrowseError();
			SetLock( FALSE );

			// sedert V16.47: CHANGES_UPLOAD_UNITS
			// Om te testen: Gaan de units doorsturen naar het node address 0x03 en 0xfc... (zie macro USE_TEST_PROBLEMSOLVEWIZARD_SEND_UNITS)
			// Worden enkel uitgevoerd in _DEBUG mode.
			DEBUG_TEST_ADDTASK2(CAppFsmNodeTaskEntry::APPFSM_TASK_SENDUNITS, 0xFC);
			DEBUG_TEST_ADDTASK2(CAppFsmNodeTaskEntry::APPFSM_TASK_SENDUNITS, 0x03);

			Trigger( EV_SIGNAL_TASK_DONE );
		} return 0;

		case Q_EXIT_SIG:
		{			
		} return 0;	

		case EV_SIGNAL_TASK_DONE:
		{		
			// sedert V16.47: CHANGES_UPLOAD_UNITS
			// Na het afsluiten van de problem solve wizard controleren of er taken moeten uitgevoerd worden.			
			if (!CAppState::getInstance().getTaskEntries().isIdle()) {
				// Wanneer er taken moeten uitgevoerd worden gaan we naar de toestand Installation_Online_Busy_Continue_NodeFsmTasks.
				// Na het uitvoeren van deze taken komen we in CQHSMApplicationGlobal::Installation_Online_Busy_Continue waar er 
				// gestart wordt met een browse for nodes.
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_NodeFsmTasks);
				return 0;
			}		

			// Er hoeven geen extra taken uitgevoerd worden voor we starten met browsen.
			// Controleren of er een PNP of een Physical browse for nodes moet uitgevoerd worden.
			const CAppGlobalDbaseActions::Actions appdbaseActions = CAppStateUtil::getDbaseActions(CAppState::getInstance());
			if (CAppGlobalDbaseActions::APPDBASE_ACTION_PNP==appdbaseActions) {
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_AutomaticPnp);
			}
			else {			
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
			}
		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
}


// sedert V16.47: CHANGES_UPLOAD_UNITS
// Helper functie voor de 'Installation_Online_Busy_Continue_NodeFsmTasks' toestand.
void CQHSMApplicationGlobal::helperGoToNextTask(CAppFsmNodeTaskEntries& refTaskEntries) {
	if (refTaskEntries.isIdle()) {
		Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
		return;
	}
	CAppFsmNodeTaskEntry taskEntry = refTaskEntries.getNextTask();
	if (taskEntry.getTask() == CAppFsmNodeTaskEntry::APPFSM_TASK_SENDUNITS) {	
		// Oppassen: Via de trigger wordt het bericht assynchroon verstuurd.
		// Dit is nodig voor de parent FSM.
		// Na het uitvoeren van de vorige upload wordt de FSM assynchroon gedealloceerd.
		// Moeten een assynchrone trigger gebruiken om een nieuwe instantie aan te maken.
		this->Trigger(EV_OPERATION_UPLOAD_UNITCONFIG);		// Assynchroon.
	} else { ASSERT(0); }		
}

// sedert V16.47: CHANGES_UPLOAD_UNITS
// Nieuwe toestand voor het doorsturen van de unit configuratie naar de modules.
//
// Deze toestand wordt uitgevoerd voor het starten van de physical browse for nodes.
// 
// Eerst worden alle taken (versturen van units) voor alle nodes uitgevoerd.
// Daarna gaan we naar de toestand 'CQHSMApplicationGlobal::Installation_Online_Busy_Continue' waar er een 
// physical browse for nodes uitgeveord wordt.
//
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_NodeFsmTasks(QEvent const *e) {
	switch (e->sig) {
		case Q_ENTRY_SIG: {			
			m_pParent->ClearScrollView();

			// Oppassen: Bij een cancel (try again) komen we opnieuw in deze toestand.
			// Wanneer er geen taken meer moeten uitgevoerd worden gaan we verder met de FSM (=browse for physical nodes.)
			CAppFsmNodeTaskEntries& refTaskEntries = CAppState::getInstance().getTaskEntries();
			if (refTaskEntries.isIdle()) {				
				// Wanneer er niks meer moet uitgevoerd worden kunnen we verder gaan.

				// Oppassen: 
				// Doen geen retry op de vorige taak want anders komen we in een loop en blijven we deze taak herhalen.
				// Nadeel: Zullen wellicht incompatible nodes krijgen na het browsen...
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue);	
				return 0;
			}

			// Starten met de fsm.
			this->helperGoToNextTask(refTaskEntries);		
		} return 0;	

		case Q_EXIT_SIG: {			
			CDlgMainApplicationTask::Destroy();
		} return 0;	

		case EV_SIGNAL_TASK_DONE: {			
			GenericTask_Invoke_DoneNotify(m_pGenericTask);			
			this->helperGoToNextTask(CAppState::getInstance().getTaskEntries());
		} return 0;

		case EV_OPERATION_UPLOAD_UNITCONFIG: {
				CAppFsmNodeTaskEntries& refTaskEntries = CAppState::getInstance().getTaskEntries();	
				CAppFsmNodeTaskEntry taskEntry = refTaskEntries.getCurrentTask();
				const int nodeAddress = taskEntry.getNodeAddress();
				if (FALSE == CQHSMTaskBuilder::Build_UploadUnitsCreateConfigReplaceNode(m_pGenericTask,nodeAddress)) {
					Trigger(EV_SIGNAL_TASK_ERROR);	
					return 0;
				}			
				GenericTask_Invoke_Start(m_pGenericTask);											 											 
		} break;

		case EV_SIGNAL_TASK_ERROR: {						
			GenericTask_Invoke_ErrorNotify(m_pGenericTask);		
			this->helperGoToNextTask(CAppState::getInstance().getTaskEntries());
		} return 0;	// Override !!!
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePostBrowseErrors(QEvent const *e)
 *   
 * Handle errors that occured after comparing databasese 
 *	- Nodes added,removed,incompatible,XOR differences,Name differences,...
 *
 * Q_ENTRY_SIG: - Start_HandlePostBrowseError();		Modal Wizard
 *				- Trigger( EV_SIGNAL_TASK_DONE );
 * Q_EXIT_SIG:
 * 
 * EV_SIGNAL_TASK_DONE: - If(Is_ErrorNodesAdded()) 
 *							+ Yes: - AddNewNodesToRTDatabases()
 *								   - AssignBindingDatabaseToDocNodes()
 *						- AssignBindingDatabaseToRTNodes();
 *						- UpdateNodeDatabaseWithRTSpecs();
 *						- If(Is_ActionBrowseForNodes())
 *							+ Yes: Transition to:
 *								- CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes
 *							+ No
 *								- Let the parent state handle the error...
 *
 *
 *						(**) Installation_Online_Busy_Continue : (See Device mode Idiom)
 *						 --> goes via initial transition to Browse for nodes ...
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online_Busy_Continue
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePostBrowseErrors(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePostBrowseErrors\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			// Komen in deze toestand wanneer er een verschil is bij het vergelijken van de runtime en 
			// offline database.
			// Tonen hier de probleem solve wizard.

	
			// Weergave van de problem solve wizard: Deze bepaalt welke taken er moeten uitgevoerd worden.
			SetLock( TRUE );
			Start_HandlePostBrowseError();
			SetLock( FALSE );

			
			bool fTaskDone = TRUE;
			CAppState& refAppState = CAppState::getInstance();			
			const CAppDbasePostBrowseError::Result appdbasePostBrowseErrors = CAppStateUtil::getAppDbasePostBrowseErrorResult(refAppState);					

			// Sedert V16.16:
			// indien er nodes toegevoegd werden dan nog eens de databases vergelijken om te controleren of er 
			// naam verschillen of configuratie verschillen zijn...
			SetLock( TRUE );
			if (CAppDbasePostBrowseError::DBASE_EC_NODES_ADDED==appdbasePostBrowseErrors) {
				TMAddNewNodes();		// Toevoegen van de nieuwe nodes.
				fTaskDone = FALSE;
			}
			if (CAppDbasePostBrowseError::DBASE_EC_NODES_NOT_FOUND==appdbasePostBrowseErrors) {
				if (CAppGlobalDbaseActions::APPDBASE_ACTION_IDLE == CAppStateUtil::getDbaseActions(CAppState::getInstance())) {
					fTaskDone = FALSE;
				}
			}
			SetLock( FALSE );

			if (fTaskDone) {
				// het signaal 'EV_SIGNAL_TASK_DONE' wordt verder in deze toestand afgehandeld (zie code hieronder).				
				Trigger( EV_SIGNAL_TASK_DONE );						
			}
			else {
				// Nog eens controleren.
				// Mbv deze trigger gaan we opnieuw naar de toestand 'Installation_Online_Busy_Continue_CompareDbases'.
				Trigger( EV_OPERATION_CHECK_POSTBROWSE_ERRORS );	
			}			
		} return 0;

		case Q_EXIT_SIG: {			
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{				
			/*************************************************************************************
			 * TM 20070927: Prevent the application from crashing 
			 * Use a Try catch + Are You Sure + EV_OPERATION_MESSAGEBOX_RESPONSE
			 ************************************************************************************/
			try {
				CAppState& refAppState = CAppState::getInstance();				
				const CAppDbasePostBrowseError::Result appdbasePostBrowseErrors = CAppStateUtil::getAppDbasePostBrowseErrorResult(refAppState);			
				const CAppGlobalDbaseActions::Actions appdbaseActions =  CAppStateUtil::getDbaseActions(refAppState);
				this->SetLock( TRUE );						
				if (!(CAppDbasePostBrowseError::DBASE_EC_NODES_NOT_FOUND==appdbasePostBrowseErrors || CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE==appdbasePostBrowseErrors)) {								
					// Database mag enkel gekopieerd worden wanneer alle nodes gevonden werden.
					BOOL copyNames = TRUE;					
					if (CAppGlobalDbaseActions::APPDBASE_ACTION_SENDNAMES==appdbaseActions || CAppGlobalDbaseActions::APPDBASE_ACTION_SENDPROGFILE==appdbaseActions) {
						// Zie BUG-0130: Mogen de namen van de nodes enkel copieren indien deze niet moeten doorgestuurd worden. (Via sendNames of via volledige prog bestand.)		
						copyNames = FALSE;
					}				
					TMUpdateDocumentNodeDatabase(copyNames);
				}
				this->SetLock( FALSE );

				if (CAppGlobalDbaseActions::APPDBASE_ACTION_BROWSE==appdbaseActions) {
					Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes);
					return 0; 
				}			
			}
			catch (CExceptionNodeDatabaseInterface* e ) {			
				delete e;				
				SetLock( FALSE );
				CString s("Error cannot be handled !!!\nRestart Application");				
				ShowDialog_AreYouSure(s);			
				return 0;	// Cannot go to idle !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!				
			}				
		} break;	// No action --> Go To Idle ...

		case EV_OPERATION_MESSAGEBOX_RESPONSE: {		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);				
			switch(pMyEvent->lParam) {
				case EV_OPERATION_USER_NO: {						
					Q_TRAN(&CQHSMApplicationGlobal::Installation_Online);				
					Trigger(EV_OPERATION_SHUTDOWN);
				} return 0;
				case EV_OPERATION_USER_YES: {	
					Q_TRAN(&CQHSMApplicationGlobal::NewApplication);					
					return 0; 
				}
			}
		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_AutomaticPnp(QEvent const *e)
 *   
 * Online Continue: Task: Automatic Pnp
 *
 * Q_ENTRY_SIG: - ClearScrollView()
 *				- Build Generic Task : Build_AutomaticPlugAndPlay
 *				- Generic Task : Invoke Start.
 *
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy();
 * 
 * EV_SIGNAL_TASK_DONE: Notify when task is done 
 *						- Generic Task: Notify Done
 *						- Transition to:
 *							CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online_Busy_Continue
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_AutomaticPnp(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_Continue_AutomaticPnp\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG: {			
			m_pParent->ClearScrollView();
			if (CQHSMTaskBuilder::Build_AutomaticPlugAndPlay(m_pGenericTask) != FALSE) {
				GenericTask_Invoke_Start(m_pGenericTask);	
				CAppState::getInstance().setTask(CAppState::TASK_FLAG_PNP);
				// Sedert v0x800, TM20141117 (CHANGES_ACTIVATION_LICENSE_CODE)
				clearRequestedDbaseAction(CAppGlobalDbaseActions::APPDBASE_ACTION_PNP);
				return 0;
			}				
		} return 0;

		case Q_EXIT_SIG:
		{		
			CDlgMainApplicationTask::Destroy();

		} return 0;	

		case EV_SIGNAL_TASK_DONE:
		{
			GenericTask_Invoke_DoneNotify(m_pGenericTask);	

			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_BrowseForNodes);

		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_CompareDbases(QEvent const *e)
 *   
 * Online Continue: Task: Compare document & runtime node database.
 *
 * Q_ENTRY_SIG: - CompareNodeDatabase()
 *				- Trigger( EV_SIGNAL_TASK_DONE )
 *
 * Q_EXIT_SIG
 *
 * EV_SIGNAL_TASK_DONE	- Is_CompareSuccess()
 *						+ yes: transition to:
 *							CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePostBrowseErrors
 *						+ No:
 *							- Leave transition to parent : Change mode to idle 
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online_Busy_Continue
 *
 **************************************************************************************************/
#define DEBUG_SIMULATE_NODES_ADDED					0		// Testcode.
#define DEBUG_SIMULATE_NODES_INCOMPATIBLE			0		// Testcode.

#if(DEBUG_SIMULATE_NODES_ADDED == 1)
	#include "app/appnodedbase/appnodedbase_test.h"
#endif 

#if(_DEBUG == 1)
static void dbgTestSimulateProblems( ) {
	static int debugCounter = 0;
#if(DEBUG_SIMULATE_NODES_ADDED == 1)			
	if (1 == debugCounter || 0 == debugCounter) {										
		AppNodeDbaseTest dbgDbase(TMGetRuntimeNodes());
		dbgDbase.testAddNewNodes(5); // , 0x72);
	}
#endif 
#if (DEBUG_SIMULATE_NODES_INCOMPATIBLE == 1)
	if (0 == debugCounter) {			
		AppNodeDbaseTest dbaseTest(TMGetRuntimeNodes());
		dbaseTest.testChangeNodeUnit(0x04,0,UNIT_TYPE_DIM);			
	}
#endif 	
	debugCounter++;
}
#endif 

#if(_DEBUG == 1)
	#define DEBUG_SUMULATE_PROBLEMS()	dbgTestSimulateProblems()
#else
	#define DEBUG_SUMULATE_PROBLEMS()
#endif 
/***********************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_Continue_CompareDbases(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_Continue_CompareDbases\n"));
	switch (e->sig) {
		case Q_ENTRY_SIG: {
			DEBUG_SUMULATE_PROBLEMS();		// Testcode (zie hierboven)
			SetLock( TRUE );
			if (m_pParent != NULL)  {
				m_pParent->CompareNodeDatabases();
			}
			SetLock( FALSE );
			Trigger(EV_SIGNAL_TASK_DONE);
		} return 0;

		case Q_EXIT_SIG:
		{			
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{
			const CAppDbasePostBrowseError::Result appdbasePostBrowseErrors = 
				CAppStateUtil::getAppDbasePostBrowseErrorResult(CAppState::getInstance());
			if (CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS != appdbasePostBrowseErrors) {
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Online_Busy_Continue_HandlePostBrowseErrors);
				return 0;
			}
		} break;	// If success : Goto Idle !!!!
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy_Continue);
}

QSTATE CQHSMApplicationGlobal::Installation_Online_Busy_HandleLicActCodeNeeded(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Busy_HandleLicActCodeNeeded\n"));	

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			// Indien de klant op de cancel button drukt omdat deze geen activatie code
			// heeft dan zal de applicatie afsluiten.

			BOOL fResult = FALSE;
			// EV_QHSM_APPLICATIONGLOBAL ev = EV_SIGNAL_TASK_DONE;

			SetLock( TRUE );
			fResult = Start_HandleLicAccCodeNeeded();
			SetLock( FALSE );
		
			if (TRUE==fResult)
			{
				Trigger(EV_SIGNAL_TASK_DONE);
			}
			else
			{
				if (ShowDialog_AreYouSure("Exit application ?") == FALSE)
				{
					// Indien de dialoog niet kan getoond worden.
					// Dit zal normaal nooit gebeuren...

					Trigger(EV_OPERATION_SHUTDOWN);
				}
			}

		} return 0;

		// Event is het antwoord op de modeless dialog box van de ShowDialog_AreYouSure( ) 
		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);
				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{
					Trigger(EV_SIGNAL_TASK_DONE);
			
				} break;

				case EV_OPERATION_USER_YES:
				{					
					Trigger(EV_OPERATION_SHUTDOWN);					

				} break;							
			}

		} return 0;

		case Q_EXIT_SIG:
		{			

		} return 0;

		case EV_OPERATION_SHUTDOWN:
		{
			// Moeten dit event hier overschrijven.
			// Indien niet overschrijven wordt gevraagd of we willen afsluiten indien
			// niet dan kunnen we toch verder doen...

			if (AfxGetMainWnd() != NULL)
			{						
				AfxGetMainWnd()->DestroyWindow();			
			}

		} return 0;

/*
		case EV_SIGNAL_TASK_DONE:
		{

		} break;
*/

	}
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online_Busy);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Error(QEvent const *e)
 *   
 * Online : Error occurred during a task.
 *			The user is notified via the CDlgApplicationTask of this error 
 *			To leave this state the user must confirm the error via the "error" btn.
 *			
 *
 * Q_ENTRY_SIG: - Show CDlgApplicationTask 			
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy();
 *
 * EV_OPERATION_TASKDIALOG_USERBUTTON: The user confirms the error 
 *										- if (Is_TryAgain)
 *										 + Yes: Transition to Installation_OnlineBusy_HistoryState
 *											= Task current currently busy ...
 *										 + No: Transition to Installation_Online_SafeRestore
 *											= CQHSMApplicationGlobal::NewApplication
 *											= CQHSMApplicationGlobal::Installation_Online_Idle
 *											
 * Parent: CQHSMApplicationGlobal::Installation_Online
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Error(QEvent const *e)
{
	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online_Error, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance() ;
			if (pDialog != 0)
			{
				CApplicationDlgAppearance Appearance("Error Happened",
													 "Error Happened - Try again ?",
													 TASK_PROGRESSBAR_ERROR,
													 TASK_BUTTON_ERROR);

				pDialog->SetModeTaskStatus(&Appearance);
			}
		} return 0;

		case Q_EXIT_SIG:
		{
			CDlgMainApplicationTask::Destroy();

		} return 0;

		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);
				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{
					Q_TRAN_DYN(Installation_Online_SafeRestore);	

				} break;

				case EV_OPERATION_USER_YES:
				{					
					Q_TRAN_DYN(Installation_OnlineBusy_HistoryState);					

				} break;							
			}
		} return 0;

		case EV_OPERATION_TASKDIALOG_USERBUTTON:
		{
			//CDlgApplicationTask::Destroy();		// NEW TM 20060725

			CDlgMainApplicationTask::Destroy();

			if (ShowDialog_TryAgain() == FALSE)
			{
				Q_TRAN_DYN(Installation_Online_SafeRestore);
			}

		} return 0;

		case EV_SIGNAL_COMMUNICATION_NOTIFY_ACK:
		{
			// TM 20070720 
		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online);
}



/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Online_Cancelled(QEvent const *e)
 *   
 * Online : The user cancelled the current task
 *			The user is notified via the CDlgApplicationTask.(**)
 *			To leave this state the user must confirm the error via the "Done" btn.
 *			(**) When the Task is cancelled correctly the button displays 'Done'.
 *
 * Q_ENTRY_SIG: - Show CDlgApplicationTask 			
 * Q_EXIT_SIG: - CDlgApplicationTask::Destroy();
 *
 * EV_OPERATION_TASKDIALOG_USERBUTTON: The user confirms  
 *										- if (Is_TryAgain)
 *										 + Yes: Transition to Installation_OnlineBusy_HistoryState
 *											= Task current currently busy ...
 *										 + No: Transition to Installation_Online_SafeRestore
 *											= CQHSMApplicationGlobal::NewApplication
 *											= CQHSMApplicationGlobal::Installation_Online_Idle
 *
 * EV_USER_CANCEL : MAY NOT BE OVERRIDEN !!!!!
 *
 *
 * Parent: CQHSMApplicationGlobal::Installation_Online
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Online_Cancelled(QEvent const *e)
{
	if (1)
	{
		CString szDebug;
		szDebug.Format("CQHSMApplicationGlobal::Installation_Online_Cancelled, Event %d\n",e->sig);
		TRACER_FSM((szDebug));
	}

	switch (e->sig) 
	{
		TRACER_FSM(("CQHSMApplicationGlobal::Installation_Online_Cancelled\n"));

		case Q_ENTRY_SIG:
		{
			CDlgMainApplicationTask* const pDialog = CDlgMainApplicationTask::GetInstance() ;
			if (pDialog != 0)
			{
				CApplicationDlgAppearance Appearance("Cancelled by user",
													 "Action cancelled by user",
													 TASK_PROGRESSBAR_ERROR,
													 TASK_BUTTON_CANCEL);

				pDialog->SetModeTaskStatus(&Appearance);
			}

		} return 0;

		case Q_EXIT_SIG:
		{
			CDlgMainApplicationTask::Destroy();				
		} return 0;

		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			const QMyEvent* const pMyEvent = static_cast<const QMyEvent*> (e);				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{
					Q_TRAN_DYN(Installation_Online_SafeRestore);	

				} break;

				case EV_OPERATION_USER_YES:
				{					
					Q_TRAN_DYN(Installation_OnlineBusy_HistoryState);					
				} break;							
			}
		} return 0;

		case EV_OPERATION_TASKDIALOG_USERBUTTON:		// Cancelled by user ...
		{
			CDlgMainApplicationTask::Destroy();

			if (ShowDialog_TryAgain() == FALSE)
			{
				Q_TRAN_DYN(Installation_Online_SafeRestore);
			}		
		} return 0;

		case EV_SIGNAL_TASK_CANCEL:
		{
			// MUST BE LEFTT BLANK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		} break;

		case EV_SIGNAL_COMMUNICATION_NOTIFY_ACK:
		{
			// TM 20070720 
		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Online);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Offline(QEvent const *e)
 *
 * Disconnected from the communication port.
 *
 * Q_ENTRY_SIG: Future: Notify main application disonnected from com-Port	
 * Q_EXIT_SIG
 * Q_INIT_SIG: Initial transition to:
 *					CQHSMApplicationGlobal::Installation_Offline_Idle
 *
 * EV_OPERATION_COMMUNICATION_ONLINE: Operation : Go_Online()
 * EV_SIGNAL_COMMUNICATION_ONLINE: Signal confirms connected to the serial port.
 *								   - Transition to:
 *										CQHSMApplicationGlobal::Installation_Online
 *				
 * Parent: CQHSMApplicationGlobal::Installation
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Offline(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Offline\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{				
		} return 0;

		case Q_EXIT_SIG:
		{			
		} return 0;

		case Q_INIT_SIG:
		{			
			Q_INIT(&CQHSMApplicationGlobal::Installation_Offline_Idle);

		} return 0;

		case EV_OPERATION_COMMUNICATION_ONLINE:
		{
			// Sedert versie 0x0800
			// Indien een licence activatie code nodig is moet de problem solve wizard getoond worden.
			// Zolang de code niet ok is kan niet online gegaan worden...

			if (TRUE==CAppLicActCodeAPI::IsLicenseNeeded())
			{
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Offline_HandleLicActCodeNeeded);	
			}			
			else
			{
				Go_Online();
			}
		} return 0;

		case EV_SIGNAL_COMMUNICATION_OFFLINE:
		case EV_OPERATION_COMMUNICATION_OFFLINE:
		{
			// Absorbe ...
		} return 0;
		
		case EV_SIGNAL_COMMUNICATION_ONLINE:
		{				
			Q_TRAN(&CQHSMApplicationGlobal::Installation_Online);			

		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation);
}



/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Installation_Offline_Idle(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Installation_Offline
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Installation_Offline_Idle(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Offline_Idle\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			// Sedert V16.28: Request om de volledige lijst van de node services up te daten met de gegevens 
			// gevonden in het lokaal LAN netwerk.
			// Nodig voor een correcte mapping van node address / physical address / MAC-address / IP-adres.
			// Het updaten van deze lijst zorgt ervoor dat de applicatie automatisch gegevens aangepast of ingevuld worden.
			//
			// Sedert V16.36: De View / GUI modes updaten ifv het projecvtbestand.
			// Nodig voor de configuratie van de smartbox ultra: (=DALI subbus configuratie).
			CMainFrame* const pMainWnd = (CMainFrame*) AfxGetMainWnd();
			if (NULL != pMainWnd) {
				pMainWnd->setRequestUpdateServerDeviceNodes();
				pMainWnd->updateAppStateViewModes();
			}

			// Sedert V16.42:
			// Extra info voor de units.
			// Momenteel wordt deze enkel gebruikt om een extra tekstlabel weer te geven bij de units.
			TMSetDatabaseUnitExtraInfo( );

		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Offline);
}


QSTATE CQHSMApplicationGlobal::Installation_Offline_HandleLicActCodeNeeded(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Installation_Offline_HandleLicActCodeNeeded\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			// Indien de klant op de cancel button drukt omdat deze geen activatie code
			// heeft dan zal de applicatie afsluiten.

			BOOL fResult = FALSE;
			EV_QHSM_APPLICATIONGLOBAL ev = EV_SIGNAL_TASK_DONE;

			SetLock( TRUE );
			fResult = Start_HandleLicAccCodeNeeded();
			SetLock( FALSE );
		
			if (TRUE==fResult)
			{
				Q_TRAN(&CQHSMApplicationGlobal::Installation_Offline);				
			}
			else
			{
				if (ShowDialog_AreYouSure("Exit application ?") == FALSE)
				{
					// Indien de dialoog niet kan getoond worden.
					// Dit zal normaal nooit gebeuren...

					Trigger(EV_OPERATION_SHUTDOWN);
				}
			}

		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;

		// Event is het antwoord op de modeless dialog box van de ShowDialog_AreYouSure( ) 
		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);
				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{
					// Trigger(EV_OPERATION_SHUTDOWN);

					Q_TRAN(&CQHSMApplicationGlobal::Installation_Offline);
			
				} break;

				case EV_OPERATION_USER_YES:
				{					
					// Trigger(EV_SIGNAL_TASK_DONE);	

					Trigger(EV_OPERATION_SHUTDOWN);

				} break;							
			}

		} return 0;

		case EV_OPERATION_SHUTDOWN:
		{
			// Moeten dit event hier overschrijven.
			// Indien niet overschrijven wordt gevraagd of we willen afsluiten indien
			// niet dan kunnen we toch verder doen...

			if (AfxGetMainWnd() != NULL)
			{						
				AfxGetMainWnd()->DestroyWindow();			
			}

		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{
			Q_TRAN(&CQHSMApplicationGlobal::Installation_Offline_Idle);

		} break;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Installation_Offline);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::MainApplication
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering\n"));

	switch (e->sig) 
	{
		case Q_INIT_SIG:
		{
			ASSERT(CAppState::APP_TYPE_ENGINEERING==CAppState::getInstance().getAppType());				
			Q_INIT(&CQHSMApplicationGlobal::Engineering_Normal);		

		} return 0;
		
		case Q_ENTRY_SIG:
		{			
			
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{

		} return 0;	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::MainApplication);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Normal(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Normal(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Normal\n"));

	switch (e->sig) 
	{
		case Q_INIT_SIG:
		{
			ASSERT(CAppState::APP_TYPE_ENGINEERING==CAppState::getInstance().getAppType());		

			if (Is_CommunicationOnline() == TRUE)
			{
				Q_INIT(&CQHSMApplicationGlobal::Engineering_Online);
			}
			else
			{
				Q_INIT(&CQHSMApplicationGlobal::Engineering_Offline);
			}

		} return 0;

		case Q_ENTRY_SIG:
		{
			Engineering_Online_Idle_History =
				Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Idle);		
			
		} return 0;

		case Q_EXIT_SIG:
		{
			Engineering_Normal_History = getState();

		} return 0;

		case EV_OPERATION_CONVERT_TO_BINDINGDATABASE:
		{
			CString szMessage("First Physical browse + read all bindings ...");
			ShowDialog_Info(szMessage);

		} return 0;

	}

	return ((QSTATE) &CQHSMApplicationGlobal::Engineering);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Online(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Online(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Online\n"));

	switch (e->sig) 
	{
		case Q_INIT_SIG:
		{
			Q_INIT(&CQHSMApplicationGlobal::Engineering_Online_Idle);

		} return 0;

		case Q_ENTRY_SIG:
		{
						
		} return 0;

		case Q_EXIT_SIG:
		{
			/**
			 * Going from Online --> Offline : All Tasks are destroyed by the
			 * CCommApplication Factory ...
			 */

			if (TMGetCommunicationApp() != NULL) 
			{
				TMGetCommunicationApp()->ShutDownAllTasks();
			}

		} return 0;

		case EV_OPERATION_COMMUNICATION_OFFLINE:
		{	
			TMGetCommunicationApp()->setDisconnect();					
		} return 0;

		case EV_SIGNAL_COMMUNICATION_OFFLINE:
		{
			Q_TRAN(&CQHSMApplicationGlobal::Engineering_Offline);

		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Normal);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Online_Idle(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering_Online
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Online_Idle(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Online_Idle\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
			ASSERT(m_pGenericTask);
			ASSERT(m_pCompositeTask);

			if (m_pGenericTask != NULL) 
			{
				m_pGenericTask->Clear();
			}
			if (m_pCompositeTask != NULL)
			{
				m_pCompositeTask->Clear();
			}
				
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;	

		case EV_OPERATION_BROWSE_FOR_PHNODES:
		{
			if (CQHSMTaskBuilder::Build_ExtendedBrowseAllPhNodes(m_pCompositeTask) == TRUE)
			{
				if (m_pGenericTask != NULL)
				{
					CQHSMGenericTask::Copy(m_pGenericTask,m_pCompositeTask->Begin());

					Engineering_Online_Idle_History =
						Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Idle_Ready);	// CHANGES_MVS_2008

					m_fInitialiseDatabases = TRUE;	// !!!!

					Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_CompositeTask);

					return 0;
				}
			}
		} return 0;

		case EV_OPERATION_REQUEST_FOR_STATES:
		{
			if (CQHSMTaskBuilder::Build_RequestForStates(m_pGenericTask) != FALSE) 
			{						
				Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask);
			}
		} return 0;
		
		case EV_OPERATION_AUTOMATIC_PNP:
		{
			if (CQHSMTaskBuilder::Build_AutomaticPlugAndPlay(m_pGenericTask) != FALSE) 
			{			
				Engineering_Online_Idle_History =
						Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Idle);	// CHANGES_MVS_2008

				Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask);
			}
		} return 0;	

		case EV_OPERATION_DOWNLOAD_BINDINGS:
		{
			CString szMessage("Not Ready: First physical browse network");
			ShowDialog_Info(szMessage);

		} return 0;

		case EV_OPERATION_UPLOAD_ALLBINDINGS:
		{
			CString szMessage("Not Ready: First physical browse network");
			ShowDialog_Info(szMessage);
		} return 0;
	
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Online);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Online_Idle_Ready(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering_Online_Idle
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Online_Idle_Ready(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Online_Idle_Ready\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{			
			if (m_fInitialiseDatabases == TRUE)
			{				
				SetLock( TRUE );

				m_pParent->ClearScrollView();

				TMGetNodeDatabaseApp()->CopyRuntimeToDocDatabase();
				TMCreateNewBindingFiles();
				TMCreateUnitListDelegation();
				TMCreateUnitListRequestMeasuredValues();	// sedert versie 0x0C03

				// Sedert V16.00: USE_SENSOR_PIDCONTROL	
				// Een lijst aanmaken van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
				// Controleren of alle units die gekoppeld werden als PID control unit nog aanwezig zijn.
				// Indien er units gekoppeld werden die niet meer beschikbaar zijn de P-Binding verwijderen.
				TMCreateUnitDimListPIDControl();

				SetLock( FALSE );
			
				m_fInitialiseDatabases = FALSE;
			}

		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;	

		case EV_OPERATION_DOWNLOAD_BINDINGS:
		{
			if (CQHSMTaskBuilder::Build_DownloadBindings(m_pGenericTask) != FALSE) 
			{			
				Engineering_Online_Idle_History =
						Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Idle_Ready);	// CHANGES_MVS_2008

				Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask);
			}
		} return 0;

		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);
				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{						
					// Absorbe ...

				} break;

				case EV_OPERATION_USER_YES:
				{					
					if (CQHSMTaskBuilder::Build_UploadAllBindings(m_pGenericTask) != FALSE) 
					{			
						Engineering_Online_Idle_History =
								Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Idle_Ready);	// CHANGES_MVS_2008

						Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask);
					}				
				} break;
			}						
		} return 0;

		case EV_OPERATION_UPLOAD_ALLBINDINGS:
		{
			ShowDialog_AreYouSure("All nodes in the network will lose their programmation");

		} return 0;

		/*
		case EV_OPERATION_UPLOAD_ALLBINDINGS:
		{
			if (Is_AreYouSure("All nodes in the network will lose their programmation") == TRUE)
			{
				if (CQHSMTaskBuilder::Build_UploadAllBindings(m_pGenericTask) != FALSE) 
				{			
					Engineering_Online_Idle_History =
							Q_STATIC_CAST(QState, CQHSMApplicationGlobal::Engineering_Online_Idle_Ready);

					Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask);
				}
			}
		} return 0;
		*/

		case EV_OPERATION_CONVERT_TO_BINDINGDATABASE:
		{
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);

			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_SHOW:
				{
					CDlgConversionFromNodes::Show();
					Q_TRAN(&CQHSMApplicationGlobal::Engineering_Tools_ConvertToDatabase);	// CHANGES_MVS_2008

				} break;

				case EV_SIGNAL_DESTROY:
				case EV_OPERATION_START:
				case EV_OPERATION_CANCEL:
				case EV_OPERATION_CLEAR:
				{
					
				} break;			
			}

		} return 0;
	}
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Online_Idle);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Online_Busy(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering_Online
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Online_Busy(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Online_Busy\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
					
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;

		case EV_SIGNAL_TASK_ERROR:
		{
			Engineering_Online_Idle_History =
					Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Idle);	// CHANGES_MVS_2008

			Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Idle);	
		
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{
			GenericTask_Invoke_DoneNotify(m_pGenericTask);

			Q_TRAN_DYN( Engineering_Online_Idle_History );

		} return 0;

		case EV_OPERATION_TASKDIALOG_USERBUTTON:
		{
			GenericTask_Invoke_Cancel(m_pGenericTask);
			
		} return 0;

		case EV_SIGNAL_TASK_CANCEL:
		{
			GenericTask_Invoke_CancelNotify(m_pGenericTask);

			Engineering_Online_Idle_History =
					Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Idle);	// CHANGES_MVS_2008

			Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Idle);

		} return 0;

		case EV_SIGNAL_COMMUNICATION_NOTIFY_ACK:
		{
			if (CDlgMainApplicationTask::GetInstance() != 0)
			{
				CDlgMainApplicationTask::GetInstance()->ShowProgressStepUp();
			}

		} return 0;

		case EV_OPERATION_CONVERT_TO_BINDINGDATABASE:
		{
			CString szMessage("Application is currently busy - cannot convert to binding database");
			ShowDialog_Info(szMessage);

		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Online);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering_Online_Busy
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{			
			m_pParent->ClearScrollView();

			if (GenericTask_Invoke_Start(m_pGenericTask) == FALSE)
			{
				Trigger( EV_SIGNAL_TASK_ERROR );
			}		

		} return 0;

		case Q_EXIT_SIG:
		{
			CDlgMainApplicationTask::Destroy();

		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{
			GenericTask_Invoke_DoneNotify(m_pGenericTask);
			
		} break;	// Parent Transition : Busy state --> Idle ...

		
		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);
				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{
					Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy);	
					Trigger(EV_SIGNAL_TASK_ERROR);
			
				} break;

				case EV_OPERATION_USER_YES:
				{					
					Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_GenericTask);					

				} break;							
			}
		} return 0;

		case EV_SIGNAL_TASK_ERROR:			
		{
			GenericTask_Invoke_ErrorNotify(m_pGenericTask);

			if (ShowDialog_TryAgain() == TRUE)
			{
				return 0;
			}

		} break;
	}
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Online_Busy);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Online_Busy_CompositeTask(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering_Online_Busy
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Online_Busy_CompositeTask(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Online_Busy_CompositeTask\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{			
			//Engineering_Online_Busy_History =
			//	Q_STATIC_CAST(QState, &CQHSMApplicationGlobal::Engineering_Online_Busy_CompositeTask);

			m_pParent->ClearScrollView();

			if (GenericTask_Invoke_Start(m_pGenericTask) == FALSE)
			{
				Trigger( EV_SIGNAL_TASK_ERROR );
			}		
		} return 0;

		case Q_EXIT_SIG:
		{
			CDlgMainApplicationTask::Destroy();

		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{
			GenericTask_Invoke_DoneNotify(m_pGenericTask);

			if (m_pCompositeTask != NULL)
			{
				CQHSMGenericTask *const pTask = m_pCompositeTask->Next();
				if (pTask != NULL)
				{
					m_pParent->ClearScrollView();

					CQHSMGenericTask::Copy(m_pGenericTask,pTask);						
					if (GenericTask_Invoke_Start(m_pGenericTask) == FALSE)
					{
						Trigger( EV_SIGNAL_TASK_ERROR );
					}		
					return 0;
				}						
			}
			else
			{
				Trigger( EV_SIGNAL_TASK_ERROR);
				return 0;
			}
		} break;	// No Task Left --> Leave event for parent ...

		case EV_OPERATION_MESSAGEBOX_RESPONSE:
		{		
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);
				
			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_USER_NO:
				{
					Q_TRAN_DYN(&CQHSMApplicationGlobal::Engineering_Online_Busy);	
					Trigger(EV_SIGNAL_TASK_ERROR);
			
				} break;

				case EV_OPERATION_USER_YES:
				{					
					Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_CompositeTask);					

				} break;							
			}
		} return 0;

		case EV_SIGNAL_TASK_ERROR:			
		{
			GenericTask_Invoke_ErrorNotify(m_pGenericTask);

			if (ShowDialog_TryAgain() == TRUE)
			{
				return 0;
			}
		} break;

/*
		case EV_SIGNAL_TASK_ERROR:			
		{
			GenericTask_Invoke_ErrorNotify(m_pGenericTask);
	
			if (Is_TryAgain() == TRUE)
			{
				Q_TRAN(&CQHSMApplicationGlobal::Engineering_Online_Busy_CompositeTask);
				return 0;
			}

		} break;
*/
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Online_Busy);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Offline(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Offline(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Offline\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
				
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;

		case EV_SIGNAL_COMMUNICATION_ONLINE:
		{
			Q_TRAN_DYN(Engineering_Online_Idle_History);

		} return 0;

		case EV_OPERATION_COMMUNICATION_ONLINE:
		{						
			TMGetCommunicationApp()->setSerialUsbConnect(-1);		// Standaard USb Poort		
		} return 0;		

		case EV_OPERATION_DOCUMENT:
		{
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);

			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_LOAD:
				{
					CStringArray strArrayErrors;

					SetLock( TRUE );
					
					if (m_pParent->LoadDocumentFile(&strArrayErrors) == TRUE) 
					{
#ifdef SKIP
						// Sedert versie 0x0902:
						// Controle van de binding databases & evt reconversie.
						m_pParent->VerifyDocumentBindingDatabases(&strArrayErrors);
#endif

						SetLock( FALSE );
						Q_TRAN(&CQHSMApplicationGlobal::Engineering_Offline_Ready);
					}		
					else
					{
						//CHANGES_INCOMPATIBLE_VERSION

						//TODO - Low prio.
						//Engineering mode - Opvangen van incompatible versie.
						//Zal bijna nooit voorkomen...
					}
					SetLock( FALSE );
				} break;
			}
		} return 0;

		case EV_OPERATION_CONVERT_TO_BINDINGDATABASE:
		{
			CString s("First import *.duo file + bindingfiles");
			ShowDialog_Info(s);

		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Normal);
}

QSTATE CQHSMApplicationGlobal::Engineering_Offline_Ready(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Offline_Ready\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
				
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;

		case EV_OPERATION_CONVERT_TO_BINDINGDATABASE:
		{
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);

			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_SHOW:
				{
					CDlgConversionFromNodes::Show();
					Q_TRAN(&CQHSMApplicationGlobal::Engineering_Tools_ConvertToDatabase);	// CHANGES_MVS_2008

				} break;

				case EV_SIGNAL_DESTROY:
				case EV_OPERATION_START:
				case EV_OPERATION_CANCEL:
				case EV_OPERATION_CLEAR:
				{
					
				} break;			
			}

		} return 0;
	}
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Offline);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Tools(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Engineering_Tools(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Tools\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
				
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		case EV_SIGNAL_TASK_CANCEL:
		case EV_SIGNAL_TASK_ERROR:
		{
			// Absorbe ...

		} return 0;
	
		case EV_OPERATION_COMMUNICATION_ONLINE:
		case EV_OPERATION_COMMUNICATION_OFFLINE:
		case EV_OPERATION_BROWSE_FOR_PHNODES:
		case EV_OPERATION_REQUEST_FOR_STATES:
		case EV_OPERATION_AUTOMATIC_PNP:
		case EV_OPERATION_UPLOAD_ALLBINDINGS:
		case EV_OPERATION_DOWNLOAD_BINDINGS:
		{
			CString szMessage("Not allowed - Busy executing tool");
			ShowDialog_Info(szMessage);

		} return 0;
		
	}
	return ((QSTATE) &CQHSMApplicationGlobal::Engineering);
}


/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Engineering_Tools_ConvertToDatabase(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::Engineering_Tools
 *
 **************************************************************************************************/

QSTATE CQHSMApplicationGlobal::Engineering_Tools_ConvertToDatabase(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Engineering_Tools_ConvertToDatabase\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
				
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;

		case EV_OPERATION_CONVERT_TO_BINDINGDATABASE:
		{
			QMyEvent const* pMyEvent = static_cast<QMyEvent const*> (e);

			switch(pMyEvent->lParam)
			{
				case EV_OPERATION_SHOW:
				{									
					CDlgConversionFromNodes::Show();				
				} break;

				case EV_SIGNAL_DESTROY:
				{
					Q_TRAN_DYN( Engineering_Normal_History );

				} break;

				case EV_OPERATION_START :
				{					
					CQHSMBindingConversionFromNodes::GetInstance()->EvOperationStart();

				} break;

				case EV_OPERATION_CANCEL :
				{
					CQHSMBindingConversionFromNodes::GetInstance()->EvOperationCancel();

				} break;

				case EV_OPERATION_CLEAR :
				{
					CQHSMBindingConversionFromNodes::GetInstance()->EvOperationClear();

				} break;
			}

		} return 0;
	}

	return ((QSTATE) &CQHSMApplicationGlobal::Engineering_Tools);
}

/**************************************************************************************************
 *
 * QSTATE CQHSMApplicationGlobal::Simulation(QEvent const *e)
 *
 *
 *
 * Parent: CQHSMApplicationGlobal::MainApplication
 *
 **************************************************************************************************/
QSTATE CQHSMApplicationGlobal::Simulation(QEvent const *e)
{
	TRACER_FSM(("CQHSMApplicationGlobal::Simulation\n"));

	switch (e->sig) 
	{
		case Q_ENTRY_SIG:
		{
						
		} return 0;

		case Q_EXIT_SIG:
		{
			
		} return 0;
    }
	return ((QSTATE) &CQHSMApplicationGlobal::MainApplication);
}

/**************************************************************************************************/
BOOL CQHSMApplicationGlobal::GenericTask_Invoke_Start(CQHSMGenericTask* m_pTask)
{
	if (m_pTask != NULL) {
		return (m_pTask->Start(this));
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::GenericTask_Invoke_Cancel(CQHSMGenericTask* m_pTask)
{
	if (m_pTask != NULL) {
		return (m_pTask->Cancel(this));
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::GenericTask_Invoke_ErrorNotify(CQHSMGenericTask* m_pTask)
{
	if (m_pTask != NULL) {
		return (m_pTask->NotifyOnError(this));
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::GenericTask_Invoke_DoneNotify(CQHSMGenericTask* m_pTask)
{
	if (m_pTask != NULL) {
		return (m_pTask->NotifyOnDone(this));
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::GenericTask_Invoke_CancelNotify(CQHSMGenericTask* m_pTask)
{
	if (m_pTask != NULL) {
		return (m_pTask->NotifyOnCancel(this));
	}
	return FALSE;
}

/**************************************************************************************************/
/**
 * Physical Browse For Nodes ...
 */
BOOL CQHSMApplicationGlobal::Start_BrowseAllPhysicalNodes()
{
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->BrowseAllPhysicalNodes();
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_BrowseAllPhysicalNodes() 
{
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->CancelBrowsePnp();
		return TRUE;
	}
	return FALSE;
}

/**
 * Browse For Nodes: Single node.
 */
BOOL CQHSMApplicationGlobal::Start_BrowseSingleNode() {
	if (TMGetCommunicationApp() == NULL) {
		return FALSE;
	}

	// NOTE_BROWSE_SINGLE_NODE_ADDRESS
	// Doen hier geen controle op de MODE_BROWSE_SINGLE_NODE want deze is mogelijk al op IDLE geplaatst 
	// door de user cancel of een fout.
	// Node Address zou nog moeten ok zijn.
	AppCfgBrowseInfo& refInfo = AppCfgBrowseInfo::getInstance();
	const int nodeAddress = refInfo.getNodeAddress();
	if (nodeAddress < 0x03 || nodeAddress > 0xfc) {
		ASSERT(nodeAddress>=0x03 && nodeAddress <= 0xfc);
		return FALSE;
	}	

	// Moeten deze info opnieuw herstellen want na een cancel/error wordt de mode gewijzigd.
	refInfo.setMode(AppCfgBrowseInfo::MODE_BROWSE_SINGLE_NODE);

	// Start algoritme.
	TMGetCommunicationApp()->BrowseSingleNode(nodeAddress);
	return TRUE;
}

BOOL CQHSMApplicationGlobal::Cancel_BrowseSingleNode() {
	if (TMGetCommunicationApp() == NULL) {
		return FALSE;
	}
	TMGetCommunicationApp()->CancelBrowsePnp();
	return TRUE;
}

/**
 * Request for states
 */
BOOL CQHSMApplicationGlobal::Start_RequestForStates() {
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->Start_RequestForStatussen();
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Start_RequestForStatesSingleNode() {
	if (TMGetCommunicationApp() == NULL) {
		return FALSE;
	}
	AppCfgBrowseInfo& refInfo = AppCfgBrowseInfo::getInstance();
	const int nodeAddress = refInfo.getNodeAddress();
	if (nodeAddress < 0x03 || nodeAddress > 0xfc) {
		ASSERT(nodeAddress>=0x03 && nodeAddress <= 0xfc);
		return FALSE;
	}	
	TMGetCommunicationApp()->Start_RequestForStatussen(nodeAddress);
	return TRUE;
}

BOOL CQHSMApplicationGlobal::Cancel_RequestForStates() {
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->Cancel_RequestForStatussen();
		return TRUE;
	}
	return FALSE;
}

void CQHSMApplicationGlobal::Notify_OnTaskRequestForStatesDone() {
	m_pParent->ClearScrollView();
	m_pParent->DisplayScrollMessage("Request for node statussen done");
}

void CQHSMApplicationGlobal::Notify_OnTaskRequestForStatesCancelled() {
	m_pParent->ClearScrollView();
	m_pParent->DisplayScrollMessage("Request for node statussen cancelled");
}

/**
 * Automatic Pnp
 */
BOOL CQHSMApplicationGlobal::Start_AutomaticPnp()
{
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->StartPnP();
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_AutomaticPnp()
{
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->CancelBrowsePnp();
		return TRUE;
	}
	return FALSE;
}

/**
 * Read bindings
 */
BOOL CQHSMApplicationGlobal::Start_DownloadBindings()
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_ReadAllBindings();
		return TRUE;
	}
	return FALSE;
}

/**
 * Write Bindings ...
 */
BOOL CQHSMApplicationGlobal::Start_WriteAllBindings()
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_WriteAllBindings();
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Start_WriteModifiedBindings()
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_WriteModifiedBindings();
		return TRUE;
	}
	return FALSE;
}

void CQHSMApplicationGlobal::Notify_OnTaskWriteBindingsDone()
{
	if (m_pParent != 0) 
	{
		m_pParent->TaskSetDateToMasterLcd();		
	}
}

/**
 * Upload Screenmaps ...
 */
BOOL CQHSMApplicationGlobal::Start_UploadLcdScreenmaps()
{	
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadLcdScreenmaps();
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadLcdScreenmaps()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadLcdScreenmaps();
		return TRUE;
	}
	return FALSE;
}

void CQHSMApplicationGlobal::Notify_OnTaskUploadScreenmapsDone()
{
	CNodeLCDScreenmapFSMInfo* const ptr = CNodeLCDScreenmapFSMInfo::GetInstance( );

	if (ptr)
	{	
		ptr->DumpInfo();
		ptr->SetLock(FALSE);		
	}	
}

void CQHSMApplicationGlobal::Notify_OnTaskUploadScreenmapsError()
{	
	CNodeLCDScreenmapFSMInfo* const ptr = CNodeLCDScreenmapFSMInfo::GetInstance( );

	if (ptr)
	{
		ptr->DumpInfo();
		ptr->SetLock(FALSE);	
	}	
}

void CQHSMApplicationGlobal::Notify_OnTaskUploadScreenmapsCancelled()
{
	CNodeLCDScreenmapFSMInfo* const ptr = CNodeLCDScreenmapFSMInfo::GetInstance( );

	if (ptr)
	{
		ptr->DumpInfo();
		ptr->SetLock(FALSE);		
	}	
}

/**
 * Upload Multimedia config
 */
BOOL CQHSMApplicationGlobal::Start_UploadMultimMediaConfig()
{	
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadMultiMediaConfig();
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadMultimMediaConfig()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadMultiMediaConfig();
		return TRUE;
	}
	return FALSE;
}

/**
 * ftp config, since v0562.
 */
BOOL CQHSMApplicationGlobal::Start_ftpConfig( )		// since v0562
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_ftpConfig();
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_ftpConfig( )	// since v0562
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_ftpConfig( );
		return TRUE;
	}
	return FALSE;
}



/**
 * Upload Units config
 */
BOOL CQHSMApplicationGlobal::Start_UploadUnitsConfig()
{	
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadUnitCreateConfig( );
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadUnitsConfig()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadUnitCreateConfig();
		return TRUE;
	}
	return FALSE;
}


/**
 * Upload Moods config
 */
BOOL CQHSMApplicationGlobal::Start_UploadMoodConfig( )
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadMoodConfig( );
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadMoodConfig( )
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadMoodConfig();
		return TRUE;
	}
	return FALSE;
}


/**
 * Upload irrx config
 */
BOOL CQHSMApplicationGlobal::Start_UploadIRRXConfig( )
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadIRRXConfig( );
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadIRRXConfig( )
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadIRRXConfig();
		return TRUE;
	}
	return FALSE;
}

/**
 * Upload Gateway config
 */
BOOL CQHSMApplicationGlobal::Start_UploadGatewayCfg()
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadGatewayCfg( );
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_UploadGatewayCfg()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadGatewayCfg();
		return TRUE;
	}
	return FALSE;
}

/**
 * Upload RoomController audio cfg
 */
BOOL CQHSMApplicationGlobal::Start_UploadRoomCtrlAudioCfg()
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadRoomCtrlAudioCfg( );
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_UploadRoomCtrlAudioCfg()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadRoomCtrlAudioCfg();
		return TRUE;
	}
	return FALSE;
}

/**
 * Upload Bose Soundtouch audio cfg
 */
BOOL CQHSMApplicationGlobal::Start_UploadBoseSTCfg()		// since v0802
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadBoseSTConfig( );
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_UploadBoseSTCfg()		// since v0802
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadBoseSTConfig();
		return TRUE;
	}
	return FALSE;
}

/**
 * Upload Leds Cfg
 */
BOOL CQHSMApplicationGlobal::Start_UploadLedsCfg()		// since v0804
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadLedsConfig( );
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_UploadLedsCfg()		// since v0804
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadLedsConfig();
		return TRUE;
	}
	return FALSE;
}


BOOL CQHSMApplicationGlobal::Start_UploadSmapLiteCfg()		// since v0C05
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadSmapLiteCfg( );
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_UploadSmapLiteCfg()	// since v0C05
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadSmapLiteCfg();
		return TRUE;
	}
	return FALSE;
}


/**
 * Upload schedule config
 */
BOOL CQHSMApplicationGlobal::Start_UploadScheduleConfig( )
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadScheduleConfig( );
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadScheduleConfig( )
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadScheduleConfig();
		return TRUE;
	}
	return FALSE;
}


/**
 * Upload smap pageInfo
 */
BOOL CQHSMApplicationGlobal::Start_UploadSmapPageInfo( )
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_uploadSmapPageInfo( );
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadSmapPageInfo( )
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_uploadSmapPageInfo();
		return TRUE;
	}
	return FALSE;
}

/**
 * Upload smap menu
 */
BOOL CQHSMApplicationGlobal::Start_UploadSmapMenu( )
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadSmapMenu( );
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_UploadSmapMenu( )
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadSmapMenu();
		return TRUE;
	}
	return FALSE;
}

/**
 * Upload smap AVRoom
 */
BOOL CQHSMApplicationGlobal::Start_UploadSmapAVRoom( )
{
	if (TMGetCommunicationApp() != NULL) 
	{		
		TMGetCommunicationApp()->Start_UploadSmapAVRoom( );
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_UploadSmapAVRoom( )
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_UploadSmapAVRoom();
		return TRUE;
	}
	return FALSE;
}

/** general task */
void CQHSMApplicationGlobal::Notify_OnTaskConfigDone()
{
	m_pParent->ClearScrollView();
	m_pParent->DisplayScrollMessage("configuration Done ...");
			
	DUOTECNO::PLUGIN::IConfigToolApp::getInstance().refreshDisplay( );	// since v0x0565
}
void CQHSMApplicationGlobal::Notify_OnTaskConfigError()
{
	m_pParent->DisplayScrollMessage("configuration Error ...");
		
	DUOTECNO::PLUGIN::IConfigToolApp::getInstance().refreshDisplay( );	// since v0x0565
}
void CQHSMApplicationGlobal::Notify_OnTaskConfigCancelled()
{
	m_pParent->ClearScrollView();
	m_pParent->DisplayScrollMessage("configuration Cancelled by user...");
			
	DUOTECNO::PLUGIN::IConfigToolApp::getInstance().refreshDisplay( );	// since v0x0565
}

// Sedert V16.43: Na het doorsturen van de unit config een bericht sturen naar de parent
// window. Deze vraagt of er moet gebrowsed worden op de node.
void CQHSMApplicationGlobal::Notify_OnTaskUploadUnitsDone() {

	if (NULL == this->m_pParent) {
		INVARIANT(NULL != this->m_pParent);
		return;
	}

	this->m_pParent->ClearScrollView();

	CString strMessage;
	const int nodeAddress = CConfigUnitCreateInfo::getInstance().getNodeAddress();
	strMessage.Format("Uploading node 0x%02x unit configuration done...",nodeAddress);	
	this->m_pParent->DisplayScrollMessage(strMessage);			

	DUOTECNO::PLUGIN::IConfigToolApp::getInstance().refreshDisplay( );	// since v0x0565

	this->m_pParent->PostMessage(WM_APPLICATION_UNITCONFIG_DONE,nodeAddress);
}


/** Smap Lite Cfg */

// Sedert V15.02.
// Indien nodig (Smarttouch) wordt na het doorsturen van de configuratie een melding gegeven
// dat de instellingen pas werken na een update nodedatabase van de master.
void CQHSMApplicationGlobal::Notify_OnTaskSmapLiteCfgDone()
{	
	CAppCfgSmapLiteInfo& configInfo = CAppCfgSmapLiteInfo::getInstance();
	if (TRUE==configInfo.getUpdateDatabaseNeeded()) {
		m_pParent->PostMessage(WM_APPLICATION_USERMESSAGE,USERMESSAGE_UPDATE_DATABASE_NEEDED);
	}
	this->Notify_OnTaskConfigDone();
}

/** Sedert 0x0C05: AV Room Ctrl van de smarttouch */
// Er wordt na het doorsturen van de configuratie een melding gegeven
// dat de instellingen pas werken na een update nodedatabase van de master.
void CQHSMApplicationGlobal::Notify_OnTaskUploadRoomCtrlAudioCfg()
{
	m_pParent->PostMessage(WM_APPLICATION_USERMESSAGE, USERMESSAGE_UPDATE_DATABASE_NEEDED);
	this->Notify_OnTaskConfigDone();
}

// Oppassen: NOTE_BROWSE_SINGLE_NODE_ADDRESS
// Mogelijk staat AppCfgBrowseInfo niet meet in de mode 'MODE_BROWSE_SINGLE_NODE' 
// omdat we reeds de toestand verlaten hebben - mogen het Node address niet veranderen
// bij de exit.
void CQHSMApplicationGlobal::Notify_OnTaskBrowseSingleNodeFailed() {
	// De runtime node opnieuw herstellen met de properties en units van de document node.
	m_pParent->ClearScrollView();
	m_pParent->DisplayScrollMessage("Browse single node cancelled/failed: Restore network node.");
	AppCfgBrowseInfo& refAppcfgBrowsenode = AppCfgBrowseInfo::getInstance();	
	TMRestoreRuntimeNode(refAppcfgBrowsenode.getNodeAddress());								
}

/**
 * Memory Dump
 */
BOOL CQHSMApplicationGlobal::Start_UploadMemory()
{	
	if (TMGetCommunicationApp() != NULL) 
	{			
		if (TMGetCommunicationApp()->Start_MemoryDump())
			CMemoryDumpInfo::GetInstance()->SetLock(TRUE);		// TM 20070524

		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadMemory()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_MemoryDump();
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Start_DownloadMemory()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		if (TMGetCommunicationApp()->Start_MemoryDump())
			CMemoryDumpInfo::GetInstance()->SetLock(TRUE);		// TM 20070524

		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_DownloadMemory()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_MemoryDump();
		return TRUE;
	}
	return FALSE;
}

void CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpDone()
{
	m_pParent->ClearScrollView();
	m_pParent->DisplayScrollMessage("Memory dump Done ...");

	m_pParent->StoreMemoryDumpInfo();

	CMemoryBitmapInfo::GetInstance()->SetLock(FALSE);
	CMemoryDumpInfo::GetInstance()->SetLock(FALSE);
}

void CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpError()
{
	m_pParent->ClearScrollView();
	m_pParent->DisplayScrollMessage("Memory dump Error ...");

	m_pParent->StoreMemoryDumpInfo();
	CMemoryBitmapInfo::GetInstance()->SetLock(FALSE);
	CMemoryDumpInfo::GetInstance()->SetLock(FALSE);
}

void CQHSMApplicationGlobal::Notify_OnTaskMemoryDumpCancelled()
{
	m_pParent->ClearScrollView();
	m_pParent->DisplayScrollMessage("Memory dump Cancelled ...");

	m_pParent->StoreMemoryDumpInfo();
	CMemoryBitmapInfo::GetInstance()->SetLock(FALSE);
	CMemoryDumpInfo::GetInstance()->SetLock(FALSE);
}



/**
 * Upload bitmaps ...
 */
BOOL CQHSMApplicationGlobal::Start_UploadBitmaps()
{
	if (TMGetCommunicationApp() != NULL) 
	{	
		if (TMGetCommunicationApp()->Start_UploadBitmaps())
			CMemoryBitmapInfo::GetInstance()->SetLock(TRUE);		// TM 20070524

		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_UploadBitmaps()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_MemoryDump();
		return TRUE;
	}
	return FALSE;
}

void CQHSMApplicationGlobal::Notify_OnTaskUploadBitmapsDone()
{	
	CMemoryBitmapInfo::GetInstance()->SetLock(FALSE);
	CMemoryDumpInfo::GetInstance()->SetLock(FALSE);
			
	DUOTECNO::PLUGIN::IConfigToolApp::getInstance().refreshDisplay( );	// since v0x0565
}

void CQHSMApplicationGlobal::Notify_OnTaskUploadBitmapsError()
{
	CMemoryBitmapInfo::GetInstance()->SetLock(FALSE);
	CMemoryDumpInfo::GetInstance()->SetLock(FALSE);
			
	DUOTECNO::PLUGIN::IConfigToolApp::getInstance().refreshDisplay( );	// since v0x0565
}

void CQHSMApplicationGlobal::Notify_OnTaskUploadBitmapsCancelled()
{
	CMemoryBitmapInfo::GetInstance()->SetLock(FALSE);
	CMemoryDumpInfo::GetInstance()->SetLock(FALSE);
			
	DUOTECNO::PLUGIN::IConfigToolApp::getInstance().refreshDisplay( );	// since v0x0565
}


BOOL CQHSMApplicationGlobal::Start_UploadAllNames()
{
	if (TMGetCommunicationApp() != NULL) 
	{	
		TMGetCommunicationApp()->Start_SendAllNames();
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Cancel_UploadAllNames() 
{
	if (TMGetCommunicationApp() != NULL) 
	{
		TMGetCommunicationApp()->Cancel_SendAllNames();
		return TRUE;
	}
	return FALSE;
}

/**
 *
 */
BOOL CQHSMApplicationGlobal::Start_NicErrorsFsm()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		return (TMGetCommunicationApp()->Start_NicErrorsFsm());	
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Cancel_NicErrorsFsm()
{
	if (TMGetCommunicationApp() != NULL) 
	{
		return (TMGetCommunicationApp()->Stop_NicErrorsFsm());	
	}
	return FALSE;
}

void CQHSMApplicationGlobal::Notify_OnTaskNicErrorsFsmDone()
{	
	m_pParent->DisplayScrollMessage("Requesting NIC-Medium statussen done ...");
}
void CQHSMApplicationGlobal::Notify_OnTaskNicErrorsFsmError()
{
	m_pParent->DisplayScrollMessage("Requesting NIC-Medium statussen error ...");
}
void CQHSMApplicationGlobal::Notify_OnTaskNicErrorsFsmCancelled()
{
	m_pParent->DisplayScrollMessage("Requesting NIC-Medium statussen user cancelled ...");
}

/**
 * Handle Pre and post Browse Errors ...
 */
inline BOOL CQHSMApplicationGlobal::Start_HandlePreBrowseError() {
	if (m_pParent != NULL)  {
		// sedert V16.47: CHANGES_UPLOAD_UNITS 
		// Applicatie taken wissen. 
		// Via de problem solve wizard kunnen er taken gevraagd worden die moeten uitgevoerd worden
		// voor het starten van de browse for nodes.
		CAppState::getInstance().getTaskEntries().clear( );

		// Tonen van de problem solve wizard.
		m_pParent->ShowProblemSolveWizard(TRUE);
		return TRUE;
	}
	return FALSE;
}

inline BOOL CQHSMApplicationGlobal::Start_HandlePostBrowseError() {
	if (m_pParent != NULL) {
		m_pParent->ShowProblemSolveWizard(TRUE);
		return TRUE;
	}
	return FALSE;
}

// Return TRUE indien FINISH knop werd gedrukt.
// Return FALSE indien de CANCEL knop werd gedrukt.
inline BOOL CQHSMApplicationGlobal::Start_HandleLicAccCodeNeeded()
{
	if (m_pParent != NULL) {
		return m_pParent->ShowProblemSolveWizard(FALSE,WIZARD_LICENCE_ACTIVATION_CODE);
	}
	return FALSE;
}


/**
 *
 */
BOOL CQHSMApplicationGlobal::Is_CommunicationOnline()
{
	if (TMGetCommunicationApp() != NULL) {
		return (TMIsConnected());
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::Go_Online()
{
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->setConnect(); //setSerialUsbConnect(-1);	// de laatst gekende usb poort
		return TRUE;
	}
	return FALSE;
}
BOOL CQHSMApplicationGlobal::Go_Offline()
{
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->setDisconnect();	
		return TRUE;
	}
	return FALSE;
}

BOOL CQHSMApplicationGlobal::CloseConnections() {
	if (TMGetCommunicationApp() != NULL) {
		TMGetCommunicationApp()->closeConnections();
		return TRUE;
	}
	return FALSE;
}


/**************************************************************************************************
 *
 * 
 *
 **************************************************************************************************
 *
 * 
 *
 **************************************************************************************************/
BOOL CQHSMApplicationGlobal::StartErrorLog()
{
	CCommunicationLogfile::GetInstance()->Clear();
	return TRUE;
}

BOOL CQHSMApplicationGlobal::ShowErrorLog() {
	CString szErrors;
	if (CCommunicationLogfile::GetInstance()->Get(szErrors) != 0) {
		CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
		if (pDialog != 0) {
			pDialog->SetModeTaskErrorLog(szErrors);
			CCommunicationLogfile::GetInstance()->Clear();
			return TRUE;
		}
	}
	return FALSE;
}

/**
 *
 */
BOOL CQHSMApplicationGlobal::ShowDialog_TryAgain()
{
	CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
	if (pDialog != 0)
	{
		CApplicationDlgInfoAppearance Appearance("Binding configuration software",
												 "Try Again",
												 ENUM_DIALOG_MODE_RETRY);
		pDialog->SetModeTaskAskUser(&Appearance);
		return TRUE;	
	}
	return FALSE;	
}

BOOL CQHSMApplicationGlobal::ShowDialog_AreYouSure(CString s)
{
	CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
	if (pDialog != 0)
	{
		CString szMessage = s + "\nAre you sure?";

		CApplicationDlgInfoAppearance Appearance("Binding configuration software",
												 szMessage,
												 ENUM_DIALOG_MODE_CONFIRMATION);
		pDialog->SetModeTaskAskUser(&Appearance);
		return TRUE;	
	}
	return FALSE;	
}

BOOL CQHSMApplicationGlobal::ShowDialog_Info(CString s)
{
	CDlgMainApplicationTask* pDialog = CDlgMainApplicationTask::GetInstance();
	if (pDialog != 0)
	{
		CApplicationDlgInfoAppearance Appearance("Binding configuration software",
												 s,
												 ENUM_DIALOG_MODE_INFORMATION);
		pDialog->SetModeTaskAskUser(&Appearance);
		return TRUE;	
	}
	return FALSE;	
}


