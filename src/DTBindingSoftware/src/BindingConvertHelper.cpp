// BindingConvertHelper.cpp: implementation of the CBindingConvertHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConvertHelper.h"

#include "MyWindowMessages.h"
#include "MainFrm.h"
#include "views/main/MyFrameWnd.h"		// ???
#include "views/debug/DlgDebugBindingConversion.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingConvertHelper::CBindingConvertHelper()
{
}

CBindingConvertHelper::~CBindingConvertHelper()
{
}

/****************************************************************************************
 * BOOL CBindingConvertHelper::ConvertOperatorType2Char(OperatorType Type,char *c)
 * Param1: OperatorType Type [in]
 * Param2: char *c [Out]
 * 
 * PM_OPERATOR_TIMER => 'Y'
 * PM_OPERATOR_AND, PM_OPERATOR_OR, PM_OPERATOR_XOR => 'X'
 *
 ****************************************************************************************/
BOOL CBindingConvertHelper::ConvertOperatorType2Char(const OperatorType Type,char *c)
{	
	ASSERT(c);

	switch(Type)
	{
		case PM_OPERATOR_TIMER:
		{ 
			*c='Y'; 

		} return TRUE;

		default: 
		{
			*c='X'; 

		} return TRUE;
	}
	return TRUE;
}

BOOL CBindingConvertHelper::ParseOutputs(CString szData,char *cData,unsigned short *nId,int nUnitNr)
{
	int i,nCnt;
	char c;

	*cData = '\0';
	*nId = 0;

	for (i=0,nCnt=0;i<szData.GetLength();i++)
	{
		c = szData.GetAt(i);
		if (c=='U')
		{
			*cData = c;
			if (nCnt++ == nUnitNr)
			{
				int temp = -1;
				const char* const szDataToParse = (LPCTSTR) szData;			// CHANGES_MVS_2008_CSTRING		

				if (sscanf(&szDataToParse[i+1],"%i",&temp) == 0)			// CHANGES_MVS_2008_SSCANF
				{
					return FALSE;	
				}

				*nId = static_cast<unsigned short>(temp);				
				return TRUE;
			}
		}
		else if (c == '=') 
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CBindingConvertHelper::ParseInputs(CString szData,char *cData,unsigned short *nId,int nUnitNr)
{
	char c;
	int i,nCnt;	
	int nIndex = 0;
	
	*cData = '\0';	
	*nId = 0;
	
	nIndex = szData.Find('=');
	
	if (nIndex != 0) 
	{
		nIndex++;
	}

	for (i=nIndex,nCnt=0;i<szData.GetLength();i++) 
	{
		c = szData.GetAt(i);

		if ((c=='U') || (c=='X') || (c=='Y'))	 
		{
			*cData = c;
			if (nCnt++ == nUnitNr)
			{
				const char* const szDataToParse = (LPCTSTR) szData;		// CHANGES_MVS_2008_CSTRING			
				int temp = -1;
				if (sscanf(&szDataToParse[i+1],"%i",&temp) == FALSE)	// CHANGES_MVS_2008_SSCANF
				{
					return FALSE;	
				}

				*nId = static_cast<unsigned short>(temp);				
				return TRUE;
			}
		}
	}// end for
	
	return FALSE;
}

BOOL CBindingConvertHelper::ParseTOperators(CString szData,char *cData,unsigned short *nId,BOOL *fInverse,int OperatorNr)
{		
	*fInverse = FALSE;	
	*cData = '\0';

	for (int i=0,nCnt=0;i<szData.GetLength();i++) 
	{
		char c = szData.GetAt(i);
	
		if (c=='T') 
		{
			const char* const szDataToParse = (LPCTSTR) szData;			// CHANGES_MVS_2008_CSTRING
			*cData = c;

			if (1)
			{
				int temp = -1;
				if (sscanf(&szDataToParse[i+1],"%i",&temp) == FALSE)	// CHANGES_MVS_2008_SSCANF
				{
					return FALSE;
				}
				*nId = static_cast<unsigned short>(temp);		
			}

			if (i>0) 
			{	
				c = szData.GetAt(i-1);

				if (c=='!') 
				{
					*fInverse = TRUE;
				}
				else 
				{
					*fInverse = FALSE;
				}
			}

			if (OperatorNr == nCnt) 
			{
				return TRUE;
			}

			nCnt++;
		}
	}
	return FALSE;
}

/**
 * BOOL CBindingConvertHelper::ParseLogOperators(CString szData,char *cData,int OperatorNr)
 * 
 * Task : Search szData string and Return an Operator for specified index.
 *
 * Arguments: 
 *	- CString szData: Evaluaton string: [IN]
 *	- char* cData : [Out] +,*,^
 *	- int OperatorNr : Index 
 * Returns True if a Operator char is returned ...
 *
 */
BOOL CBindingConvertHelper::ParseLogOperators(CString szData,char *cData,int OperatorNr)
{
	char c;
	int i,nCnt;
	
	*cData = '\0';

	for (i=0,nCnt=0;i<szData.GetLength();i++) 
	{
		c = szData.GetAt(i);
	
		if ((c=='+') || (c=='^') || (c=='*')) 
		{
			if (nCnt == OperatorNr)
			{
				*cData = c;
				return TRUE;
			}

			nCnt++;
		}
	}
	return FALSE;
}

int CBindingConvertHelper::ParseNrOfOperators(CString szData)
{
	char cPrev;
	int i,nCnt;

	for (i=0,nCnt=0,cPrev='\0';
		 i<szData.GetLength();
		 i++)
	{
		const char c= szData.GetAt(i);	
		
		if ((c != cPrev) && ((c == 'T') || 
			(c == '*') || 
			(c == '+') || 
			(c == '^')))
		{
			nCnt++;
			cPrev = c;
		}
	}
	return nCnt;
}


int CBindingConvertHelper::ParseNInputs(CString szData)
{
	if (szData.IsEmpty()) return 0;

	int nIndex = szData.Find("=");

	if (nIndex != 0) 
	{
		nIndex++;
	}

	int nInputs = 0;

	for (int i=nIndex;i<szData.GetLength();i++) 
	{
		const char c = szData.GetAt(i);

		switch (c) 
		{
			case 'U':	
			case 'O':
			case 'Y':
			case 'X':
			{
				nInputs++;

			} break;
		}
	}
	return nInputs;
}

int CBindingConvertHelper::ParseNOutputs(CString szData)
{
	int nOutputs = 1;

	if (szData.IsEmpty() == TRUE) 
	{
		return 0;
	}

	for (int i=0;i<szData.GetLength();i++)
	{
		const char c = szData.GetAt(i);

		if (c == '>') 
		{
			nOutputs++;
		}
		else if (c == '=') 
		{
			break;
		}
	}
	return nOutputs;
}


void CBindingConvertHelper::DebugInfo(CString szString)
{
	if (szString.IsEmpty() == FALSE) 
	{
		CDlgDebugBindingConversion::Add(szString);
	}
}


