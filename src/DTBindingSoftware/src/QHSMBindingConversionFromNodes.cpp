// QHSMBindingConversionFromNodes.cpp: implementation of the CQHSMBindingConversionFromNodes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QHSMBindingConversionFromNodes.h"
#include "MainFrm.h"
#include "BindingFilesOperations.h"
#include "BindingConversionFromNodesOperations.h"
#include "LogFileConversionFromNodes.h"


#include "views/bindings/DlgConversionFromNodes.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntry.h"

// -----------------------------------
#include "TMGlobals.h"
using namespace TMBindingDatabase;
// -----------------------------------


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CQHSMBindingConversionFromNodes* CQHSMBindingConversionFromNodes::GetInstance()
{
	static CQHSMBindingConversionFromNodes instance;
	return &instance;
}

//////////////////////////////////////////////////////////////////////

CQHSMBindingConversionFromNodes::~CQHSMBindingConversionFromNodes()
{
	if (m_pConversionFromNodes != 0)
	{
		delete m_pConversionFromNodes;
		m_pConversionFromNodes = 0;
	}
}

//////////////////////////////////////////////////////////////////////
void CQHSMBindingConversionFromNodes::initial(QEvent const *e)
{
	ASSERT( m_pConversionFromNodes == 0);

	try
	{
		m_pConversionFromNodes = new CBindingConversionFromNodesOperations(
				TMGetBindingFiles(),TMGetBindingEntries());
	}
	catch( CMemoryException* e)
	{	
		if (m_pConversionFromNodes != 0)
		{
			delete m_pConversionFromNodes;
			m_pConversionFromNodes = 0;
		}

		e->ReportError();
		e->Delete();		

		exit( -1 );
	}

	m_pLogFile = CLogFile::GetConversionFromNodes();

	Q_INIT(&CQHSMBindingConversionFromNodes::Main);
}


/**
 * QSTATE CQHSMBindingConversionFromNodes::Main(QEvent const *e)
 */
QSTATE CQHSMBindingConversionFromNodes::Main(QEvent const *e)
{
	switch (e->sig) 
	{
		case Q_INIT_SIG:
		{
			Q_INIT(&CQHSMBindingConversionFromNodes::Idle);

		} return 0;

		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{		

		} return 0;

		case 0:
		{
			TRACE("\nQ_EMPTY_SIG");
		
		} break;

		default:
		{
			//CString s;
			//s.Format("Unhandled event CQHSMBindingConversionFromNodes : 0x%02x",e->sig);
			//AfxMessageBox(s);		

		} break;	// return  !!!!0
	}

	return ((QSTATE) &CQHSMBindingConversionFromNodes::top);
}

/**
 * QSTATE CQHSMBindingConversionFromNodes::Idle(QEvent const *e)
 */
QSTATE CQHSMBindingConversionFromNodes::Idle(QEvent const *e)
{
	switch (e->sig) 
	{		
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{
		} return 0;

		case EV_OPERATION_TASK_CLEAR:
		{
			((CMainFrame*) AfxGetMainWnd())->ClearViews();	
			StartTimer();

			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Clear);

		} return 0;

		case EV_OPERATION_TASK_START:	
		{		
			// Initialise FSM !!!
			if (1)
			{
				CString szMessage("Start Conversion From Nodes");
				NotifyUser(szMessage);
			}		

			((CMainFrame*) AfxGetMainWnd())->ClearViews();		

			ASSERT( m_pConversionFromNodes );

			m_pConversionFromNodes->Init();	//Clear();

			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Verify);
			StartTimer();			

		} break;
	
		case EV_OPERATION_TASK_CANCEL:
		{
			StopTimer();
			Trigger(EV_SIGNAL_TASK_CANCELLED);

		} break;

		case EV_AFTER_TIMER_NEXT:	
		{
			//ASSERT(0);
			StopTimer();

		} break;

		case EV_SIGNAL_TASK_CANCELLED:
		case EV_SIGNAL_TASK_DONE:
		{
			ASSERT(0);
		} break;

		case 0:
		{
			TRACE("\nQ_EMPTY_SIG");
		} break;
	}

	return ((QSTATE) &CQHSMBindingConversionFromNodes::Main);
}

/**
 * QSTATE CQHSMBindingConversionFromNodes::Busy(QEvent const *e)
 */
