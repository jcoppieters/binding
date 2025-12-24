#include "stdafx.h"
/*************************************************************/
#include "ArrayErrors.h"
/*************************************************************/
using namespace DUOTECNO::MFC_HELPER;
/*************************************************************/
void DUOTECNO::MFC_HELPER::CArrayErrors::
reportError(const char* szError)
{
	m_strArrayErrors.InsertAt(m_errorCnt++, szError);
}
void DUOTECNO::MFC_HELPER::CArrayErrors::
removeAll( )
{
	m_errorCnt = 0;
	m_strArrayErrors.RemoveAll();
	m_strArrayErrors.FreeExtra();
}
BOOL DUOTECNO::MFC_HELPER::CArrayErrors::
GetErrors(int i, const char** szString) 
{
	if (i<m_errorCnt) 
	{
		*szString = (const char*) m_strArrayErrors.GetAt(i);
		return TRUE;
	}
	return FALSE;
}

