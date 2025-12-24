// BindingConversionToNodes.cpp: implementation of the CBindingConversionToNodes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConversionToNodes.h"


#include "MainFrm.h"
#include "BindingFile1.h"
#include "NodeDatabase.h"
#include "BindingFileInfo.h"
#include "BindingFileWorker.h"

#include "App\Appbindings\AppBindingFileConstruct\BindingFileConstruct.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingConversionToNodes::CBindingConversionToNodes()
{
}

CBindingConversionToNodes::~CBindingConversionToNodes()
{
}

// CHANGES_BINDINGS_CONDITIONLENGTH.
// Controle node properties max C binding conditie lengte.
// Zoeken in welke node de C-Binding moet bewaard worden en controle op de lengte
// van de C-binding conditie string.
BOOL CBindingConversionToNodes::CheckCBindingConditionLength(
	CNodeDatabase *pDocDatabase)
{
	BOOL fResult=TRUE;

	const int nEntries = GetStringArraySize();	
	for (int i=0;i<nEntries;++i)
	{
		const CString szBindingString(GetStringFromArray(i));

		CBindingFileEntryOperations alg(szBindingString);
		if (TRUE==alg.ConditionBinding())
		{
			const BYTE bNodeAddress = alg.GetNodeAddress();
			if (bNodeAddress != 0xFF) 
			{
				int j;
				CCanNode *pNode = 0;

				if (pDocDatabase->FindNode(bNodeAddress,&j,&pNode) == TRUE)
				{		
					int length = alg.GetCBindingConditionLength();
					int maxNodeLength = pNode->MaxConditionLength();

					if (length>maxNodeLength)
					{
						notifyError(ERROR_MAX_C_LENGTH,pNode,length);
						fResult=FALSE;						
					}
				}
			}
			// Break for loop - enkel de C-binding nodig.
			break;	
		}
	}
	return fResult;
}

// CHANGES_BINDINGS_ERRORMESSAGES: Foutmelding op een andere manier.
BOOL CBindingConversionToNodes::CheckNrEntries(CNodeDatabase *pDocDatabase)
{
	const int nEntries = GetStringArraySize();	
	if (nEntries == 0) 
	{
		return FALSE;	
	}
	
	for (int i=0;i<nEntries;i++) 
	{
		BYTE bPreviousNode = 0xFF;
		BYTE bNodeAddress = 0;

		const CString szBindingString(GetStringFromArray(i));
		if (1)
		{
			CBindingFileEntryOperations Operations(szBindingString);
			bNodeAddress = Operations.GetNodeAddress();
			if (bNodeAddress == 0xFF) 
			{
				return FALSE;
			}
		}

		if (bNodeAddress != bPreviousNode) 
		{		
			int j;
			CCanNode *pNode = 0;

			if (pDocDatabase->FindNode(bNodeAddress,&j,&pNode) == TRUE)
			{		
				// zie CHANGES_CALCULATE_NROFSTRINGS
				// Moeten hier geen parse doen, moeten enkel controleren of het
				// aantal bindings niet overschreden wordt.

				CBindingFileWorker BindingFileWorker(pNode->GetBindingFilePtr());
				if (BindingFileWorker.CalculateNrOfBindingStrings() == TRUE)
				{
					const int nCnt = 
						CBindingFileConstruct::GetNrOfEntries(
							GetStringArray(),bNodeAddress);

					if (BindingFileWorker.CheckNrOfBindings(nCnt) == FALSE)
					{						
						notifyError(ERROR_MAX_BINDINGS,pNode,nCnt);	
						return FALSE;
					}									
				}	
				bPreviousNode = bNodeAddress;
			}
			else
			{
				ASSERT(0);
				return FALSE;
			}
		}// if bNodeAddress != bPreviousNode ...
	}
	return TRUE;
}

// Zie CHANGES_BINDINGS_ERRORMESSAGES
// Foutmeldingen uniform gemaakt via deze functie.
void CBindingConversionToNodes::notifyError(
	Error_t err,
	CCanNode* pNode,
	int neededEntries)
{
	const CBindingFileInfo* const ptrBindingFileInfo = 
		pNode->GetBindingFilePtr()->GetBindingFileInfo();
				
	if (ptrBindingFileInfo != NULL)
	{		
		const BYTE bNodeAddress = pNode->getNodeAddress();
		const char* const szNodeName = &pNode->szNodeName[0];

		CString szMessage;

		switch(err)
		{
			case ERROR_MAX_BINDINGS:
			{
				const int maxNodeBindings = ptrBindingFileInfo->GetNodeMaxBindings();
				const int maxCurrBindings = ptrBindingFileInfo->GetNrBinding();

				szMessage.Format(_T("Cannot add this binding because the device with logical address=0x%02x,name='%s' "
									"already holds %d of maximum %d binding strings."
									"This binding needs %d binding strings."),
									bNodeAddress,
									szNodeName,
									maxCurrBindings,
									maxNodeBindings,
									neededEntries);					
			} break;

			case ERROR_MAX_TIMER_OPERATORS:		
			{
				const int maxNodeTmrOperators = ptrBindingFileInfo->GetNodeMaxTimers();
				const int maxCurrTmrOperators = ptrBindingFileInfo->GetNrTimers();

				szMessage.Format(_T("Cannot add this binding because the device with logical address=0x%02x,name='%s' "
									"already holds %d of maximum %d operator timers."
									"This binding needs %d operator timers."),
									bNodeAddress,
									szNodeName,
									maxCurrTmrOperators,
									maxNodeTmrOperators,
									neededEntries);
			} break;

			case ERROR_MAX_C_BINDINGS:		
			{
				const int maxNodeCBindings = ptrBindingFileInfo->GetNodeMaxCBindings();
				const int maxCurrCBindings = ptrBindingFileInfo->GetNrCBindings();

				szMessage.Format(_T("Cannot add this binding because the device with logical address=0x%02x,name='%s' "
									"already holds %d of maximum %d condition bindings."
									"This binding needs %d condition binding."),
									bNodeAddress,
									szNodeName,
									maxCurrCBindings,
									maxNodeCBindings,
									neededEntries);
			} break;

			case ERROR_MAX_REFERENCES:		
			{
				const int maxNodeReferences = ptrBindingFileInfo->GetNodeMaxReferences();
				const int maxCurrReferences = ptrBindingFileInfo->GetNrReferences();

				szMessage.Format(_T("Cannot add this binding because the device with logical address=0x%02x,name='%s' "
								    "already holds %d of maximum %d references(=inputs)."
									"This binding needs %d references(=inputs)."),
									bNodeAddress,
									szNodeName,
									maxCurrReferences,
									maxNodeReferences,
									neededEntries);			
			} break;

			case ERROR_MAX_C_LENGTH:
			{		
				const int maxNodeCBindingLength = ptrBindingFileInfo->getNodeMaxConditionLength();

				szMessage.Format(_T("Cannot add this binding to the device with logical address=0x%02x,name='%s'."
									"The maximum condition length in this device is %d bytes."
									"This binding needs a condition length of %d bytes.\n"
									"A possible reason is because the binding has to much inputs."
									"A possible solution is to split up the binding in two or more bindings."),								
									bNodeAddress,
									szNodeName,
									maxNodeCBindingLength,
									neededEntries);
			} break;

			default:
			{
				ASSERT(0);

			} break;
		}

		if (!szMessage.IsEmpty())
		{
			AfxMessageBox(szMessage,MB_ICONERROR);	
		}
	}
}