QSTATE CQHSMBindingConversionFromNodes::Busy(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{		
			ASSERT( m_pLogFile );

			if ( m_pLogFile != 0) {
				m_pLogFile->Stop();	
			}

		} return 0;
		
		case Q_ENTRY_SIG:
		{
			ASSERT( m_pLogFile );

			if ( m_pLogFile != 0) {
				m_pLogFile->Start();	
			}

		} return 0;

		case EV_OPERATION_TASK_CANCEL:
		{
			StopTimer();
			Trigger(EV_SIGNAL_TASK_CANCELLED);

		} return 0;

		case EV_SIGNAL_TASK_CANCELLED:
		{							
			if (1)
			{
				CString szMessage("Processing binding Conversion Cancelled!");				
				NotifyUser(szMessage);
			}

			Q_TRAN(&CQHSMBindingConversionFromNodes::Done);

		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{		
			//m_pConversionFromNodes->SetDefaultNames();					
					
			if (1)
			{
				CString szMessage("Processing binding Conversion Done!");				
				NotifyUser(szMessage);
			}

			Q_TRAN(&CQHSMBindingConversionFromNodes::Done);

		} return 0;
	}

	return ((QSTATE) &CQHSMBindingConversionFromNodes::Main);
}

/**
 * QSTATE CQHSMBindingConversionFromNodes::Busy_Verify(QEvent const *e)
 */
QSTATE CQHSMBindingConversionFromNodes::Busy_Verify(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{
			if (1)
			{
				CString szMessage("Conversion From Nodes: Busy Verify");
				NotifyUser(szMessage);
			}

			if (m_pLogFile != 0)
			{
				CString szMessage("Step 1: Binding Files: Verifying syntax, addresses");
				m_pLogFile->AddHeader(szMessage);
			}
	
			m_usCurrentBindingFile = 0;		
			StartTimer();		

		} return 0;

		case EV_AFTER_TIMER_NEXT:
		{					
			StopTimer();

			if (1)
			{
				CString szMessage("Conversion From Nodes: Busy Verify\n"
								  "Get Next Bindingfile");
				NotifyUser(szMessage);
			}

			if (m_usCurrentBindingFile < m_pConversionFromNodes->GetMaxBindingFiles())
			{						
				Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Verify_Syntaxsis);										
				
				Trigger(EV_SIGNAL_TASK_PROCESS);

				//m_usCurrentBindingFile++;
			}
			else
			{						
				Trigger(EV_SIGNAL_TASK_DONE);			// done ...				
			}
								
		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{
			ASSERT( 0 );

		} return 0;			

		case EV_SIGNAL_TASK_DONE:
		{
			//m_usCurrentBindingNr = 1;
			//m_usMaxBindingNr = 	m_pConversionFromNodes->GetMaxBindingNr();			

			m_usCurrentBindingFile = 0;
			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Adjust);

			StartTimer();		

		} return 0;

	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy);
}


/**
 * QSTATE CQHSMBindingConversionFromNodes::Busy_Verify_Syntaxsis(QEvent const *e)
 */
QSTATE CQHSMBindingConversionFromNodes::Busy_Verify_Syntaxsis(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{						

		} return 0;
		
		case Q_ENTRY_SIG:
		{		
			if (1)
			{
				CString szMessage("Conversion From Nodes: Busy Verify\n"
								  "Get Next Bindingfile\n"
								  "Busy Verifying");
				NotifyUser(szMessage);
			}		

		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{		
			const BOOL fResult = m_pConversionFromNodes->VerifySyntaxsis(m_usCurrentBindingFile);
			ASSERT( fResult );		

			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Verify_Addresses);	
			Trigger(EV_SIGNAL_TASK_PROCESS);
			
		} return 0;	
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy_Verify);
}

/**
 * QSTATE CQHSMBindingConversionFromNodes::Busy_Verify_Addresses(QEvent const *e)
 */
QSTATE CQHSMBindingConversionFromNodes::Busy_Verify_Addresses(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{						
		} return 0;
		
		case Q_ENTRY_SIG:
		{						
		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{		
			const BOOL fResult = m_pConversionFromNodes->VerifyAddresses(m_usCurrentBindingFile);
			ASSERT( fResult );

			StartTimer();			

			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Verify);	
			m_usCurrentBindingFile++;
			
		} return 0;
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy_Verify);

}


QSTATE CQHSMBindingConversionFromNodes::Busy_Adjust(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{			
			if (1)
			{
				CString szMessage("Conversion From Nodes: Busy adjusting bindingfiles");
				NotifyUser(szMessage);
			}

			if (m_pLogFile != 0)
			{
				CString szMessage("Step 2: Binding Files: Adjusting property and instruction bindings");
				m_pLogFile->AddHeader(szMessage);
			}
	
			m_usCurrentBindingFile = 0;		
			StartTimer();		

		} return 0;

		case EV_AFTER_TIMER_NEXT:
		{					
			StopTimer();
			
			if (m_usCurrentBindingFile < m_pConversionFromNodes->GetMaxBindingFiles())
			{						
				Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Adjust_PropertyStrings);														
				Trigger(EV_SIGNAL_TASK_PROCESS);

				//m_usCurrentBindingFile++;
			}
			else
			{						
				Trigger(EV_SIGNAL_TASK_DONE);			// done ...				
			}
								
		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{
			m_usCurrentBindingNr = 1;
			m_usMaxBindingNr = 	m_pConversionFromNodes->GetMaxBindingNr();
			
			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Normal);

			StartTimer();		

		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{
			ASSERT( 0 );
		} return 0;	
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy);
}

