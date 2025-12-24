// BindingFileEntryParserP.cpp: implementation of the CBindingFileEntryParserP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserP::CBindingFileEntryParserP() :
CBindingFileEntryParser()
{
}
CBindingFileEntryParserP::~CBindingFileEntryParserP()
{
}

// Sedert versie 0x0902 extra controle op de lengte. 
BOOL CBindingFileEntryParserP::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	return ((m_szBindingFileEntry.GetLength()>15)&&
		    ((m_szBindingFileEntry.GetAt(14) == 'P') || 
			 (m_szBindingFileEntry.GetAt(15) == 'P')));
}
