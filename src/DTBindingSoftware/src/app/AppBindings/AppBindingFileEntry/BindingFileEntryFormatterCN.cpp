// BindingFileEntryFormatterCN.cpp: implementation of the CBindingFileEntryFormatterCN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryFormatterCN.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryFormatterCN::CBindingFileEntryFormatterCN(CBindingEntryInfo* pBindingEntryInfo,
														   CBindingUnit* pForwardUnit) :
CBindingFileEntryFormatterC(pBindingEntryInfo,pForwardUnit)
{
}

CBindingFileEntryFormatterCN::~CBindingFileEntryFormatterCN()
{

}

/**
 *
 */
BOOL CBindingFileEntryFormatterCN::Format()
{
	CBindingUnits* const pBindingUnits = m_pBindingEntry->GetColUnit();
	CBindingOperators* const pBindingOperators = m_pBindingEntry->GetColOperator();

	//if (static_cast<CBindingFileEntryTypeC*>(this)->Format() == TRUE)
	
	if (this->CBindingFileEntryFormatterC::Format() == TRUE)
	{		
		CBindingEvaluationString* const pBindingEvaluationString =
			m_pBindingEntry->GetEvaluationString();

		if (pBindingEvaluationString == NULL)
		{
			ASSERT(0);
			return FALSE;
		}

		CString szEvaluationString = pBindingEvaluationString->Get();

		int index = szEvaluationString.Find("=");

		const char* const szDataToParse = (LPCTSTR) szEvaluationString;	// CHANGES_MVS_2008_CSTRING
		const int nLen = szEvaluationString.GetLength();

		for (int i=index;i<nLen;i++) 
		{
			const char c = szEvaluationString.GetAt(i);

			switch(c) 
			{
				case '>':
				{
					// Get pNextUnit ...
					unsigned short nId;

					if (pBindingEvaluationString->ParseUnit(i+1,&nId) == TRUE)
					{
						CString szTemp;
						if (pBindingEvaluationString->FormatUnit(szTemp,nId) == TRUE)
						{
							CString szTemp2 = '>' + szTemp;

							CBindingUnit* const pNextUnit = pBindingUnits->Find(nId);

							if (pNextUnit != NULL)
							{																		
								CString szEventString;
								szEventString.Format(_T(">A0000%02XU%02X"),pNextUnit->GetNodeAddress(),
																   		   pNextUnit->GetUnitAddress());
				
								const int n = m_szBindingFileEntry.Replace(szTemp2,szEventString);
								if (n == 0)
								{
									ASSERT(0);
									return FALSE;
								}

								return TRUE;
							}
							else
							{
								ASSERT(0);
								return FALSE;
							}
						}
						else
						{
							ASSERT(0);
							return FALSE;
						}
					}
					else
					{
						ASSERT(0);
						return FALSE;
					}
				} break;

			} // switch(c)
		} // end for ...
	}
	return FALSE;
}



