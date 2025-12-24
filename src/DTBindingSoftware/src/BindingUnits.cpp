// BindingUnits.cpp: implementation of the CBindingUnits class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingUnits.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "BindingConstants.h"
#include "BindingUnitFactory.h"
#include "ExceptionDatabase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CBindingUnits::CBindingUnits()
{
}
CBindingUnits::CBindingUnits(const CBindingUnits& BindingUnits)
{
	Copy(&BindingUnits);
}
CBindingUnits& CBindingUnits::operator=(const CBindingUnits& BindingUnits)
{
	if (this == &BindingUnits)
		return (*this);

	Copy(&BindingUnits);
	return(*this);
}

CBindingUnits::~CBindingUnits()
{
	RemoveAll();
}
////////////////////////////////////////////////////////////////////////////
void CBindingUnits::Copy(const CBindingUnits* const pSrcBindingUnits)
{
	if (pSrcBindingUnits == 0) 
	{
		throw new CExceptionDatabase("Pointer to null");
	}

	RemoveAll();

	CBindingUnits_it it(const_cast<CBindingUnits*>(pSrcBindingUnits)->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingUnit *const pNew = CBindingUnitFactory::_Clone(it.CurrentItem());	
		if (pNew == 0)
		{
			throw new CExceptionDatabase("Pointer to null");
		}

		if (Add(pNew) == 0) 
		{
			ASSERT(0);			
			throw new CExceptionDatabase("Failed to add BindingOperator");
		}	
	}
}
void CBindingUnits::RemoveAll(void)
{
	while (!List.IsEmpty())
	{
		CBindingUnit* const pEntry = 
			reinterpret_cast<CBindingUnit*>(List.RemoveHead( ));

		ASSERT( pEntry );
		delete pEntry;
	}
	ASSERT( List.GetCount( ) == 0 );

	List.RemoveAll();
}
//////////////////////////////////////////////////////////////////////
unsigned short CBindingUnits::GetNextAvailableID()
{
	unsigned short nCurr;
	unsigned short nLast;

	CBindingUnit *pUnit;
	
	nCurr = nLast = 0;

	POSITION pos = List.GetHeadPosition();
	while(pos!=NULL)
	{
		pUnit = (CBindingUnit*) List.GetNext(pos);
		nCurr = pUnit->GetID();			

		if (nCurr > nLast) 
		{
			nLast = nCurr;
		}
	}
	
	nLast +=1;
    return (nLast);
}

//////////////////////////////////////////////////////////////////////

unsigned short CBindingUnits::Count(void) const
{
	return (List.GetCount());
}

/**
 * Add new bindingunit to the collection, if ID is 0 then Get first available ID.
 * returns 0 is function fails ...
 * otherwise return ID ...
 */
unsigned short CBindingUnits::Add(CBindingUnit* pBindingUnit)
{
	if (pBindingUnit != NULL)
	{	
		if (pBindingUnit->GetID() == 0)
		{
			pBindingUnit->SetID(GetNextAvailableID());
		}

		List.AddTail(pBindingUnit);
	
		ASSERT(pBindingUnit == List.GetTail());
		ASSERT(pBindingUnit->GetID());

		return(pBindingUnit->GetID());
	}
	return 0;
}
CBindingUnit* CBindingUnits::Find(unsigned short nID) const
{
	CBindingUnit* pUnit;

	ASSERT (nID);

	POSITION pos = List.GetHeadPosition();
	while(pos!=NULL)
	{
		if ((pUnit = (CBindingUnit*) List.GetNext(pos)) != NULL)
		{		
			if ((pUnit->GetID())==nID)
			{
				return(pUnit);
			}
		}
	}
	
	if (1)
	{	
		CString szError;
		szError.LoadString(IDS_ERROR_COLUNITS_NOTFOUND);
		AfxMessageBox(szError);
	}

	return NULL;
}

CBindingUnit* CBindingUnits::Get(unsigned short nEntry) const
{	
	CBindingUnit* pUnit;

	int number = 0;

	if (nEntry >= Count()) 
	{
		CString szError;
		szError.LoadString(IDS_ERROR_COLUNITS_NOTFOUND);
		AfxMessageBox(szError);
		return NULL;	
	}

	POSITION pos = List.GetHeadPosition();
	
	while(pos!=NULL) 
	{
		pUnit = (CBindingUnit*) List.GetNext(pos);	

		if (number++ == nEntry) 
		{
			return pUnit;			
		}
	}

	return NULL;
}
BOOL CBindingUnits::Remove(unsigned short nID)
{
	CBindingUnit *pUnit;
	
	ASSERT (nID);

	POSITION pos1,pos2;
	
	pos1 = List.GetHeadPosition();

	while(pos1!=NULL)
	{
  	   pos2 = pos1;
 
	   if ((pUnit = (CBindingUnit*) List.GetNext( pos1 )) != NULL)
	   {
			if ((pUnit->GetID()) == nID)	
			{
				pUnit = (CBindingUnit*) List.GetAt( pos2 );	// Save the old pointer for deletion
				List.RemoveAt( pos2 );
				delete pUnit;						// Deletion avoids memory leak.
				return (TRUE);
			}
	   }
   }

   if (1)
   {
		CString szError;
		szError.LoadString(IDS_ERROR_COLUNITS_NOTFOUND);
		AfxMessageBox(szError);
   }

   return FALSE;
}

BOOL CBindingUnits::Replace(CBindingUnit* const pOld,CBindingUnit* const pNew)
{
	ASSERT(pOld && pNew);
	ASSERT(pOld->GetID() == pNew->GetID());

	CBindingUnit* pUnit;

	POSITION pos = List.GetHeadPosition();
	POSITION pos2 = pos;

	while(pos!=NULL)
	{
		if ((pUnit = (CBindingUnit*) List.GetNext(pos)) != NULL)
		{			
			if (pOld->GetID() == pUnit->GetID())
			{					
				List.SetAt(pos2,pNew);
				delete pUnit;
				pUnit = NULL;
				return TRUE;				
			}
			pos2 = pos;
		}
	}
	return FALSE;
}
