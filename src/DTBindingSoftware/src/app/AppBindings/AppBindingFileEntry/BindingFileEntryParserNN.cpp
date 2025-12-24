// BindingFileEntryParserNN.cpp: implementation of the CBindingFileEntryParserNN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserNN.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserNN::CBindingFileEntryParserNN() :
CBindingFileEntryParserN()
{

}

CBindingFileEntryParserNN::~CBindingFileEntryParserNN()
{

}

// Sedert versie 0x0902 extra controle op de lengte. 
BOOL CBindingFileEntryParserNN::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	if (m_szBindingFileEntry.GetLength()>15)
	{
		if ((m_szBindingFileEntry.GetAt(14) == 'N') || 
			(m_szBindingFileEntry.GetAt(15) == 'N'))
		{
			return (m_szBindingFileEntry.Find('>',14) != -1);
		}
	}
	return FALSE;
}



