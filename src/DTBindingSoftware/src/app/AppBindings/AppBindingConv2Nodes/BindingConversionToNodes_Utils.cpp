#include "stdafx.h"
#include "bindingconversiontonodes_utils.h"
#include "bindingconversiontonodesnormal.h"
#include "BindingConversionToNodesProperty.h"

#include "bindingfilesoperations.h"

#include "nodedatabase.h"
#include "bindingentries.h"

#include "app\appbindings\appbindingproperty\bindingpropertyunit.h"

#include "tmglobals.h"
using namespace TMBindingDatabase;


// Prototypes.
static
void bindingconversiontonodes_AddError(
    CPtrList* pListErrors,
	CBindingEntry* pEntry);

static
void bindingconversiontonodes_AddError(
    CPtrList* pListErrors,
	CBindingPropertyUnit* pUnit);



// Public: struct CBindingConversionToNodes_Utils

int CBindingConversionToNodes_Utils::AddBindingEntries(
	CBindingEntries& ref,
	CPtrList* pListErrors,
	void* pvUserData,
	pf_progress pf)
{
	typedef CBindingEntries::Iterator Iterator;

	CBindingConversionToNodesNormal conv;
	int retVal=0;
	int count=0;
	const int max=ref.Count();

	for (Iterator it(ref.CreateIterator());
		 !it.IsDone();
		 it.Next())
	{
		CBindingEntry* const pEntry = it.CurrentItem( );
		if (0!=pEntry)
		{		
			if (TRUE==conv.ProcessBindings(pEntry))
			{
				// Ervoor zorgen dat alle bindings opnieuw doorgestuurd worden.
				pEntry->SetSend(FALSE);
				++retVal;
			}
			else
			{
				bindingconversiontonodes_AddError(
					pListErrors,
					pEntry);
			}

			if(0!=pf)
			{
				pf(pvUserData,count,max);
			}
			++count;
		}		
	}
	return retVal;	
}

int CBindingConversionToNodes_Utils::AddBindingProperty(
	CNodeDatabase& refNodeDatabase,
	CPtrList* pListErrors,	
	void* pvUserData,
	pf_progress pf)
{
	int retVal=0;
	int counter=0;
	int maxElements=0;
	CNodeIterator itNodes(refNodeDatabase.CreateNodeIterator());

	for(itNodes.Begin();!itNodes.IsDone();itNodes.Next())
	{
		CCanNode* const pNode = itNodes.CurrentItem();

		if (0!=pNode)
		{
			maxElements+=pNode->getNROfUnits();
		}
	}

	for(itNodes.Begin();!itNodes.IsDone();itNodes.Next())
	{
		CCanNode* const pNode = itNodes.CurrentItem();

		if (0!=pNode)
		{
			CUnitIterator itUnits(pNode);

			for(itUnits.Begin();!itUnits.IsDone();itUnits.Next(),++counter)
			{
				STRUCT_UNIT* const pUnit=itUnits.CurrentItem();
				if(NULL!=pUnit) {
					CBindingPropertyUnit* const pBindingUnit = pUnit->m_pBindingUnit;
					if (NULL!=pBindingUnit)
					{
						int tempVal=0;
						CBindingConversionToNodesProperty conv(pBindingUnit);

						// Oppassen: fMakeNoDefaults==TRUE.
						// Dit zorgt ervoor dat enkel de noodzakelijke P-Bindings aangemaakt
						// worden.

						// Oppassen: fShowErrorMessage==FALSE
						// Dit zorgt ervoor dat er geen ppi-up messagebox komt.
						// Foutmeldingen komen in het stringArray.

						const BOOL fMakeNoDefaults_YES = TRUE;
						const BOOL fShowErrorMessage_NO = FALSE;

						tempVal = conv.ProcessPropertyBindings(
									fMakeNoDefaults_YES, 
									fShowErrorMessage_NO);

						if(tempVal>=0)
						{
							retVal+=tempVal;	
						}					
						else
						{
							bindingconversiontonodes_AddError(
								pListErrors,
								pBindingUnit);
						}
					}

					if(0!=pf)
					{
						pf(pvUserData,counter,maxElements);
					}
				}				
			}
		}
	}

	return retVal;
}

CBindingConversionToNodes_Utils::CompareResult_t 
CBindingConversionToNodes_Utils::SearchBindingEntriesInBindingFiles(
	CNodeDatabase& refNodeDatabase,
	CBindingEntries& ref,
	CPtrList* pListErrors,
	void* pvUserData,
	pf_progress pf)
{
	typedef CBindingEntries::Iterator Iterator;

	CompareResult_t result=COMPARE_RESULT_EC_GENERAL;
	CBindingConversionToNodesNormal conv;
	CBindingFilesOperations bindingFilesOperations(TMGetBindingFiles());
	const int count = ref.Count();
	int retVal=0;
	int errors=0;
	int counter=0;

	for (Iterator it(ref.CreateIterator());
		 !it.IsDone();
		 it.Next())
	{
		CBindingEntry* const pEntry = it.CurrentItem( );

		if (0!=pEntry)
		{		
			CStringArray strArray;

			if (TRUE==conv.CreateBindingStrings(pEntry,&strArray))
			{
				int i=0;
				const int count=strArray.GetCount();

				for(i=0;i<count;++i)
				{
					CString s(strArray.GetAt(i));

					if (bindingFilesOperations.FindString(s)<0)
					{
						if(0!=pListErrors)
						{
							pListErrors->AddTail(pEntry);
						}
						++errors;
						break;
					}
				}
			}

			if(0!=pf)
			{
				pf(pvUserData,counter,count);
			}

			counter++;
		}
	}

	if(0==errors)
	{
		result=COMPARE_RESULT_SUCCESS;
	}
	return result;
}

