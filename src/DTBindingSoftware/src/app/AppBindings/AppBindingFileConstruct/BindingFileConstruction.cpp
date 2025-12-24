// BindingFileConstruction.cpp: implementation of the CBindingFileConstruction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstruction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFileConstruction::CBindingFileConstruction(CStringArray* pStrArray) :
m_pStringArray(pStrArray),
m_BindingEntryInfo(&m_BindingEntry),
m_BindingFileConstructionI(m_pStringArray),
m_BindingFileConstructionBG(m_pStringArray),
m_BindingFileConstructionBGN(m_pStringArray),
m_BindingFileConstructionBC(m_pStringArray),
m_BindingFileConstructionBCN(m_pStringArray)
{
	m_pBindingConstructions[0] = &m_BindingFileConstructionI;
	m_pBindingConstructions[1] = &m_BindingFileConstructionBG;
	m_pBindingConstructions[2] = &m_BindingFileConstructionBGN;
	m_pBindingConstructions[3] = &m_BindingFileConstructionBC;
	m_pBindingConstructions[4] = &m_BindingFileConstructionBCN;

	m_BindingEntry.Clear();
	m_BindingEntryInfo.Clear();

#ifdef _DEBUG	
	if (1)
	{
		for (int i=0;i<m_pStringArray->GetSize();i++)
		{
			TRACE1("\n%s",m_pStringArray->GetAt(i));
		}		
	}
#endif
}

CBindingFileConstruction::~CBindingFileConstruction()
{

}

BOOL CBindingFileConstruction::Construct()
{
	ASSERT(m_pStringArray);

	for (int i=0;i<MAX_BINDINGCONSTRUCTION_TYPES;i++)
	{		
		if (m_pBindingConstructions[i]->Construct(&m_BindingEntryInfo) == TRUE)
		{					
			return TRUE;
		}		
	}
	return FALSE;
}
