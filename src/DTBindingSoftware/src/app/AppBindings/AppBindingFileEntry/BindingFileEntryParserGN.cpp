// BindingFileEntryParserGN.cpp: implementation of the CBindingFileEntryParserGN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserGN.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserGN::CBindingFileEntryParserGN() :
CBindingFileEntryParserG()
{

}

CBindingFileEntryParserGN::~CBindingFileEntryParserGN()
{

}

// Sedert versie 0x0902 extra controle op de lengte.
BOOL CBindingFileEntryParserGN::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	if(m_szBindingFileEntry.GetLength()>15)
	{
		if ((m_szBindingFileEntry.GetAt(14) == 'G') || 
			(m_szBindingFileEntry.GetAt(15) == 'G'))
		{
			return (m_szBindingFileEntry.Find('>',14) != -1);
		}
	}
	return FALSE;
}
