// BindingEntries.cpp: implementation of the CBindingEntries class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEntries.h"
#include "BindingUnitFactory.h"
#include "BindingOperatorFactory.h"
#include "BindingLinkFactory.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingEntries::CBindingEntries()
{
}
CBindingEntries::~CBindingEntries()
{
	RemoveAll();
}

/////////////////////////////////////////////////////////////////////

// TM 20081223: Changed function ...
void CBindingEntries::RemoveAll(void)
{
	while (!List.IsEmpty())
	{
		CBindingEntry* const pEntry = 
			reinterpret_cast<CBindingEntry*>(List.RemoveHead( ));

		ASSERT( pEntry );
		delete pEntry;
	}
	ASSERT( List.GetCount( ) == 0 );
}

unsigned short CBindingEntries::GetNextAvailableID(void)
{
	unsigned short nCurr;
	unsigned short nLast;
	
	CBindingEntry *pEntry;
	
	nCurr = nLast = 0;

	POSITION pos = List.GetHeadPosition();
	
	while(pos!=NULL)
	{
		pEntry = (CBindingEntry*) List.GetNext(pos);		
		
		ASSERT(pEntry);

		if (pEntry != NULL)
		{
			nCurr = pEntry->GetID();
			if (nCurr > nLast) 
			{
				nLast = nCurr;
			}
		}
	}

    nLast +=1;

	return (nLast);
}

#ifdef SKIP
unsigned short CBindingEntries::GetNextAvailableBindingNr(void)
{	
	unsigned short nCurrNr = 0;
	CBindingEntry *pEntry;

	for (int i=0;i<Count();i++) 
	{			
		if ((pEntry = GetEntry(i)) != NULL)
		{				
			if (pEntry->GetBindingNr() >= nCurrNr) 
			{
				nCurrNr = pEntry->GetBindingNr();
			}
		}

		ASSERT(pEntry);
	}

	return (nCurrNr+1);
}
#endif

// TM 20081223: Gebruik van Iterators.
unsigned short CBindingEntries::GetNextAvailableBindingNr(void)
{	
	unsigned short nCurrNr = 0;
	Iterator it(CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingEntry* const pEntry = it.CurrentItem();
		if (pEntry->GetBindingNr() >= nCurrNr) 
		{
			nCurrNr = pEntry->GetBindingNr();
		}
	}
	return (nCurrNr+1);
}

unsigned short CBindingEntries::Count(void)
{
	return (List.GetCount());
}

CBindingEntry* CBindingEntries::Get(unsigned short nID,BOOL fShowError)
{
	CBindingEntry* pEntry;

	ASSERT (nID);

	POSITION pos = List.GetHeadPosition();

	while(pos!=NULL)
	{
		pEntry = (CBindingEntry*) List.GetNext(pos);		

		ASSERT(pEntry);
		
		if (pEntry != NULL)
		{
			if ((pEntry->GetID()) == nID)
			{
				return(pEntry);
			}
		}	
	}
   
	if (fShowError == TRUE)
	{
		CString szError;
		szError.Format("CBindingEntries::Get(nID) - Binding Entry Not Found");
		AfxMessageBox(szError,MB_ICONSTOP);
	}
   
	return NULL;
}

// Private Use : When ID not Found returns NULL !
CBindingEntry* CBindingEntries::_Get(unsigned short nID)
{
	CBindingEntry* pEntry;

	if (nID == 0) 
	{
		//ASSERT(0);
		return NULL;
	}

	POSITION pos = List.GetHeadPosition();

	while(pos!=NULL)
	{
		pEntry = (CBindingEntry*) List.GetNext(pos);		
		
		ASSERT(pEntry);

		if (pEntry != NULL)
		{
			if ((pEntry->GetID()) == nID)
			{
				return(pEntry);
			}
		}	
	}
   
   return NULL;
}

CBindingEntry* CBindingEntries::GetEntry(unsigned short nEntry,BOOL fShowError)
{
	CBindingEntry* pEntry;

	int number = 0;

	if (nEntry >= Count())
	{
		if (fShowError)
		{
			CString szError("CBindingEntries::GetEntry(nEntry) - Binding Entry Not Found");
			AfxMessageBox(szError,MB_ICONSTOP);
		}

		return NULL;	
	}

	POSITION pos = List.GetHeadPosition();
	
	while(pos!=NULL)
	{
		pEntry = (CBindingEntry*) List.GetNext(pos);

		ASSERT(pEntry);

		if (pEntry != NULL)
		{
			if (number++ == nEntry) 
			{
				return pEntry;			
			}
		}	
	}
	return NULL;
}

BOOL CBindingEntries::Remove(unsigned short nID,BOOL fShowError)
{
	CBindingEntry *pEntry;
	
	ASSERT (nID);

	POSITION pos2;	
	POSITION pos1 = List.GetHeadPosition();

	while(pos1!=NULL)
	{
  	   pos2 = pos1;

	   pEntry = (CBindingEntry*) List.GetNext( pos1 );	 
	   ASSERT(pEntry);

	   if (pEntry != NULL)
	   {
			if ((pEntry->GetID()) == nID)
			{
				pEntry = (CBindingEntry*) List.GetAt( pos2 );	// Save the old pointer for deletion
				ASSERT(pEntry);

				if (pEntry != NULL)
				{
					List.RemoveAt( pos2 );	           

					delete pEntry;						// Deletion avoids memory leak.		   
					pEntry = NULL;

					return (TRUE);
				}
			}
	   }	  
   }
   
	if (fShowError)
	{
		CString szError;
		szError.Format("BindingEntry Not Found");
		AfxMessageBox(szError,MB_ICONSTOP);
	}

	return FALSE;
}

unsigned short CBindingEntries::Add(CBindingEntry* pEntry)
{
	ASSERT(pEntry);	
	ASSERT(GetNextAvailableID());
	if (pEntry != NULL) {
		pEntry->SetID(GetNextAvailableID());
		List.AddTail(pEntry);
		return(pEntry->GetID());
	}
	return 0;
}