QSTATE CQHSMBindingConversionFromNodes::Busy_Adjust_PropertyStrings(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{
		} return 0;		

		case EV_SIGNAL_TASK_PROCESS:
		{			
			const BOOL fResult = m_pConversionFromNodes->GetPropertyBindings(m_usCurrentBindingFile);
			if (fResult == TRUE) 
			{
				const BOOL fResult2 = m_pConversionFromNodes->AdjustPBindings();
				ASSERT(fResult2);
			}
						
			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Adjust_InstructionStrings);														
			Trigger(EV_SIGNAL_TASK_PROCESS);

		} return 0;	
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy_Adjust);
}

QSTATE CQHSMBindingConversionFromNodes::Busy_Adjust_InstructionStrings(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{
		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{		
			const BOOL fResult = m_pConversionFromNodes->GetInstructionBindings(m_usCurrentBindingFile);
			if (fResult == TRUE) 
			{
				const BOOL fResult2 = m_pConversionFromNodes->AdjustIBindings();
				ASSERT(fResult2);
			}

			StartTimer();			
			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Adjust);				
			
			m_usCurrentBindingFile++;

		} return 0;	
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy_Adjust);
}

/**
 * QSTATE CQHSMBindingConversionFromNodes::Busy_Normal(QEvent const *e)
 */
QSTATE CQHSMBindingConversionFromNodes::Busy_Normal(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{
			if (1)
			{
				CString szMessage("Conversion From Nodes: Normal BindingEntries");
				NotifyUser(szMessage);
			}

			if (m_pLogFile != 0)
			{
				CString szMessage("Step 3: Binding Files: Processing bindings");
				m_pLogFile->AddHeader(szMessage);
			}

		} return 0;

		case EV_AFTER_TIMER_NEXT:
		{
			ASSERT( m_pConversionFromNodes );
		
			StopTimer();
			
			if (m_usCurrentBindingNr <= m_usMaxBindingNr)	// [Not ALL Processed]
			{		
				if (1)
				{
					CString szMessage;
					szMessage.Format("Retrieving bindingnr: 0x%04x",m_usCurrentBindingNr);
					NotifyUser(szMessage);
				}

				if (m_pLogFile != 0) 
				{
					m_pLogFile->Search( m_usCurrentBindingNr );
				}								
									
				if (m_pConversionFromNodes->GetBindings(m_usCurrentBindingNr) 
					== TRUE)
				{
					Trigger(EV_SIGNAL_TASK_PROCESS);
					Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Normal_Process);
				}			
				else
				{
					StartTimer();
				}

				m_usCurrentBindingNr++;
			}
			else	// [ALL Processed] 
			{			
				Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Property);
			}

		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{
			ASSERT( 0 );		

		} return 0;
	
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy);
}

QSTATE CQHSMBindingConversionFromNodes::Busy_Normal_Process(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{
			if (1)
			{
				CString szMessage;
				szMessage.Format("Retrieving bindingnr: 0x%04x\n"
								 "Processing",
								 m_usCurrentBindingNr);
				NotifyUser(szMessage);
			}
		} return 0;
	
		case EV_SIGNAL_TASK_PROCESS:
		{				
			const BOOL fResult = m_pConversionFromNodes->Process();
			//ASSERT( fResult );
			
			StartTimer();
			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Normal);		

		} return 0;
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy_Normal);
}


/** 
 * QSTATE CQHSMBindingConversionFromNodes::Busy_Property(QEvent const *e)
 *
 */
QSTATE CQHSMBindingConversionFromNodes::Busy_Property(QEvent const *e)
{
	switch (e->sig) 
	{		
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{			
			if (1)
			{
				CString szMessage("Conversion From Nodes: Property bindings");
				NotifyUser(szMessage);
			}

			if (m_pLogFile != 0)
			{
				CString szMessage("Step 4: Binding Files: Processing property bindings");
				m_pLogFile->AddHeader(szMessage);
			}

			m_usCurrentBindingFile = 0;		
			StartTimer();		

		} return 0;

		case EV_AFTER_TIMER_NEXT:
		{			
			StopTimer();

			if (m_usCurrentBindingFile < m_pConversionFromNodes->GetMaxBindingFiles())
			{
				if (m_pConversionFromNodes->GetPropertyBindings(m_usCurrentBindingFile) == TRUE)
				{
					Trigger(EV_SIGNAL_TASK_PROCESS);

					Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Property_Upgrade);				
				}
				else
				{
					StartTimer();					
				}

				m_usCurrentBindingFile++;
			}
			else
			{			
				m_pConversionFromNodes->SetDefaultNames();
				Trigger(EV_SIGNAL_TASK_DONE);			// done ...				
			}
								
		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{
			ASSERT( 0 );

		} return 0;			
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy);
}

