// BindingFileEntryParserCN.cpp: implementation of the CBindingFileEntryParserCN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserCN.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserCN::CBindingFileEntryParserCN() :
CBindingFileEntryParserC()
{

}

CBindingFileEntryParserCN::~CBindingFileEntryParserCN()
{

}

// Sedert versie 0x0902: Controle op de lengte.
BOOL CBindingFileEntryParserCN::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	if(m_szBindingFileEntry.GetLength()>15)
	{
		if ((m_szBindingFileEntry.GetAt(14) == 'C') || 
			(m_szBindingFileEntry.GetAt(15) == 'C'))
		{
			return (m_szBindingFileEntry.Find('>',14) != -1);
		}
	}
	return FALSE;
}
