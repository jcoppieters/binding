// BindingEvaluationStringOperations.cpp: implementation of the CBindingEvaluationStringOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEvaluationStringOperations.h"

#include "BindingUnits.h"
#include "BindingOperators.h"
#include "BindingUnitsOperations.h"
#include "BindingOperatorsOperations.h"
#include "Log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingEvaluationStringOperations::CBindingEvaluationStringOperations(CBindingEvaluationString* p) :
m_pBindingEvaluationString(p),
m_pErrorLog( new CLog() )
{
	ASSERT( m_pBindingEvaluationString );
	ASSERT( m_pErrorLog );

	Init();

}

CBindingEvaluationStringOperations::CBindingEvaluationStringOperations(CBindingEvaluationString* p,
																	   const CBindingUnits* const pOldUnits,
																	   CBindingUnits* const pNewUnits,
																	   const CBindingOperators* const pOldOperators,
																	   CBindingOperators* const pNewOperators) :
m_pBindingEvaluationString( p ),
m_pOldBindingUnits( pOldUnits ),
m_pNewBindingUnits( pNewUnits ),
m_pOldBindingOperators( pOldOperators ),
m_pNewBindingOperators( pNewOperators ),
m_pErrorLog( new CLog() )
{
	ASSERT( m_pBindingEvaluationString );
	ASSERT( m_pOldBindingUnits );
	ASSERT( m_pNewBindingUnits );
	ASSERT( m_pOldBindingOperators );
	ASSERT( m_pNewBindingOperators );

	ASSERT( m_pErrorLog );

	Init();

#ifdef _DEBUG

	if(1)
	{
		TRACE1("\n%s",m_pBindingEvaluationString->Get());
	}

	if(1)
	{
		for (int i=0;i<m_pOldBindingUnits->Count();i++)
		{
			const CBindingUnit* const pBindingUnit = m_pOldBindingUnits->Get(i);
			if (pBindingUnit != 0)
			{	
				CString s;
				s.Format("Old BindingUnit : %d: 0x%02x,0x%02x",pBindingUnit->GetID(),
														   pBindingUnit->GetNodeAddress(),
														   pBindingUnit->GetUnitAddress());
				TRACE1("\n%s",s);
			}
		}
	}

	if (1)
	{
		for (int i=0;i<m_pNewBindingUnits->Count();i++)
		{
			const CBindingUnit* const pBindingUnit = m_pNewBindingUnits->Get(i);
			if (pBindingUnit != 0)
			{
				CString s;
				s.Format("New BindingUnit : %d: 0x%02x,0x%02x",pBindingUnit->GetID(),
														pBindingUnit->GetNodeAddress(),
														pBindingUnit->GetUnitAddress());
				TRACE1("\n%s",s);
			}
		}
	}

#endif
}


CBindingEvaluationStringOperations::~CBindingEvaluationStringOperations()
{
	if (m_pParser != 0)
	{
		delete m_pParser;
		m_pParser = NULL;
	}

	if (m_pBindingNewEvaluationString != 0)
	{
		delete m_pBindingNewEvaluationString;
		m_pBindingNewEvaluationString = NULL;
	}

	if (m_pErrorLog != 0)
	{
		delete m_pErrorLog;
	}
}


//////////////////////////////////////////////////////////////////////

void CBindingEvaluationStringOperations::Init()
{	
	try
	{
		m_pParser = new CBindingEvaluationStringParser(m_pBindingEvaluationString,
													   this);

		m_pBindingNewEvaluationString = new CBindingEvaluationString();

		
		ASSERT( m_pErrorLog );
		m_pErrorLog->Clear();

	}
	catch ( CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
	}
}


//////////////////////////////////////////////////////////////////////

