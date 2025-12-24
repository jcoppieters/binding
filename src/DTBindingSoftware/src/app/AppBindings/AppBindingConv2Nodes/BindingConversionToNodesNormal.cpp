// BindingConversionToNodesNormal.cpp: implementation of the CBindingConversionToNodesNormal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConversionToNodesNormal.h"

#include "BindingEntry.h"
#include "BindingEntryInfo.h"
#include "BindingFile1.h"
#include "BindingFileInfo.h"
#include "BindingFileWorker.h"

#include "views/debug/DlgDebugBindingConversion.h"

#include "App\AppBindings\AppBindingEvalString\BindingEvaluationString.h"

#include "App\Appbindings\AppBindingFileConstruct\BindingFileConstructFactory.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"

#include "TMGlobals.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingConversionToNodesNormal::CBindingConversionToNodesNormal()
{
	ClearStringArray();
}

CBindingConversionToNodesNormal::~CBindingConversionToNodesNormal()
{

}

#ifdef SKIP

BOOL CBindingConversionToNodesNormal::ProcessBindings(CBindingEntry *pEntry)
{
	ASSERT(pEntry);
	
	ClearStringArray();

	CBindingEvaluationString *const pBindingEvaluationString = 
		pEntry->GetEvaluationString();

	if (pBindingEvaluationString != NULL)
	{
		pBindingEvaluationString->Parse();

		CBindingEntryInfo BindingEntryInfo(pEntry);		
		if (BindingEntryInfo.Build() == TRUE)
		{
			ASSERT(BindingEntryInfo.Verify());

			if (ConstructBindingStrings(&BindingEntryInfo) == TRUE) 
			{
				if (AddBindingsToNodes(pEntry,&BindingEntryInfo) == TRUE) 
				{
					return TRUE;
				}
				//ASSERT(0);
			}			
			//ASSERT(0);
		}		
		//ASSERT(0);
	}	
	ASSERT(0);
	return FALSE;
}

#endif


BOOL CBindingConversionToNodesNormal::ProcessBindings(
	CBindingEntry *pEntry)
{
	BOOL fResult=FALSE;
	CBindingEntryInfo bindingEntryInfo(pEntry);

	if (TRUE==CreateBindingStrings(pEntry,&bindingEntryInfo))
	{
		if (AddBindingsToNodes(pEntry,&bindingEntryInfo) == TRUE) 
		{
			fResult=TRUE;
		}
	}			

	return fResult;
}


BOOL CBindingConversionToNodesNormal::CreateBindingStrings(
	CBindingEntry *pEntry,
	CBindingEntryInfo* pBindingEntryInfo)
{
	BOOL fResult=FALSE;

	CBindingEvaluationString *const pBindingEvaluationString = 
		pEntry->GetEvaluationString();

	ClearStringArray();

	if (pBindingEvaluationString != NULL)
	{				
		pBindingEvaluationString->Parse();

		if (pBindingEntryInfo->Build() == TRUE)
		{
			ASSERT(pBindingEntryInfo->Verify());

			if (ConstructBindingStrings(pBindingEntryInfo)==TRUE) 
			{
				fResult=TRUE;
			}
		}
	}
	return fResult;
}

// Aanmaken van de binding strings.
BOOL CBindingConversionToNodesNormal::CreateBindingStrings(
	CBindingEntry *pEntry,
	CStringArray* pStrArray)
{
	BOOL fResult=FALSE;
	CBindingEntryInfo BindingEntryInfo(pEntry);		

	if (TRUE==CreateBindingStrings(pEntry,&BindingEntryInfo))
	{
		CStringArray* const pBindingStrings = this->GetStringArray();

		if (0!=pBindingStrings)
		{
			if (0!=pStrArray)
			{
				pStrArray->Copy(*pBindingStrings);
			}
			fResult=TRUE;
		}
	}
	return fResult;
}

BOOL CBindingConversionToNodesNormal::DeleteBindings(unsigned short usBindingNr)
{
	if (1)
	{
		CString szDebug;
		szDebug.Format("Remove Bindingnr : %04x from Nodes ... ",usBindingNr);
		CDlgDebugBindingConversion::Add(szDebug);
	}
	
	int iNrOfNodes;
	CCanNode *pNode;		
	CBindingFile *ptrBindingFile = NULL;


	TMGetDocumentNodes()->NROfNodes(&iNrOfNodes);
	
	for (int i=0;i<iNrOfNodes;i++) 
	{
		if (TMGetDocumentNodes()->GetNode(i,&pNode))	
		{					
			if ((ptrBindingFile = pNode->GetBindingFilePtr()) != NULL) 
			{
				ptrBindingFile->RemoveStrings(usBindingNr);
			}							
		}
	}

#ifdef _DEBUG
	if (1)
	{
		/**
		 * Check if all entries for usBindingNr are removed in de bindingfiles.
		 */
		for (int t=0;t<iNrOfNodes;t++) 
		{
			if (TMGetDocumentNodes()->GetNode(t,&pNode))	
			{	
				ASSERT(pNode->GetBindingFilePtr());
				ASSERT(pNode->GetBindingFilePtr()->FindStrings(usBindingNr) == FALSE);
			}
		}
	}
#endif
	return TRUE;
}


