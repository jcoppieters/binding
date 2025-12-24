// BindingLinks.cpp: implementation of the CBindingLinks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingLinks.h"

#include "addflow\AddflowConst.h"

#include "ExceptionDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingLinks::CBindingLinks()
{
}

CBindingLinks::CBindingLinks(const CBindingLinks& BindingLinks)
{
	Copy(&BindingLinks);
}

CBindingLinks::~CBindingLinks()
{
	RemoveAll();
}

CBindingLinks& CBindingLinks::operator=(const CBindingLinks& BindingLinks)
{
	if (this == &BindingLinks) 
		return (*this);

	Copy(&BindingLinks);
	return (*this);
}

////////////////////////////////////////////////////////////////////////////////

void CBindingLinks::Copy(const CBindingLinks* const pBindingLinks)
{
	if (pBindingLinks == 0) {
		throw new CExceptionDatabase("Pointer to null");
	}
				
	RemoveAll();

	for(int i=0;i<pBindingLinks->Count();i++)
	{
		CBindingLink* const pSource = pBindingLinks->Get(i);
		if (pSource == 0) {
			ASSERT(pSource);
			throw new CExceptionDatabase("Failed to get Bindinglink");
		}

		CBindingLink* const pNew = new CBindingLink(*pSource);
		if (pNew == 0){
			throw new CExceptionDatabase("Pointer to null");
		}
		if (Add(pNew) == 0) {
			ASSERT(0);			
			throw new CExceptionDatabase("Failed to add Bindinglink");
		}		
	}
}

void CBindingLinks::RemoveAll(void)
{
	CBindingLink *pLink;
		
	for (int i=0;i<Count();i++)
	{
		if ((pLink = Get(i)) != 0)
		{
			delete pLink;
			pLink = NULL;
		}
	}
	List.RemoveAll();
}

unsigned short CBindingLinks::GetNextAvailableID(void)
{
	unsigned short nCurr;
	unsigned short nLast;
	CBindingLink* pLink;
	
	nCurr = nLast = 0;

	POSITION pos = List.GetHeadPosition();
	while(pos!=NULL)
	{
		if ((pLink = (CBindingLink*) List.GetNext(pos)) != NULL)
		{		
			nCurr = pLink->GetID();
		}

		if (nCurr > nLast) nLast = nCurr;
	}
    
	nLast +=1;
	return (nLast);
}

unsigned short CBindingLinks::Count(void) const
{
	return (List.GetCount());
}

unsigned short CBindingLinks::Add(CBindingLink* pLink)
{
	if (pLink != 0)
	{	
		if (pLink->GetID() == 0)
		{
			pLink->SetID(GetNextAvailableID());
		}

		List.AddTail(pLink);
		return(pLink->GetID());
	}
	return 0;
}

CBindingLink* CBindingLinks::Find(unsigned short nID) const
{
	CBindingLink* pLink;
	
	ASSERT (nID);

	POSITION pos = List.GetHeadPosition();
	while(pos!=NULL)
	{
		if ((pLink = (CBindingLink*) List.GetNext(pos)) != NULL)
		{	
			if ((pLink->GetID()) == nID)
			{
				return(pLink);
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

CBindingLink* CBindingLinks::Get(unsigned short nEntry) const
{
	CBindingLink* pLink;

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
		pLink = (CBindingLink*) List.GetNext(pos);
		ASSERT(pLink);

		if (number++ == nEntry) return pLink;			
	}

	return NULL;
}

BOOL CBindingLinks::Remove(unsigned short nID)
{
	CBindingLink *pLink;	
	
	ASSERT (nID);

	POSITION pos2;
	
	POSITION pos1 = List.GetHeadPosition();

	while(pos1!=NULL)
	{
  	   pos2 = pos1;
 
	   if ((pLink = (CBindingLink*) List.GetNext( pos1 )) != NULL)
	   {	   
			if ((pLink->GetID()) == nID)
			{
				if ((pLink = (CBindingLink*) List.GetAt( pos2 )) != NULL)
				{
					List.RemoveAt( pos2 );
			
					delete pLink;						// Deletion avoids memory leak.
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