BOOL CBindingEvaluationStringOperations::UpdateData()
{
	BOOL fResult = FALSE;

	ASSERT(m_pParser);
	
	if (m_pParser != NULL)
	{
		ASSERT(m_pBindingEvaluationString);
		ASSERT(m_pOldBindingUnits);
		ASSERT(m_pNewBindingUnits);
	
		m_pBindingNewEvaluationString->Clear();

		CurrentMode = MODE_UPDATE;
		m_fParseCorrect = TRUE;
		m_pParser->Start();

		if (m_fParseCorrect == TRUE)
		{					
			*m_pBindingEvaluationString = *m_pBindingNewEvaluationString;

			fResult = TRUE;
		}
		else
		{
			fResult = FALSE;
		}
	}
	return fResult;
}


BOOL CBindingEvaluationStringOperations::Append(const char c,
												CBindingEvaluationString* pNewEvaluationString)
{
	ASSERT(m_pBindingEvaluationString);

	CString szEvalString = m_pBindingEvaluationString->Get();

	if (szEvalString.IsEmpty() == FALSE)
	{
		szEvalString += c;
		szEvalString += pNewEvaluationString->Get();

		m_pBindingEvaluationString->Set(szEvalString);

		CurrentMode = MODE_VERIFY;

		m_fParseFatalError = FALSE;
		m_pParser->Start();

		return (!m_fParseFatalError);	
	}
	return FALSE;
}


/**
 * BOOL CBindingEvaluationStringOperations::ReplaceTimer(unsigned short nID)
 *
 */
BOOL CBindingEvaluationStringOperations::ReplaceTimer(unsigned short nID)
{
	BOOL fResult = FALSE;

	if (m_pOldBindingOperators != 0 && m_pNewBindingOperators != 0)
	{
		const CBindingOperator* const pBindingOperator = m_pOldBindingOperators->Find(nID);
		if (pBindingOperator != 0)
		{
			CBindingOperatorsOperations BindingOperatorsOperations( m_pNewBindingOperators );

			CBindingOperator* pNewOperator = 0;			
			if (BindingOperatorsOperations.Find(pBindingOperator,&pNewOperator) == TRUE)
			{
				const unsigned short nNewID = pNewOperator->GetID();
				fResult = m_pBindingNewEvaluationString->AddTimer(nNewID);			
			}		
			
			//ASSERT( fResult );
		}
	}
	return fResult;
}