int CBindingConversionToNodes_Utils::SearchManualBindingEntries(
	CStringArray* pStringArray,
	void* pvUserData,
	pf_progress pf)
{
	int nDif = 0;	
	CBindingFiles* const pBindingFiles=TMGetBindingFiles();
	CBindingEntries* const pBindingEntries=TMGetBindingEntries();
	
	ASSERT(0!=pStringArray);

	if ((0!=pBindingFiles)&&(0!=pBindingEntries))
	{
		int counter=0;
		const int count=pBindingFiles->Count();
		CBindingFiles::Iterator it(pBindingFiles->CreateIterator());

		for (it.Begin();!it.IsDone();it.Next(),++counter)
		{
			CBindingFile* const pBindingFile = it.CurrentItem();								
			BOOL fMatch = FALSE;
			const int MAX = pBindingFile->Count();

			for (int j=0;j<MAX;j++,fMatch=FALSE) 
			{									
				const unsigned short usBindingNr = pBindingFile->GetBindingNr(j);

				if (usBindingNr != 0) 
				{	
					// Check BindingEntries if match:
					// Controleren of er voor deze entry in de bindingFile een binding in de database
					// aanwezig is met hetzelfde binding NR.
					// Indien niet aanwezig wordt deze bindingstring toegevoegd aan een string array.

					CBindingEntries::Iterator bindEntry_it(pBindingEntries->CreateIterator());
					for (bindEntry_it.Begin( );!bindEntry_it.IsDone( );bindEntry_it.Next())
					{
						CBindingEntry* const pBindingEntry = bindEntry_it.CurrentItem( );
						ASSERT( pBindingEntry );			
				
						if (pBindingEntry->GetBindingNr() == usBindingNr) 
						{
							fMatch = TRUE;
							break;
						}					
					} 			
					if (fMatch == FALSE) 
					{
						nDif++;							
						pStringArray->Add(pBindingFile->GetString(j));					
					}						
				} // if (usBindingNr != 0) 
			}//for (int j=0;j<pBindingFile->Count();j++,fMatch=FALSE) 

			if (0!=pf)
			{
				pf(pvUserData,counter,count);
			}

		}//for (int i=0;i<m_pBindingFiles->Count();i++) 
	} /* if (0!=pBindingFiles) */

	ASSERT(nDif>= 0 && nDif<=0xFFFF);			
	ASSERT(pStringArray->GetCount()==nDif);
	return nDif;		
}


// refListErrors bevat referenties naar binding entries.
int CBindingConversionToNodes_Utils::formatStrArrECBindingEntriesInBindingFiles(
	CPtrList& refListErrors,
	CStringArray* pStringArray)
{
	int retVal = 0;

	if (0!=pStringArray)
	{
		const int count = refListErrors.GetCount();

		pStringArray->RemoveAll( );

		if(count>0)
		{
			if(!refListErrors.IsEmpty())
			{
				POSITION pos = refListErrors.GetHeadPosition();
				while(pos!=NULL) 
				{
					CBindingEntry* pEntry = 0;

					if ((pEntry=(CBindingEntry*) refListErrors.GetNext(pos)) != NULL)
					{
						CString szMessage;

						szMessage.Format(_T("Failed to add binding nr 0x%04x, name='%s'"),
							pEntry->GetBindingNr(),
							pEntry->GetName());

						pStringArray->Add(szMessage);

						retVal++;
					}
				}
			}

			ASSERT(retVal == pStringArray->GetCount());
		}
	}
	return retVal;
}

// refListErrors bevat referenties naar binding property units.
int CBindingConversionToNodes_Utils::formatStrAttECBindingUnitsInBindingFiles(
	CPtrList& refListErrors,
	CStringArray* pStringArray)
{
	int retVal = 0;

	if (0!=pStringArray)
	{
		const int count = refListErrors.GetCount();

		pStringArray->RemoveAll( );

		if(count>0)
		{
			if(!refListErrors.IsEmpty())
			{
				POSITION pos = refListErrors.GetHeadPosition();
				while(pos!=NULL) 
				{
					CBindingPropertyUnit* pUnit=0;

					if ((pUnit=(CBindingPropertyUnit*) refListErrors.GetNext(pos)) != NULL)
					{
						CString szMessage;

						szMessage.Format(_T("Failed to add property bindings for the unit %02x,%02x"),
							pUnit->GetNodeAddress(),
							pUnit->GetUnitAddress());
							
						pStringArray->Add(szMessage);

						retVal++;
					}
				}
			}

			ASSERT(retVal == pStringArray->GetCount());
		}
	}
	return retVal;
}



// Private.

static
void bindingconversiontonodes_AddError(
    CPtrList* pListErrors,
	CBindingEntry* pEntry)
{
	if(0!=pListErrors)
	{
		pListErrors->AddTail(pEntry);
	}
}


void bindingconversiontonodes_AddError(
    CPtrList* pListErrors,
	CBindingPropertyUnit* pUnit)
{
	if(0!=pListErrors)
	{
		pListErrors->AddTail(pUnit);
	}
}