// BindingFileConstructFormatterBCN.cpp: implementation of the CBindingFileConstructFormatterBCN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFormatterBCN.h"

#include "BindingFileConstructFormatterN_H.h"
#include "BindingFileConstructFormatterB_H.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryFormatterCN.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFormatterBCN::CBindingFileConstructFormatterBCN(CBindingEntryInfo* pBindingEntryInfo,
																     CStringArray* pArrayBindingStrings) :
CBindingFileConstructFormatterBC(pBindingEntryInfo,pArrayBindingStrings)

{

}

CBindingFileConstructFormatterBCN::~CBindingFileConstructFormatterBCN()
{

}

BOOL CBindingFileConstructFormatterBCN::Format()
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
				if (FormatCNBindings() == TRUE)
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

BOOL CBindingFileConstructFormatterBCN::FormatCNBindings()
{
	CBindingFileEntryFormatterCN CNBinding(m_pBindingEntryInfo,m_pForwardUnit);
	
	if (CNBinding.Format() == TRUE) 
	{
		CString szBindingString = CNBinding.GetBindingFileEntry();
		AddToStringArray(szBindingString);
		return TRUE;
	}	
	return FALSE;
}

