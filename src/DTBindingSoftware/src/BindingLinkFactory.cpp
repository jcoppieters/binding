// BindingLinkFactory.cpp: implementation of the CBindingLinkFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingLinkFactory.h"

#include "BindingLink.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingLinkFactory::CBindingLinkFactory()
{

}

CBindingLinkFactory::~CBindingLinkFactory()
{

}

//////////////////////////////////////////////////////////////////////

CBindingLink* CBindingLinkFactory::Create()
{
	CBindingLink* pNew = NULL;

	try
	{
		pNew = new CBindingLink();
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
	}

	return pNew;
}

CBindingLink* CBindingLinkFactory::Clone(const CBindingLink* pBindingLink)
{
	CBindingLink* pNew = NULL;

	try
	{
		pNew = new CBindingLink(*pBindingLink);
		//pNew->Copy(pBindingLink);
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
	}

	return pNew;
}
