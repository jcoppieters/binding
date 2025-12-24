// BindingOperators.cpp: implementation of the CBindingOperators class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingOperators.h"

#include "BindingOperatorFactory.h"
#include "ExceptionDatabase.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingOperators::CBindingOperators()
{
}
CBindingOperators::CBindingOperators(const CBindingOperators& BindingOperators)
{
	Copy(&BindingOperators);
}

CBindingOperators::~CBindingOperators()
{
	RemoveAll();

}

CBindingOperators& CBindingOperators::operator=(const CBindingOperators& BindingOperators)
{
	if (this == &BindingOperators)
		return (*this);
	
	Copy(&BindingOperators);
	return (*this);
}
//////////////////////////////////////////////////////////////////////
void CBindingOperators::Copy(const CBindingOperators* const pSourceOperators)
{
	if (pSourceOperators == 0) {
		throw new CExceptionDatabase("Pointer to null");
	}
				
	RemoveAll();
	
	for(int i=0;i<pSourceOperators->Count();i++)
	{
		CBindingOperator* const pSource = pSourceOperators->Get(i);
		if (pSource == 0) {
			ASSERT(pSource);
			throw new CExceptionDatabase("Failed to get BindingOperator");
		}

		CBindingOperator* const pNew = CBindingOperatorFactory::Clone(pSource);	
		if (pNew == 0){
			throw new CExceptionDatabase("Pointer to null");
		}

		if (Add(pNew) == 0) {
			ASSERT(0);			
			throw new CExceptionDatabase("Failed to add BindingOperator");
		}		
	}
}

//////////////////////////////////////////////////////////////////////

void CBindingOperators::RemoveAll(void)
{
	CBindingOperator *pUnit;

	for (int i=0;i<Count();i++)
	{
		if ((pUnit = Get(i)) != NULL)
		{
			delete pUnit;
			pUnit = NULL;
		}
	}

	List.RemoveAll();
}


///////////////////////////////////////////////////////////////////////
unsigned short CBindingOperators::GetNextAvailableID(void)
{
	unsigned short nCurr;
	unsigned short nLast;
	CBindingOperator* pUnit;
	nCurr = nLast = 0;

	POSITION pos = List.GetHeadPosition();
	
	while(pos!=NULL)
	{
		if ((pUnit = (CBindingOperator*) List.GetNext(pos)) != NULL)
		{		
			nCurr = pUnit->GetID();	
		}

		if (nCurr > nLast) nLast = nCurr;
	}

    nLast +=1;
	return (nLast);
}

///////////////////////////////////////////////////////////////////////

unsigned short CBindingOperators::Count(void) const
{
	return (List.GetCount());
}

CBindingOperator* CBindingOperators::Find(unsigned short nID) const
{
	CBindingOperator* pUnit;

	ASSERT (nID);

	POSITION pos = List.GetHeadPosition();

	while (pos!=NULL)
	{
		if ((pUnit = (CBindingOperator*) List.GetNext(pos)) != NULL)
		{	
			if ((pUnit->GetID()) == nID)
			{
				return(pUnit);
			}
		}
	}
	
	if (1)
	{
		CString szError;
		szError.LoadString(IDS_ERROR_COLOPERATORS_NOTFOUND);
		AfxMessageBox(szError);
	}
	return NULL;

}

CBindingOperator* CBindingOperators::Get(unsigned short nEntry) const
{
	CBindingOperator* pUnit;

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
		pUnit = (CBindingOperator*) List.GetNext(pos);
	
		if (number++ == nEntry) 
		{
			return pUnit;
		}
	}
	return NULL;
}

unsigned short CBindingOperators::Add(CBindingOperator *pUnit)
{
	ASSERT(pUnit);

	if (pUnit != NULL)
	{
		if (pUnit->GetID() == 0)
		{
			pUnit->SetID(GetNextAvailableID());
		}
	
		List.AddTail(pUnit);

		ASSERT(pUnit == List.GetTail());

		return(pUnit->GetID());
	}
	return 0;
}

BOOL CBindingOperators::Remove(unsigned short nID)
{
	ASSERT (nID);

	CBindingOperator *pUnit;
	POSITION pos2;
	
	POSITION pos1 = List.GetHeadPosition();

	while(pos1!=NULL)
	{
  	   pos2 = pos1;
 
	   if ((pUnit = (CBindingOperator*) List.GetNext( pos1 )) != NULL)
	   {	  
			if ((pUnit->GetID())==nID)
			{
				pUnit = (CBindingOperator*) List.GetAt( pos2 );
				List.RemoveAt( pos2 );

				if (pUnit != NULL)
				{
					delete pUnit;					
					pUnit = NULL;

					return (TRUE);
				}
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

OperatorType CBindingOperators::GetType(unsigned short nID) const
{
	ASSERT (nID);
	CBindingOperator* const pUnit = Find(nID);	

	ASSERT(pUnit);
	return(pUnit->GetType());
}