BOOL CBindingEvaluationStringOperations::ReplaceUnit(unsigned short nID,int nEquals)
{
	BOOL fResult = FALSE;

	if (m_pOldBindingUnits != 0 && m_pNewBindingUnits != 0)
	{	
		const CBindingUnit* const pBindingUnit = m_pOldBindingUnits->Find(nID);
		if (pBindingUnit != 0)
		{						
			CBindingUnits BindingUnits;
			CBindingUnitsOperations BindingUnitsOperations(m_pNewBindingUnits);
			const int nFound = BindingUnitsOperations.GetEquals(pBindingUnit,&BindingUnits);
			
			ASSERT( nFound == nEquals );
			ASSERT( nFound > 1);
			ASSERT( nFound == BindingUnits.Count() );

			if (1)
			{
				/**
				 * Find if pBindingUnit has the same ID as one of the BindingUnits in collection
				 * with equal binding units.			 
				 */	
				
				for (int i=0;i<nFound;i++)
				{
					const CBindingUnit* const pEqualBindingUnit = BindingUnits.Get( i );
			
					if (pBindingUnit->GetID() == pEqualBindingUnit->GetID())
					{
						const unsigned short nNewID = pEqualBindingUnit->GetID();

						if (m_pBindingNewEvaluationString->FindUnit(nNewID) == FALSE)
						{
							fResult = m_pBindingNewEvaluationString->AddUnit(nNewID);			
							return fResult;
						}
	
					} // 	if (pBindingUnit->GetID() == pEqualBindingUnit->GetID())
	
				} // for (int i=0;i<nFound;i++)
			}


			if (1)
			{
				/**
				 * Find index of equal BindingUnit in m_pNewBindingUnits and search for index 
				 * equal BindingUnit in collection Equal BindingUnits ...			 
				 */

				int index;
				int nFoundEqual = 0;

				for (index=0;index<m_pOldBindingUnits->Count();index++)
				{
					const CBindingUnit* const pBindingUnitIndex = m_pOldBindingUnits->Get( index );

					if (pBindingUnitIndex->GetID() == pBindingUnit->GetID()) {
						break;
					}

					if (pBindingUnitIndex->IsEqual(pBindingUnit,TRUE,FALSE) == TRUE) {
						nFoundEqual++;
					}
				
				} // for (int index=0;index<m_pOldBindingUnits->Count();index++)

				if ((index < m_pOldBindingUnits->Count()) && 
					(nFoundEqual < BindingUnits.Count()))
				{			
					const CBindingUnit* const pEqualBindingUnit = BindingUnits.Get( nFoundEqual );
					if ( pEqualBindingUnit != 0 )
					{
						const unsigned short nNewID = pEqualBindingUnit->GetID();

						if (m_pBindingNewEvaluationString->FindUnit(nNewID) == FALSE)
						{
							fResult = m_pBindingNewEvaluationString->AddUnit(nNewID);			
							return fResult;
						}
					}							
				}
				else
				{
					ASSERT( index < m_pOldBindingUnits->Count() );
					ASSERT( nFoundEqual < BindingUnits.Count() );

					fResult = FALSE;
				}
			}

			if (1)
			{
				/**
				 * Last Option : Search all units in Equal bindingUnits for id that is not in
				 * bindingEvaluation string.			 
				 */
			 
				for (int i=0;i<BindingUnits.Count();i++)
				{
					const CBindingUnit* const pEqualBindingUnit = BindingUnits.Get( i );
					if ( pEqualBindingUnit != 0 )
					{
						const unsigned short nNewID = pEqualBindingUnit->GetID();

						if (m_pBindingNewEvaluationString->FindUnit(nNewID) == FALSE)
						{
							fResult = m_pBindingNewEvaluationString->AddUnit(nNewID);			
							return fResult;
						}
						
					} // if ( pEqualBindingUnit != 0 )			 

				 } // for (int i=0;i<BindingUnits.Count();i++)
			}

		} // if (pBindingUnit != 0)

	} // if (m_pOldBindingUnits != 0 && m_pNewBindingUnits != 0)

	return fResult;
}

/**
 * BOOL CBindingEvaluationStringOperations::ReplaceUnit(unsigned short nID)
 *
 */
BOOL CBindingEvaluationStringOperations::ReplaceUnit(unsigned short nID)
{
	BOOL fResult = FALSE;

	if (m_pOldBindingUnits != 0 && m_pNewBindingUnits != 0)
	{	
		const CBindingUnit* const pBindingUnit = m_pOldBindingUnits->Find(nID);
		if (pBindingUnit != 0)
		{						
			CBindingUnitsOperations BindingUnitsOperations(m_pNewBindingUnits);

			CBindingUnit* pNewUnit = 0;	
			if (BindingUnitsOperations.FindUnit(pBindingUnit,&pNewUnit) == TRUE)
			{				
				const unsigned short nNewID = pNewUnit->GetID();
				fResult = m_pBindingNewEvaluationString->AddUnit(nNewID);				
			}

			//ASSERT( fResult );
		}
	}
	return fResult;
}

/**
 * int CBindingEvaluationStringOperations::CountEquals(unsigned short nID)
 *
 */
int CBindingEvaluationStringOperations::CountEquals(unsigned short nID)
{	
	if (m_pOldBindingUnits != 0 && m_pNewBindingUnits != 0)
	{	
		const CBindingUnit* const pBindingUnit = m_pOldBindingUnits->Find(nID);
		if (pBindingUnit != 0)
		{						
			CBindingUnitsOperations BindingUnitsOperations(m_pNewBindingUnits);
		
			return (BindingUnitsOperations.GetEquals( pBindingUnit) );
		}
	}
	return 0;
}

/*****
 *
 */
