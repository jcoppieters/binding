// BindingEntryOperations.cpp: implementation of the CBindingEntryOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEntryOperations.h"

#include "BindingOperatorOperations.h"
#include "BindingUnitsOperations.h"
#include "BindingLinksOperations.h"
#include "BindingLinkOperations.h"
#include "CanNode.h"
#include "NodeDatabase.h"

#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringOperations.h"
#include "App\AppBindings\AppBindingEvalString\BindingEvaluationStringOperationsInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEntryOperations::CBindingEntryOperations(CBindingEntry *pBindingEntry) :
m_pBindingEntry(pBindingEntry)
{
	ASSERT(m_pBindingEntry);
}

CBindingEntryOperations::~CBindingEntryOperations()
{

}

/**
 *
 */
BOOL CBindingEntryOperations::SetDefaultName(BOOL fUpdateUnits)
{
	CBindingEvaluationStringOperationsInfo Operations(m_pBindingEntry->GetEvaluationString());
	if (Operations.Parse() == TRUE)
	{
		if (m_pBindingEntry->GetColUnit() != 0)
		{
			ASSERT(m_pBindingEntry->GetColUnit());

			CBindingUnit* pFirst = m_pBindingEntry->GetColUnit()->Find(Operations.GetFirstId());
			CBindingUnit* pLast = m_pBindingEntry->GetColUnit()->Find(Operations.GetLastId());

			if (pFirst != 0 && pLast != 0)
			{
				if (fUpdateUnits == TRUE)
				{
					// Update Units ...
					CBindingUnitsOperations UnitsOperations(m_pBindingEntry->GetColUnit());
					UnitsOperations.SetDefaultNames(pLast);
				}

				return (SetDefaultName(pFirst, pLast));						
			}

			ASSERT(pFirst);
			ASSERT(pLast);
		}
	}
	return FALSE;
}

/**
 *
 */
BOOL CBindingEntryOperations::SetDefaultName(CBindingUnit* const pFirst,CBindingUnit* const pLast)
{
	ASSERT(m_pBindingEntry);

	CString szBindingName;
	const BYTE bMessageCode = pFirst->BindingData.Get_MessageCode();

	switch (bMessageCode)
	{
		case EV_UNITCONTROLTOGGLE:
		{
			szBindingName.Format(_T("%s  L"),pLast->GetName());
		
		} break;

		case EV_UNITCONTROLPULSTOGGLE:
		{
			szBindingName.Format(_T("%s  S"),pLast->GetName());	

		} break;

		case EV_UNITCONTROLPULS:
		{
			szBindingName.Format(_T("%s  P"),pLast->GetName());				

		} break;

		default:
		{
			szBindingName.Format(_T("%s  ?"),pLast->GetName());	
			ASSERT(0);
		} break;			
	}

	m_pBindingEntry->SetName(szBindingName);
	return TRUE;
}

