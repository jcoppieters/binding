// BindingEntryInfo.cpp: implementation of the CBindingEntryInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEntryInfo.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEntryInfo::CBindingEntryInfo(CBindingEntry *pBindingEntry) :
m_pBindingEntry(pBindingEntry)
{
	ASSERT(m_pBindingEntry);
}

CBindingEntryInfo::~CBindingEntryInfo()
{

}

// ------------------ m_fInverseOperator --------------------------------

void CBindingEntryInfo::Set_Inverse(BOOL flag)
{
	m_fInverseOperator = flag; 
};
BOOL CBindingEntryInfo::Is_Inverted(void) const			
{
	return m_fInverseOperator; 
};

// ------------------ m_bBindingReturnFlags --------------------------------

BYTE CBindingEntryInfo::Get_BindingReturnFlags(void) const		
{
	return m_bBindingReturnFlags;
};

void CBindingEntryInfo::Set_BindingReturnFlags(BYTE flags)	
{
	m_bBindingReturnFlags = flags; 
};

// ------------------ m_NrOfOutputs --------------------------------
void CBindingEntryInfo::Set_NOutputs(int outputs)			
{
	m_NrOfOutputs = outputs;
};

int CBindingEntryInfo::Get_NOutputs(void) const		
{
	return m_NrOfOutputs;
};

// ------------------ m_BindingType --------------------------------
void CBindingEntryInfo::Set_Type(BINDINGTYPE Type)		
{
	m_BindingType = Type; 
};

BINDINGTYPE CBindingEntryInfo::Get_Type(void) const		
{
	return m_BindingType; 
};

// ------------------ m_NrOfTimers --------------------------------

void CBindingEntryInfo::Set_NTimers(int timers)
{
	m_NrOfTimers = timers;
};

int CBindingEntryInfo::Get_NTimers(void) const
{
	return m_NrOfTimers;
};

// ------------------ m_NrOfInputs --------------------------------

void CBindingEntryInfo::Set_NInputs(int inputs)			
{
	m_NrOfInputs = inputs;
};

int CBindingEntryInfo::Get_NInputs(void) const			
{
	return m_NrOfInputs;
};

//////////////////////////////////////////////////////////////////////
void CBindingEntryInfo::Clear() 
{
	m_NrOfTimers = 0;
	m_NrOfInputs = 0;
	m_NrOfOutputs = 0;

	m_fInverseOperator = FALSE;
	m_bBindingReturnFlags = BINDINGFLAGS_STATUS_NOCHANGE;
	m_BindingType = TYPE_UNDEFINED_BINDING;
}


BOOL CBindingEntryInfo::Build()
{
	ASSERT(m_pBindingEntry);

	if (m_pBindingEntry != NULL)
	{
		m_bBindingReturnFlags = m_pBindingEntry->GetBindingReturnFlags();

		CBindingEvaluationString *const pBindingEvaluationString = m_pBindingEntry->GetEvaluationString();
		if (pBindingEvaluationString != NULL)
		{
			m_NrOfTimers = pBindingEvaluationString->GetNrOfTimers();
			m_NrOfInputs = pBindingEvaluationString->GetNrOfInputs();
			m_NrOfOutputs = pBindingEvaluationString->GetNrOfOutputs();
			m_fInverseOperator = pBindingEvaluationString->Is_Inverse();			
			
			return (FillBindingType());
		}
	}
	return FALSE;
}