/**
 *
 */
QSTATE CQHSMBindingConversionFromNodes::Busy_Property_Upgrade(QEvent const *e)
{
	switch (e->sig) 
	{		
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{			
			if (1)
			{
				CString szMessage("Conversion From Nodes: Property bindings\n"
								  "Upgrading property bindings");

				NotifyUser(szMessage);
			}

		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{		
			const BOOL fResult = m_pConversionFromNodes->UpgradePropertyBindings();
			if (fResult == TRUE)
			{			
				Trigger(EV_SIGNAL_TASK_PROCESS);
				Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Property_Process);
			}
			else
			{
				ASSERT( fResult );
				
				StartTimer();
				Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Property);
			}						

		} return 0;			
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy_Property);
}

/**
 *
 */
QSTATE CQHSMBindingConversionFromNodes::Busy_Property_Process(QEvent const *e)
{
	switch (e->sig) 
	{		
		case Q_EXIT_SIG:
		{			
			if (1)
			{
				CString szMessage("Conversion From Nodes: Property bindings - Get Next bindingfile");
				NotifyUser(szMessage);
			}

		} return 0;
		
		case Q_ENTRY_SIG:
		{			
			if (1)
			{
				CString szMessage("Conversion From Nodes: Property bindings\n"
								  "Upgrading property bindings\n"
								  "Processing property bindings");

				NotifyUser(szMessage);
			}

		} return 0;

		case EV_SIGNAL_TASK_PROCESS:
		{
			const BOOL fResult = m_pConversionFromNodes->ProcessPropertyBindings();
			ASSERT( fResult );

			StartTimer();			
			Q_TRAN(&CQHSMBindingConversionFromNodes::Busy_Property);	
			
		} return 0;
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy_Property);
}

QSTATE CQHSMBindingConversionFromNodes::Busy_Clear(QEvent const *e)
{
	switch (e->sig) 
	{		
		case Q_EXIT_SIG:
		{
			
		} return 0;

		
		case Q_ENTRY_SIG:
		{			
			if (1)
			{
				CString szMessage("Clearing databases");
				NotifyUser(szMessage);
			}

		} return 0;

		case EV_AFTER_TIMER_NEXT:
		{			
			ASSERT( m_pConversionFromNodes );

			StopTimer();
			m_pConversionFromNodes->Clear();

			Trigger(EV_SIGNAL_TASK_DONE);

		} return 0;
	}

	return ((QSTATE) &CQHSMBindingConversionFromNodes::Busy);
}

/**
 * 
 * QSTATE CQHSMBindingConversionFromNodes::Done(QEvent const *e)
 */
QSTATE CQHSMBindingConversionFromNodes::Done(QEvent const *e)
{
	switch (e->sig) 
	{	
		case Q_EXIT_SIG:
		{			
		} return 0;
		
		case Q_ENTRY_SIG:
		{
			((CMainFrame*) AfxGetMainWnd())->UpdateViews();	

			Trigger(EV_SIGNAL_TASK_DONE);

		} return 0;

		case EV_SIGNAL_TASK_DONE:
		{			
			Q_TRAN(&CQHSMBindingConversionFromNodes::Idle);

		} return 0;
	}
	return ((QSTATE) &CQHSMBindingConversionFromNodes::Main);
}

/**
 *
 */
void CQHSMBindingConversionFromNodes::Trigger(EV_FSM_CONVERSION_FSM_NODES event)
{
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->PostMessage(WM_QFSM_CONVERTFROMNODES_EVENT,
									 event,
									 0);
	}
}

void CQHSMBindingConversionFromNodes::StartTimer()
{
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->SetTimer(TIMERID_CONVERTFROMNODES_NEXT,
								  TIMER_CONVERFROMNODES_TIMEOUT_VALUE,
								  (TIMERPROC) NULL);
	}
}

void CQHSMBindingConversionFromNodes::StopTimer()
{
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->KillTimer(TIMERID_CONVERTFROMNODES_NEXT);									  									  
	}
}

void CQHSMBindingConversionFromNodes::NotifyUser(CString s)
{
	if (CDlgConversionFromNodes::GetInstance() != NULL) {
		CDlgConversionFromNodes::GetInstance()->AddInfo(s);
	}
}