BOOL CBindingEntryOperations::Import(CBindingEntry* const pImport)
{
	CBindingUnits* const pBindingUnits = m_pBindingEntry->GetColUnit();
	const CBindingUnits* const pBindingUnitsImport = pImport->GetColUnit();
	CBindingLinks* const pBindingLinks = m_pBindingEntry->GetColLink();
	CBindingLinks* const pBindingLinksImport = pImport->GetColLink();
	CBindingOperators* const pBindingOperators = m_pBindingEntry->GetColOperator();
	const CBindingOperators* const pBindingOperatorsImport = pImport->GetColOperator();


	// Last = Last in existing bindingUnits...
	CBindingUnit* pLast = pBindingUnits->Get(pBindingUnits->Count() - 1);

	// First = First in BindingUnits to import...
	CBindingUnit* pFirst = pBindingUnitsImport->Get(0);

	if (1)
	{
		CBindingUnitsOperations BindingUnitsOperations(pBindingUnits);

		if (BindingUnitsOperations.Import(pBindingUnitsImport) == FALSE)
		{
			return FALSE;
		}

		if (1)
		{
			// Find first in New collection ...
			CBindingUnit *pTemp = NULL;
			if (BindingUnitsOperations.FindUnit(pFirst,&pTemp) == TRUE)
			{
				pFirst = pTemp;
			}
			else
			{
				ASSERT(0);
				return FALSE;
			}
		}
	}

	if (1)
	{	
		if (pBindingUnitsImport->Count() > 1)
		{
			CBindingLinksOperations BindingLinksOperations(pBindingLinksImport);
			if (BindingLinksOperations.UpdateData(pBindingUnitsImport,
												  pBindingUnits) 
												  == FALSE)
			{
				ASSERT(0);
				return FALSE;
			}

			// Todo => Also update Operators ...
		}

		if (pBindingUnitsImport->Count() > 1)
		{
			CBindingLinksOperations BindingLinksOperations(pBindingLinks);
			if (BindingLinksOperations.Import(pBindingLinksImport) == FALSE)
			{
				ASSERT(0);
				return FALSE;
			}
		}

		/**
		 * Add BindingLink Last in Sequence BGN Construct to first N construct ...
		 */
		if (1)
		{
			ASSERT(pFirst);
			ASSERT(pLast);

			if (pFirst != NULL && pLast != NULL)
			{			
				ASSERT(pFirst->GetID());
				ASSERT(pLast->GetID());
				ASSERT(pFirst->GetID() != pLast->GetID());
		
				CBindingLinksOperations BindingLinksOperations(pBindingLinks);
				const BOOL fResult = BindingLinksOperations.Add(pLast,pFirst);
			
				ASSERT(fResult);
				if (fResult == FALSE) 
				{
					return fResult;
				}
			}
			else
			{
				return FALSE;
			}
		}
	}

	if (1)
	{
		/**
		 * Build evaluation string !!!
		 */

		if (1)
		{
			CBindingEvaluationString* const pBindingEvalString = pImport->GetEvaluationString();
			CBindingEvaluationStringOperations BindingEvalOperations(pBindingEvalString,
																	 pBindingUnitsImport,
																	 pBindingUnits,
																	 pBindingOperatorsImport,
																	 pBindingOperators);
			if (BindingEvalOperations.UpdateData() == FALSE)
			{
				ASSERT(0);
				return FALSE;
			}
		}

		if (1)
		{
			CBindingEvaluationString* const pBindingEvalString = m_pBindingEntry->GetEvaluationString();	
			CBindingEvaluationStringOperations BindingEvalOperations(pBindingEvalString);

			/**
			 * ??? > ???
			 */
			if (BindingEvalOperations.Append('>',pImport->GetEvaluationString()) == FALSE)
			{
				ASSERT(0);
				return FALSE;
			}
		}	
	}

	return TRUE;
}

