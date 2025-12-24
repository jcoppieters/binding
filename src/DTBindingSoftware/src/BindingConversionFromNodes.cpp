// BindingConversionFromNodes.cpp: implementation of the CBindingConversionFromNodes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "timer.h"
#include "BindingConversionFromNodes.h"

#include "MainFrm.h"

#include "BindingDatabaseApplication.h"
#include "BindingFileEntry.h"
#include "BindingFilesOperations.h"
#include "DlgConversionFromNodes.h"
#include "BindingFileConstruction.h"
#include "NodeDatabaseApplication.h"

#include "BindingUnitFactory.h"
#include "BindingEntriesOperations.h"
#include "BindingUnitOperations.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingConversionFromNodes* CBindingConversionFromNodes::GetInstance()
{
	static CBindingConversionFromNodes instance;
	return &instance;
}

///////////////////////////////////////////////////////////////////////

CBindingConversionFromNodes::CBindingConversionFromNodes() : 
QFsm((QFsmState) initial)
{
	m_pBindingFilesOperations  = NULL;
}

CBindingConversionFromNodes::~CBindingConversionFromNodes()
{

}


void CBindingConversionFromNodes::Trigger(EV_FSM_CONVERSION_FSM_NODES event)
{
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_QFSM_CONVERTFROMNODES_EVENT,
									 event,
									 0);
	}
}

void CBindingConversionFromNodes::StartTimer()
{
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->SetTimer(TIMERID_CONVERTFROMNODES_NEXT,
								  TIMER_CONVERFROMNODES_TIMEOUT_VALUE,
								  (TIMERPROC) NULL);
	}
}

void CBindingConversionFromNodes::StopTimer()
{
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->KillTimer(TIMERID_CONVERTFROMNODES_NEXT);									  									  
	}
}

void CBindingConversionFromNodes::NotifyUser(CString s)
{
	if (CDlgConversionFromNodes::GetInstance() != NULL)
	{
		CDlgConversionFromNodes::GetInstance()->AddInfo(s);
	}
}

/**
 *
 */
void CBindingConversionFromNodes::initial(QEvent const*)
{
	tran((QFsmState) &CBindingConversionFromNodes::State_Idle);
}

/**
 *
 *
 */
void CBindingConversionFromNodes::State_Idle(QEvent const* e)
{
	switch(e->sig)
	{
		case EV_OPERATION_TASK_START:	
		{
			// Initialise FSM !!!
			if (1)
			{
				CString szMessage("Start Conversion From Nodes");
				NotifyUser(szMessage);
			}

			((CMainFrame*) AfxGetMainWnd())->ClearViews();		
		

			if (TMGetBindingEntries() != NULL)
			{
				TMGetBindingEntries()->RemoveAll();
			}


			if (m_pBindingFilesOperations == NULL)
			{			
				m_pBindingFilesOperations = new CBindingFilesOperations(TMGetBindingFiles());		
			}
			
			ASSERT(m_pBindingFilesOperations);	

			m_usCurrentBindingNr = 1;
			m_usMaxBindingNr = m_pBindingFilesOperations->GetMax();
			
			tran((QFsmState) &CBindingConversionFromNodes::State_Busy);
			StartTimer();		

		} break;

		case EV_OPERATION_TASK_CANCEL:
		{
			StopTimer();
			Trigger(EV_SIGNAL_TASK_CANCELLED);

		} break;

		case EV_AFTER_TIMER_NEXT:	
		{
			ASSERT(0);
			StopTimer();

		} break;

		case EV_SIGNAL_TASK_CANCELLED:
		case EV_SIGNAL_TASK_DONE:
		{
			ASSERT(0);

		} break;
	}
}