// Bepalen van het binding type...
BOOL CBindingEntryInfo::FillBindingType()
{
	if (m_NrOfInputs == 1)
	{
		if ( m_NrOfTimers == 0 && 
			 m_NrOfOutputs == 1 && 
			 m_bBindingReturnFlags == BINDINGFLAGS_STATUS_NOCHANGE)
		{
			CBindingUnits* const pBindingUnits = m_pBindingEntry->GetColUnit();
			if (pBindingUnits != NULL)
			{
				BOOL fResult = FALSE;

				for (int i=0;i<2;i++) 
				{
					CBindingUnit* pBindingUnit = pBindingUnits->Get(i);
					if (pBindingUnit == 0)
					{
						ASSERT(0);
						return FALSE;
					}
			
					if (pBindingUnit->GetInOutType() == UNIT_TYPE_INPUT)
					{
						switch(pBindingUnit->GetBindingUnitType())
						{
							// Onderstaande zorgt ervoor dat er altijd een 
							// TYPE_BG_BINDING binding aangemaakt wordt.
							case BINDING_UNIT_TYPE_SENS:
							case BINDING_UNIT_TYPE_SENSUNIV:	// Sedert versie 0x0D00
							{
								fResult = TRUE;		/* Exception !! */
							
							} break;
						}	
					}				
					if (fResult == TRUE) 
					{
						break;
					}
				}
				
				if (fResult == FALSE) 
				{
					m_BindingType = TYPE_I_BINDING;	/* Default Type !!! */
				}
				else 
				{
					m_BindingType = TYPE_BG_BINDING; /* Exceptions!! */
				}				
				return TRUE;
			}
			return FALSE;
		} // if ( m_NrOfTimers == 0 && m_NrOfOutputs == 1 && m_bBindingFlags == 0)

		else if ( m_NrOfTimers == 0 && 
				  m_fInverseOperator == FALSE )
		{
			if (m_NrOfOutputs == 1) 
			{
				m_BindingType = TYPE_BG_BINDING;
				return TRUE;
			}
			else 
			{
				m_BindingType = TYPE_BGN_BINDING;
				return TRUE;
			}
		}

	} // if (m_NrOfInputs == 1)

	if (m_NrOfOutputs > 1) 
	{
		m_BindingType = TYPE_BCN_BINDING;
		return TRUE;
	}
	else 
	{
		m_BindingType = TYPE_BC_BINDING;
		return TRUE;
	}

	ASSERT(0);
	return FALSE;
}




BOOL CBindingEntryInfo::Verify()	// Design By Contract !!!
{
	CBindingUnits* const pBindingUnits = m_pBindingEntry->GetColUnit();
	CBindingOperators* const pBindingOperators = m_pBindingEntry->GetColOperator();
	CBindingLinks* const pBindingLinks = m_pBindingEntry->GetColLink();

	if (pBindingUnits != NULL && pBindingOperators != NULL && pBindingLinks != NULL)
	{
		if (pBindingUnits->Count() != (m_NrOfInputs+m_NrOfOutputs))
		{
			ASSERT(0);
			return FALSE;
		}			

		switch(m_BindingType)
		{
			case TYPE_BG_BINDING:
			{
				if (pBindingUnits->Count() > 2) 
				{
					ASSERT(0);
					return FALSE;
				}

				if (pBindingOperators->Count() != 0)
				{
					ASSERT(0);
					return FALSE;
				}

				if (pBindingLinks->Count() != 1)
				{
					ASSERT(0);
					return FALSE;
				}

				if (m_NrOfInputs != 1) 
				{
					ASSERT(0);
					return FALSE;
				}

				if (m_fInverseOperator == TRUE) 
				{
					ASSERT(0);
					return FALSE;
				}			

			} break;

			case TYPE_BGN_BINDING:
			{
				if (pBindingUnits->Count() < 2) 
				{
					ASSERT(0);
					return FALSE;
				}

				if (pBindingOperators->Count() != 0)
				{
					ASSERT(0);
					return FALSE;
				}

				if (pBindingLinks->Count() < 2)
				{
					ASSERT(0);
					return FALSE;
				}
				
				if (m_NrOfOutputs == 1)
				{
					ASSERT(0);
					return FALSE;
				}

				if (m_NrOfInputs != 1) 
				{
					ASSERT(0);
					return FALSE;
				}

				if (m_fInverseOperator == TRUE) 
				{
					ASSERT(0);
					return FALSE;
				}			
			} break;

			case TYPE_BC_BINDING:
			{			
				if (m_NrOfOutputs > 1)
				{
					ASSERT(0);
					return FALSE;
				}

				if (m_fInverseOperator == FALSE && pBindingOperators->Count() == 0) 
				{
					ASSERT(0);
					return FALSE;
				}		

			} break;

			case TYPE_BCN_BINDING:
			{
				if (m_NrOfOutputs == 1)
				{
					ASSERT(0);
					return FALSE;
				}

				if (m_fInverseOperator == FALSE && pBindingOperators->Count() == 0) 
				{
					ASSERT(0);
					return FALSE;
				}		
				
			} break;

			case TYPE_I_BINDING:
			{
				if (pBindingUnits->Count() > 2) 
				{
					ASSERT(0);
					return FALSE;
				}

				if (pBindingOperators->Count() != 0)
				{
					ASSERT(0);
					return FALSE;
				}

				if (pBindingLinks->Count() != 1)
				{
					ASSERT(0);
					return FALSE;
				}

				if (m_NrOfInputs != 1) 
				{
					ASSERT(0);
					return FALSE;
				}

			} break;

			default:
			{
				ASSERT(0);
				return FALSE;

			} break;		
		}

		return TRUE;
	}

	ASSERT(0);

	return FALSE;
}