BOOL CBindingConversionToNodesNormal::ConstructBindingStrings(CBindingEntryInfo* const pBindingEntryInfo)
{
	CBindingFileConstructFactory Factory(pBindingEntryInfo,GetStringArray());

	CBindingFileConstructFormatter *const pBindingFileConstruct = Factory.Create();

	if (pBindingFileConstruct != NULL) 
	{
		return (pBindingFileConstruct->Format());
	}

	ASSERT(pBindingFileConstruct);

	return FALSE;
}


BOOL CBindingConversionToNodesNormal::AddBindingsToNodes(
	CBindingEntry *pEntry,
	CBindingEntryInfo* const pBindingEntryInfo)
{	
	if (1)
	{
		CString szDebug("Add Bindings to Nodes ... ");
		CDlgDebugBindingConversion::Add(szDebug);
	}

		
	const int nEntries = GetStringArraySize();	
	ASSERT(nEntries);
	
	if (nEntries == 0) return FALSE;
	
	int nNodes;
	TMGetDocumentNodes()->NROfNodes(&nNodes);

	const unsigned short usBindingNr = pEntry->GetBindingNr();
		
	if (1)
	{
			
		/**
		 * WE MUST DELETE BINDINGNR IN ALL NODES OF DOCDATABASE !!!!
		 */

		CCanNode *pNode = NULL;
		CBindingFile *ptrBindingFile = NULL;

		for (int i=0;i<nNodes;i++) 
		{
			if (TMGetDocumentNodes()->GetNode(i,&pNode)) 
			{										
				if ((ptrBindingFile = pNode->GetBindingFilePtr()) != NULL) 
				{
					ptrBindingFile->RemoveStrings(usBindingNr);	
				}			
			}
		}
	}

#ifdef _DEBUG	
		
	if (1)
	{

		/**
		 * Check for all nodes if bindingstring with usbindingnr has been removed.
		 */

		CCanNode *pNode = NULL;

		for (int t=0;t<nNodes;t++) 
		{
			if (TMGetDocumentNodes()->GetNode(t,&pNode))	
			{	
				ASSERT(pNode->GetBindingFilePtr());
				ASSERT(pNode->GetBindingFilePtr()->FindStrings(usBindingNr) == FALSE);
			}
		}
	}

#endif
	

	// NEW --> Check after bindings are delete if nodes can hold new bindingentries ...	
	if (CheckBindingFileProperties(pBindingEntryInfo) == FALSE) 
	{
		const CString szMessage(_T("This binding has not been added to the binding files.\n"
								   "If this was an existing binding that was changed it has been removed from the binding files."));						  
		AfxMessageBox(szMessage);
		return FALSE;
	}

	if (1)
	{
		/** 
		 * NOW WE MUST ADD BINDINGS TO ALL NODES 
		 */

		CString szBindingString;
		BYTE bNodeAddress = 0;		

		for (int i=0;i<nEntries;i++) 
		{
			szBindingString = GetStringFromArray(i);	
			ASSERT(szBindingString.IsEmpty()==FALSE);

			if (1)
			{
				const CBindingFileEntryOperations Operations(szBindingString);
				bNodeAddress = Operations.GetNodeAddress();
				if (bNodeAddress == 0xFF) return FALSE;
			}

			int iEntryNr;
			CCanNode *pNode = NULL;
			CBindingFile *ptrBindingFile = NULL;

			if (TMGetDocumentNodes()->FindNode(bNodeAddress,&iEntryNr,&pNode)) 
			{										
				if ((ptrBindingFile = pNode->GetBindingFilePtr()) != NULL)
				{					
					if (ptrBindingFile->AddString(usBindingNr,szBindingString)) 
					{
						CString szDebug;

						szDebug.Format("Node 0x%02X : BindingString '%s'",
									   bNodeAddress,
									   szBindingString);

						CDlgDebugBindingConversion::Add(szDebug);		
						
					}// if (ptrBindingFile->AddString(usBindingNr,szBindingString)) 						
				}// if ((ptrBindingFile = pNode->GetBindingFilePtr()) != NULL)						
			}		
		} //for (int i=0;i<nEntries;i++) 

	} // if (1)	
	
	return TRUE;
}


