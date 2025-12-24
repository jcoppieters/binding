// BindingFileEntryParserC.cpp: implementation of the CBindingFileEntryParserC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserC.h"

#include "BindingUnitFactory.h"
#include "BindingOperatorFactory.h"
#include "BindingLinkFactory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserC::CBindingFileEntryParserC() :
CBindingFileEntryParser()
{
	m_BindingUnits.RemoveAll();
	m_BindingOperators.RemoveAll();
	m_BindingEvalString.Clear();

	CurrentSubState = &CBindingFileEntryParserC::ParseNormal;	// CHANGES_MVS_2008		
}

CBindingFileEntryParserC::~CBindingFileEntryParserC()
{

}

// Sedert versie 0x0902: Extra controle op de lengte.
BOOL CBindingFileEntryParserC::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	if (m_szBindingFileEntry.GetLength()>15)
	{
		if ((m_szBindingFileEntry.GetAt(14) == 'C') || 
			(m_szBindingFileEntry.GetAt(15) == 'C'))
		{
			return (m_szBindingFileEntry.Find('>',14) == -1);
		}
	}
	return FALSE;
}



/**
 * SUB State Machine !!!
 */
BOOL CBindingFileEntryParserC::ParseBindingUnit(const char c)
{
	//TRACE("\nParse Condition Binding - Binding Unit");

	static BYTE bNodeAddress = 0;
	static BYTE bUnitAddress = 0;
	static BYTE bEvent = 0;

	switch(m_nParserCount)
	{
		case 0:
		{		
			ASSERT(c=='0');
		
			bNodeAddress = 0;
			bUnitAddress = 0;
			bEvent = 0;

		} break;

		case 1:
		case 2:
		case 3:
		{
			// ABSORBE ...
			ASSERT(c=='0');

		} break;

		case 4:
		case 5:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bNodeAddress |= (bTemp << (4*(5-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 6:
		{
			if (c != 'U') return FALSE;

		} break;

		case 7:
		case 8:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bUnitAddress |= (bTemp << (4*(8-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 9:
		{
			if (c != 'E') return FALSE;

		} break;

		case 0x0A:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bEvent = bTemp << 4;
			}
			else
			{
				return FALSE;
			}
		} break;

		case 0x0B:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bEvent |= bTemp;
			}
			else
			{
				return FALSE;
			}

			// Store BindingUnit in Collection !!!!
			CBindingUnit *pNewUnit = CBindingUnitFactory::Create();
			if (pNewUnit != NULL)
			{
				pNewUnit->SetID(0);	// !!!!!

				const unsigned short nID = m_BindingUnits.Add(pNewUnit);
				if (nID != 0)
				{
					ASSERT(nID == pNewUnit->GetID());

					pNewUnit->SetNodeAddress(bNodeAddress);
					pNewUnit->SetUnitAddress(bUnitAddress);
					pNewUnit->BindingData.Set_MessageCode(bEvent);			

					pNewUnit->BindingData.Set_Length(0);				// New !!!!
					pNewUnit->BindingData.Set_BindingFlags(0xFF);			

					// Build Evaluation String ...
					m_BindingEvalString.AddUnit(pNewUnit->GetID());	

					CurrentSubState = &CBindingFileEntryParserC::ParseNormal;	// CHANGES_MVS_2008	

					return TRUE;
				
				}
				ASSERT(0);
			}
			
			ASSERT(0);
			return FALSE;			

		} break;
		
		default:
		{
			ASSERT(0);
			return FALSE;

		} break;
	}
	m_nParserCount++;
	return TRUE;
}


BOOL CBindingFileEntryParserC::ParseTimerOperator(const char c)
{
	//TRACE("\nParse Condition Binding - Timer");

	static BYTE bValue = 0;
	static TimerType timertype = PM_TIMER_NONE;

	switch(m_nParserCount)
	{
		case 0:
		{		
			timertype = CBindingTimerOperator::Convert(c);

		} break;

		case 1:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bValue = bTemp << 4;
			}
			else
			{
				return FALSE;
			}
		} break;

		case 2:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bValue |= bTemp;

				// Add binding operator to collection ...
				CBindingTimerOperator* const pBindingOperator = 
					static_cast<CBindingTimerOperator*> 
						(CBindingOperatorFactory::Create(PM_OPERATOR_TIMER));

				if (pBindingOperator != NULL)
				{
					pBindingOperator->SetID(0);	// !!!!!
					const unsigned short nID = m_BindingOperators.Add(pBindingOperator);
					ASSERT(nID);
					ASSERT(nID == pBindingOperator->GetID());

					if (nID != 0)
					{
						pBindingOperator->SetTimerType(timertype);
						pBindingOperator->SetTimeValue(bValue);
	
						// Build Evaluation String ...
						m_BindingEvalString.AddTimer(pBindingOperator->GetID());	

						CurrentSubState = &CBindingFileEntryParserC::ParseNormal;	// CHANGES_MVS_2008	

						return TRUE;
					}
					ASSERT(0);
				}
				ASSERT(0);
				return FALSE;
			}
			else
			{
				return FALSE;
			}
		} break;
	}
	m_nParserCount++;
	return TRUE;
}


BOOL CBindingFileEntryParserC::ParseNormal(const char c)
{
	//TRACE("\nParse Condition Binding - Normal");

	switch(c)
	{
		case 'A':
		{
			m_nParserCount = 0;
			CurrentSubState = &CBindingFileEntryParserC::ParseBindingUnit;	// CHANGES_MVS_2008	
			return TRUE;

		} break;

		case '+':
		case '^':
		case '*':
		{		
			m_BindingEvalString.Add(c);

			CBindingOperator* const pBindingOperator = CBindingOperatorFactory::Create(c);
			if (pBindingOperator != NULL)
			{
				pBindingOperator->SetID(0);	// !!!
				const unsigned short nID = m_BindingOperators.Add(pBindingOperator);
				ASSERT(nID);
				ASSERT(nID == pBindingOperator->GetID());

				if (nID != 0)
				{
					return TRUE;
				}
			
			}
			ASSERT(0);
			return FALSE;

		} break;
	

		case 'T':
		{
			m_nParserCount = 0;
		
			CurrentSubState = &CBindingFileEntryParserC::ParseTimerOperator;	// CHANGES_MVS_2008	
			return TRUE;

		} break;

		case '!':
		{
			m_BindingEvalString.Add(c);			

		} break;

		case '(':
		{
			m_BindingEvalString.Add(c);			

		} break;

		case ')':
		{
			m_BindingEvalString.Add(c);

		} break;

		case '=':		
		{		
			m_BindingEvalString.Add(c);		
			m_nParserCount = 0;
			CurrentState = static_cast<ParseAction>
				(&CBindingFileEntryParserC::ParseForwardUnit);
		
			return TRUE;

		} break;		

		default:
		{
			ASSERT(0);

			return FALSE;
		}break;
	}

	m_nParserCount++;
	return TRUE;
}

/**
 * MAIN State Machine !!!
 */
BOOL CBindingFileEntryParserC::ParseConditionString(const char c)
{
	//TRACE("\nParse Condition Binding");

	if (CurrentSubState != NULL)
	{
		if ((this->*CurrentSubState) (c) == FALSE)
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}