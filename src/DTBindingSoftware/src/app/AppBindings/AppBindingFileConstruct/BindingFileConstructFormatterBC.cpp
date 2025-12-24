// BindingFileConstructFormatterBC.cpp: implementation of the CBindingFileConstructFormatterBC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFormatterBC.h"
#include "BindingFileConstructFormatterB_H.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryFormatterC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFormatterBC::CBindingFileConstructFormatterBC(CBindingEntryInfo* pBindingEntryInfo,
																   CStringArray* pArrayBindingStrings) :
CBindingFileConstructFormatter(pBindingEntryInfo,pArrayBindingStrings)
{
	m_pForwardUnit = NULL;
}

CBindingFileConstructFormatterBC::~CBindingFileConstructFormatterBC()
{

}

BOOL CBindingFileConstructFormatterBC::Format()
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

				if (BBindingConstruct.Format() == TRUE && FormatCBindings() == TRUE)
				{
					return TRUE;
				}
			}
		}			
	}
	ASSERT(0);
	return FALSE;
}

BOOL CBindingFileConstructFormatterBC::FormatCBindings()
{
	CBindingFileEntryFormatterC CBinding(m_pBindingEntryInfo,
										m_pForwardUnit);
	
	if (CBinding.Format() == TRUE) 
	{
		CString szBindingString = CBinding.GetBindingFileEntry();
		AddToStringArray(szBindingString);
		return TRUE;
	}	
	return FALSE;
}
