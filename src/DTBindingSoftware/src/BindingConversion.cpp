// BindingConversion.cpp: implementation of the CBindingConversion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConversion.h"
#include "MainFrm.h"
#include "MyWindowMessages.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"

#include "views/debug/DlgDebugBindingConversion.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingConversion::CBindingConversion()
{		
}

CBindingConversion::~CBindingConversion()
{
}

//////////////////////////////////////////////////////////////////////

void CBindingConversion::Init(void)
{
	ASSERT(pBindingEntry);

	pColLinks = pBindingEntry->GetColLink();
	pColUnits = pBindingEntry->GetColUnit();
	pColOperators = pBindingEntry->GetColOperator();

	szArrayLinkInfo.RemoveAll();
	// + do extra clean up - delete all members !!

	//ASSERT(szArrayLinkInfo.GetSize() != NULL);		// CHANGES_MVS_2008

	nFirstId = 0;
	nLastId = 0;
}

BOOL CBindingConversion::ProcessBindingEntry(CBindingEntry *pEntry)	
{
	CString szTemp;

	pBindingEntry = pEntry;
	
	Init();

	if (pColLinks->Count() > 0) 
	{	
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// TM 20051107 : Always 
		// Nr Of links = (Nr Of Units + Nr Of Operators) - 1 
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		if (pColLinks->Count() == (pColUnits->Count()+pColOperators->Count())-1) 
		{		
			szArrayLinkInfo.SetSize(pColLinks->Count());

			if (BuildEvalStringLevel()) 
			{
				if (CheckTypeBinding()) 
				{			
					if (GenerateEvalString()) 
					{
						// Should we add eval string here ? 
						// pBindingEntry->SetEvalString(szEvaluationString);

						pBindingEntry->GetEvaluationString()->Set(szEvaluationString);

						DebugInfo("Add Evalution string to BindingEntry");
						DebugInfo("-----------------------------------------------------");

						if (ParseStringToBindingUnits())
						{
							DebugInfo("Parse Evaluation string + fill in type of unit In/Out");
	
							if (ProcessBindingUnits())
							{
								DebugInfo("For all Units : Fill in BindingData");
								
								if ((nFirstId == 0) ||( nLastId == 0)) 
								{
									ASSERT(0);
									return FALSE;
								}																														
								return TRUE;				
							}
						}
					}
				}
			}
		}
		else 
		{
			AfxMessageBox("Not all Units are connected");
		}
	}
	return FALSE;
}

BOOL CBindingConversion::ParseStringToBindingUnits(void)
{
	int i,type;
	char c,ch;	
	int id;
	CBindingUnit *pUnit=NULL;
	BOOL fInverse;

	int nLen = szEvaluationString.GetLength();
	if (nLen == 0) 
	{
		return FALSE;
	}

	const char* const szDataToParse = (LPCTSTR) szEvaluationString;		// CHANGES_MVS_2008_CSTRING
	type = UNIT_TYPE_INPUT;
	
	fInverse = FALSE;
	for (i=0;i<nLen;i++) 
	{
		c = szEvaluationString.GetAt(i);

		switch(c) 
		{			
			case '(' : case ')' :
			case '+' : case '*' : case '^' : case 'T':
				// absorbe
			break;

			case '!' :
				fInverse = TRUE;
			break;

			case '=':
				type = UNIT_TYPE_OUTPUT;
			break;
		
			case 'X' : case 'Y' :
				ASSERT(0);
			return FALSE;

			case 'U':

				if (!sscanf(&szDataToParse[i],"%c%i",&ch,&id))	// CHANGES_MVS_2008_SSCANF
				{
					AfxMessageBox("Error - sscanf(...)");
					return FALSE;
				}

				pUnit = pColUnits->Find(id);
				
				if (pUnit == NULL) 
				{
					AfxMessageBox("Error unit not found in Unit Collection...");
					return FALSE;
				}

				ASSERT(type >= 0 && type <= 0xFF);							// CHANGES_MVS_2008_WARNINGS
				pUnit->SetInOutType(static_cast<BYTE>(type));				// CHANGES_MVS_2008_WARNINGS

				if (nFirstId == 0) 
				{
					nFirstId = id;
				}

				nLastId = id;

				switch(pUnit->GetBindingUnitType())
				{
					case UNIT_TYPE_CONTROL:		
					{					
						((CBindingControlUnit*) pUnit)->Set_ControlState(static_cast<BYTE>(fInverse^0x01));	// CHANGES_MVS_2008_WARNINGS

					} break;

					case UNIT_TYPE_LCD_VIRTUAL:
					{
						if (type == UNIT_TYPE_INPUT) 
						{
							((CBindingVirtualUnit*) pUnit)->Set_ControlState(static_cast<BYTE>(fInverse^0x01));	// CHANGES_MVS_2008_WARNINGS					
						}

					} break;

					case UNIT_TYPE_RC5RX:
					{
						((CBindingRC5RXUnit*) pUnit)->Set_State(static_cast<BYTE>(fInverse^0x01));	// CHANGES_MVS_2008_WARNINGS

					} break;

					// case UNIT_TYPE_SENS:
					case BINDING_UNIT_TYPE_SENS:
					case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
					{
						((CBindingSensUnit*) pUnit)->Set_EventState(static_cast<BYTE>(fInverse^0x01));	// CHANGES_MVS_2008_WARNINGS

					} break;
				}

				fInverse = FALSE;
			break;

			default:
				// absorbe ...
			break;
		}
	}
	return TRUE;
}

