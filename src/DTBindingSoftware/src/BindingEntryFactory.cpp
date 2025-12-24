// BindingEntryFactory.cpp: implementation of the CBindingEntryFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingEntryFactory.h"

#include "BindingEntry.h"
#include "BindingEntries.h"		// Temp !!!

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingEntryFactory::CBindingEntryFactory()
{

}

CBindingEntryFactory::~CBindingEntryFactory()
{

}

CBindingEntry* CBindingEntryFactory::Create()
{
	CBindingEntry* pNew = NULL;
	try
	{
		pNew = new CBindingEntry();
	}
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
	}
	return pNew;
}

CBindingEntry* CBindingEntryFactory::Clone(CBindingEntry* pBindingEntry)
{
	CBindingEntry* pNew = NULL;
	
	if (pBindingEntry != 0)
	{
		try
		{
			pNew = new CBindingEntry(*pBindingEntry);					
		}
		catch(CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
		}	
	}
	return pNew;
}