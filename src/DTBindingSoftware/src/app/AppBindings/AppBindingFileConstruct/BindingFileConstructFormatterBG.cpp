// BindingFileConstructFormatterBG.cpp: implementation of the CBindingFileConstructFormatterBG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFormatterBG.h"
#include "BindingFileConstructFormatterB_H.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryFormatterG.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFormatterBG::CBindingFileConstructFormatterBG(CBindingEntryInfo* pBindingEntryInfo,
																   CStringArray* pArrayBindingStrings) :
CBindingFileConstructFormatter(pBindingEntryInfo,pArrayBindingStrings)
{
	m_pForwardUnit = NULL;
}

CBindingFileConstructFormatterBG::~CBindingFileConstructFormatterBG()
{

}

BOOL CBindingFileConstructFormatterBG::Format()
{	
	if (m_pBindingUnits != NULL && m_pBindingEvaluationString != NULL)
	{
		ASSERT(m_szEvaluationString.IsEmpty() == FALSE);

		const int i = m_szEvaluationString.Find("=");

		unsigned short nID;
		if (m_pBindingEvaluationString->ParseUnit(i+1,&nID) == TRUE)
		{
			m_pForwardUnit = m_pBindingUnits->Find(nID);	// Get Forward Unit ...

			if (m_pForwardUnit != NULL)
			{
				CBindingFileConstructFormatterB_H BBindingConstruct(m_pBindingEntryInfo,
																    m_pArrayBindingStrings,
																	m_pForwardUnit);
							
				if (BBindingConstruct.Format() == TRUE && 
					FormatGBindings() == TRUE)
				{
					return TRUE;
				}
			}
		}			
	}
	ASSERT(0);
	return FALSE;
}

BOOL CBindingFileConstructFormatterBG::FormatGBindings()
{		
	unsigned short nId;

	if (m_pBindingEvaluationString->ParseUnit(0,&nId) == TRUE)
	{
		CBindingUnit* const pFirstUnit = m_pBindingUnits->Find(nId);
		ASSERT(pFirstUnit);
		
		CBindingFileEntryFormatterG GBinding(m_pBindingEntryInfo,
											 m_pForwardUnit,
											 pFirstUnit);
			
		if (GBinding.Format() == TRUE) 
		{
			CString szBindingString = GBinding.GetBindingFileEntry();
			AddToStringArray(szBindingString);
			return TRUE;
		}	
	}
	return FALSE;
}
