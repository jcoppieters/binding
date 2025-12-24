// BindingFileConstructFormatterN_H.cpp: implementation of the CBindingFileConstructFormatterN_H class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFormatterN_H.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryFormatterNN.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFormatterN_H::CBindingFileConstructFormatterN_H(CBindingEntryInfo* CBindingFileConstructFormatterN_H,
																	 CStringArray* pArrayBindingStrings) :
CBindingFileConstructFormatter(CBindingFileConstructFormatterN_H,pArrayBindingStrings)
{
	
}

CBindingFileConstructFormatterN_H::~CBindingFileConstructFormatterN_H()
{

}

BOOL CBindingFileConstructFormatterN_H::Format()
{
	ASSERT(m_pBindingEntryInfo);
	ASSERT(m_pArrayBindingStrings);

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

	CBindingEvaluationString* const pBindingEvaluationString =
		pBindingEntry->GetEvaluationString();
	if (pBindingEvaluationString == 0)
	{
		ASSERT(pBindingEvaluationString);
		return FALSE;
	}

	CString szEvaluationString = pBindingEvaluationString->Get();	
	unsigned short nId;
	int nIndex = szEvaluationString.Find("=",0);	

	if (nIndex == -1) 
	{
		ASSERT(0);
		return FALSE;
	}

	int nOutputs = m_pBindingEntryInfo->Get_NOutputs();

	if (m_pBindingEvaluationString->ParseUnit(nIndex+1,&nId) == TRUE)
	{
		CBindingUnit* const pFirstUnit = pBindingEntry->GetColUnit()->Find(nId);
		ASSERT(pFirstUnit);
	
		CString szBindingString;

		CBindingUnit *pUnit = NULL;
		CBindingUnit *pForwardUnit = NULL;
		CBindingUnit *pNextUnit = NULL;

		int nPrevIndex = nIndex;

		for (int i=0;i<nOutputs-1;i++) 
		{	
			nIndex = nPrevIndex;      // !!!!!!

			nIndex = m_szEvaluationString.Find(">",nIndex+1);
			nPrevIndex = nIndex;
			
			if (pBindingEvaluationString->ParseUnit(nIndex+1,&nId) == TRUE)
			{
				pForwardUnit = m_pBindingEntry->GetColUnit()->Find(nId);
				int j=i;

				if (j<nOutputs-2) 
				{
					nIndex = szEvaluationString.Find(">",nIndex+1);
					
					if (pBindingEvaluationString->ParseUnit(nIndex+1,&nId) == TRUE)
					{
						pNextUnit = m_pBindingEntry->GetColUnit()->Find(nId);
						
						CBindingFileEntryFormatterNN NNBinding(m_pBindingEntryInfo,
															  pForwardUnit,
															  pFirstUnit,
															  pNextUnit);

						if (NNBinding.Format() == TRUE)
						{
							m_pArrayBindingStrings->Add(NNBinding.GetBindingFileEntry());						
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
					CBindingFileEntryFormatterN NBinding(m_pBindingEntryInfo,
														pForwardUnit,
														pFirstUnit);
					if (NBinding.Format() == TRUE)
					{
						m_pArrayBindingStrings->Add(NBinding.GetBindingFileEntry());					
					}
					else
					{
						ASSERT(0);
						return FALSE;
					}					
				}
			}
		} // end forloop ...	

		return TRUE;		
	}
	return FALSE;
}
