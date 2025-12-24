// BindingFileEntryFormatterNN.cpp: implementation of the CBindingFileEntryFormatterNN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryFormatterNN.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryFormatterNN::CBindingFileEntryFormatterNN(CBindingEntryInfo* pBindingEntryInfo,
														   CBindingUnit* pForwardUnit,
														   CBindingUnit* pReturnUnit,
														   CBindingUnit* pNextUnit) :
CBindingFileEntryFormatterN(pBindingEntryInfo,pForwardUnit,pReturnUnit),
m_pNextUnit(pNextUnit)
{
	ASSERT( m_pNextUnit );
}

CBindingFileEntryFormatterNN::~CBindingFileEntryFormatterNN()
{

}

BOOL CBindingFileEntryFormatterNN::Format()
{
	ASSERT(m_pBindingEntry);
	ASSERT(m_pForwardUnit);
	ASSERT(m_pReturnUnit);
	ASSERT(m_pNextUnit);

	m_szBindingFileEntry.Empty();
	
	if (m_pNextUnit != NULL)
	{						
		//if (static_cast<CBindingFileEntryTypeN*>(this)->Format() == TRUE)
		if (this->CBindingFileEntryFormatterN::Format() == TRUE)
		{			
			CString szBindingNext;
			szBindingNext.Format(_T(">A0000%02XU%02XS"),
								 m_pNextUnit->GetNodeAddress(),
								 m_pNextUnit->GetUnitAddress());

			if (m_szBindingFileEntry.Replace("S",szBindingNext) == TRUE)
			{
				return TRUE;
			}
			ASSERT(0);
		}
		ASSERT(0);
	}
	ASSERT(0);
	return FALSE;
}