BOOL CBindingEntryOperations::Export(FILE *fp)
{
	if (fp == 0) 
	{
		return FALSE;
	}

	if (1)
	{
		CString szMessage("\nEvaluation string: " +
						  m_pBindingEntry->GetEvaluationString()->Get());

		const int nResult = fputs((LPCTSTR)szMessage,fp);		// CHANGES_0X0A01_USE_FPUTS
		if (nResult<0) 
		{
			const DWORD dwError = ferror(fp);
			CString szError;
			szError.Format("Error 0x%02x on stream",dwError);
			AfxMessageBox(szError);
		}
	}

	const CBindingUnits* const pBindingUnits = m_pBindingEntry->GetColUnit();
	if (pBindingUnits != 0)
	{	
		if (1)
		{
			CString szMessage;
			szMessage.Format("\nBindingUnits : #%d",pBindingUnits->Count());

			const int nResult = fputs((LPCTSTR)szMessage,fp);		// CHANGES_0X0A01_USE_FPUTS
			if (nResult<0) 
			{
				const DWORD dwError = ferror(fp);
				CString szError;
				szError.Format("Error 0x%02x on stream",dwError);
				AfxMessageBox(szError);
			}
		}

		CString szMessage;
		for (int i=0;i<pBindingUnits->Count();i++)
		{
			const CBindingUnit* const pBindingUnit = pBindingUnits->Get(i);
			if (pBindingUnit != 0)
			{
				szMessage.Format("\nUnit id:%d : name: %s : address: 0x%02x,0x%02x",
								 pBindingUnit->GetID(),
								 pBindingUnit->GetName(),
								 pBindingUnit->GetNodeAddress(),
								 pBindingUnit->GetUnitAddress());

				const int nResult = fputs((LPCTSTR)szMessage,fp);		// CHANGES_0X0A01_USE_FPUTS
				if (nResult<0) 
				{
					const DWORD dwError = ferror(fp);
					CString szError;
					szError.Format("Error 0x%02x on stream",dwError);
					AfxMessageBox(szError);
				}
			}
		}			
	}

	const CBindingOperators* const pBindingOperators = m_pBindingEntry->GetColOperator();
	if (pBindingOperators != 0)
	{
		if (1)
		{
			CString szMessage;
			szMessage.Format("\nBindingOperators : #%d",pBindingOperators->Count());
			
			const int nResult = fputs((LPCTSTR)szMessage,fp);	// CHANGES_0X0A01_USE_FPUTS
			if (nResult<0) 
			{
				const DWORD dwError = ferror(fp);
				CString szError;
				szError.Format("Error 0x%02x on stream",dwError);
				AfxMessageBox(szError);
			}
		}

		CString szMessage;
		for (int i=0;i<pBindingOperators->Count();i++)
		{
			const CBindingOperator* const pBindingOperator = pBindingOperators->Get(i);
			if (pBindingOperator != 0)
			{
				CBindingOperatorOperations BindingOperatorOperations(pBindingOperators->Get(i));
				szMessage.Format("\nOperator id:%d - %s",
								 pBindingOperator->GetID(),
								 BindingOperatorOperations.GetFunction());
				
				const int nResult = fputs((LPCTSTR)szMessage,fp);	// CHANGES_0X0A01_USE_FPUTS
				if (nResult<0) 
				{
					const DWORD dwError = ferror(fp);
					CString szError;
					szError.Format("Error 0x%02x on stream",dwError);
					AfxMessageBox(szError);
				}
			}
		}	
	}

	const CBindingLinks* const pBindingLinks = m_pBindingEntry->GetColLink();
	if (pBindingLinks != 0)
	{
		if (1)
		{
			CString szMessage;
			szMessage.Format("\nBindingLinks : #%d",pBindingLinks->Count());
			
			const int nResult = fputs((LPCTSTR)szMessage,fp);		// CHANGES_0X0A01_USE_FPUTS
			if (nResult<0) 
			{
				const DWORD dwError = ferror(fp);
				CString szError;
				szError.Format("Error 0x%02x on stream",dwError);
				AfxMessageBox(szError);
			}
		}

		CString szMessage;
		for (int i=0;i<pBindingLinks->Count();i++)
		{
			CBindingLink* const pBindingLink = pBindingLinks->Get(i);
			if (pBindingLink != 0)
			{
				CBindingLinkOperations BindingLinkOperations(pBindingLink);

				szMessage.Format("\nLink id:%d '%s'",
								 pBindingLink->GetID(),
								 BindingLinkOperations.ToString());

				const int nResult = fputs((LPCTSTR)szMessage,fp);		// CHANGES_0X0A01_USE_FPUTS
				if (nResult<0) 
				{
					const DWORD dwError = ferror(fp);
					CString szError;
					szError.Format("Error 0x%02x on stream",dwError);
					AfxMessageBox(szError);
				}
			}				
		}
		return TRUE;
	}
	return FALSE;
}


