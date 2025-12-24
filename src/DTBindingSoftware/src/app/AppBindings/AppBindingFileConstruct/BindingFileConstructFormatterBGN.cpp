// BindingFileConstructFormatterBGN.cpp: implementation of the CBindingFileConstructFormatterBGN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFormatterBGN.h"

#include "BindingFileConstructFormatterN_H.h"
#include "BindingFileConstructFormatterB_H.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryFormatterGN.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFormatterBGN::CBindingFileConstructFormatterBGN(CBindingEntryInfo* pBindingEntryInfo,
																   CStringArray* pArrayBindingStrings) :
CBindingFileConstructFormatterBG(pBindingEntryInfo,pArrayBindingStrings)
{

}

CBindingFileConstructFormatterBGN::~CBindingFileConstructFormatterBGN()
{

}

BOOL CBindingFileConstructFormatterBGN::Format()
{	
	if (m_pBindingUnits != NULL && m_pBindingEvaluationString != NULL)
	{	
		ASSERT(m_szEvaluationString.IsEmpty() == FALSE);

		const int i = m_szEvaluationString.Find("=");

		unsigned short nID;
		if (m_pBindingEvaluationString->ParseUnit(i+1,&nID) == TRUE)
		{
			m_pForwardUnit = m_pBindingUnits->Find(nID);	// Get Forward Unit ...
			
			CBindingFileConstructFormatterB_H BBindingConstruct(m_pBindingEntryInfo,
															    m_pArrayBindingStrings,
																m_pForwardUnit);
				
			if (BBindingConstruct.Format() == TRUE)
			{
				if (FormatGNBindings() == TRUE)
				{
					CBindingFileConstructFormatterN_H NBindingConstruct(m_pBindingEntryInfo,
																		m_pArrayBindingStrings);
															
					if (NBindingConstruct.Format() == TRUE)
					{						
						return TRUE;
					}
				}
			}
		}			
	}
	ASSERT(0);
	return FALSE;
}


BOOL CBindingFileConstructFormatterBGN::FormatGNBindings()
{		
	unsigned short nId;

	if (m_pBindingEvaluationString->ParseUnit(0,&nId) == TRUE)
	{
		CBindingUnit* const pFirstUnit = m_pBindingUnits->Find(nId);
		ASSERT(pFirstUnit);

		const int index = m_szEvaluationString.Find('>');
		
		if (index != -1)
		{
			if (m_pBindingEvaluationString->ParseUnit(index+1,&nId) == TRUE)
			{
		
				CBindingUnit* const pNextUnit = m_pBindingUnits->Find(nId);		
				CBindingFileEntryFormatterGN GNBinding(m_pBindingEntryInfo,
													   m_pForwardUnit,
													   pFirstUnit,
													   pNextUnit);
						
				if (GNBinding.Format() == TRUE) 
				{
					CString szBindingString = GNBinding.GetBindingFileEntry();
					AddToStringArray(szBindingString);
					return TRUE;
				}	
			}
		}
	}
	return FALSE;
}