void CBindingConversionFromNodes::State_Busy(QEvent const* e)
{
	switch(e->sig)
	{
		case EV_OPERATION_TASK_CANCEL:
		{
			StopTimer();
			Trigger(EV_SIGNAL_TASK_CANCELLED);

		} break;

		case EV_AFTER_TIMER_NEXT:
		{
			// Process Next Binding Nr ..

			StopTimer();
			
			if (m_usCurrentBindingNr <= m_usMaxBindingNr)	// [Not ALL Processed]
			{		
				if (m_pBindingFilesOperations->FindStrings(m_usCurrentBindingNr) == TRUE)
				{
					m_StringArray.RemoveAll();

					m_pBindingFilesOperations->GetStrings(m_StringArray);
					
					Trigger(EV_SIGNAL_TASK_PROCESS);
				}
				else
				{
					StartTimer();
				}

				m_usCurrentBindingNr++;
			}
			else	// [ALL Processed] 
			{			
				Trigger(EV_SIGNAL_TASK_DONE);
			}

		} break;

		case EV_SIGNAL_TASK_PROCESS:
		{
			if (1)
			{
				CString szMessage;
				szMessage.Format("Processing bindingNr: 0x%04x",
								 m_usCurrentBindingNr);
				NotifyUser(szMessage);
			}

			CBindingFileConstruction BindingFileConstruction(&m_StringArray);
			if (BindingFileConstruction.Construct() == TRUE)
			{
				TRACE("\nCBindingFileConstruction::Construct Success");

				CBindingEntryInfo *pBindingEntryInfo = 
					BindingFileConstruction.GetBindingEntryInfo();

				if (pBindingEntryInfo != NULL)
				{
					BOOL fResult = FALSE;

					if (Update(pBindingEntryInfo->GetBindingEntry()) == TRUE)
					{
						if (Add(pBindingEntryInfo->GetBindingEntry()) == TRUE)
						{
							fResult = TRUE;
						}

						ASSERT(fResult);
					}

					ASSERT(fResult);
				}
			}
			else
			{
				TRACE("\nCBindingFileConstruction::Construct Failed");
			}
			
			StartTimer();

		} break;

		case EV_SIGNAL_TASK_CANCELLED:
		{
			tran((QFsmState) &CBindingConversionFromNodes::State_Done);
			
			Trigger(EV_SIGNAL_TASK_DONE);

			if (1)
			{
				CString szMessage("Processing binding Conversion Cancelled!");				
				NotifyUser(szMessage);
			}

		} break;

		case EV_SIGNAL_TASK_DONE:
		{
			tran((QFsmState) &CBindingConversionFromNodes::State_Done);
			
			Trigger(EV_SIGNAL_TASK_DONE);

			((CMainFrame*) AfxGetMainWnd())->UpdateViews();	

			if (1)
			{
				CString szMessage("Processing binding Conversion Done!");				
				NotifyUser(szMessage);
			}

		} break;
	}
}

void CBindingConversionFromNodes::State_Done(QEvent const* e)
{
	switch(e->sig)
	{
		case EV_SIGNAL_TASK_DONE:
		{
			if (m_pBindingFilesOperations != NULL)
			{
				delete m_pBindingFilesOperations;
				m_pBindingFilesOperations = NULL;
			}

			tran((QFsmState) &CBindingConversionFromNodes::State_Idle);

		} break;

		case EV_AFTER_TIMER_NEXT:
		{
			ASSERT(0);

			StopTimer();

		} break;

		default:
		{
			ASSERT(0);

		} break;
	}
}

BOOL CBindingConversionFromNodes::Add(CBindingEntry* pBindingEntry)
{
	//CBindingEntries* const pBindingEntries = 
	//CBindingDatabaseApplication::GetBindingEntries();

	CBindingEntriesOperations BindingEntriesOperations(TMGetBindingEntries());
	const unsigned short nID = BindingEntriesOperations.Add(pBindingEntry,
															pBindingEntry->GetBindingNr());

	ASSERT(nID);
	if (nID != 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBindingConversionFromNodes::Update(CBindingEntry* pBindingEntry)
{
	ASSERT(pBindingEntry);	
	BOOL fResult = FALSE;
	
	CBindingUnits* const pBindingUnits = pBindingEntry->GetColUnit();
	if (pBindingUnits != NULL)
	{
		const int nCount = pBindingUnits->Count();
		CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());

		for (int i=0;i<nCount;i++)
		{						
			STRUCT_UNIT* pUnit;
			
			CBindingUnit *const pBindingUnit = pBindingUnits->Get(i);
			ASSERT(pBindingUnit);

			if (pBindingUnit != NULL)
			{					
				if (NodeDatabaseOperations.Find(pBindingUnit,&pUnit) == TRUE)
				{														
					CBindingUnitOperations BindingUnitOperations(pBindingUnit);

					pBindingUnit->SetType(BindingUnitOperations.ConvertType(pUnit));
					pBindingUnit->SetName(pUnit->szUnitName);
					
					/**
					 * pBindingUnit = Base class CBindingUnit --> pNew = Derived class
					 */
					CBindingUnit *const pNew = CBindingUnitFactory::Clone(pBindingUnit);
					if (pNew != NULL)
					{
						ASSERT(pNew->GetType() != 0);

						if (pBindingUnits->Replace(pBindingUnit,pNew) == TRUE)
						{
							if (pNew->ConvertFromBindingData() == TRUE)
							{
								fResult = TRUE;		
							}
							ASSERT(fResult);
						}
						ASSERT(fResult);
					}
					ASSERT(fResult);						
				}
			}

			if (fResult == FALSE)
			{
				ASSERT(0);
				return fResult;
			}
		}		
	}				

	ASSERT(pBindingUnits);
	return fResult;
}

