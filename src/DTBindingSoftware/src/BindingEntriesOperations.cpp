// BindingEntriesOperations.cpp: implementation of the CBindingEntriesOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEntriesOperations.h"

#include "BindingEntries.h"
#include "BindingEntryFactory.h"
#include "ExceptionDatabase.h"
#include "BindingEntryOperations.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////

static void helper_setAllSend(CBindingEntries* pBindingEntries,BOOL state) {
	CBindingEntries::Iterator it(pBindingEntries->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		it.CurrentItem()->SetSend(state);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingEntriesOperations::CBindingEntriesOperations(CBindingEntries* pBindingEntries) :
m_pBindingEntries( pBindingEntries )
{
	ASSERT( m_pBindingEntries );
}
CBindingEntriesOperations::~CBindingEntriesOperations()
{
}

/**
 * Check's if there is already an entry for nID 
 * Yes --> Copy Source to existing Ptr.
 * No --> Allocate new object + Store in List
 */
unsigned short CBindingEntriesOperations::Add(CBindingEntry *pSource,unsigned short usBindingNr)
{
	CBindingEntry *pNew = m_pBindingEntries->_Get(pSource->GetID());

	try
	{	
		if (pNew == NULL) 
		{	
			if ((pNew = CBindingEntryFactory::Clone(pSource)) != 0)
			{
				if (usBindingNr == 0) 
				{
					pNew->SetBindingNr(m_pBindingEntries->GetNextAvailableBindingNr());
				}

				const unsigned short nID = m_pBindingEntries->Add(pNew);
				ASSERT(pNew->GetID() == nID);
				ASSERT(nID);

				return (nID);
			}		
		}
		else 
		{		
			*pNew = *pSource;

			ASSERT(pNew->GetID());
			ASSERT(pNew->GetID() == pSource->GetID());								
			return (pNew->GetID());				
		}			
	}

	catch (CExceptionDatabase* e)
	{
		e->Display();
		e->Delete();

		if (pNew != 0)
		{
			delete pNew;
			pNew = 0;
		}
	}

	ASSERT(0);
	return 0;
}



void CBindingEntriesOperations::SetAllSend() {	
	helper_setAllSend(m_pBindingEntries,TRUE);
}

void CBindingEntriesOperations::SetAllErased() {
	helper_setAllSend(m_pBindingEntries,FALSE);
}

void CBindingEntriesOperations::SetSendFailed(BYTE bNodeAddress)
{
	CBindingEntries::Iterator it(m_pBindingEntries->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingEntry* const pBindingEntry = it.CurrentItem();
		ASSERT( pBindingEntry );
	
		CBindingUnits::Iterator it2(pBindingEntry->GetColUnit()->CreateIterator());
		for (it2.Begin();!it2.IsDone();it2.Next())
		{		
			if (it2.CurrentItem()->GetNodeAddress() == bNodeAddress)
			{
				pBindingEntry->SetSend( FALSE );
				break;
			}
		}			
	}
}

void CBindingEntriesOperations::SetDefaultNames()
{
	CBindingEntries::Iterator it(m_pBindingEntries->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingEntryOperations Operations(it.CurrentItem());
		Operations.SetDefaultName();	
	}
}