void CBindingEvaluationStringOperations::OnInputUnit(unsigned short nID) 
{
	switch( CurrentMode )
	{
		case MODE_UPDATE:
		{
			const int nEquals = CountEquals(nID);
			//ASSERT( nEquals );

			if (nEquals == 0)
			{			
				if (m_pErrorLog != 0)
				{
					m_pErrorLog->AddWarning("Forward binding missing for one of the references in C-Binding.\n");
				}
			}

			if (nEquals > 1)
			{
				if (m_pErrorLog != 0)
				{
					// (Alarm, RC5RX units ...)
					m_pErrorLog->AddWarning("Possible error: C-binding holds references with the same ID.\n");
				}
				
				if ((m_fParseCorrect = ReplaceUnit(nID,nEquals)) == FALSE)			
				{
					ASSERT(m_fParseCorrect);
				
					if (m_pErrorLog != 0)
					{
						CString szError;
						szError.Format("No common unit found for unit ID : 0x%02x\n",nID);
						m_pErrorLog->AddError(szError);
					}
				}
			}
			else
			{					
				if ((m_fParseCorrect = ReplaceUnit(nID)) == FALSE)
				{
					//ASSERT(m_fParseCorrect);

					if (m_pErrorLog != 0)
					{
						CString szError;
						szError.Format("No common unit found for unit ID : 0x%02x\n",nID);
						m_pErrorLog->AddError(szError);
					}
				}				
			}

		} break;

		case MODE_VERIFY:
		{
			
		} break;
	}
};
	
void CBindingEvaluationStringOperations::OnOutputUnit(unsigned short nID)
{
	switch( CurrentMode )
	{
		case MODE_UPDATE:
		{
			m_fParseCorrect = ReplaceUnit(nID);		
			ASSERT(m_fParseCorrect);

		} break;

		case MODE_VERIFY:
		{
			
		} break;
	}
};

void CBindingEvaluationStringOperations::OnNextUnit(unsigned short nID)
{
	switch( CurrentMode )
	{
		case MODE_UPDATE:
		{
			m_fParseCorrect = ReplaceUnit(nID);		
			ASSERT(m_fParseCorrect);

		} break;

		case MODE_VERIFY:
		{
			
		} break;
	}
};

void CBindingEvaluationStringOperations::OnTimer(unsigned short nID)
{
	switch( CurrentMode )
	{
		case MODE_UPDATE:
		{
			//ASSERT(0);	// Todo !!
			/* m_fParseCorrect = */
			//ReplaceTimer(nID);	
			//ASSERT(m_fParseCorrect);

			m_pBindingNewEvaluationString->AddTimer(nID);

		} break;

		case MODE_VERIFY:
		{
			//ASSERT(0);	// Todo !!

		} break;
	}
};

/**
 * '+','^','*','!',
 */
void CBindingEvaluationStringOperations::OnCharOperator(const char c)
{
	switch( CurrentMode )
	{
		case MODE_UPDATE:
		{
			ASSERT(m_pBindingNewEvaluationString);
			m_pBindingNewEvaluationString->Add(c);			
		
		} break;

		case MODE_VERIFY:
		{		

		} break;
	}

};

/**
 * '=','>'
 */
void CBindingEvaluationStringOperations::OnCharSyntax(const char c)
{
	switch( CurrentMode )
	{
		case MODE_UPDATE:
		{
			ASSERT(m_pBindingNewEvaluationString);
			m_pBindingNewEvaluationString->Add(c);
		
		} break;

		case MODE_VERIFY:
		{		

		} break;
	}
};

/**
 * '(', ')'
 */
void CBindingEvaluationStringOperations::OnBrackets(const char c)
{
	switch( CurrentMode )
	{
		case MODE_UPDATE:
		{
			ASSERT(m_pBindingNewEvaluationString);
			m_pBindingNewEvaluationString->Add(c);
		
		} break;

		case MODE_VERIFY:
		{		

		} break;
	}
};

void CBindingEvaluationStringOperations::OnError()
{
	switch( CurrentMode )
	{
		case MODE_UPDATE:
		{
			m_fParseCorrect = FALSE;
			//ASSERT(m_fParseCorrect);
		
		} break;

		case MODE_VERIFY:
		{		
			m_fParseFatalError = TRUE;

		} break;
	}	
};