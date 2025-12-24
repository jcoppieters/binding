// BindingEvaluationString.cpp: implementation of the CBindingEvaluationString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEvaluationString.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEvaluationString::CBindingEvaluationString(CString s)
{
	m_szEvaluationString = s;

	m_TimerCnt = 0;
	m_InputCnt = 0;
	m_OutputCnt = 0;
	m_fInverse = FALSE;
}

CBindingEvaluationString::~CBindingEvaluationString()
{

}

//////////////////////////////////////////////////////////////////////
void CBindingEvaluationString::Clear()
{
	m_szEvaluationString.Empty();

	m_TimerCnt = 0;
	m_InputCnt = 0;
	m_OutputCnt = 0;

	m_fInverse = FALSE;
}

CString CBindingEvaluationString::GetInput()
{
	if (m_szEvaluationString.IsEmpty() == FALSE)
	{
		const int nIndex = m_szEvaluationString.Find('=');		
		return (m_szEvaluationString.Left(nIndex));	
	}
	return CString("");
}

CString CBindingEvaluationString::GetOutput()
{
	if (m_szEvaluationString.IsEmpty() == FALSE)
	{
		const int nIndex = m_szEvaluationString.Find('=');
		const int nLength = m_szEvaluationString.GetLength();		
		return (m_szEvaluationString.Right((nLength-1)-nIndex));
	}
	return CString("");
}


BOOL CBindingEvaluationString::Parse()
{
	const int nLen = m_szEvaluationString.GetLength();

	BYTE bUnitType = UNIT_TYPE_INPUT;
	int iForwardOperator = 0;

	m_TimerCnt = 0;
	m_InputCnt = 0;
	m_OutputCnt = 0;
	m_fInverse = FALSE;

	for (int i=0;i<nLen;i++) 
	{
		const char c = m_szEvaluationString.GetAt(i);

		switch(c) 
		{
			case 'X' : 
			case 'Y' :
			{
				ASSERT(0);

			} return FALSE;

			case 'T':
			{
				m_TimerCnt++;

			} break;

			case 'U':
			{
				if (bUnitType == UNIT_TYPE_INPUT) 
				{
					m_InputCnt++;
				}
				else 
				{
					m_OutputCnt++;
				}

			} break;

			case '=':
			{
				bUnitType = UNIT_TYPE_OUTPUT;

			} break;

			case '!':
			{			
				m_fInverse = TRUE;
			} break;

			case '>':
			{
				iForwardOperator++;

			} break;
		}
	}
	
	if (iForwardOperator+1 != m_OutputCnt) 
	{
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}


BOOL CBindingEvaluationString::ParseUnit(int IndexCh,unsigned short *nID)
{
	ASSERT(nID);
	
	if (nID != NULL)
	{		
		char ch = '\0';		// CHANGES_MVS_2008_SSCANF
		int temp = -1;

		const char* const szDataToParse = (LPCTSTR) m_szEvaluationString;	// CHANGES_MVS_2008_CSTRING
		if (sscanf(&szDataToParse[IndexCh],"%c%i",&ch,&temp) == 2)			// CHANGES_MVS_2008_SSCANF
		{
			if (ch == 'U')
			{
				*nID = static_cast<unsigned short>(temp);
				return TRUE;
			}

			ASSERT(0);
		}
		else
		{
			ASSERT(0);
		}
	}
	
	ASSERT(0);	
	return FALSE;
}

BOOL CBindingEvaluationString::ParseUnit(int IndexCh,unsigned short *nID,char *chOperator)
{
	ASSERT(nID);
	ASSERT(chOperator);
	
	if (nID != NULL)
	{		
		char ch;
		int temp = -1;
		const char* const szDataToParse = (LPCTSTR) m_szEvaluationString;	// CHANGES_MVS_2008_CSTRING

		if (sscanf(&szDataToParse[IndexCh],"%c%i%c",		// CHANGES_MVS_2008_SSCANF
				   &ch,
				   &temp,
				   chOperator) == 3) 
		{
			if (ch == 'U')
			{
				*nID = static_cast<unsigned short>(temp);
				return TRUE;
			}

			ASSERT(0);
		}
	}
	
	ASSERT(0);	
	return FALSE;
}

BOOL CBindingEvaluationString::ParseTimer(int IndexCh,unsigned short* nID)
{
	ASSERT(nID);

	if (nID != NULL)
	{		
		char ch = '\0';
		int temp = -1;
		const char* const szDataToParse = (LPCTSTR) m_szEvaluationString;	// CHANGES_MVS_2008_CSTRING

		if (sscanf(&szDataToParse[IndexCh],"%c%i",&ch,&temp) == 2) // CHANGES_MVS_2008_SSCANF
		{
			if (ch == 'T')
			{
				*nID = static_cast<unsigned short>(temp);
				return TRUE;
			}

			ASSERT(0);
		}
	}
	
	ASSERT(0);	
	return FALSE;
}

BOOL CBindingEvaluationString::FormatUnit(CString& szSubString,unsigned short nID,char chOperator)
{
	szSubString.Format("U%i%c",nID,chOperator);
	return TRUE;
}

BOOL CBindingEvaluationString::FormatUnit(CString& szSubString,unsigned short nID)
{
	szSubString.Format("U%i",nID);
	return TRUE;
}

BOOL CBindingEvaluationString::FormatTimer(CString& szSubString,unsigned short nID)
{
	szSubString.Format("T%i",nID);
	return TRUE;
}

/**
 * Used by Parser ...
 */
BOOL CBindingEvaluationString::AddUnit(unsigned short nID)
{
	CString szTemp;
	if (FormatUnit(szTemp,nID) == TRUE)
	{
		m_szEvaluationString+= szTemp;
		return TRUE;
	}
	return FALSE;
}
BOOL CBindingEvaluationString::AddTimer(unsigned short nID)
{
	CString szTemp;
	if (FormatTimer(szTemp,nID) == TRUE)
	{
		m_szEvaluationString+= szTemp;
		return TRUE;
	}
	return FALSE;
}
BOOL CBindingEvaluationString::Add(const char c)
{
	m_szEvaluationString += c;

	return TRUE;
}

BOOL CBindingEvaluationString::FindUnit(unsigned short nID)
{
	CString szTemp;
	if (FormatUnit(szTemp,nID) == TRUE)
	{
		return ( m_szEvaluationString.Find(szTemp) != -1 );
#ifdef SKIP
#ifdef _DEBUG
		CString szDebug;
		szDebug.Format("ID:0x%02x,Found:%i",nID,nFound);
		TRACE1("\n%s",szDebug);
#endif
#endif
	}
	return FALSE;
}