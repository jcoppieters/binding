// BindingFileConstructFormatterB_H.cpp: implementation of the CBindingFileConstructFormatterB_H class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFormatterB_H.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryFormatterB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFormatterB_H::CBindingFileConstructFormatterB_H(CBindingEntryInfo* pBindingEntryInfo,
																	 CStringArray* pArrayBindingStrings,
																	 CBindingUnit* pForwardUnit) :
CBindingFileConstructFormatter(pBindingEntryInfo,pArrayBindingStrings),
m_pForwardUnit(pForwardUnit)
{
	ASSERT(m_pForwardUnit);
	ASSERT(pBindingEntryInfo);
}

CBindingFileConstructFormatterB_H::~CBindingFileConstructFormatterB_H()
{

}

BOOL CBindingFileConstructFormatterB_H::Format()
{
	if (m_pBindingEntryInfo == 0)
	{
		ASSERT(m_pBindingEntryInfo);
		return FALSE;
	}

	CBindingEntry* const pBindingEntry = m_pBindingEntryInfo->GetBindingEntry();
	if (pBindingEntry == 0)
	{
		ASSERT(pBindingEntry);
		return FALSE;
	}

	if (m_pForwardUnit == 0)
	{
		ASSERT(m_pForwardUnit);
		return FALSE;
	}

	CString szEvaluationString = pBindingEntry->GetEvaluationString()->Get();

	if (m_pForwardUnit != 0)
	{					
		BYTE bUnitType;
		const int nLen = m_szEvaluationString.GetLength();	
		
		int nInputs = m_pBindingEntryInfo->Get_NInputs();
		
		for (int i=0;i<nLen;i++) 
		{
			const char c = szEvaluationString.GetAt(i);

			switch(c) 
			{
				case 'U':
				{				
					nInputs--;

					unsigned short nID;
					CBindingUnit *pReturnUnit = NULL;

					if (m_pBindingEvaluationString->ParseUnit(i,&nID) == TRUE)
					{					
						// Get Return Unit ...
						ASSERT( pBindingEntry->GetColUnit() );

						if ((pReturnUnit = pBindingEntry->GetColUnit()->Find(nID)) != NULL)
						{										
							CBindingFileEntryFormatterB BBinding(m_pBindingEntryInfo,
																 m_pForwardUnit,
																 pReturnUnit);
							
							if (BBinding.Format() == TRUE) 
							{					
								CString szBindingString = BBinding.GetBindingFileEntry();
								m_pArrayBindingStrings->Add(szBindingString);							
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

				case '=':
				{
					bUnitType = UNIT_TYPE_OUTPUT;
					ASSERT(0);

				} break;

				case '>':
				{
					ASSERT(0);

				} break;
			}

			if (nInputs == 0) 
			{
				break;	// all Forward Bindings Made ...
			}
		}
	}

	return TRUE;
}