BOOL CBindingConversion::ProcessBindingUnits(void)
{
	if (pColUnits->Count() == 0) 	
		return FALSE;	

	CBindingUnits::Iterator it(pColUnits->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingUnit* const pUnit = it.CurrentItem();
		ASSERT( pUnit );

		if (pUnit->ConvertToBindingData() == FALSE)
		{
			AfxMessageBox("Failed to fill in Binding Data struct");
			return FALSE;
		}
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////
BOOL CBindingConversion::GenerateEvalString(void)
{
	#define UNDEFINED_ID	255
	
	int i,j,k;
	int NInputs,nEntries=0,nOutputs=0;
	int nStartId,nStopId;
	int nLength;

	char cControlType,c;
	unsigned short nId,nOutId,nInId;
	BOOL fResult;
	
	CString szError;
	CString szOutputString,szInputString;
	CString szData,szResult;
	CString szInput;
	CString szReplace;
	
	// Convert all entries from CstringArray
	// if nInputs >2 Build C-Expression else Check Output and build T or U Expression
	nEntries = szArrayLinkInfo.GetSize();
	if (nEntries == 0) return FALSE;

	for (i=0;i<nEntries;i++) 
	{
		szData = szArrayLinkInfo.GetAt(i);
		NInputs = ParseNInputs(szData);
	
		if (NInputs > 1) 
		{	
			// build c-expression					
			if (GenerateCExpression(szData,&szResult,NInputs)) 
			{
				szArrayLinkInfo.SetAt(i,szResult);	
			}
			else 
			{
				szError.FormatMessage("Error - Conditional expression must have more\nthan 1 input");
				AfxMessageBox(szError);
				ASSERT(0);return FALSE;
			}
		}
		else
		{	// check type of output
			
			if (ParseOutput(szData,&szInput)) 
			{				
				GetIdentifier(szInput,&nId);
				GetControlType(szInput,&cControlType);								
				switch(cControlType)
				{
					case 'T':
						
						if (GenerateTExpression(szData,&szResult)) 
						{
							szArrayLinkInfo.SetAt(i,szResult);	
						}
						else 
						{
							// TODO Generate Error
							ASSERT(0); return FALSE;
						}

					break;

					case 'U':
						
						if (GenerateUExpression(szData,&szResult)) 
						{
							szArrayLinkInfo.SetAt(i,szResult);	
						}
						else 
						{
							// TODO Generate Error
							ASSERT(0); return FALSE;
						}
					break;

					default:
					case 'O':

						// TODO GENERATE ERROR 
						szError.Format("%s","Logical operator missing incoming link");
						AfxMessageBox(szError);
						/*ASSERT(0);*/
						return FALSE;

					break;
				}			
			}
			else { ASSERT(0); return FALSE;}
		}
	}// end for

	// Generate Debug Info
	// X1=(U1*U2*Y3)  // Y3=Y4   // Y4=U3
	DebugInfo("Generate Conditional String Level 1");
	for (i=0;i<szArrayLinkInfo.GetSize();i++)
	{
		DebugInfo(szArrayLinkInfo.GetAt(i));
	}


	// Start Generating Level 2
	// Build Evaluationstring = input + output string

	nStartId = nStopId = UNDEFINED_ID;
	szInputString.Empty(); szOutputString.Empty();
	
					//nEntries = szArrayLinkInfo.GetSize();
	
	if ((_nOperators == 0) && (_nTimers == 0))
	{
			// Must check which unit is input
		// fill in nr of outputs, start id, stop id
		// Calculate start,stop id's + nr of outputs
		for (i=0;i<nEntries;i++) 
		{
			szData = szArrayLinkInfo.GetAt(i);
			if (!(ParseInput(szData,&szResult,0))) return FALSE;
			if (!(GetIdentifier(szResult,&nInId))) return FALSE;
							
			for (j=0;j<nEntries;j++) 
			{	
				szData = szArrayLinkInfo.GetAt(j);
				if (!(ParseOutput(szData,&szResult))) return FALSE;
				if (!(GetIdentifier(szResult,&nOutId))) return FALSE;				
				if (nInId == nOutId) break;
			}
				
			if (j>=nEntries)	// i = link-output, but has no link-input
			{
				nStartId = i;
				break;
			}	
		}// end for	
			
		if (nStartId != 0) nStopId = 0;
		else 
		{
			if (nEntries > 1) nStopId = nStartId + 1;
			else nStopId = nStartId;
		}

		// 
		nOutputs = 0;
		for (i=0;i<nEntries;i++) 
		{
			szData = szArrayLinkInfo.GetAt(i);
			if (!(ParseInput(szData,&szResult,0))) return FALSE;
			if (!(GetControlType(szResult,&cControlType))) return FALSE;	
			if (cControlType == 'U') nOutputs++;
		}

		//nOutputs--;
	}
	else 
	{
		// Calculate start,stop id's + nr of outputs
		for (i=0;i<nEntries;i++) 
		{
			szData = szArrayLinkInfo.GetAt(i);
			if (!(ParseOutput(szData,&szResult))) return FALSE;
			if (!(GetControlType(szResult,&cControlType))) return FALSE;	
			if (cControlType == 'U') 
			{
				nOutputs++;
				if (!(ParseInput(szData,&szResult,0))) return FALSE;
				if (!(GetControlType(szResult,&cControlType))) return FALSE;
				//if (!(GetIdentifier(szResult,&nInId))) return FALSE;
				if (cControlType == 'U') 
				{
					if (nStartId == UNDEFINED_ID) 	nStartId = i;
				}
				else 
				{
					if (nStopId == UNDEFINED_ID) nStopId = i;
				}
			}
		}
	}
	

	// Generate Debug info ...	
	CString szDebugMessage;
	szDebugMessage.Format("StartId=%02d,StopId=%02d,Nr of Outputs=%02d",
	nStartId,nStopId,nOutputs);
	DebugInfo(szDebugMessage);
	
	
	// Check Start,StopId,nEntries,...
	if (nOutputs == 0) {
		AfxMessageBox("No outputs found in Binding");
		return FALSE;
	}

	if (nStopId == UNDEFINED_ID) {
		if (nStartId == UNDEFINED_ID) {
			AfxMessageBox("Incomplete binding, cannot be interpreted!");
			return FALSE;
		}
		
		nStopId = nStartId;
	}
	if (nStartId == UNDEFINED_ID) nStartId = nStopId;

	
	
	if ((_nOperators == 0) && (_nTimers == 0))
	{
		// StartGenerating Input - Complete
		szData = szArrayLinkInfo.GetAt(nStartId);
		if (!(ParseInput(szData,&szResult,0))) return FALSE;
		szInputString.Format("%s;",szResult);

		szData = szArrayLinkInfo.GetAt(nStartId);
		if (!(ParseOutput(szData,&szResult))) return FALSE;
		szOutputString = '=';
		szOutputString += szResult;	

		if (!(GetIdentifier(szResult,&nOutId))) return FALSE;
		nOutputs -= 1;

		// j holds nr of times we have to search in CstringArray so that
		// all outputs have been checked ...

		k = nOutputs;	//k holds nr of outputs, coz nOutputs--
							//and not all outputs are assigned at that moment;
		for (j=0;j<k;j++)			
		{
			for (i=0;i<nEntries;i++) 
			{
				//if (i != nStopId) {
				szData = szArrayLinkInfo.GetAt(i);
												
				if (!(ParseOutput(szData,&szResult))) return FALSE;
				if (!(GetControlType(szResult,&cControlType))) return FALSE;			
				if (cControlType == 'U') {				
					if (!(ParseInput(szData,&szResult,0))) return FALSE;				
					if (!(GetIdentifier(szResult,&nInId))) return FALSE;
					if (!(GetControlType(szResult,&cControlType))) return FALSE;

					if ((cControlType == 'U') && (nOutId == nInId)) {
						// Chain must be correct : InId & OutId 
						if (!(ParseOutput(szData,&szResult))) return FALSE;
						szOutputString += '>';
						szOutputString += szResult;
						nOutputs--;
						// Get new OutId to check chain
						if (!(GetIdentifier(szResult,&nOutId))) return FALSE;
					}
				}
			}				
			//}
		}
		//return FALSE;	
	}
	else
	{
		// Start Generating szOutputString
		// U3>U4>U5>U6
		if (nOutputs == 1) {
			szData = szArrayLinkInfo.GetAt(nStopId);
			if (!(ParseOutput(szData,&szResult))) return FALSE;
			szOutputString = '=';	
			szOutputString += szResult;		
		}
		else if (nOutputs == 2) {
				szData = szArrayLinkInfo.GetAt(nStopId);
				if (!(ParseOutput(szData,&szResult))) return FALSE;
				szOutputString = '=';
				szOutputString += szResult;	
				szData = szArrayLinkInfo.GetAt(nStartId);
				if (!(ParseOutput(szData,&szResult))) return FALSE;
				szOutputString += '>';	
				szOutputString += szResult;	
		}
		else {
			// Insert StopId
			szData = szArrayLinkInfo.GetAt(nStopId);
			if (!(ParseOutput(szData,&szResult))) return FALSE;
			szOutputString = '=' + szResult;	
		
			if (!(GetIdentifier(szResult,&nOutId))) return FALSE;
			nOutputs -= 1;

			// j holds nr of times we have to search in CstringArray so that
			// all outputs have been checked ...

			k = nOutputs;	//k holds nr of outputs, coz nOutputs--
							//and not all outputs are assigned at that moment;
			for (j=0;j<k;j++)			
			{
				for (i=0;i<nEntries;i++) {
					if (i != nStopId) {
						szData = szArrayLinkInfo.GetAt(i);
												
						if (!(ParseOutput(szData,&szResult))) return FALSE;
						if (!(GetControlType(szResult,&cControlType))) return FALSE;			
						if (cControlType == 'U') {				
							if (!(ParseInput(szData,&szResult,0))) return FALSE;				
							if (!(GetIdentifier(szResult,&nInId))) return FALSE;
							if (!(GetControlType(szResult,&cControlType))) return FALSE;

							if ((cControlType == 'U') && (nOutId == nInId)) {
								// Chain must be correct : InId & OutId 
								if (!(ParseOutput(szData,&szResult))) return FALSE;
								szOutputString += '>';
								szOutputString += szResult;
								nOutputs--;
								// Get new OutId to check chain
								if (!(GetIdentifier(szResult,&nOutId))) return FALSE;
							}
						}
					}				
				}
			}

			if (nOutputs != 0) {
				AfxMessageBox("Not all units are connected,Cannot interpret binding correct");
				return FALSE;
			}
		}
		
		// Start generating input string

		// Generate First part of input String
		szData = szArrayLinkInfo.GetAt(nStopId);
		if (!(ParseInput(szData,&szResult,0))) return FALSE;
		szInputString.Format("%s;",szResult);
	}


	if (!((_nOperators == 0) && (_nTimers == 0)))
	{
		fResult = TRUE;
		while(fResult)
		{
			fResult = FALSE;

			// Search for operator , if none found break;
			nLength = szInputString.GetLength();		
			for (i=0;i<nLength;i++) {
				c = szInputString.GetAt(i);
				if ((c =='X') || (c == 'Y')) {
					fResult = TRUE;
					break;
				}		
			}// end for

			// Operator found ... 
			// replace operator by expression
			if (fResult == TRUE) 
			{
				const char* const szDataToParse = (LPCTSTR) szInputString;		// CHANGES_MVS_2008_CSTRING
			
				for (i=0;i<nLength;i++) 
				{
					c = szInputString.GetAt(i);

					if ((c =='X') || (c=='Y')) 
					{
						if (!GetIdentifier(&szDataToParse[i],&nInId)) return FALSE;;

						// Seach in String Array for output that has CcontrolId + inId
						nEntries = szArrayLinkInfo.GetSize();
						
						for (j=0;j<nEntries;j++) 
						{
							szData = szArrayLinkInfo.GetAt(j);
							if (!(ParseOutput(szData,&szResult))) return FALSE;					
							if (!(GetIdentifier(szResult,&nOutId))) return FALSE;
							if (!(GetControlType(szResult,&cControlType))) return FALSE;
	
							if ((c==cControlType) && (nOutId == nInId)) 
							{		
								// Found Match !!
								// Get outputString + Replace in InputString !!
								szReplace.Empty();
								switch(c)
								{
									case 'X':
									{							
										k = szData.Find('=');
										szReplace = &(szData.GetBuffer(0))[k+1];
																				
									}break;

									case 'Y':
									{
										k = szData.Find('=');
										szReplace.Format("T%d%s",nOutId,&(szData.GetBuffer(0))[k+1]);;
												
									}break;
								}
							
								break; // break for loop
							}
						};

						if (j<nEntries) 
						{
							// szDataToParse[i] still holds identifier string
							if (!ParseOutput(szData,&szResult)) return FALSE;

							szInputString.Replace(szResult,szReplace);
							TRACE("Input string = '%s'\n",szInputString);
							break; // break for loop
						}
					}		
				}// if (fResult==TRUE)
			}// end for
		}// end while 			
	}

	// Remove all ';' in string
	szInputString.Remove(';');

	// Generate complete string
	szEvaluationString = szInputString + szOutputString;

	// Debug Info
	DebugInfo("-----------------------------------------------------");
	DebugInfo("Generate Evaluation string Level2");
	DebugInfo(szEvaluationString);
	return TRUE;
}

////////////////////////////////////////////////////////////////////
BOOL CBindingConversion::GenerateTExpression(CString szData,CString *szResult)
{
	// Yid=Tid(Uid)
	CString szInput,szOutput;
	char cControlType;
	unsigned short nId;
	CString szResultIn,szResultOut;

	if (!(ParseOutput(szData,&szOutput)))
		return FALSE;	

	if (!(ParseInput(szData,&szInput,0))) 
		return FALSE;

	GetIdentifier(szOutput,&nId);
	GetControlType(szOutput,&cControlType);
	ASSERT(cControlType=='T');

	ConvertTtoY(szOutput,&szResultOut);

	GetIdentifier(szInput,&nId);
	GetControlType(szInput,&cControlType);

	if (cControlType == 'O')
	{
		// Replace operators by X.	
		ConvertOtoX(szInput,&szResultIn);
		szResult->Format("%s=%s;",szResultOut,szResultIn);
		return TRUE;
	}
	else if (cControlType == 'T')
	{
		ConvertTtoY(szInput,&szResultIn);
		szResult->Format("%s=%s;",szResultOut,szResultIn);
		return TRUE;
	}
	else
	{		
		szResult->Format("%s=%s;",szResultOut,szInput);
		return TRUE;
	}

	ASSERT(0);
	return FALSE;
}

BOOL CBindingConversion::GenerateUExpression(CString szData,CString *szResult)
{
	//Uid=Uid
	CString szInput,szOutput;
	char cControlType;
	unsigned short nId;
	CString szResultIn;

	if (!(ParseOutput(szData,&szOutput)))
		return FALSE;	

	if (!(ParseInput(szData,&szInput,0))) 
		return FALSE;

	GetIdentifier(szInput,&nId);
	GetControlType(szInput,&cControlType);
	if (cControlType == 'O')
	{
		// Replace operators by X.
		//szInput.Empty();
		//szInput.Format("X%i",nId);
		ConvertOtoX(szInput,&szResultIn);
		szResult->Format("%s=%s;",szOutput,szResultIn);
		return TRUE;

	}
	else if (cControlType == 'T')
	{
		ConvertTtoY(szInput,&szResultIn);
		szResult->Format("%s=%s;",szOutput,szResultIn);
		return TRUE;
	}
	else
	{
		szResult->Format("%s=%s;",szOutput,szInput);
		return TRUE;
	}

	ASSERT(0);
	return FALSE;
}

BOOL CBindingConversion::GenerateCExpression(CString szData,CString *szResult,int nInputs)
{
	// Xid=(U0*U1*U11*O1)
	
	CString szOutput,szInput;
	CString szTemp;
	char cControlType,cOperatorType;	
	unsigned short nId;
	BOOL fInverse;


	CBindingOperator *pOperator;

	if (ParseOutput(szData,&szOutput))
	{	
		GetIdentifier(szOutput,&nId);
		GetControlType(szOutput,&cControlType);	
		ASSERT(cControlType == 'O');

		if ((pOperator = pColOperators->Find(nId)) == 0)
		{
			ASSERT(pOperator);
			return FALSE;
		}
				

		switch(pOperator->GetType())
		{
			case PM_OPERATOR_AND:
				cOperatorType = '*';
			break;

			case PM_OPERATOR_OR:
				cOperatorType = '+';
			break;
			
			case PM_OPERATOR_XOR :
				cOperatorType = '^';
			break;
				
			case PM_OPERATOR_TIMER:
			default:
				ASSERT(0);
			break;
		}

		szResult->Format("X%i=(",nId);
		for (int i=0;i<nInputs;i++)
		{			
			if (!(ParseInput(szData,&szInput,i))) return FALSE;					
		
			GetIdentifier(szInput,&nId);
			GetControlType(szInput,&cControlType);
			GetInverted(szInput,&fInverse);
			if (cControlType == 'O')
			{
				// Replace operators by X.
				//szInput.Empty();
				//szInput.Format("X%i",nId);
				BuildIdentifierString(&szInput,nId,'X',fInverse);
			}			
			else if (cControlType == 'T')
			{
				// CHANGED !!!
				//BuildIdentifierString(&szInput,nId,'Y',fInverse);
				if (fInverse == TRUE) {
					szInput.Format("!(Y%i)",nId);
				}
				else {
					szInput.Format("(Y%i)",nId);
					//return TRUE;
				}
			}			
			(*szResult) += szInput;
			(*szResult) += cOperatorType;			// MVS_2008_WARNINGS_UNITIALISED_!!!!!
		}

		szResult->SetAt(szResult->GetLength()-1,')');
		(*szResult) +=';';

		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////

BOOL CBindingConversion::BuildEvalStringLevel(void)
{
	int i = 0;
	int j = 0;
	int nEntries = 0;
	unsigned short InId = 0;
	unsigned short OutId = 0;
	BOOL fInverse = FALSE;
	char chIn = '\0';
	char chOut = '\0';
	ControlType InType,OutType;
	CString szData;
	CString szIn,szCurrIn;
	CString szOut,szCurrOut;
	CString szNewData,szTemp;
	CString szEvalString;

	CBindingLink *pLink = 0;
	CBindingOperator *pOperator = 0;
	
	////////////////// BUILD EVALUATION STRING LEVEL 1 //////////////
	/////////////////////////////////////////////////////////////////
	////////////// Each entry in CStringArray is a link /////////////
	/////////////////////////////////////////////////////////////////

	for (i=0;i<pColLinks->Count();i++)
	{			
		if ((pLink = pColLinks->Get(i)) == 0)
		{
			ASSERT(pLink);
			return FALSE;
		}
	
		szEvalString.Empty();
		
		InId = pLink->GetInLinkID();
		OutId = pLink->GetOutLinkID();
		InType = pLink->GetInLinkType();
		OutType = pLink->GetOutLinkType();

		if (InType == PM_UNIT) 
		{
			chIn = 'U';
		}
		else 
		{ 
			//If Operator - Check if Logical operator or Timer
			if ((pOperator = pColOperators->Find(InId)) == 0)
			{
				ASSERT(pOperator);
				return FALSE;
			}						

			if (pOperator->GetType() == PM_OPERATOR_TIMER) 
			{
				chIn = 'T';
			}
			else 
			{
				chIn = 'O';
			}		
		}

		if (OutType == PM_UNIT) 
		{
			chOut = 'U';
		}
		else 
		{
			if ((pOperator = pColOperators->Find(OutId)) == 0)
			{
				ASSERT(pOperator);
				return FALSE;
			}

			if (pOperator->GetType() == PM_OPERATOR_TIMER) 
			{
				chOut = 'T';			
			}
			else 
			{
				chOut = 'O';
			}
		}

		fInverse = pLink->isInverted();

		BuildIdentifierString(&szIn,InId,chIn,fInverse);
		BuildIdentifierString(&szOut,OutId,chOut);
		szEvalString.Format("%s-%s;",szOut,szIn);
		szArrayLinkInfo.SetAt(i,szEvalString);	
	}

	if(1)
	{
		// Generate Debug Info : CstringArray holds Links 
		// U1-U2  //O1-U1 //O2-T3 // ...

		DebugInfo("Build Evaluation - String Level 1");
		for (i=0;i<szArrayLinkInfo.GetSize();i++) 
		{
			DebugInfo(szArrayLinkInfo.GetAt(i));
		}
	}

	////////////////// BUILD EVALUATION STRING LEVEL 2 //////////////
	/////////////////////////////////////////////////////////////////
	/////////// For all Current entries in CstringArray /////////////
	/////// Search for common outputs an insert in CStringArray /////
	/////////////////////////////////////////////////////////////////
	nEntries = szArrayLinkInfo.GetSize();	
	
	for (i=0;i<nEntries;i++) 
	{	
		szData = szArrayLinkInfo.GetAt(i);
		ParseInput(szData,&szCurrIn,0);
		ParseOutput(szData,&szCurrOut);
		szNewData.Empty();
		szNewData.Format("%s-%s;",szCurrOut,szCurrIn);
	
		for (j=(i+1);j<nEntries;j++) 
		{
			szData.Empty();
			szData = szArrayLinkInfo.GetAt(j);	

			ParseInput(szData,&szIn,0);
			ParseOutput(szData,&szOut);
							
			if (strcmp(szOut,szCurrOut) == 0) 
			{			
				szTemp.Format("%s;",szIn);
				szNewData += szTemp;			
			}		
		}	
		szArrayLinkInfo.SetAt(i,szNewData);		
	}

	if (1)
	{
		// Generate Debug Info:
		// O1-U1;U2;U3  //01-U2;U3
		DebugInfo("Build Evaluation - String Level 2");
	
		for (i=0;i<szArrayLinkInfo.GetSize();i++) 
		{
			DebugInfo(szArrayLinkInfo.GetAt(i));
		}
	}


	////////////////// BUILD EVALUATION STRING LEVEL 3 //////////////
	/////////////////////////////////////////////////////////////////
	////// Remove entries with common output from CstringArray //////
	/////////////////////////////////////////////////////////////////
	TRACE1("\nArray Size %1d (AtEntry)",szArrayLinkInfo.GetSize());

	for (i=0;i<szArrayLinkInfo.GetSize();i++) 
	{			
		szData = szArrayLinkInfo.GetAt(i);		
		
		//TRACE3("\ni=%02d,j=%02d,ArraySize = %02d",i,j,szArrayLinkInfo.GetSize());
		//TRACE1("- String to compare : %s",szData);

		ParseOutput(szData,&szCurrOut);	

		for (j=(i+1);j<szArrayLinkInfo.GetSize();j++) 
		{			
			szData = szArrayLinkInfo.GetAt(j);	
				
			//TRACE3("\ni=%02d,j=%02d,ArraySize = %02d",i,j,szArrayLinkInfo.GetSize());
			//TRACE1("%s",szData);

			ParseOutput(szData,&szOut);
			if (strcmp(szOut,szCurrOut) == 0) 
			{
				//TRACE1("\n --> Remove : %s",szData);
				szArrayLinkInfo.RemoveAt(j);	// ArraySize Decrements !!!
				szData.Empty();			
				j--;	// MUST BE HERE !!!								
			}
			else 
			{ 
				//TRACE1("\n --> Keep : %s",szData);
			}
		}
	}	
	
	TRACE3("\nArray Size %1d (atExit),i=%02d,j=%02d",szArrayLinkInfo.GetSize(),i,j);
	
	if (1)
	{
		// Generate Debug Info:
		// O1-U1;U2;U3  //01-U2;U3
		DebugInfo("Build Evaluation - String Level 3");
		for (i=0;i<szArrayLinkInfo.GetSize();i++) 
		{
			DebugInfo(szArrayLinkInfo.GetAt(i));
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////

void CBindingConversion::DebugInfo(CString szString)
{
	if (szString.IsEmpty() == FALSE)
	{
		CDlgDebugBindingConversion::Add(szString);
	}
}

///////////////////////////////////////////////////////////////////////////

int CBindingConversion::ParseNInputs(CString szData)
{
	char c;
	int nNrOfInputs = 0;

	for (int i=0;i<szData.GetLength();i++) 
	{
		c = szData.GetAt(i);

		if (c == ';') 
		{
			nNrOfInputs++;		
		}

	}
	return nNrOfInputs;
}

int CBindingConversion::GetStartIndex(CString szData,int nInput)
{
	char c;
	int i,nstart;
	int ncnt;

	nstart = 0;

	for (i=0;i<szData.GetLength()-1;i++) 
	{
		c = szData.GetAt(i);		
		if ((c == '-') || (c == '=')) nstart = (i+1);
	}

	if (nstart == 0)
	{
		ASSERT(0);
		return 0;
	}

	if (nInput == 0)
		return nstart;

	for (i=0,ncnt=0;i<szData.GetLength()-1;i++) 
	{
		c = szData.GetAt(i);
		if (c == ';') 
		{
			ncnt ++;
			nstart = (i+1);						
			if (ncnt == nInput) 
			{
				return nstart;
			}
		}
	}	
	return 0;
}

BOOL CBindingConversion::ParseInput(CString szData,CString *szResult,int nIndex)
{
	unsigned short nID;
	char cControlType;
	int nNrOfInputs,start;
	BOOL fInverted;

	nNrOfInputs = ParseNInputs(szData);
	start = GetStartIndex(szData,nIndex);

	if (nIndex > nNrOfInputs) return FALSE;
	if (start == 0) return FALSE;

	const char* const szDataToParse = (LPCTSTR) szData;			// CHANGES_MVS_2008_CSTRING
	
	if (!GetControlType(&szDataToParse[start],&cControlType)) return FALSE;
	if (!GetIdentifier(&szDataToParse[start],&nID)) return FALSE;
	if (!GetInverted(&szDataToParse[start],&fInverted)) return FALSE;

	if (!BuildIdentifierString(szResult,nID,cControlType,fInverted)) return FALSE;

	//szResult->Format("%c%i",cControlType,nID);
	return TRUE;
}

BOOL CBindingConversion::ParseOutput(CString szData,CString *szResult)
{
	char cControlType;
	unsigned short nID;
	BOOL fInverted;
	
	if (!GetControlType(szData,&cControlType)) return FALSE;
	if (!GetIdentifier(szData,&nID)) return FALSE;
	if (!GetInverted(szData,&fInverted)) return FALSE;

	if (!BuildIdentifierString(szResult,nID,cControlType,fInverted)) return FALSE;

	return TRUE;
}

BOOL CBindingConversion::ConvertOtoX(CString szData,CString *szResult)
{
	char cControlType;
	unsigned short nID;
	BOOL fInverted;

	*szResult = szData;

	if (!GetControlType(szData,&cControlType)) return FALSE;
	if (!GetIdentifier(szData,&nID)) return FALSE;
	if (!GetInverted(szData,&fInverted)) return FALSE;

	if (cControlType == 'O')
	{
		return (BuildIdentifierString(szResult,nID,'X',fInverted));
	}

	return FALSE;
}

BOOL CBindingConversion::ConvertTtoY(CString szData,CString *szResult)
{
	char cControlType;
	unsigned short nID;
	BOOL fInverted;
	
	*szResult = szData;

	if (!GetControlType(szData,&cControlType)) return FALSE;
	if (!GetIdentifier(szData,&nID)) return FALSE;
	if (!GetInverted(szData,&fInverted)) return FALSE;

	if (cControlType == 'T')
	{
		return (BuildIdentifierString(szResult,nID,'Y',fInverted));
	}

	return FALSE;
}

/////////////////////// NEW FUNCTIONS //////////////////////////
BOOL CBindingConversion::GetInverted(CString szData,BOOL *fInverted)
{
	char c;
	int nLength = szData.GetLength();
	
	*fInverted = FALSE;

	if (nLength == 0) 
	{
		AfxMessageBox("Error - String is empty");
		ASSERT(0);
		return FALSE;
	}

	int i=0;
	c = szData.GetAt(i);
	if (c == '(') i++;

	c = szData.GetAt(i);
	if (c == '!') *fInverted = TRUE;
	
	return TRUE;
}

BOOL CBindingConversion::GetIdentifier(CString szData,unsigned short *nId)
{	
	int nLength = szData.GetLength();
	
	*nId = 0;

	if (nLength == 0) 
	{
		AfxMessageBox("Error - String is empty");
		ASSERT(0);
		return FALSE;
	}	

	char c = szData.GetAt(0);
	if (c == '(') 
	{
		c = szData.GetAt(1);

		if (c == '!') 
		{
			int temp = -1;
			if (!(sscanf(szData,"(!%*1c%i",&temp)))		// CHANGES_MVS_2008_SSCANF
			{
				return FALSE;
			}

			ASSERT( temp >= 0 );
			*nId = static_cast<unsigned short>(temp);
		}
		else 
		{
			int temp = -1;
			if (!(sscanf(szData,"(%*1c%i",&temp)))		// CHANGES_MVS_2008_SSCANF
			{
				return FALSE;
			}

			ASSERT( temp >= 0 );
			*nId = static_cast<unsigned short>(temp);
		}
	}
	
	if (c == '!') 
	{
		// use scanf with ((!)

		int temp = -1;
		if (!(sscanf(szData,"!%*1c%i",&temp)))		// CHANGES_MVS_2008_SSCANF
		{
			return FALSE;
		}

		ASSERT( temp >= 0 );
		*nId = static_cast<unsigned short>(temp);
	}
	else 
	{
		int temp = -1;
		if (!(sscanf(szData,"%*1c%i",&temp)))		// CHANGES_MVS_2008_SSCANF
		{
			return FALSE;
		}

		ASSERT( temp >= 0 );
		*nId = static_cast<unsigned short>(temp);
	}
	return TRUE;
}


BOOL CBindingConversion::GetControlType(CString szData,char *cControlType)
{
	char c;
	int nLength = szData.GetLength();
	
	*cControlType = '\0';

	if (nLength == 0) {
		AfxMessageBox("Error - String is empty");
		ASSERT(0);
		return FALSE;
	}

	int i = 0;
	c = szData.GetAt(i);
	
	if (c == '(') 
	{
		i++;
		if (nLength > 2) c = szData.GetAt(i);
	}
	
	if (c == '!') 
	{
		i++;
		if (nLength > 2) c = szData.GetAt(i);
	}


	if ((c == 'U') || (c == 'T') || (c == 'O') || (c == 'X') || (c == 'Y')) 
	{
		*cControlType = c;
		return TRUE;
	}
	else 
	{
		AfxMessageBox("Error - Undefined controlType");
		ASSERT(0);
		return FALSE;
	}
}


////////////////////
BOOL CBindingConversion::BuildIdentifierString(CString *szResult,unsigned short nId,char cControlType,BOOL fInverse)
{
	ASSERT(szResult);
	szResult->Empty();

	if (nId == 0) 
	{
		AfxMessageBox("Error - Undefined Id");
		ASSERT(0);
		return FALSE;
	}

	if ((cControlType == 'U') || (cControlType == 'O') ||
		(cControlType == 'X') || ((cControlType == 'T') || (cControlType == 'Y'))) 
	{
		if (fInverse == TRUE) 
		{
			szResult->Format("!%c%i",cControlType,nId);
			return TRUE;
		}
		else
		{
			szResult->Format("%c%i",cControlType,nId);
			return TRUE;
		}
	}
	else 
	{
		AfxMessageBox("Error - Undefined control type");
		ASSERT(0);
		return FALSE;
	}
}
////////////////
BOOL CBindingConversion::CheckTypeBinding()
{
	// check nOperators
	// Outputs = operators

	CString szDebugInfo;
	CString szData,szResult;
	char cControlType;
	unsigned short nOutId,nInId;
	int i,j,k,t;
	BOOL fInverse;

	_nTimers=0,_nOperators=0,_nOutputs=0;
	_nInputs = 0,_nInverted = 0;

	int nEntries = szArrayLinkInfo.GetSize();
	
	for (i=0;i<nEntries;i++)
	{
		szData = szArrayLinkInfo.GetAt(i);
		if (!(ParseOutput(szData,&szResult))) return FALSE;					
		if (!(GetIdentifier(szResult,&nOutId))) return FALSE;
		if (!(GetControlType(szResult,&cControlType))) return FALSE;
		{
			if (cControlType == 'T') _nTimers++;
			if (cControlType == 'O') _nOperators++;
			if (cControlType == 'U') _nOutputs++;
		}
	}

	for (i=0;i<nEntries;i++)
	{
		szData = szArrayLinkInfo.GetAt(i);
		k = ParseNInputs(szData);
		if (k != 0)
		{
			for (j=0;j<k;j++)
			{
				szData = szArrayLinkInfo.GetAt(i);
				if (!(ParseInput(szData,&szResult,j))) return FALSE;					
				if (!(GetIdentifier(szResult,&nInId))) return FALSE;
				if (!(GetControlType(szResult,&cControlType))) return FALSE;
				if (cControlType == 'U')
				{
					for (t=0;t<nEntries;t++)
					{
						szData = szArrayLinkInfo.GetAt(t);
						if (!(ParseOutput(szData,&szResult))) return FALSE;					
						if (!(GetIdentifier(szResult,&nOutId))) return FALSE;
						if (!(GetControlType(szResult,&cControlType))) return FALSE;
						if ((cControlType == 'U') && (nOutId == nInId)) break;
					}

					if (t>=nEntries) _nInputs++;
				}
			}
		}
	}

	_nInverted = 0;
	for (i=0;i<nEntries;i++)
	{
		szData = szArrayLinkInfo.GetAt(i);
		k = ParseNInputs(szData);
		if (k != 0)
		{
			for (j=0;j<k;j++)
			{
				szData = szArrayLinkInfo.GetAt(i);
				if (!(ParseInput(szData,&szResult,j))) return FALSE;					
				if (!(GetIdentifier(szResult,&nInId))) return FALSE;
				if (!(GetControlType(szResult,&cControlType))) return FALSE;
				if (!(GetInverted(szResult,&fInverse))) return FALSE;
				if (fInverse == TRUE) _nInverted++;
			}
		}
	}

	szDebugInfo.Format("Inverse Operators:%02d , Operators:%02d , Timers:%02d , Inputs:%02d , Outputs:%02d",
		_nInverted,_nOperators,_nTimers,_nInputs,_nOutputs);
	DebugInfo(szDebugInfo);

	if (pColUnits->Count() != (_nInputs + _nOutputs)) 
	{
		AfxMessageBox("Error Not all units are Connected");
		return FALSE;
	}

	if (pColOperators->Count() != (_nTimers + _nOperators)) 
	{
	 	AfxMessageBox("Error Not all operators/Timers are connected");
		return FALSE;
	}

	if ((_nOperators == 0) &&(_nTimers == 0) && (_nInverted == 0)) 
	{
		if (_nOutputs == 1)  
		{
			// Check Bindingflags, if 0- Make I-Binding
			TypeOfBinding = Type_BG;
		}
		else TypeOfBinding = Type_BGN;
	}
	else 
	{
		if (_nOutputs == 1)  
		{
			// Check Bindingflags, if 0- Make I-Binding
			TypeOfBinding = Type_BC;
		}
		else TypeOfBinding = Type_BCN;
	}
	return TRUE;
}