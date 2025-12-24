// BindingFilesOperations.cpp: implementation of the CBindingFilesOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFilesOperations.h"

#include "BindingFileOperations.h"
#include "NodeDatabase.h"
#include "CanNode.h"
#include "BindingFileInfo.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Private module functies.

static
int bindingstring_parseNodeAddress(
	const char* s)
{
	int retVal = -1;

	if (0!=s)
	{
		int temp=-1;

		if(sscanf(s,"0000%02X_",&temp)==1)
		{
			if(temp>=0x03 && temp<=0xFC)
			{
				retVal=temp;
			}
		}
	}
	return retVal;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFilesOperations::CBindingFilesOperations(CBindingFiles* pBindingFiles) :
m_pBindingFiles(pBindingFiles)
{
	ASSERT(m_pBindingFiles);
}
CBindingFilesOperations::~CBindingFilesOperations()
{
}

// TM 20081223: Gebruik van Iterators.
unsigned short CBindingFilesOperations::GetMax(unsigned short usMin,unsigned short usMax)
{
	unsigned short usMaxBindingNr = 0;
	unsigned short usTemp = 0;
	
	CBindingFiles::Iterator it(m_pBindingFiles->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{	
		ASSERT( it.CurrentItem() );	

		CBindingFileOperations BindingFileOperations(it.CurrentItem());		
		usTemp = BindingFileOperations.GetMax(usMin,usMax);

		if (usTemp > usMaxBindingNr) 
		{
			usMaxBindingNr = usTemp;
		}		
	}
	return usMaxBindingNr;
}

// TM 20081223: Gebruik van Iterators.
BOOL CBindingFilesOperations::FindStrings(unsigned short usBindingNr,CStringArray& strArray)
{	
	strArray.RemoveAll();

	CBindingFiles::Iterator it(m_pBindingFiles->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{			
		ASSERT( it.CurrentItem() );
		
		CBindingFileOperations BindingFileOperations(it.CurrentItem());			
		BindingFileOperations.FindStrings(usBindingNr,strArray);	
	}	

	if (strArray.GetSize() == 0) 
		return FALSE;
	
	return TRUE;
}
// TM 20081223: Gebruik van Iterators.
BOOL CBindingFilesOperations::CheckModified()
{	
	CBindingFiles::Iterator it(m_pBindingFiles->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{					
		CBindingFile* const pFile = it.CurrentItem();
		ASSERT( pFile );
				
		if (pFile->IsModified())		
			return TRUE;		
	}
	return FALSE;
}

// TM 20081223: Gebruik van Iterators.
BOOL CBindingFilesOperations::Replace(const CStringArray &m_StringArray,const CStringArray& StringsToReplace)
{
	CBindingFiles::Iterator it(m_pBindingFiles->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{	
		CBindingFile* const pBindingFile = it.CurrentItem();		
		ASSERT( pBindingFile );
	
		const BYTE bNodeAddress = pBindingFile->GetNodeAddress();			
		const unsigned short usMaxArrayEntries = m_StringArray.GetSize();
				
		CStringArray NewStringArray;
		CStringArray NewStringArrayToReplace;
		
		for (int j=0,t=0; j<usMaxArrayEntries; j++)
		{	
			CString szEntry(m_StringArray.GetAt(j));

			CBindingFileEntryOperations BindingFileEntryOperations(szEntry);
			if (BindingFileEntryOperations.GetNodeAddress() == bNodeAddress)
			{
				NewStringArray.InsertAt(t,szEntry);
				NewStringArrayToReplace.InsertAt(t,StringsToReplace.GetAt(j));

				t++;					
			}
		}

		if (NewStringArray.GetSize() != 0)
		{
			CBindingFileOperations BindingFileOperations(pBindingFile);
			const BOOL fResult = BindingFileOperations.Replace(NewStringArray,NewStringArrayToReplace);
			ASSERT( fResult );
		}		
	}
	return TRUE;
}

/**
 * Create new bindingFiles for nodes in NodeDatabase that have no bindingfile
 */
BOOL CBindingFilesOperations::CreateNewFiles(CNodeDatabase* const pNodeDatabase)
{
	if (pNodeDatabase != 0)
	{
		int NrOfNodes;
		if (pNodeDatabase->NROfNodes(&NrOfNodes))
		{
			for (int i=0;i<NrOfNodes;i++) 
			{
				CCanNode *pNode = 0;
				if (pNodeDatabase->GetNode(i,&pNode) == TRUE) 
				{						
					if (m_pBindingFiles->Get(pNode->bLogicalAddress) == FALSE)
					{
						m_pBindingFiles->Add(pNode->bLogicalAddress);				
					}
				}
				else
				{
					ASSERT(0);
					return FALSE;
				}
			}
			return TRUE;
		}		
	}
	return FALSE;
}

BOOL CBindingFilesOperations::Assign(CNodeDatabase* const pNodeDatabase)
{
	ASSERT( pNodeDatabase );
	ASSERT( m_pBindingFiles );

	if (pNodeDatabase != 0)
	{
		int NrOfNodes;
		if (pNodeDatabase->NROfNodes(&NrOfNodes))
		{
			if (NrOfNodes != m_pBindingFiles->Count()) 
			{
				ASSERT( 0 );
				CString szMessage("Error: CBindingFilesOperations::Assign\n"
								  "NrOfNodes != NrOfBindingFiles");
				AfxMessageBox(szMessage);
				return FALSE;
			}
	
			CBindingFiles::Iterator it(m_pBindingFiles->CreateIterator());
			for (it.Begin();!it.IsDone();it.Next())
			{	
				CBindingFile* const pFile = it.CurrentItem();		
				ASSERT( pFile );

				if (pFile != 0)
				{
					int t;
					const BYTE bNodeAddress = pFile->GetNodeAddress();				
					CCanNode* pNode = 0;

					if (pNodeDatabase->FindNode(bNodeAddress,&t,&pNode) == FALSE) 
					{
						ASSERT( 0 );
						CString szMessage("Error: CBindingFilesOperations::Assign\n"
										  "CNodeDatabase::FindNode() failed ...");
						AfxMessageBox(szMessage);
						return FALSE;
					}

					pNode->SetBindingFilePtr(pFile);

					CBindingFileInfo* const ptrBindingFileInfo = pFile->GetBindingFileInfo();							
					if (ptrBindingFileInfo == 0) 
					{
						ASSERT( 0 );
						CString szMessage("Error: CBindingFilesOperations::Assign\n"
										  "CBindingFile::GetBindingFileInfo() failed ...");
						AfxMessageBox(szMessage);
						return FALSE;
					}

					ptrBindingFileInfo->SetNodeMaxBindings(pNode->MaxBindingEntries());
					ptrBindingFileInfo->SetNodeMaxCBindings(pNode->MaxConditions());
					ptrBindingFileInfo->SetNodeMaxReferences(pNode->MaxReferences());
					ptrBindingFileInfo->SetNodeMaxTimers(pNode->MaxTimerOperators());
					ptrBindingFileInfo->setNodeMaxConditionLength(pNode->MaxConditionLength());
				}			
			} 
			return TRUE;

		} // if (pNodeDatabase->NROfNodes(&NrOfNodes))
	} // if (pNodeDatabase != 0)

	return FALSE;
}

int CBindingFilesOperations::RemoveStrings(
	BYTE bNodeAddress,
	BYTE bUnitAddress)
{
	int result = 0;

	CBindingFiles::Iterator it(m_pBindingFiles->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{			
		CStringArray strArray;
		CBindingFileOperations bindingFileOperations(it.CurrentItem());

		const int nStringsToRemove = bindingFileOperations.find(
										bNodeAddress, 
										bUnitAddress, 
										strArray);

		ASSERT( nStringsToRemove == strArray.GetSize() );

		if (nStringsToRemove > 0)
		{
			if (bindingFileOperations.remove(strArray))
			{
				result += nStringsToRemove;
			}		
		}
	}
	return result;
}



// Sedert versie 0x0902: Zoeken of de strings aanwezig zijn.
// Return value = Node Address 
// Niet gevonden = -1
int CBindingFilesOperations::FindString(
	const CString& refString)
{
	int retVal = -1;

	if (0!=m_pBindingFiles)
	{
		const int nodeAddress = 
			bindingstring_parseNodeAddress((const char*)refString);

		if (nodeAddress>0)
		{
			CBindingFile* const pBindingFile=m_pBindingFiles->Get(nodeAddress);

			if (0!=pBindingFile)
			{
				CBindingFileOperations bindingFileOperations(pBindingFile);

				if (TRUE==bindingFileOperations.findString(refString))
				{
					retVal=nodeAddress;
				}							
			}
		}
	}
	return retVal;
}