// CHANGES_BINDINGS_CONDITIONLENGTH: Controle op de C binding conditie lengte.
BOOL CBindingConversionToNodesNormal::CheckBindingFileProperties(CBindingEntryInfo* const pBindingEntryInfo)
{			
	CNodeDatabase* const pNodeDatabaseDoc = TMGetDocumentNodes();
	if (pNodeDatabaseDoc)
	{
		ASSERT(pBindingEntryInfo);

		// Controle op het aantal bindingstrings in een node.
		const BINDINGTYPE type = pBindingEntryInfo->Get_Type();
		if (CheckNrEntries(pNodeDatabaseDoc)==FALSE) 
		{
			return FALSE;
		}
		
		if (type == TYPE_BC_BINDING || type == TYPE_BCN_BINDING)
		{	
			// Indien type een C of CN binding is.

			// Controle op het aantal C-Bindings, references, timers, ...
			if (CheckCBindingEntry(pNodeDatabaseDoc)==FALSE) 
			{
				return FALSE;
			}

			// Controle op de C binding conditie lengte.
			// Geven node database mee als param omdat max conditie lengte per node
			// bepaalt wordt.
			if (CheckCBindingConditionLength(pNodeDatabaseDoc)==FALSE) 
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

// Zie CHANGES_BINDINGS_REFERENCES: Extra controle op het aantal references.
// Zie CHANGES_BINDINGS_ERRORMESSAGES: Fout berichten op een andere manier.
BOOL CBindingConversionToNodesNormal::CheckCBindingEntry(CNodeDatabase *pDocDatabase)
{	
	const int nEntries = GetStringArraySize();	
	int nrOfCBindingsNeeded = 0;			
	int nrOfTimersNeeded = 0;		
	int nrOfReferencesNeeded = 0;
	BYTE bNodeAddress = 0xFF;	

	if (nEntries == 0) 
	{
		return FALSE;	
	}

	ASSERT(pDocDatabase);

	if (1)	
	{					
		// Controle hoeveel C-Bindings, references, timers er nodig zijn voor deze
		// nieuwe binding.
		// Oppassen: Hier wordt de klasse CBindingFileEntryOperations gebruikt.
		for (int i=0;i<nEntries;i++) 
		{
			const CString szBindingString(GetStringFromArray(i));
			const CBindingFileEntryOperations bindingFileOperations(szBindingString);

			if (bindingFileOperations.ConditionBinding() == TRUE)
			{
				nrOfCBindingsNeeded++;
				
				bNodeAddress = bindingFileOperations.GetNodeAddress();
				if (bNodeAddress == 0xFF) 
				{
					return FALSE;
				}
				nrOfTimersNeeded = bindingFileOperations.GetNrOfTimers();				
				nrOfReferencesNeeded = bindingFileOperations.GetNrOfReferences();
				break;	
			}
		}	
		if (nrOfCBindingsNeeded == 0)
		{		
			return FALSE;
		}
	}

	if (1)			
	{
		int i;	
		CCanNode *pNode = 0;
	
		ASSERT(bNodeAddress>0x00 && bNodeAddress<0xFF);

		if (pDocDatabase->FindNode(bNodeAddress,&i,&pNode) == TRUE)	
		{
			// Controle op de binding strings die aanwezig zijn in de binding
			// files van de nodes
			// Oppassen: Hiervoor moet de klasse CBindingFileWorker gebruikt worden.

			CBindingFileWorker BindingFileWorker(pNode->GetBindingFilePtr());
			if (BindingFileWorker.CalculateCBindingProperties() == TRUE)
			{
				// Controle of het aantal nodig kan toegevoegd worden aan de binding
				// files van de nodes.

				if (FALSE==BindingFileWorker.CheckNrOfConditionBindings(nrOfCBindingsNeeded))
				{					
					notifyError(ERROR_MAX_C_BINDINGS,pNode,nrOfCBindingsNeeded);
					return FALSE;
				}			
				if (FALSE==BindingFileWorker.CheckNrOfTimers(nrOfTimersNeeded))
				{				
					notifyError(ERROR_MAX_TIMER_OPERATORS,pNode,nrOfTimersNeeded);
					return FALSE;
				}							
				if (FALSE==BindingFileWorker.CheckNrOfReferences(nrOfReferencesNeeded))
				{
					notifyError(ERROR_MAX_REFERENCES,pNode,nrOfReferencesNeeded);
					return FALSE;
				}
				return TRUE;
			}
		}
	}	
	ASSERT(0);
	return FALSE;
}




















