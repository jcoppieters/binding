// BindingConversionToGui.cpp: implementation of the CBindingConversionToGui class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConversionToGui.h"

#include "MainFrm.h"
#include "MyWindowMessages.h"
#include "MainFrm.h"
#include "views/main/MyFrameWnd.h"			// ???
#include "BindingGuiComponent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
char convertLevelToChar(int i)
{
	char c = '\0';
	if (i < 10) c = i + '0';
	else if (i < ('Z'-'A')) c = i + 'A' - 10;

	return c;
}

int convertCharToLevel(char c)
{
	int i=0;
	if ((c >= '0') && (c <= '9')) i = c - '0';
	else if ((c >= 'A') && (c <= 'Z')) i = c - 'A' + 10;
	return i;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingConversionToGui::CBindingConversionToGui()
{

}

CBindingConversionToGui::~CBindingConversionToGui()
{

}
////////////////////////////////////////////////////////////////////////


BOOL CBindingConversionToGui::ProcessBindingEntry(CBindingEntry *pEntry)
{
	ASSERT(pEntry);
	pBindingEntry = pEntry;

	pColLinks = pBindingEntry->GetColLink();
	pColUnits = pBindingEntry->GetColUnit();
	pColOperators = pBindingEntry->GetColOperator();

	szArrayLinkInfo.RemoveAll(); // + do extra clean up - delete all members !!
	ASSERT(szArrayLinkInfo.GetSize() != NULL);		// CHANGES_MVS_2008

	if (ProcessBindingEntry()) return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////

BOOL CBindingConversionToGui::ProcessBindingEntry(void)
{
	CString szDebugInfo;
	CBindingConvertHelper Convert;

	ASSERT(pBindingEntry);

	if (pBindingEntry->GetEvaluationString()->Get().IsEmpty() == FALSE)
	{
		szEvalIn = pBindingEntry->GetEvaluationString()->GetInput();
		szEvalOut = pBindingEntry->GetEvaluationString()->GetOutput();

		ParseInputString();
		ParseOutputString();
	
		// Debug Info ...
		for (int i=0;i<szArrayLinkInfo.GetSize();i++) 
		{			
			CString szTemp(szArrayLinkInfo.GetAt(i));	
			szDebugInfo.Format("Entry %02d : '%s'",i,szTemp);

			Convert.DebugInfo(szDebugInfo);
		}

		SetCalculatedPosition();

		return TRUE;
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////

BOOL CBindingConversionToGui::SetCalculatedPosition(void)
{
	CBindingConvertHelper Convert;
	//CString szData;
	int nLength,i,k,nInputs;
	char c;
	unsigned short nId;
	//char *szDataToParse;
	int nCntInputs=0,nCntOperators = 0,nCntOutputs = 0;
	CBindingUnit *pUnit;
	CBindingOperator *pOperator;
	float xCurrPos=0 ,yCurrPos = 0;

	int nSize = szArrayLinkInfo.GetSize();

	// Position inputs
	for (i=0;i<nSize;i++)
	{
		CString szData = szArrayLinkInfo.GetAt(i);
		const char* const szDataToParse = (LPCTSTR) szData;		// CHANGES_MVS_2008_CSTRING
		nLength = szData.GetLength();
		nInputs = Convert.ParseNInputs(szData);

		for (int j=0;j<nInputs;j++)
		{
			if (!Convert.ParseInputs(szData,&c,&nId,j)) break;

			if (c=='U')
			{
				pUnit = pColUnits->Find(nId);

				if (pUnit == NULL) 
				{
					return FALSE;			
				}
				
				xCurrPos = 200;
				yCurrPos = (float) ((nCntInputs*1000) + ((nCntInputs+1)* 200));				
				pUnit->SetPosition(xCurrPos,yCurrPos);
				nCntInputs++;					//new
			}
		}
	}

	// Position Operators
	for (i=0;i<nSize;i++)
	{
		CString szData = szArrayLinkInfo.GetAt(i);
		const char* const szDataToParse = (LPCTSTR) szData;		// CHANGES_MVS_2008_CSTRING

		nInputs = Convert.ParseNInputs(szData);
		k = 0;

		for (int j=0;j<nInputs;j++)
		{
			if (Convert.ParseInputs(szData,&c,&nId,j) == FALSE) break;

			if ((c!='U') && (c!= 'T'))
			{			
				if ((pOperator = pColOperators->Find(nId)) != NULL)
				{
					//if (pOperator == NULL) return FALSE;
				
					yCurrPos = (float) (440 + (k*600));
					xCurrPos = (float) (2000 + ((nCntOperators-k)*1000));
				
					pOperator->SetPosition(xCurrPos,yCurrPos);
	
					nCntOperators++;
					k++;	// k holds nr operators/Level
				}
				else
				{
					return FALSE;
				}
			}
		}
	}

	// Position Outputs
	
	yCurrPos = 200; 
	nCntOutputs = 0;
	CString szData = szArrayLinkInfo.GetAt((szArrayLinkInfo.GetSize()-1));

	for (int j=0;j<Convert.ParseNOutputs(szData);j++)
	{
		if (Convert.ParseOutputs(szData,&c,&nId,j) == FALSE) return FALSE;

		if (c!='U') return FALSE;

		pUnit = pColUnits->Find(nId);
		if (pUnit == NULL) 
		{
			return FALSE;			
		}
				
		xCurrPos += 1500;
		yCurrPos = 200;
		pUnit->SetPosition(xCurrPos,yCurrPos);	
	}


	return TRUE;
}

BOOL CBindingConversionToGui::ParseOutputString(void)
{
	CString szDebugInfo;
	CBindingConvertHelper Convert;
	int nSize;
	CString szTemp;
	
	// DebugInfo
	szDebugInfo.Format("Parse Output String - Level 1: '%s'",szEvalIn);
	Convert.DebugInfo(szDebugInfo);

	nSize = szArrayLinkInfo.GetSize();
	if (nSize == 0) return FALSE;
	szTemp = szArrayLinkInfo.GetAt(nSize-1);

	szTemp.Format("%s%s",szEvalOut,szArrayLinkInfo.GetAt(nSize-1));
	szArrayLinkInfo.SetAt(nSize-1,szTemp);

	return TRUE;
}

BOOL CBindingConversionToGui::ParseInputString(void)
{
	CString szDebugInfo;
	int nDeepestLevel;
	int i;
	CString szLevel,szTemp;
	CString szReplace,szPrevReplace;
	CBindingConvertHelper Convert;
	BOOL fFound;
	int nLoop=0;

	// DebugInfo
	szDebugInfo.Format("Parse Input String - Level 1: '%s'",szEvalIn);
	Convert.DebugInfo(szDebugInfo);

	szArrayLinkInfo.RemoveAll();	
	szPrevReplace.Empty();

	while (1)
	{
		fFound = FALSE;
		nDeepestLevel = FormatLevelString(&szLevel);
		// check if there are operators left to be evaluated ...

		// Fill String Array
		szTemp.Empty();
		for (i=0;i<szEvalIn.GetLength();i++) 
		{		
			if (nDeepestLevel == convertCharToLevel(szLevel.GetAt(i))) 
			{
				szTemp += szEvalIn.GetAt(i);				
				fFound = TRUE;
			}
			else 
			{			
				if (fFound==TRUE) break;
			}
		}
	
		ReplaceString(szTemp,&szEvalIn,&szReplace);
		szArrayLinkInfo.Add(szTemp);

		// use szReplace for previous entry 	
		if (nLoop != 0) 
		{
			AddOutputToString(szPrevReplace,nLoop-1);
		}

		szPrevReplace = szReplace;

		if (nLoop++ >=20) break;

		if (Convert.ParseNrOfOperators(szEvalIn) == 0) 
		{
			break;
		}
	}

	ReplaceString(szTemp,&szEvalIn,&szReplace);

	szTemp.Format("=%s",szEvalIn);
	szArrayLinkInfo.Add(szTemp);

	AddOutputToString(szPrevReplace,nLoop-1);

	// ToDo Layout manager
	return FALSE;
}

BOOL CBindingConversionToGui::AddOutputToString(const CString szOutput,int nIndex)
{
	CString szTemp;

	szTemp = szArrayLinkInfo.GetAt(nIndex);
	szTemp = szOutput + '=' + szTemp;
	szArrayLinkInfo.SetAt(nIndex,szTemp);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
int CBindingConversionToGui::FormatLevelString(CString *szLevelString)
{
	CBindingConvertHelper Convert;
	CString szDebugInfo;
	int nLength;
	int i,k;
	char c,cPrev,cLevel;
	int nDeepestLevel;
	
	nLength = szEvalIn.GetLength();
	szLevelString->Empty();

	for (i=0,k=1,nDeepestLevel=0,cPrev='\0';
		 i<nLength;
		 i++)
	{
		c= szEvalIn.GetAt(i);
		cLevel = convertLevelToChar(k);
		switch(c)
		{
			case '(':
				k++;					
				cLevel = convertLevelToChar(k);
				cPrev = '(';
			break;

			case ')':			
				cLevel = convertLevelToChar(k);
				k--;
				cPrev = ')';
			break;
			
			case 'T':			
				if (cPrev == 'T') k++;
				cLevel = convertLevelToChar(k);
				cPrev = 'T';
			break;

			case 'U':						
				cLevel = convertLevelToChar(k);			
			break;

			case '+':
			case '^':
			case '*':				
				if (cPrev == 'T') k--;
				cLevel = convertLevelToChar(k);
				cPrev = c;
			break;
		}			
		(*szLevelString) += cLevel;
		if (k >= nDeepestLevel)	nDeepestLevel = k;
	}
	
	// DebugInfo
	szDebugInfo.Format("String : '%s'",szEvalIn);
	Convert.DebugInfo(szDebugInfo);
	// DebugInfo
	szDebugInfo.Format("Level String : '%s'",(*szLevelString));
	Convert.DebugInfo(szDebugInfo);

	return nDeepestLevel;
}

BOOL CBindingConversionToGui::SearchForOperator(CString szData,unsigned short *nId,char *cOperatorType,BOOL *fInverse)
{
	char c = '\0';							// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	char cOperator = '\0';					// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	unsigned short nTimerId	= 0;			// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	unsigned short nInputId = 0;			// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	BOOL fMatch = FALSE;					// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd		
	CBindingOperator *pOperator = 0;		// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	CBindingLink *pLink = 0;				// CHANGES_MVS_2008_WARNINGS : was niet geïnitialisserd
	ControlType InType;
	CBindingConvertHelper Convert;
	
	ASSERT(nId);
	ASSERT(cOperatorType);

	*nId = 0;
	*cOperatorType = '\0';
	*fInverse = FALSE;
	
	if (szData.IsEmpty() == TRUE) 
	{
		return FALSE;
	}
	
	// Parse Type of Current Operators

	/**
	 * if ParseLogOperators returns TRUE, cOperator holds : *,+,^
	 */
	if (Convert.ParseLogOperators(szData,&cOperator) == FALSE) 
	{
		if (Convert.ParseTOperators(szData,&cOperator,&nTimerId,fInverse) == FALSE)
		{
			return FALSE;
		}
	}

	ASSERT(cOperator!='\0');

	const int nEntries = pColOperators->Count();

	for (int i=0;i<nEntries;i++)
	{
		//Search for operator
		fMatch = FALSE;

		const OperatorType Type = CBindingOperator::Convert(cOperator);

		if (Type != PM_OPERATOR_UNDEFINED)
		{
			if ((pOperator = pColOperators->Get(i)) == 0)
			{
				ASSERT(pOperator);
				return FALSE;
			}

			if (Type == pOperator->GetType()) 
			{
				if (Type == PM_OPERATOR_TIMER) 
				{
					if (nTimerId == pOperator->GetID())		// MVS_2008_WARNINGS_UNITIALISED_!!!!!
					{
						fMatch = TRUE;
					}
				}
				else 
				{
					fMatch = TRUE;
				}
			}			
		}

		
		// Type of operator = ok
		if (fMatch == TRUE)
		{	
			// pOperator holds possible Operator
			const int nInputs = Convert.ParseNInputs(szData);

			// if 1 of the inputs is connected to this operator
			// then 100% it is ok
			// all controls can have only 1 output link !!!
			
			if (Convert.ParseInputs(szData,&c,&nInputId,0) == FALSE) 
			{
				return FALSE;			
			}

/*
			// nInputId  + c == Controltype
			if(Convert.ConvertChar2ControlType(c,&InType) == FALSE)
			{
				return FALSE;
			}
*/

			// nInputId  + c == Controltype
			if ((InType = CBinding2Controls::Convert(c)) == UNDEFINED)
			{
				return FALSE;
			}		

			int k=0;
			for (k=0;k<pColLinks->Count();k++) 
			{
				// Search for plink with input == nInputId + c
				if ((pLink = pColLinks->Get(k)) == 0)
				{
					ASSERT(pLink);
					return FALSE;
				}
				
				if ((pLink->GetInLinkType() == InType) && 
					(pLink->GetInLinkID() == nInputId)) 
				{
					ASSERT(pOperator);									// CHANGES_MVS_2008_WARNINGS

					if (pOperator != 0 &&								// CHANGES_MVS_2008_WARNINGS 
						pLink->GetOutLinkID() == pOperator->GetID())
					{
						break;
					}
				}
			}
			
			if (k>=pColLinks->Count()) 
			{
				fMatch = FALSE;
			}
		}

		if (fMatch == TRUE) 
		{
			// Give cControlType + nId of operator ...
			Convert.ConvertOperatorType2Char(pOperator->GetType(),
											 cOperatorType);

			*nId = pOperator->GetID();	
			
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CBindingConversionToGui::ReplaceString(CString szData,CString *szEvalIn,CString *szReplace)
{
	unsigned short nOperatorId;
	char cControlType;
	BOOL fInverse;

	if (SearchForOperator(szData,&nOperatorId,&cControlType,&fInverse) == FALSE) 
	{
		return FALSE;
	}
	
	switch(cControlType)
	{
		case 'Y':
		{			// Must remove T-operator 				

			if (fInverse == TRUE) 
			{
				szReplace->Format("!Y%i",nOperatorId);	// Must add Inverse '!'				
			}
			else 
			{
				szReplace->Format("Y%i",nOperatorId);	// Must add Inverse '!'						
			}

			szEvalIn->Replace(szData,*szReplace);
			szReplace->Format("Y%i",nOperatorId);		

		} break;

		case 'X':
		{
			szReplace->Format("%c%i",cControlType,nOperatorId);
			szEvalIn->Replace(szData,*szReplace);

		} break;

		default: 
		{
			ASSERT(0);

		} return FALSE;
	}
	
	return TRUE;	
}


//////////////////////////////////////////////////////////////////////////////////////
